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
struct dataIO
{
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

//  static void readVoxPlaintext_arr(seal::Plaintext  plaintext[], std::string path, int size, int precision, uint64_t modulus, int invert);
  //static void readVoxPlaintext_arr1(seal::Plaintext * plaintext, std::string path, int size, int precision, uint64_t modulus, int invert);
  //static void readVoxPlaintextSquare_arr(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus);

};

