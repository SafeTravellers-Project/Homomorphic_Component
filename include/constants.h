#pragma once
#include <cstddef>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <memory>
#include <limits>
#include <experimental/filesystem>

// SEAL library headers
#include "seal/seal.h"

// TFHE library headers
#include "tfhe.h"

namespace Constants{
    // Precision for floating point operations
    const int PRECISION = 3;

    // Size of biometric vectors
    const int VECTOR_SIZE = 512;

    // Plaintext modulus for SEAL encryption
    const uint64_t PLAIN_MODULUS = 60000;     

    // Power of 10 array for precision calculations
    const long int POW10[12] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
    };
}
