# SAFETravellers: Homomorphic Encryption-Based Secure Biometrics Registration & Verification

## Overview

SAFETravellers is a C++ project for secure registration and verification of biometric data using homomorphic encryption. It leverages [Microsoft SEAL](https://github.com/microsoft/SEAL) and [TFHE](https://github.com/tfhe/tfhe) libraries to enable privacy-preserving operations on encrypted data.

## Features

- **System Parameter Generation:** Generates cryptographic parameters for SEAL and TFHE.
- **Key Generation:** Creates encryption keys for system and users.
- **Biometric Registration:** Reads, encrypts, and registers biometric embeddings.
- **Biometric Verification:** Supports secure verification using encrypted data.
- **Self-contained:** Can be configured to use local builds of SEAL, TFHE, and OpenSSL.

## Project Structure

```
Project25Update/
├── all_libs/           # Local libraries (SEAL, TFHE, OpenSSL, TFHE_operations_masters)
├── bin/                # Compiled executables
├── data/               # Input/output data files
├── build/              # MakeFile
├── include/            # Header files
├── src/                # Source code
│   ├── HESysInit.cpp   # System parameter and key generation
│   ├── Register.cpp    # Biometric registration
│   ├── Verify.cpp      # Biometric verification
│   └── ...             # Other source files
├── CMakeLists.txt      # Build configuration  (Change line 5 of this file to represent the project library at destination)
└── README.md           # Project documentation
```
## Build Instructions

1. **Clone the repository** and ensure all required libraries (SEAL, TFHE, OpenSSL & TFHE_operations_masters) are present in `all_libs/`.
2. **Build the project** using CMake(Version used 3.28.3):

    ```bash
    cd build
    cmake ..
    make
    ```

3. **Run the executables** from the `bin/` directory:

    ```bash
    ./bin/HESysInit      # Generate system parameters and keys
    ./bin/Register       # Register user biometrics
    ./bin/Verify         # Verify biometrics
    ```
## Dependencies

- [Microsoft SEAL](https://github.com/microsoft/SEAL)
- [TFHE](https://github.com/tfhe/tfhe)
- [OpenSSL](https://www.openssl.org/)

All dependencies can be built locally and referenced in the `CMakeLists.txt`.

## Usage

- **HESysInit:** Initializes system parameters and keys.
- **Register:** Encrypts and registers user biometric data.
- **Verify:** Verifies user biometrics using encrypted data.

## Notes

- Ensure all data files and keys are in the correct paths as expected by the executables.
- For portability, use absolute paths in `CMakeLists.txt` and avoid using `~`.
- In case there are issues with OpenSSL, we encourage to install OpenSSL into the all_libs folder.
- For the moment, both the result and cosine similarity values are being decrypted by the key of the HE component. Change this if need be. 

## License

This project is for academic and research purposes. See individual library licenses for SEAL, TFHE, and OpenSSL.

---

**Contact:**  
For questions or contributions, please contact Dr. Olive Chakraborty (Research Scientist, CEA).
