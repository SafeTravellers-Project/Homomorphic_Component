# SAFETravellers API Wrapper

This wraps existing binaries as HTTP APIs:
- `HESysInit`
- `Register`
- `EncBio`
- `Verify`

## 1) Install Python deps

From project root:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r api/requirements.txt
```

## 2) Set environment

```bash
cp api/.env.example api/.env
# edit api/.env and set SAFE_API_KEY
export $(grep -v '^#' api/.env | xargs)
```

## 3) Start API server

```bash
cd api
python server.py
```

Server runs on `http://0.0.0.0:8080` by default.

## 4) Endpoints

All endpoints require header:

`x-api-key: <SAFE_API_KEY>`

### Health

- `GET /health`

### System Initialization

- `POST /api/v1/init`

### Register Biometrics (encrypt + store)

- `POST /api/v1/register`
- Form-data fields:
  - `files`: one or more biometric files
  - `output_bucket`: optional label (default `default`)

### Encrypt Test Biometrics

- `POST /api/v1/encrypt-bio`
- Form-data fields:
  - `files`: one or more biometric files
  - `output_bucket`: optional label (default `default`)

### Verify Identity

- `POST /api/v1/verify`
- JSON body:

```json
{
  "threshold": 2000,
  "test_encrypted_folder": "data/E-Gate/Test_Bio_Enc/default/20260313_120000/fileA_enc",
  "stored_encrypted_folder": "data/CountryDB/Reg_Biometrics_Enc/default/20260313_110000/fileA_enc"
}
```

## 5) Linux server exposure (simple production)

Use Nginx as reverse proxy and TLS.

- Run API only on localhost by setting `SAFE_API_HOST=127.0.0.1`
- Expose only Nginx on ports 80/443
- Keep `SAFE_API_KEY` secret

### Nginx config from this repo

1. Copy the provided config:

```bash
sudo cp deploy/nginx/safetravellers-api.conf /etc/nginx/sites-available/safetravellers-api.conf
```

2. Replace `<DOMAIN_NAME>` in `/etc/nginx/sites-available/safetravellers-api.conf`

3. Enable site and test Nginx:

```bash
sudo ln -s /etc/nginx/sites-available/safetravellers-api.conf /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

4. Issue TLS cert (Let's Encrypt):

```bash
sudo apt install -y certbot python3-certbot-nginx
sudo certbot --nginx -d <DOMAIN_NAME>
```

5. (Optional) Add rate limit globally in `/etc/nginx/nginx.conf` inside `http { ... }`:

```nginx
limit_req_zone $binary_remote_addr zone=safe_api_limit:10m rate=20r/m;
```

Then inside `location /` in site config:

```nginx
limit_req zone=safe_api_limit burst=20 nodelay;
```

### Example systemd service

Create `/etc/systemd/system/safetravellers-api.service`:

```ini
[Unit]
Description=SAFETravellers API
After=network.target

[Service]
Type=simple
User=ubuntu
WorkingDirectory=/path/to/Project26Update1024_v4/api
EnvironmentFile=/path/to/Project26Update1024_v4/api/.env
ExecStart=/path/to/Project26Update1024_v4/.venv/bin/python server.py
Restart=always

[Install]
WantedBy=multi-user.target
```

Then:

```bash
sudo systemctl daemon-reload
sudo systemctl enable safetravellers-api
sudo systemctl start safetravellers-api
sudo systemctl status safetravellers-api
```

### Use service template from this repo

1. Copy template:

```bash
sudo cp deploy/systemd/safetravellers-api.service /etc/systemd/system/safetravellers-api.service
```

2. Edit placeholders in `/etc/systemd/system/safetravellers-api.service`:

- `<SERVICE_USER>`: Linux user that owns project files
- `<PROJECT_PATH>`: absolute path to your repo root

3. Reload and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable safetravellers-api
sudo systemctl restart safetravellers-api
sudo systemctl status safetravellers-api
```

4. Check logs:

```bash
sudo journalctl -u safetravellers-api -f
```

## Notes

- The binaries are executed with working directory set to `bin`, matching your relative data paths.
- For heavy traffic, move long-running calls to a job queue.
