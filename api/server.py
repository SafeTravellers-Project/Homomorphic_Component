import os
import re
import shutil
import subprocess
import tempfile
import threading
import uuid
import base64
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List, Optional

from fastapi import Depends, FastAPI, File, Header, HTTPException, UploadFile
from pydantic import BaseModel

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BIN_DIR = PROJECT_ROOT / "bin"
TMP_DIR = PROJECT_ROOT / "tmp"
LOCAL_LIB_PATHS = ":".join(
    [
        str(PROJECT_ROOT / "all_libs" / "tfhe" / "build" / "libtfhe"),
        str(PROJECT_ROOT / "all_libs" / "tfhe-operators-master" / "new_tfhe" / "build" / "lib"),
        str(PROJECT_ROOT / "all_libs" / "tfhe-operators-master" / "more_operations" / "build" / "lib"),
        str(PROJECT_ROOT / "all_libs" / "openssl-3.0.14" / "install" / "lib64"),
    ]
)

API_KEY = os.getenv("SAFE_API_KEY", "change-this-api-key")
HOST = os.getenv("SAFE_API_HOST", "0.0.0.0")
PORT = int(os.getenv("SAFE_API_PORT", "8080"))

app = FastAPI(title="SAFETravellers API", version="2.0.0")
TMP_DIR.mkdir(parents=True, exist_ok=True)

# ---------------------------------------------------------------------------
# Session store
# Maps session_id (str) -> session record (dict).
# Persists for the lifetime of the container process.
# ---------------------------------------------------------------------------
_sessions: Dict[str, dict] = {}
_sessions_lock = threading.Lock()

# Session lifecycle: PENDING → STARTED → COMPLETED | FAILED
SESSION_PENDING   = "PENDING"
SESSION_STARTED   = "STARTED"
SESSION_COMPLETED = "COMPLETED"
SESSION_FAILED    = "FAILED"


# ---------------------------------------------------------------------------
# Pydantic models
# ---------------------------------------------------------------------------

class CryptoFile(BaseModel):
    name: str # e.g., "ciph", "cosineciph", "sqciph"
    content_base64: str

class Biometric(BaseModel):
    bio_name: str
    files: List[CryptoFile]

class VerifyRequest(BaseModel):
    threshold: int
    registered_enc_set: List[Biometric]
    test_enc_set: List[Biometric]


class CommandResponse(BaseModel):
    ok: bool
    command: List[str]
    exit_code: int
    stdout: str
    stderr: str


class SessionCreated(BaseModel):
    session_id: str
    operation: str
    status: str          # always PENDING on creation
    created_at: str
    poll_url: str
    result_url: str


class SessionStatus(BaseModel):
    session_id: str
    operation: str
    status: str          # PENDING | STARTED | COMPLETED | FAILED
    created_at: str
    updated_at: str
    error: Optional[str] = None


class SessionResult(BaseModel):
    session_id: str
    operation: str
    status: str          # COMPLETED
    created_at: str
    completed_at: str
    result: dict


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------


def _folder_to_base64(output_dir: Path) -> List[dict]:
    """
    Converts all files in a folder into base64 JSON payload.
    """
    results = []

    for bio_dir in output_dir.iterdir():
        if not bio_dir.is_dir():
            continue

        files = []

        for file_path in bio_dir.iterdir():
            if not file_path.is_file():
                continue

            files.append({
                "name": file_path.name,
                "content_base64": base64.b64encode(file_path.read_bytes()).decode()
            })

        results.append({
            "bio_name": bio_dir.name,
            "files": files
        })

    return results

def _write_biometrics(encs: List[Biometric], root: Path) -> None:
    for bio in encs:
        bio_dir = root / bio.bio_name
        bio_dir.mkdir(parents=True, exist_ok=True)

        seen = set()

        for f in bio.files:
            if f.name not in {"ciph", "cosineciph", "sqciph"}:
                raise HTTPException(400, f"Invalid file: {f.name}")

            data = base64.b64decode(f.content_base64)
            (bio_dir / f.name).write_bytes(data)

            seen.add(f.name)

        if seen != {"ciph", "cosineciph", "sqciph"}:
            raise HTTPException(
                400,
                f"Incomplete biometric {bio.bio_name}"
            )




def _check_api_key(x_api_key: str = Header(default="")) -> None:
    if not API_KEY or x_api_key != API_KEY:
        raise HTTPException(status_code=401, detail="Invalid API key")


def _run_binary(command: List[str]) -> CommandResponse:
    process_env = os.environ.copy()
    existing_ld_path = process_env.get("LD_LIBRARY_PATH", "")
    process_env["LD_LIBRARY_PATH"] = (
        f"{LOCAL_LIB_PATHS}:{existing_ld_path}" if existing_ld_path else LOCAL_LIB_PATHS
    )

    try:
        result = subprocess.run(
            command,
            cwd=str(BIN_DIR),
            text=True,
            capture_output=True,
            timeout=1800,
            check=False,
            env=process_env,
        )
    except subprocess.TimeoutExpired:
        raise HTTPException(status_code=504, detail="Binary execution timed out after 1800 s")
    except FileNotFoundError:
        raise HTTPException(status_code=500, detail=f"Binary not found: {command[0]}")

    response = CommandResponse(
        ok=result.returncode == 0,
        command=command,
        exit_code=result.returncode,
        stdout=result.stdout,
        stderr=result.stderr,
    )

    if result.returncode != 0:
        raise HTTPException(status_code=500, detail=response.model_dump())

    return response


def _save_uploads(files: List[UploadFile], destination: Path) -> int:
    destination.mkdir(parents=True, exist_ok=True)
    saved_count = 0
    for file in files:
        filename = Path(file.filename or "").name
        if not filename:
            continue
        target = destination / filename
        with target.open("wb") as out:
            shutil.copyfileobj(file.file, out)
        saved_count += 1
    if saved_count == 0:
        raise HTTPException(status_code=400, detail="No valid files uploaded")
    return saved_count

def _save_uploads_grouped(files: List[UploadFile], destination: Path) -> int:
    destination.mkdir(parents=True, exist_ok=True)
    count = 0

    for file in files:
        filename = Path(file.filename or "").name
        if not filename:
            continue

        bio_name = Path(filename).stem  # face.bin → face

        bio_dir = destination / bio_name
        bio_dir.mkdir(parents=True, exist_ok=True)

        target = bio_dir / filename

        with target.open("wb") as out:
            shutil.copyfileobj(file.file, out)

        count += 1

    if count == 0:
        raise HTTPException(status_code=400, detail="No valid files uploaded")

    return count



def _path_with_trailing_sep(path_obj: Path) -> str:
    path_text = str(path_obj)
    if path_text.endswith(os.sep):
        return path_text
    return path_text + os.sep


def _now_iso() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def _create_session(operation: str) -> str:
    session_id = str(uuid.uuid4())
    with _sessions_lock:
        _sessions[session_id] = {
            "session_id": session_id,
            "operation": operation,
            "status": SESSION_PENDING,
            "created_at": _now_iso(),
            "updated_at": _now_iso(),
            "result": None,
            "error": None,
        }
    return session_id


def _update_session(session_id: str, **kwargs: object) -> None:
    with _sessions_lock:
        if session_id in _sessions:
            _sessions[session_id].update(kwargs)
            _sessions[session_id]["updated_at"] = _now_iso()


def _run_session_thread(session_id: str, fn, *args, **kwargs) -> None:
    """Run fn in a background thread, advancing the session through its states."""
    def _worker():
        _update_session(session_id, status=SESSION_STARTED)
        try:
            result = fn(*args, **kwargs)
            _update_session(session_id, status=SESSION_COMPLETED, result=result)
        except HTTPException as exc:
            _update_session(session_id, status=SESSION_FAILED, error=str(exc.detail))
        except Exception as exc:
            _update_session(session_id, status=SESSION_FAILED, error=str(exc))

    threading.Thread(target=_worker, daemon=True).start()


def _session_created_response(session_id: str, operation: str, created_at: str) -> SessionCreated:
    return SessionCreated(
        session_id=session_id,
        operation=operation,
        status=SESSION_PENDING,
        created_at=created_at,
        poll_url=f"/api/v1/sessions/{session_id}",
        result_url=f"/api/v1/sessions/{session_id}/result",
    )


def _get_session_record(session_id: str) -> dict:
    with _sessions_lock:
        record = _sessions.get(session_id)
    if record is None:
        raise HTTPException(status_code=404, detail=f"Session not found: {session_id}")
    return record


# ---------------------------------------------------------------------------
# Background task implementations
# ---------------------------------------------------------------------------

def _task_init() -> dict:
    result = _run_binary(["./HESysInit"])
    return result.model_dump()


def _task_register(input_dir: Path, count: int) -> dict:
    output_dir = Path(tempfile.mkdtemp(prefix="register_out_", dir=str(TMP_DIR)))
    try:
        result = _run_binary(["./Register", str(input_dir), str(output_dir)])
        files_b64 = _folder_to_base64(output_dir)
        
        return {
        "ok": True,
        "uploaded_files": count,
        "encrypted_biometrics": files_b64,
        "stdout": result.stdout,
        "stderr": result.stderr,
        }
    
    finally:
        shutil.rmtree(input_dir, ignore_errors=True)
        shutil.rmtree(output_dir, ignore_errors=True)

    
    
    

def _task_encrypt_bio(input_dir: Path, count: int) -> dict:
    output_dir = Path(tempfile.mkdtemp(prefix="encrypt_out_", dir=str(TMP_DIR)))
    try:
        result = _run_binary(["./EncBio", str(input_dir), str(output_dir)])
        files_b64 = _folder_to_base64(output_dir)
        return {
        "ok": True,
        "uploaded_files": count,
        "encrypted_biometrics": files_b64,
        "stdout": result.stdout,
        "stderr": result.stderr,
        }
    finally:
        shutil.rmtree(input_dir, ignore_errors=True)
        shutil.rmtree(output_dir, ignore_errors=True)
    


def _task_verify(threshold: int, test_set: List[Biometric], registered_set: List[Biometric]) -> dict:
    reg_dir = Path(tempfile.mkdtemp(prefix="verify_reg_", dir=str(TMP_DIR)))
    test_dir = Path(tempfile.mkdtemp(prefix="verify_test_", dir=str(TMP_DIR)))
    
    try:
        _write_biometrics(registered_set, reg_dir)
        _write_biometrics(test_set, test_dir)


        cmd = [
            "./Verify",
            str(threshold),
            _path_with_trailing_sep(test_dir),
            _path_with_trailing_sep(reg_dir),
        ]
        result = _run_binary(cmd)
        match_line = next((l for l in result.stdout.splitlines() if "||" in l), "")

    finally:
        shutil.rmtree(reg_dir, ignore_errors=True)
        shutil.rmtree(test_dir, ignore_errors=True)


    return {
        "ok": True,
        #"verify_result_line": match_line,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }
    

# ---------------------------------------------------------------------------
# Routes
# ---------------------------------------------------------------------------

@app.get("/health")
def health() -> dict:
    with _sessions_lock:
        active = sum(1 for s in _sessions.values() if s["status"] == SESSION_STARTED)
    return {
        "status": "ok",
        "service": "SAFETravellers API",
        "active_sessions": active,
    }


# --- Session status polling ---

@app.get(
    "/api/v1/sessions/{session_id}",
    response_model=SessionStatus,
    dependencies=[Depends(_check_api_key)],
)
def get_session_status(session_id: str) -> SessionStatus:
    record = _get_session_record(session_id)
    return SessionStatus(
        session_id=record["session_id"],
        operation=record["operation"],
        status=record["status"],
        created_at=record["created_at"],
        updated_at=record["updated_at"],
        error=record.get("error"),
    )


# --- Session result retrieval ---

@app.get(
    "/api/v1/sessions/{session_id}/result",
    response_model=SessionResult,
    dependencies=[Depends(_check_api_key)],
)
def get_session_result(session_id: str) -> SessionResult:
    record = _get_session_record(session_id)

    if record["status"] == SESSION_FAILED:
        # Raise condition: client receives 422 with the failure reason
        raise HTTPException(
            status_code=422,
            detail={
                "session_id": session_id,
                "operation": record["operation"],
                "status": SESSION_FAILED,
                "error": record.get("error"),
            },
        )

    if record["status"] != SESSION_COMPLETED:
        raise HTTPException(
            status_code=409,
            detail=f"Session is not yet completed. Current status: {record['status']}",
        )

    return SessionResult(
        session_id=record["session_id"],
        operation=record["operation"],
        status=SESSION_COMPLETED,
        created_at=record["created_at"],
        completed_at=record["updated_at"],
        result=record["result"],
    )


# --- List all sessions ---

@app.get(
    "/api/v1/sessions",
    response_model=List[SessionStatus],
    dependencies=[Depends(_check_api_key)],
)
def list_sessions() -> List[SessionStatus]:
    with _sessions_lock:
        snapshot = list(_sessions.values())
    return [
        SessionStatus(
            session_id=s["session_id"],
            operation=s["operation"],
            status=s["status"],
            created_at=s["created_at"],
            updated_at=s["updated_at"],
            error=s.get("error"),
        )
        for s in snapshot
    ]


# --- Operation endpoints (all return immediately with a session_id) ---

@app.post(
    "/api/v1/sessions/init",
    response_model=SessionCreated,
    dependencies=[Depends(_check_api_key)],
)
def init_system() -> SessionCreated:
    session_id = _create_session("init")
    created_at = _sessions[session_id]["created_at"]
    _run_session_thread(session_id, _task_init)
    return _session_created_response(session_id, "init", created_at)


@app.post(
    "/api/v1/sessions/register",
    response_model=SessionCreated,
    dependencies=[Depends(_check_api_key)],
)
def register_biometrics(
    files: List[UploadFile] = File(...),
    # output_bucket: str = Form("default"),
) -> SessionCreated:
    
    
    #timestamp = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")

    input_dir = Path(tempfile.mkdtemp(prefix="register_in_", dir=str(TMP_DIR)))
    
    
    #output_dir.mkdir(parents=True, exist_ok=True)
    # if not input_dir.exists():
    #     raise HTTPException(status_code=400, detail="Input folder not found")

    count = _save_uploads_grouped(files, input_dir)
    #count = len(list(input_dir.iterdir()))

    session_id = _create_session("register")
    created_at = _sessions[session_id]["created_at"]
    _run_session_thread(session_id, _task_register, input_dir, count)
    return _session_created_response(session_id, "register", created_at)


@app.post(
    "/api/v1/sessions/encrypt-bio",
    response_model=SessionCreated,
    dependencies=[Depends(_check_api_key)],
)
def encrypt_test_biometrics(
    files: List[UploadFile] = File(...),
    # output_bucket: str = Form("default"),
    #payload: FolderRequest
) -> SessionCreated:
    
   #timestamp = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")

    input_dir = Path(tempfile.mkdtemp(prefix="encbio_in_", dir=str(TMP_DIR)))
    
    #output_dir.mkdir(parents=True, exist_ok=True)

    # if not input_dir.exists() or not input_dir.is_dir():
    #     raise HTTPException(status_code=400, detail=f"Input folder not found: {input_dir}")

    count = _save_uploads_grouped(files, input_dir)

    session_id = _create_session("encrypt-bio")
    created_at = _sessions[session_id]["created_at"]
    _run_session_thread(session_id, _task_encrypt_bio, input_dir, count)
    return _session_created_response(session_id, "encrypt-bio", created_at)


@app.post(
    "/api/v1/sessions/verify",
    response_model=SessionCreated,
    dependencies=[Depends(_check_api_key)],
)
def verify_identity(payload: VerifyRequest) -> SessionCreated:
    

    #if not test_dir.exists() or not test_dir.is_dir():
    #    raise HTTPException(status_code=400, detail=f"Test folder not found: {test_dir}")
    #if not stored_dir.exists() or not stored_dir.is_dir():
    #    raise HTTPException(status_code=400, detail=f"Stored folder not found: {stored_dir}")

    session_id = _create_session("verify")
    created_at = _sessions[session_id]["created_at"]
    
    
    _run_session_thread(session_id, _task_verify, payload.threshold, payload.test_enc_set, payload.registered_enc_set)
    return _session_created_response(session_id, "verify", created_at)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run("server:app", host=HOST, port=PORT, reload=False)



