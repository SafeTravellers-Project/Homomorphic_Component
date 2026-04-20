# SAFETravellers API — Deployment Steps (Docker)

This guide covers deploying the SAFETravellers HE web service as a Docker
container. No build tools, compilers, or cryptographic libraries need to be
installed manually — everything is packaged inside the Docker image.

For full hardware and resource requirements see `DEPLOYMENT_SPECS.txt`.

---

## 0) What the partner receives

The project folder must contain the following paths:

```
Dockerfile
docker-compose.yml
.dockerignore
bin/                  ← pre-compiled binaries (HESysInit, Register, EncBio, Verify)
api/server.py
api/requirements.txt
all_libs/tfhe/build/libtfhe/libtfhe-spqlios-avx.so
data/System_Parameters/
data/HEComp/
data/Traveller/
```

Everything else (source code, CMake files, large library trees) is not required
to run the service.

---

## 1) Install Docker on the server (one-time)

```bash
sudo apt-get update
sudo apt-get install -y docker.io docker-compose-plugin
sudo usermod -aG docker $USER
```

Log out and back in so the group membership takes effect, then verify:

```bash
docker --version
docker compose version
```

---

## 2) Copy the project to the server

Transfer the project folder via `scp`, `rsync`, or a zip archive:

```bash
scp -r Project26Update1024_v4/ ubuntu@<server-ip>:/home/ubuntu/
```

On the server, enter the project directory:

```bash
cd /home/ubuntu/Project26Update1024_v4
```

---

## 3) Set the API key

Create a `.env` file at the project root:

```bash
echo "SAFE_API_KEY=your-long-random-secret" > .env
```

Choose a strong random value — this key is required in the `X-Api-Key` header
for every API call.

---

## 4) Build the Docker image and start the service

```bash
docker compose up --build -d
```

- `--build` compiles the image from the `Dockerfile` (needed only on first run
  or after receiving an updated project folder).
- `-d` runs the container in the background.

The build takes approximately 1–2 minutes. Once complete, the service listens
on port `8080`.

---

## 5) Verify the service is running

```bash
curl http://localhost:8080/health
```

Expected response:

```json
{"status": "ok", "service": "SAFETravellers API"}
```

---

## 6) (Optional) Set up Nginx for TLS

If the service is exposed on the internet, terminate TLS with Nginx.

Install Nginx and Certbot:

```bash
sudo apt-get install -y nginx certbot python3-certbot-nginx
```

Copy and configure the provided Nginx template:

```bash
sudo cp deploy/nginx/safetravellers-api.conf \
        /etc/nginx/sites-available/safetravellers-api.conf

# Replace the placeholder domain name
sudo sed -i 's/<DOMAIN_NAME>/your.domain.com/g' \
        /etc/nginx/sites-available/safetravellers-api.conf

sudo ln -s /etc/nginx/sites-available/safetravellers-api.conf \
           /etc/nginx/sites-enabled/
sudo nginx -t && sudo systemctl reload nginx
```

Issue a TLS certificate:

```bash
sudo certbot --nginx -d your.domain.com
```

Open the firewall:

```bash
sudo ufw allow 80
sudo ufw allow 443
```

---

## 7) Test all API endpoints

Replace `http://localhost:8080` with `https://your.domain.com` if Nginx/TLS
is configured.

**Health check (no auth required):**

```bash
curl http://localhost:8080/health
```

**Initialise FHE keys:**

```bash
curl -X POST http://localhost:8080/api/v1/init \
  -H "X-Api-Key: your-long-random-secret"
```

**Register biometrics:**

```bash
curl -X POST http://localhost:8080/api/v1/register \
  -H "X-Api-Key: your-long-random-secret" \
  -F "output_bucket=traveller1" \
  -F "files=@/path/to/bio1.txt" \
  -F "files=@/path/to/bio2.txt"
```

**Encrypt test biometrics (E-Gate):**

```bash
curl -X POST http://localhost:8080/api/v1/encrypt-bio \
  -H "X-Api-Key: your-long-random-secret" \
  -F "output_bucket=testsession1" \
  -F "files=@/path/to/testbio.txt"
```

**Verify identity:**

```bash
curl -X POST http://localhost:8080/api/v1/verify \
  -H "Content-Type: application/json" \
  -H "X-Api-Key: your-long-random-secret" \
  -d '{
    "threshold": 2000,
    "test_encrypted_folder": "data/E-Gate/Test_Bio_Enc/testsession1/<timestamp>/<file_enc_folder>",
    "stored_encrypted_folder": "data/CountryDB/Reg_Biometrics_Enc/traveller1/<timestamp>/<file_enc_folder>"
  }'
```

Note: Verify is a heavy HE operation — allow up to 30 minutes for a response.

---

## 8) Daily operations

| Task | Command |
|---|---|
| Start service | `docker compose up -d` |
| Stop service | `docker compose down` |
| Restart service | `docker compose restart` |
| View live logs | `docker compose logs -f` |
| Check container status | `docker compose ps` |
| Rebuild after project update | `docker compose up --build -d` |

Biometric data written by the service is stored in named Docker volumes and
persists across restarts and image rebuilds:

```bash
docker volume ls | grep safetravellers
```

---

## 9) Security checklist

- Use a long, random `SAFE_API_KEY` (minimum 32 characters)
- Do not commit `.env` to version control
- If using Nginx, expose only ports 80 and 443 to the internet; keep port 8080 local
- Keep the host OS and Docker engine updated (`sudo apt-get upgrade`)
- Back up the Docker volumes regularly:
  ```bash
  docker run --rm \
    -v safetravellers-countrydb:/data \
    -v /backups:/backup \
    ubuntu tar czf /backup/countrydb_$(date +%Y%m%d).tar.gz /data
  ```

