#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

LOCAL_LIB_PATHS="$PROJECT_ROOT/all_libs/tfhe/build/libtfhe:$PROJECT_ROOT/all_libs/tfhe-operators-master/new_tfhe/build/lib:$PROJECT_ROOT/all_libs/tfhe-operators-master/more_operations/build/lib:$PROJECT_ROOT/all_libs/openssl-3.0.14/install/lib64"

# Prevent globally exported /usr/local/lib from overriding project TFHE.
export LD_LIBRARY_PATH="$LOCAL_LIB_PATHS"

EXPECTED_TFHE_PATH="$PROJECT_ROOT/all_libs/tfhe/build/libtfhe"
TFHE_LINE=$(ldd "$SCRIPT_DIR/Verify" | grep -E 'libtfhe-spqlios-avx\.so' || true)
if [ -z "$TFHE_LINE" ]; then
	echo "ERROR: Could not resolve libtfhe-spqlios-avx.so for Verify" >&2
	exit 1
fi
if [[ "$TFHE_LINE" != *"$EXPECTED_TFHE_PATH"* ]]; then
	echo "ERROR: Verify is not using project-local TFHE." >&2
	echo "Resolved: $TFHE_LINE" >&2
	echo "Expected path to contain: $EXPECTED_TFHE_PATH" >&2
	exit 1
fi

exec "$SCRIPT_DIR/Verify" "$@"

