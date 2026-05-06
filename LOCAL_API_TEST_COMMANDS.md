# Local API Test Commands (Copy/Paste)

Project path used below:

`/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4`

---

## 1) Start API server (Terminal 1)

```bash
cd /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4
python -m pip install --user -r api/requirements.txt
SAFE_API_HOST=127.0.0.1 SAFE_API_PORT=8080 SAFE_API_KEY=localtestkey python api/server.py
```

Keep this terminal running.

---

## 2) Health check (Terminal 2)

```bash
cd /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4
python - <<'PY'
import urllib.request
print(urllib.request.urlopen("http://127.0.0.1:8081/health").read().decode())
PY
```

---

## 3) Init endpoint (Terminal 2)

```bash
cd /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4
python - <<'PY'
import urllib.request
req = urllib.request.Request(
    "http://127.0.0.1:8081/api/v1/sessions/init",
    method="POST",
    headers={"x-api-key": "localtestkey"}
)
print(urllib.request.urlopen(req, timeout=1800).read().decode())
PY
```

---

## 4) Full flow test: register + encrypt-bio + verify (Terminal 2)

```bash
cd /home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4
python - <<'PY'
import json, uuid, urllib.request
from pathlib import Path

BASE = "http://127.0.0.1:8081"
KEY = "localtestkey"
ROOT = Path("/home/olivechakraborty/Documents/CEA/Projects/SAFETravellers/Codes/Project26Update1024_v4")

def post_json(path, payload):
    req = urllib.request.Request(
        BASE + path,
        method="POST",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json", "x-api-key": KEY},
    )
    return json.loads(urllib.request.urlopen(req, timeout=1800).read().decode())

def post_file(path, file_path, bucket):
    b = "----WebKitFormBoundary" + uuid.uuid4().hex
    p = Path(file_path)
    body = b"\r\n".join([
        f"--{b}".encode(),
        b'Content-Disposition: form-data; name="output_bucket"',
        b"",
        bucket.encode(),
        f"--{b}".encode(),
        f'Content-Disposition: form-data; name="files"; filename="{p.name}"'.encode(),
        b"Content-Type: application/octet-stream",
        b"",
        p.read_bytes(),
        f"--{b}--".encode(),
        b"",
    ])
    req = urllib.request.Request(
        BASE + path,
        method="POST",
        data=body,
        headers={"Content-Type": f"multipart/form-data; boundary={b}", "x-api-key": KEY},
    )
    return json.loads(urllib.request.urlopen(req, timeout=1800).read().decode())

reg = post_file("/api/v1/sessions/register", ROOT / "data/Traveller/Reg_Bio/template_a0", "e2e_party_reg")
enc = post_file("/api/v1/sessions/encrypt-bio", ROOT / "data/E-Gate/Test_Bio/template_b0", "e2e_party_test")

reg_dir = str(Path(reg["encrypted_output_dir"]) / "template_a0_enc")
enc_dir = str(Path(enc["encrypted_output_dir"]) / "template_b0_enc")

ver = post_json("/api/v1/sessions/verify", {
    "threshold": 7000,
    "test_encrypted_folder": enc_dir,
    "stored_encrypted_folder": reg_dir
})

print("REGISTER:", reg["ok"], reg["encrypted_output_dir"])
print("ENCRYPT :", enc["ok"], enc["encrypted_output_dir"])
print("VERIFY  :", ver["ok"], ver.get("verify_result_line", ""))
PY
```

---

## 5) Stop server

In Terminal 1, press:

```bash
Ctrl + C
```
