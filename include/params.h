
/**
 * @file params.h
 * @brief Header file containing parameter generation functions for TFHE and SEAL libraries.
 *
 * This file includes necessary headers and defines a structure for generating parameters
 * for TFHE and SEAL homomorphic encryption libraries.
 */

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

/**
 * @struct paramsGen
 * @brief A structure for generating parameters for TFHE and SEAL libraries.
 *
 * This structure contains static methods to generate parameters required for
 * homomorphic encryption using TFHE and SEAL libraries.
 */
struct paramsGen
{
    /**
     * @brief Generates parameters for TFHE library.
     *
     * This static method initializes and generates the necessary parameters
     * for using the TFHE homomorphic encryption library.
     */
    static void generateTFHEParams();

    /**
     * @brief Generates parameters for SEAL library.
     *
     * This static method initializes and generates the necessary parameters
     * for using the SEAL homomorphic encryption library.
     */
    static void generateSealParams();
};
