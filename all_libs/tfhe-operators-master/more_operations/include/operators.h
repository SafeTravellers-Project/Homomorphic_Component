#ifndef OPERATORS_H
#define OPERATORS_H

#include <stdio.h>
#include <iostream>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"
// Extended boot
#include "extended-bootstrap.h"

/**
* Full-torus operators
* They take values in [-1/2, 1/2] and ouput values in [-1/2, 1/2]
*/
//template<typename TORUS>
struct FTOperator
{
  static void identity(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk1,
      LweBootstrappingKeyFFT * bk2,
      LweSample * lweIn);

  static void absoluteValue(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  static void xcube(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk1,
      LweBootstrappingKeyFFT * bk2,
      LweSample * lweIn);

  static void OddpowerOf(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk1,
      LweBootstrappingKeyFFT * bk2,
      LweSample * lweIn,
      int exponent);

  static void EvenpowerOf(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk1,
      LweBootstrappingKeyFFT * bk2,
      LweSample * lweIn,
      int exponent);

  static void Sigmoid(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk1,
      LweBootstrappingKeyFFT * bk2,
      LweSample * lweIn,
      double modulus);
};

//template struct FTOperator<Torus32>;
//template struct FTOperator<Torus64>;


/**
* Half-torus operators
* They take values in [-1/4, 1/4] and ouput values in [-1/2, 1/2] by rescaling everything in order
*   to have the maximum latitude for further computations
*/
//template<typename TORUS>
struct HTOperator
{
  static void identity(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  static void absoluteValue(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn);

  static void powerOf(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      int exponent);

  static void Sigmoid(LweSample * lweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * lweIn,
      double modulus);
};

//template struct HTOperator<Torus32>;
//template struct HTOperator<Torus64>;

#endif
