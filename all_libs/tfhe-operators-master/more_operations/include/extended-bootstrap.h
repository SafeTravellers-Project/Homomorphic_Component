#ifndef EXT_BOOT_H
#define EXT_BOOT_H

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
// 64 bits
//#include "64bit-functions.h"
// Utils
#include "clear-utils.h"
// Scaled TFHE
#include "scaled_tfhe.h"

/**
* Any torus
* Some utilities for extended bootstraps
*/
//template<typename TORUS>
struct ATBootFFT
{
  // Apply a bootstrapping given a test vector
  static void extendedBoot(LweSample * lweOut,
      LweBootstrappingKey * bk,
      TorusPolynomial* testvect,
      LweSample * lweIn);

  static void extendedBoot(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      TorusPolynomial* testvect,
      LweSample * lweIn);

// Apply a bootstrapping given a test vector and extract a certain coefficient
static void extendedBoot(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    TorusPolynomial* testvect,
    LweSample * lweIn,
  int extraction_index);

  // Basically the extended boot but without the extraction
  static void boot_wo_extract(TLweSample* tlweOut,
      LweBootstrappingKey * bk,
      TorusPolynomial* testvect,
      LweSample * lweIn);
  static void boot_wo_extract(TLweSample* tlweOut,
      LweBootstrappingKeyFFT * bk,
      TorusPolynomial* testvect,
      LweSample * lweIn);
};

//template struct ATBootFFT<Torus32>;
//template struct ATBootFFT<Torus64>;

/**
* Full torus
* Here we apply a bootstrapping operation with an output testvector corresponding to
* [ f(0), .. , f(1/2)]
*/
//template<typename TORUS>
struct FTBootFFT
{
  // For a given f:[0,1/2]->[-1/4,1/4], do g_f(x) + g_f(-x)
  // Important: do not use with a function with an image [-1/2, 1/2], it will overflow
  static void boot_inverted_sum(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      TorusPolynomial* testvect,
      LweSample * lweIn);

  // Bootstrapping with the Id function
  static void boot_Id(LweSample * lweOut,
      LweBootstrappingKeyFFT* bk,
      LweSample * lweIn);

  // Bootstrapping with the Id/2 function
  static void boot_Id_over2(LweSample* lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  // Bootstrapping with the -Id/2 function
  static void boot_NegId_over2(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  // Bootstrapping with the x^e function
  static void boot_powerOf(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      int exponent);

  // Bootstrapping with the symmetric sigmoid function
  static void boot_Sigmoid(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double modulus);

  // The bit boot: outputs a value "doubleOut" if input is positive and 0 if not
  static void bitBoot(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double doubleOut);

  // The heaviside boot: outputs a value "doubleOut" if input is positive and 0 if not
  static void boot_Heaviside(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double doubleOut);

  // The sign boot: outputs a value "positiveOut" if input is positive and "-positiveOut" if not

  static void boot_Sign(LweSample * lweOut,
      LweBootstrappingKey * bk,
      LweSample * lweIn,
      double positiveOut);

  static void boot_Sign(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double positiveOut);
};

//template struct FTBootFFT<Torus32>;
//template struct FTBootFFT<Torus64>;

/**
* Half torus
* Here we apply a bootstrapping operation with an output testvector corresponding to
* [ f(-1/2), .. ,f(0), .. , f(1/2)]
* Therefore centered around 0.
*/
//template<typename TORUS>
struct HTBootFFT
{
  // Bootstrapping with the Id function
  static void boot_Id(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  // Bootstrapping with the absolute value function
  static void boot_Abs(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  // Bootstrapping with the x^e function
  static void boot_powerOf(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      int exponent);

  // Bootstrapping with the symmetric sigmoid function
  static void boot_Sigmoid(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double modulus);
};

//template struct HTBootFFT<Torus32>;
//template struct HTBootFFT<Torus64>;


/**
* Half torus
* Here we apply a bootstrapping operation with an output testvector corresponding to
* [ f(-1/2), .. ,f(0), .. , f(1/2)]
* Therefore centered around 0.
*/
//template<typename TORUS>
struct SHTBootFFT
{
  // Bootstrapping with the Id function
  static void boot_Id(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double rescale);

    // Bootstrapping with the Relu function
static void boot_Relu(LweSample * lweOut,
  LweBootstrappingKeyFFT * bk,
  LweSample * lweIn,
  double rescale);


  // Bootstrapping with the Id function
  static void boot_Mul(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * lweIn,
    double mul,
    double rescale);

  // Bootstrapping with the absolute value function
  static void boot_Abs(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      double rescale);

  // Bootstrapping with the x^e function
  static void boot_powerOf(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      int exponent,
      double rescale);

  // Bootstrapping with the symmetric sigmoid function
  static void boot_Sigmoid(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      double rescale);

  // Bootstrapping with the symmetric sigmoid function
  static void boot_Positive_Sigmoid(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      double rescale);
};

//template struct SHTBootFFT<Torus32>;
//template struct SHTBootFFT<Torus64>;

#endif
