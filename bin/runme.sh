#!/bin/bash
# SAFETravellers build and run script

# Resolve script location so this works from any current directory.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Use project-local shared libraries first to avoid accidentally loading
# incompatible system TFHE libraries from /usr/local/lib.
LOCAL_LIB_PATHS="$PROJECT_ROOT/all_libs/tfhe/build/libtfhe:$PROJECT_ROOT/all_libs/tfhe-operators-master/new_tfhe/build/lib:$PROJECT_ROOT/all_libs/tfhe-operators-master/more_operations/build/lib:$PROJECT_ROOT/all_libs/openssl-3.0.14/install/lib64"
if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$LOCAL_LIB_PATHS:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$LOCAL_LIB_PATHS"
fi

EXPECTED_TFHE_PATH="$PROJECT_ROOT/all_libs/tfhe/build/libtfhe"

check_tfhe_runtime() {
	local bin="$1"
	local line
	line=$(ldd "$bin" | grep -E 'libtfhe-spqlios-avx\.so' || true)
	if [ -z "$line" ]; then
		echo "ERROR: Could not resolve libtfhe-spqlios-avx.so for $bin" >&2
		exit 1
	fi
	if [[ "$line" != *"$EXPECTED_TFHE_PATH"* ]]; then
		echo "ERROR: $bin is not using project-local TFHE." >&2
		echo "Resolved: $line" >&2
		echo "Expected path to contain: $EXPECTED_TFHE_PATH" >&2
		exit 1
	fi
}

echo "=== Running SysInit ==="
check_tfhe_runtime "./HESysInit"
./HESysInit

echo "=== Running Register ==="
check_tfhe_runtime "./Register"
./Register ../data/Traveller/Reg_Bio ../data/CountryDB/Reg_Biometrics_Enc

echo "=== Running Enc_Test_Bio ==="
check_tfhe_runtime "./EncBio"
./EncBio ../data/E-Gate/Test_Bio/ ../data/E-Gate/Test_Bio_Enc
