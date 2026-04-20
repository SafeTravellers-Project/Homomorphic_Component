#ifndef SCALED_PRIVATE_OPERATORS_H
#define SCALED_PRIVATE_OPERATORS_H

/**
* This here is the equivalent of the scaled_operators
* but with a hidden output through the encrypted test vector
* Here we have operators that take as inputs and outputs scaled LWE samples
* This means the scaling is adapted and taken into account
* Inputs:
* - a scaled LWE sample (LWE sample [mu] and an input scale s_i)
* - an ouptut scale s_o
* Outputs:
* - a scaled LWE sample with the result being [f(mu*s_i)/s_o]
*/

/*
* Several differences:
* - here, to the best of my knowledge, rescaling is not possible
* - two ways to go:
*     - either give the test_vector encrypted as a computation key
*     - or give a number of ciphertext samples and recreate a TRLWE testvector with a public functional keyswitch
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

// Private boot
#include "private-bootstrap.h"
// Scaled TFHE
#include "scaled_tfhe.h"

/**
* Half-torus operators
* They take values in [-1/4, 1/4] and ouput values in [-1/4s_o, 1/4s_o] by rescaling everything
*/
//template<typename TORUS>
struct PHTOperator
{
  /*
  * The bootstrapping key is one from in to out
  * The keyswitching key is from in to out
  * The testvector needs to be encrypted with out
  * The lwe samples used to recreate the testvector need to be encrypted with in
  */
  static void heaviside(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    TLweKeySwitchKey * key_switch_key,
    LweSample * slweIn,
    LweSample * slweSign);

  // Version where we give the testvector instead
  // The output scale is that of the testvector
  static void unknown(LweSample * slweOut,
    LweBootstrappingKeyFFT * bk,
    TLweSample* testvect,
    LweSample * slweIn);
};

//template struct SPHTOperator<Torus32>;
//template struct SPHTOperator<Torus64>;

#endif
