#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: sudo bash deploy/scripts/install_nginx.sh <DOMAIN_NAME> [CERTBOT_EMAIL]"
  echo "Example: sudo bash deploy/scripts/install_nginx.sh api.example.com admin@example.com"
  exit 1
fi

DOMAIN_NAME="$1"
CERTBOT_EMAIL="${2:-}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
TEMPLATE_FILE="${REPO_ROOT}/deploy/nginx/safetravellers-api.conf"
TARGET_FILE="/etc/nginx/sites-available/safetravellers-api.conf"
ENABLED_FILE="/etc/nginx/sites-enabled/safetravellers-api.conf"

if [[ ! -f "$TEMPLATE_FILE" ]]; then
  echo "Nginx template not found: $TEMPLATE_FILE"
  exit 1
fi

if [[ -z "$DOMAIN_NAME" ]]; then
  echo "Domain cannot be empty"
  exit 1
fi

TMP_FILE="$(mktemp)"
trap 'rm -f "$TMP_FILE"' EXIT

sed "s|<DOMAIN_NAME>|${DOMAIN_NAME}|g" "$TEMPLATE_FILE" > "$TMP_FILE"
cp "$TMP_FILE" "$TARGET_FILE"

if [[ ! -e "$ENABLED_FILE" ]]; then
  ln -s "$TARGET_FILE" "$ENABLED_FILE"
fi

nginx -t
systemctl reload nginx

echo "Nginx config installed for domain: $DOMAIN_NAME"

if [[ -n "$CERTBOT_EMAIL" ]]; then
  certbot --nginx -d "$DOMAIN_NAME" --agree-tos --email "$CERTBOT_EMAIL" --non-interactive
  echo "TLS certificate installed via certbot for: $DOMAIN_NAME"
else
  echo "Skipped certbot step (no email provided)."
  echo "Run manually when ready: sudo certbot --nginx -d $DOMAIN_NAME"
fi
