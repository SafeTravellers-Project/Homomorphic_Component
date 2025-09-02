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

//template<typename TORUS>
struct tfhe2seal
{
  // Transforms a TFHE ciphertext into a SEAL ciphertext given a model for a SEAL ciphertext as input
  static void ciphertext(seal::Ciphertext sealCipher, TLweSample  * tlweCipher, const seal::SEALContext &sealContext);

  // Transforms a TFHE secret key into a SEAL key
  static void secretKey(seal::SecretKey * sealKey, TLweKey  * tlweKey,const seal::SEALContext &sealContext);
};
//template struct tfhe2seal ;
//template struct tfhe2seal ;

//template<typename TORUS>
struct tfheMod
{
  static void tlwe2tgswKey(TGswKey  * tgswKey, TLweKey  * tlweKey);
};
//template struct tfheMod ;
//template struct tfheMod ;

struct FileEncrypt
{
  std::string xorEncryptDecrypt(const std::string& data, const std::string& key);

};

