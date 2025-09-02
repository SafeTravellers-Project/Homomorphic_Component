#ifndef PRIV_BOOT_H
#define PRIV_BOOT_H

#include <algorithm>    // std::swap
#include <stdio.h>
#include <iostream>
#include <math.h>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"
// Utils
#include "clear-utils.h"
// Scaled TFHE
#include "scaled_tfhe.h"

//#include "scaled_tlwe_functions.h"

/**
* General private bootstrap functions
*/
//template<typename TORUS>
struct PBootFFT
{
  // Apply a bootstrapping given a test vector in TRLWE form
  static void extendedBoot(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      TLweSample* testvect,
      LweSample * lweIn);

  // Basically the extended boot but without the extraction at the end
  static void boot_wo_extract(TLweSample* tlweOut,
      LweBootstrappingKey * bk,
      TLweSample* testvect,
      LweSample * lweIn);

  static void boot_wo_extract(TLweSample* tlweOut,
      LweBootstrappingKeyFFT * bk,
      TLweSample* testvect,
      LweSample * lweIn);

  // This is actually just a copy of the one in the generic TFHE lib. Just to keep it all in the family here.
  static void MuxRotate_FFT(TLweSample* result,
    const TLweSample* accum,
    const TGswSampleFFT* bki,
    const int barai,
    const TGswParams* bk_params);

  // This is actually just a copy of the one in the generic TFHE lib. Just to keep it all in the family here.
  // NOT USED here because the swap command fails so using the original one
  static void blindRotate_FFT(TLweSample* accum,
    const TGswSampleFFT* bkFFT,
    const int* bara,
    const int n,
    const TGswParams* bk_params);

  // Same but with v a TRLWE encrypted test vecor
  static void blindRotateAndExtract_FFT(LweSample* result,
      TLweSample* v,
      const TGswSampleFFT* bkFFT,
      const int barb,
      const int* bara,
      const int n,
      const TGswParams* bk_params);
};

//template struct PBootFFT<Torus32>;
//template struct PBootFFT<Torus64>;

/**
* Half torus
* Here we apply a bootstrapping operation with an output testvector corresponding to
* [ f(-1/2), .. ,f(0), .. , f(1/2)]
* Therefore centered around 0.
*/
//template<typename TORUS>
struct HTPBootFFT
{
  // Bootstrapping with the extended sign function
  // where the result is 0 if negative and a fixed value "lweSign" if positive
  static void boot_Sign(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      TLweKeySwitchKey * key_switch_key,
      LweSample * lweIn,
      LweSample * lweSign);

};

//template struct SHTPBootFFT<Torus32>;
//template struct SHTPBootFFT<Torus64>;

#endif
