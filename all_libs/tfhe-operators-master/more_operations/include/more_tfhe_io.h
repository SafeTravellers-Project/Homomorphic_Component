#ifndef MORE_TFHE_IO_H
#define MORE_TFHE_IO_H

#include <stdio.h>
#include <iostream>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"

// Creating this here even though ideally it should be in the TFHE lib..
const int32_t PUBLIC_LWE_KEY_TYPE_UID = 44;

//template<typename TORUS>
struct MoreTfheIO
{

  /*
  * Read and write LWE parameters
  */

  //static LweParams * new_LWE_Params_fromFile(FILE* f);

  //static void write_LWE_Params_toFile(const LweParams* tlwe_params, FILE* f);

  /*
  * Read and write LWE public parameters (same but with an m on top for the public encryption)
  */

//  static LwePublicParams * new_LWE_PublicParams_fromFile(FILE * f);

  //static void write_PublicLWE_Params_toFile(const LwePublicParams* lwe_public_params, FILE * f);

  /*
  * Read and write TGSW parameters
  * (the other parameters can be extracted from these)
  */

//  static TGswParams * new_TGSW_Params_fromFile(FILE* f);

//  static void write_TGSW_Params_toFile(const TGswParams * trgsw_params, FILE* f);

  /*
  * Read and write the TGSW key
  * (the other keys can be extracted from these)
  */

//  static TGswKey * new_TGSW_Key_fromFile(FILE* f);

//  static void write_TGSW_Key_toFile(const TGswKey * trgsw_key, FILE* f);

  /*
  * Read and write the LWE key
  */

//  static LweKey* new_LWE_Key_fromFile(FILE* f);

//  static void write_LWE_Key_toFile(const LweKey* lwe_key, FILE* f);

  /*
  * Read and write the LWE public key
  */

//  static void write_LWE_PublicKey_toFile(const LwePublicKey * lwe_public_key, FILE* f);

//  static LwePublicKey * new_LWE_PublicKey_fromFile(FILE* f, const LwePublicParams * params);

  /*
  * Read and write the bootstrapping key (non-FFT)
  */

//  static LweBootstrappingKey * new_Bootstrapping_Key_fromFile(FILE* f, const LweParams* in_out_params, const TGswParams* bk_params);

//  static void write_Bootstrapping_Key_toFile(LweBootstrappingKey * boot_key, FILE * f);

  /*
  * Read and write a single TLWE sample
  */

//  static void read_TLWE_sample_fromFile(FILE* f, TLweSample * tlweSample, const TLweParams * tlwe_params);

//  static void write_TLWE_sample_toFile(TLweSample * tlweSample, const TLweParams * tlwe_params, FILE* f);

  /*
  * Read and write a single LWE sample
  */

//  static LweSample * new_LWE_sample_fromFile(FILE* f, const LweParams * lwe_params);

//  static void write_LWE_sample_toFile(LweSample * lweSample, const LweParams * lwe_params, FILE* f);

  /*
  * Read and write an array of LWE samples
  */

  static LweSample * new_LWE_array_fromFile(FILE* f, const LweParams * lwe_params, int array_size);

  static void write_LWE_array_toFile(LweSample * lweSample, int array_size, const LweParams * lwe_params, FILE* f);

  /*
  * Read and write a single TGSW sample (non-FFT)
  */

//  static void read_TGSW_sample_fromFile(FILE* f, TGswSample * tgswSample, const TGswParams * tgsw_params);

//  static void write_TGSW_sample_toFile(TGswSample * tgswSample, const TGswParams * tgsw_params, FILE* f);

};

//template struct MoreTfheIO<Torus32>;
//template struct MoreTfheIO<Torus64>;

#endif
