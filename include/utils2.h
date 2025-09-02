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

// SEAL
#include "seal/seal.h"

// TFHE

#include "tfhe.h"

//template<typename TORUS>
struct seal2tfhe
{
  // Transforms a SEAL ciphertext into a TFHE ciphertext
  static void ciphertext_new(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext );

  static void ciphertext_seal_exit(TLweSample  * tlweCipher, seal::Ciphertext sealCipher, const seal::SEALContext &sealcontext );

  // Transforms a SEAL secret key into a TFHE key
  static void secretKey(TLweKey  * tlweKey, seal::SecretKey sealKey, const seal::SEALContext &sealcontext);

  static void secretTGSWKey(TGswKey  * tgswKey, seal::SecretKey sealKey, const seal::SEALContext &sealContext);
};
//template struct seal2tfhe ;
//template struct seal2tfhe ;


