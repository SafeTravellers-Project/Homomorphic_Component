#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 ]]; then
  echo "Usage: sudo bash deploy/scripts/install_service.sh <SERVICE_USER> <PROJECT_PATH>"
  echo "Example: sudo bash deploy/scripts/install_service.sh ubuntu /home/ubuntu/Project26Update1024_v4"
  exit 1
fi

SERVICE_USER="$1"
PROJECT_PATH="$2"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
TEMPLATE_FILE="${REPO_ROOT}/deploy/systemd/safetravellers-api.service"
TARGET_FILE="/etc/systemd/system/safetravellers-api.service"

if [[ ! -f "$TEMPLATE_FILE" ]]; then
  echo "Template not found: $TEMPLATE_FILE"
  exit 1
fi

if [[ ! -d "$PROJECT_PATH" ]]; then
  echo "Project path does not exist: $PROJECT_PATH"
  exit 1
fi

TMP_FILE="$(mktemp)"
trap 'rm -f "$TMP_FILE"' EXIT

sed -e "s|<SERVICE_USER>|${SERVICE_USER}|g" \
    -e "s|<PROJECT_PATH>|${PROJECT_PATH}|g" \
    "$TEMPLATE_FILE" > "$TMP_FILE"

cp "$TMP_FILE" "$TARGET_FILE"

systemctl daemon-reload
systemctl enable safetravellers-api
systemctl restart safetravellers-api
systemctl status safetravellers-api --no-pager

echo "Service installed and started: safetravellers-api"
