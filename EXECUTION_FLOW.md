# Execution Flow Guide

This document provides a comprehensive guide on the execution flow for the Homomorphic Encryption Biometric Verification System.

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Execution Sequence](#execution-sequence)
4. [Detailed Step-by-Step Guide](#detailed-step-by-step-guide)
5. [Command Reference](#command-reference)
6. [Troubleshooting](#troubleshooting)
7. [Advanced Usage](#advanced-usage)

---

## Overview

The system consists of **4 main executables** that must be run in a specific order:

```
1. HESysInit   →  2. Register  →  3. EncBio  →  4. Verify
   (Setup)         (Enroll)        (Capture)      (Authenticate)
```

Each executable has a specific role and depends on outputs from previous steps.

---

## Prerequisites

### System Requirements
- **OS**: Linux (tested on Ubuntu 20.04+)
- **RAM**: Minimum 2 GB available
- **Disk Space**: Minimum 600 MB free
- **CPU**: Modern x86-64 processor with AVX support

### Software Dependencies
- CMake 3.28.3 or higher
- C++17 compatible compiler (g++ 9.0+)
- SEAL library (included in `all_libs/SEAL/`)
- TFHE library (included in `all_libs/tfhe-4096/`)
- OpenSSL 3.0+ (included in `all_libs/openssl-3.0.14/`)

### Initial Setup
```bash
# Navigate to project root
cd /path/to/Project25Update

# Build the project
cd build
cmake ..
make

# Verify executables are created
ls -lh ../bin/
# Should show: HESysInit, Register, EncBio, Verify
```

---

## Execution Sequence

### Complete Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    SYSTEM INITIALIZATION                         │
│                                                                  │
│  Step 1: ./HESysInit                                            │
│  └─> Generates cryptographic parameters and keys                │
│      • SEAL parameters (BFV scheme)                             │
│      • TFHE parameters (LWE, TLWE)                              │
│      • Public/Private key pairs                                 │
│      • Bootstrapping keys                                       │
└─────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    BIOMETRIC REGISTRATION                        │
│                                                                  │
│  Step 2: ./Register " <Input Registration Biometric folder> "  " <Output Registration Biometric folder> "                      │
│  └─> Encrypts and stores registered biometric template          │
│      • Reads raw biometric (1024 float values)                  │
│      • Encrypts using SEAL public key                           │
│      • Stores in CountryDB/Reg_Biometrics_Enc/                  │
└─────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    TEST BIOMETRIC ENCRYPTION                     │
│                                                                  │
│  Step 3: ./EncBio " <Input Raw Test Biometric file>"  " Output Encrypted Test Biometric folder>                         │
│  └─> Encrypts test biometric for verification                   │
│      • Reads test biometric (1024 float values)                 │
│      • Encrypts using SEAL public key                           │
│      • Stores in E-Gate/Test_Biometrics_Enc/                    │
└─────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    HOMOMORPHIC VERIFICATION                      │
│                                                                  │
│  Step 4: ./Verify <threshold>  <Test Biometric folder> " " <Stored Biometric folder>
│  └─> Compares encrypted biometrics homomorphically              │
│      • Computes Euclidean distance (encrypted)                  │
│      • Computes cosine similarity (encrypted)                   │
│      • Threshold comparison (using TFHE)                        │
│      • Outputs: Accept/Reject decision                          |
│      • Outputs: Cosine Similarity distance                      |
└─────────────────────────────────────────────────────────────────┘
```

---

**All the folder paths to be provided must be given in Aboslute folder path.**



## Detailed Step-by-Step Guide

### Step 1: System Initialization

**Purpose**: Generate all cryptographic parameters and keys required by the system.

**Command**:
```bash
cd bin
./HESysInit
```

**Arguments**: None

**Expected Output**:
```
Generating SEAL parameters...
Generating TFHE parameters...
Generating HE keys...
Generating E-Gate keys...
********************************************
* Successfully Generated System_Parameters *
********************************************
Time for System Parameter generation: X.XXX seconds
```

**Files Generated**:
```
data/System_Parameters/
├── sealparam.txt
├── params_lwe_in.txt
├── params_lwe_out.txt
├── params_tlwe_in.txt
└── params_tlwe_out.txt

data/Traveller/
├── sealpubK.txt
├── sealpvtK.txt
├── sealrelinK.txt
├── lwe_in.txt
├── tlwe_in.txt
└── KSKmed.data

data/HEComp/
├── tgsw_out1.txt
└── lwe_out1.txt

data/E-Gate/
└── lwe_out2.txt
```

**Execution Time**: ~30-60 seconds (mostly for key generation)

**Critical Notes**:
- Run this ONLY ONCE at system setup
- Running again will overwrite all keys (making existing encrypted data unreadable)
- Largest file: `KSKmed.data` (~520 MB)

**Help**:
```bash
./HESysInit --help
```

---

### Step 2: Biometric Registration

**Purpose**: Register (enroll) a user's biometric template into the system's database.

**Command**:
```bash
./Register <path_to_raw_biometric_file>
```

**Arguments**:
| Argument | Type | Description | Example |
|----------|------|-------------|---------|
| `raw_biometric_file` | File path | Path to raw biometric template file (1024 floats) | `../data/CountryDB/Test_Biometrics1` |

**Example**:
```bash
./Register ../data/CountryDB/Test_Biometrics1
```

**Expected Output**:
```
Generating User Keys...
Reading SEAL parameters from file...
Encrypting biometric template...
Writing encrypted data to database...
Registration completed successfully!
```

**Files Generated**:
```
data/CountryDB/Reg_Biometrics_Enc/
├── ciph         # Encrypted biometric vector
├── sqciph       # Encrypted squared values
└── cosineciph   # Encrypted cosine components
```

**Input File Format** (raw biometric):
```
# Text file with 1024 floating-point values (one per line)
0.123456
0.789012
0.345678
...
(1024 total values)
```

**Execution Time**: ~2-5 seconds

**Help**:
```bash
./Register --help
```

---

### Step 3: Test Biometric Encryption

**Purpose**: Encrypt a test biometric sample for verification (simulates E-Gate capture).

**Command**:
```bash
./EncBio <path_to_test_biometric_file>
```

**Arguments**:
| Argument | Type | Description | Example |
|----------|------|-------------|---------|
| `test_biometric_file` | File path | Path to test biometric file (1024 floats) | `../data/E-Gate/Example\ Tests/Test_Biometrics2` |

**Example** (matching biometric):
```bash
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1
```

**Example** (non-matching biometric):
```bash
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics2
```

**Expected Output**:
```
Reading SEAL parameters from file...
Encrypting test biometric...
Writing encrypted test data...
Test biometric encryption completed!
```

**Files Generated**:
```
data/E-Gate/Test_Biometrics_Enc/
├── ciph         # Encrypted test vector
├── sqciph       # Encrypted squared values
└── cosineciph   # Encrypted cosine components
```

**Execution Time**: ~2-5 seconds

**Help**:
```bash
./EncBio --help
```

---

### Step 4: Homomorphic Verification

**Purpose**: Perform biometric verification by comparing encrypted test and registered biometrics.

**Command**:
```bash
./Verify <threshold> <test_folder> <registered_folder>
```

**Arguments**:
| Argument | Type | Description | Example |
|----------|------|-------------|---------|
| `threshold` | Integer | Threshold value for verification decision | `2000` |
| `test_folder` | Folder name | Name of folder containing encrypted test biometric (in `data/E-Gate/`) | `Test_Biometrics_Enc` |
| `registered_folder` | Folder name | Name of folder containing encrypted registered biometric (in `data/CountryDB/`) | `Reg_Biometrics_Enc` |

**Example**:
```bash
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
```

**Expected Output** (Match):
```
Threshold value taken in, set to 2000
 **************************************************************
 Result given as -> Accept/Reject decision || Cosine Similarity Value 
Accept || 0
***************************************************************************
Total time for accept/reject check: 0.003069 seconds
Cosine computation duration: 0.002271 seconds
```

**Expected Output** (No Match):
```
Threshold value taken in, set to 2000
 **************************************************************
 Result given as -> Accept/Reject decision || Cosine Similarity Value 
Reject || XXXX
***************************************************************************
Total time for accept/reject check: 0.003XXX seconds
Cosine computation duration: 0.002XXX seconds
```

**Threshold Guidelines**:
- **Lower threshold (e.g., 1000)**: More strict matching, fewer false accepts, more false rejects
- **Higher threshold (e.g., 3000)**: More lenient matching, fewer false rejects, more false accepts
- **Recommended range**: 1500 - 2500
- **Default**: 2000

**Execution Time**: ~3-10 seconds (depends on threshold computation complexity)

**Help**:
```bash
./Verify --help
```

---

## Command Reference

### Quick Reference Table

| Executable | Required Args | Optional Flags | Dependencies | Output Location |
|------------|--------------|----------------|--------------|-----------------|
| `HESysInit` | None | `--help`, `-h` | None | `data/System_Parameters/`, `data/Traveller/`, `data/HEComp/`, `data/E-Gate/` |
| `Register` | `<biometric_file>` | `--help`, `-h` | HESysInit | `data/CountryDB/Reg_Biometrics_Enc/` |
| `EncBio` | `<test_file>` | `--help`, `-h` | HESysInit | `data/E-Gate/Test_Biometrics_Enc/` |
| `Verify` | `<threshold>` `<test_folder>` `<reg_folder>` | `--help`, `-h` | HESysInit, Register, EncBio | Console output |

### Common Command Patterns

**Complete Fresh Run**:
```bash
cd bin

# Step 1: Initialize system (run once)
./HESysInit

# Step 2: Register biometric
./Register ../data/CountryDB/Test_Biometrics1

# Step 3: Encrypt test biometric
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1

# Step 4: Verify
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
```

**Test with Different Biometrics**:
```bash
# Register first person
./Register ../data/CountryDB/Test_Biometrics1

# Test with same person (should Accept)
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc

# Test with different person (should Reject)
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics2
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
```

**Test Different Thresholds**:
```bash
# Same encrypted data, different thresholds
./Verify 1000 Test_Biometrics_Enc Reg_Biometrics_Enc  # Strict
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc  # Medium
./Verify 3000 Test_Biometrics_Enc Reg_Biometrics_Enc  # Lenient
```

---

## Troubleshooting

### Error: "Cannot open file"

**Cause**: Required input files missing

**Solution**:
```bash
# Check if HESysInit was run
ls -l data/System_Parameters/sealparam.txt

# If missing, run HESysInit
./HESysInit
```

### Error: "Usage: ./Register <biometric_file>"

**Cause**: Missing command-line argument

**Solution**:
```bash
# Correct usage
./Register ../data/CountryDB/Test_Biometrics1

# Or get help
./Register --help
```

### Error: "Segmentation fault" or Crash

**Cause**: Corrupted data files or mismatched keys

**Solution**:
```bash
# Clean all generated data
cd ../data
rm -f System_Parameters/* Traveller/* HEComp/* E-Gate/lwe_out2.txt
rm -rf CountryDB/Reg_Biometrics_Enc/* E-Gate/Test_Biometrics_Enc/*

# Re-initialize system
cd ../bin
./HESysInit

# Re-run registration and testing
./Register ../data/CountryDB/Test_Biometrics1
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
```

### Unexpected Verification Results

**Symptom**: Always "Accept" or always "Reject" regardless of biometric

**Possible Causes & Solutions**:

1. **Threshold too high/low**:
   ```bash
   # Try different thresholds
   ./Verify 1500 Test_Biometrics_Enc Reg_Biometrics_Enc
   ./Verify 2500 Test_Biometrics_Enc Reg_Biometrics_Enc
   ```

2. **Using same test file for registration and testing**:
   ```bash
   # Register with one file
   ./Register ../data/CountryDB/Test_Biometrics1
   
   # Test with a DIFFERENT file
   ./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics2
   ```

3. **Corrupted encrypted data**:
   ```bash
   # Re-run encryption steps
   ./Register ../data/CountryDB/Test_Biometrics1
   ./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1
   ./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
   ```

---

## Advanced Usage

### Batch Testing Multiple Biometrics

```bash
#!/bin/bash
# Script to test multiple biometric pairs

cd bin

# Initialize once
./HESysInit

# Test multiple scenarios
for bio in Test_Biometrics1 Test_Biometrics2; do
    echo "===== Testing with $bio ====="
    ./Register ../data/CountryDB/$bio
    ./EncBio ../data/E-Gate/Example\ Tests/$bio
    ./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
    echo ""
done
```

### Performance Benchmarking

```bash
#!/bin/bash
# Benchmark execution times

cd bin

echo "Benchmarking HESysInit..."
time ./HESysInit

echo "Benchmarking Register..."
time ./Register ../data/CountryDB/Test_Biometrics1

echo "Benchmarking EncBio..."
time ./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1

echo "Benchmarking Verify..."
time ./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc
```

### Threshold Sensitivity Analysis

```bash
#!/bin/bash
# Test verification accuracy across thresholds

cd bin

# Setup
./HESysInit
./Register ../data/CountryDB/Test_Biometrics1
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1

# Test thresholds
for threshold in 500 1000 1500 2000 2500 3000 3500; do
    echo "===== Threshold: $threshold ====="
    ./Verify $threshold Test_Biometrics_Enc Reg_Biometrics_Enc
    echo ""
done
```

---

## Best Practices

1. **Always run HESysInit first** - No other executable will work without it
2. **Keep backups** of System_Parameters and Traveller keys
3. **Use --help** if unsure about arguments
4. **Test with known matching/non-matching pairs** to validate system
5. **Monitor disk space** - Initial setup requires ~600 MB
6. **Run from bin/ directory** - Executables expect relative paths from there
7. **Clean between major tests** - Prevents data conflicts
8. **Document threshold values** used in production

---

## Production Deployment Considerations

For real-world deployment:

1. **Separate key management**: Move Traveller keys to secure key storage
2. **Database backend**: Replace file-based storage with proper database
3. **API wrapper**: Create REST API around executables
4. **Logging**: Add comprehensive logging for auditing
5. **Error handling**: Enhance error messages for end users
6. **Key rotation**: Implement periodic key regeneration
7. **Distributed architecture**: Deploy components separately (E-Gate, HEComp, DB)
8. **Load balancing**: Handle multiple simultaneous verifications

---

## Summary

**Minimum Working Flow**:
```bash
cd bin
./HESysInit                                          # Run once
./Register ../data/CountryDB/Test_Biometrics1       # Per user
./EncBio ../data/E-Gate/Example\ Tests/Test_Biometrics1  # Per verification
./Verify 2000 Test_Biometrics_Enc Reg_Biometrics_Enc     # Per verification
```

**Key Points**:
- ✅ Always run in order: HESysInit → Register → EncBio → Verify
- ✅ HESysInit runs ONCE, others run as needed
- ✅ Use --help for any executable to see usage
- ✅ Threshold 2000 is a good default for testing
- ✅ Run from bin/ directory for correct relative paths
