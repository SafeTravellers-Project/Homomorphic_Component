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

// SEAL
#include "seal/seal.h"

// TFHE

#include "tfhe.h"

//template<typename TORUS>

struct VectorValidation {
    bool is_valid;
    float raw_norm;
    float max_component_scaled;
    float square_sum_scaled;
    std::string error_msg;
};

struct dataIO
{
  static VectorValidation validateAndNormalize(
    std::vector<float>& vec,        // in/out: modified in place if needed
    int precision,
    uint32_t modulus);
  static std::vector<float> loadRawVector(std::string path, int size);
  static void readPlaintext_Int_woMod(seal::Plaintext * plaintext, std::string path, uint32_t modulus);
  
  static void makePlaintext(seal::Plaintext * plaintext, int32_t value, int size, uint32_t modulus);


  // Here precision counts the number of digits we take into account.
  // Therefore 1 gives all zeros because the values are all |~| < 1
  // invert = 0 means the values are written in the right order
  // invert = 1 means the values are written in the opposite order
  static void readPlaintext(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert);
  //static void readVoxPlaintext(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert,float coff_arr[]);

  static void readPlaintextCosine(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert);
  // Read a sum of the squares of all the coefficients into the X^(p-1) coefficient
  static void readPlaintextSquare(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus);
  
  static void addPlaintext(seal::Plaintext * plaintext1, seal::Plaintext * plaintext2,seal::Plaintext * destination, int size, uint32_t modulus);
  static void mulPlaintext(seal::Plaintext * plaintext1, seal::Plaintext * plaintext2,seal::Plaintext * destination, int size, uint32_t modulus);
  static void readPlaintext_arr(seal::Plaintext * plaintext, std::string path, uint32_t modulus);

  static void EncryptF2F(std::string path_to, std::string path_f, seal::Encryptor & encryptor, int vector_size, int precision, uint32_t modulus, int invert); // the invert parameter is to read the coefficients in the reverse order

  static void Read_F(std::string path_to, seal::SEALContext &context,seal::Ciphertext &outCipher,seal::Ciphertext &outCipherSquare,seal::Ciphertext &outCosine);
};

