#ifndef SCALED_OPERATORS_H
#define SCALED_OPERATORS_H

/**
* Here we have operators that take as inputs and outputs scaled LWE samples
* This means the scaling is adapted and taken into account
* Inputs:
* - a scaled LWE sample (LWE sample [mu] and an input scale s_i)
* - an ouptut scale s_o
* Outputs:
* - a scaled LWE sample with the result being [f(mu*s_i)/s_o]
*/

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
#include "scaled_boot.h"
// Scaled TFHE
#include "scaled_tfhe.h"
//#include "scaled_lwe_functions.h"

/**
* Half-torus operators
* They take values in [-1/4, 1/4] and ouput values in [-1/4s_o, 1/4s_o] by rescaling everything
*/
//template<typename TORUS>
struct SHTOperator
{
  static void identity(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale);

  static void identity(LweSample * slweOut,
       LweBootstrappingKeyFFT * bk,
       LweSample * slweIn,
       int plaintext_size,
       double rescale);

  static void relu(LweSample * slweOut,
     LweBootstrappingKeyFFT * bk,
     LweSample * slweIn,
     double rescale);


  static void multiplication(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double mul,
    double rescale);

  static void absolute(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale);

  static void powerOf(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    int exponent,
    double rescale);

  static void Sigmoid(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale);

  static void PositiveSigmoid(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double rescale);
};

//template struct SHTOperator<Torus32>;
//template struct SHTOperator<Torus64>;

/**
* Full-torus operators
* They take values in [-1/2, 1/2] and ouput values in [-1/2, 1/2]
*/

//template<typename TORUS>
struct SFTOperator
{
  static void heaviside(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double positive);

  // This outputs positive if positive and -positive else
  static void sign(LweSample * slweOut,
    LweBootstrappingKey * bk,
    LweSample * slweIn,
    double positive);

  static void sign(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    LweSample * slweIn,
    double positive);

  static void absolute(LweSample * slweOut,
      LweBootstrappingKeyFFT * bk,
      LweSample * slweIn,
      int plaintext_size,
      int rescale);

  static void identity(LweSample * slweOut,
        LweBootstrappingKeyFFT * bk1,
        LweBootstrappingKeyFFT * bk2,
        LweSample * slweIn,
        int plaintext_size,
        int rescale);

  static void relu(LweSample * slweOut,
          LweBootstrappingKeyFFT * bk1,
          LweBootstrappingKeyFFT * bk2,
          LweSample * slweIn,
          int plaintext_size,
          int rescale);
};

//template struct SFTOperator<Torus32>;
//template struct SFTOperator<Torus64>;

#endif
