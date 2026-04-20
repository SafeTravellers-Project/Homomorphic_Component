#ifndef MUX_OPS_H
#define MUX_OPS_H

#include <stdio.h>
#include <iostream>

// Main TFHE

#include "tfhe.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"

//#include "funxtensions.h"
#include "scaled_tfhe.h"

struct MuxOperations
{
static void CMux_NS(TLweSample * stlweOut, LweSample * slweDist1, LweSample * slweDist2,LweSample * slweInd1, LweSample * slweInd2,  TLweKeySwitchKey * key_switch_21_array, int gamma, TLweParams * tlwe_params_1,const LweParams * lwe_params_1, const LweParams * lwe_params_2, LweKeySwitchKey * key_switch_12, LweBootstrappingKeyFFT * boot_key_12, LweBootstrappingKeyFFT * boot_key_21, LweKey * lwe_key_1, LweKey * lwe_key_2,TLweKey * tlwe_key_1);


static void CMux_AS(TLweSample * stlweOut, LweSample * slweDist1, LweSample * slweDist2,LweSample * slweInd1, LweSample * slweInd2, TLweKeySwitchKey * key_switch_21_array, int gamma, TLweParams * tlwe_params_1,const LweParams * lwe_params_1,const  LweParams * lwe_params_2, LweKeySwitchKey * key_switch_12, LweBootstrappingKeyFFT * boot_key_12, LweBootstrappingKeyFFT * boot_key_21, LweKey * lwe_key_1, LweKey * lwe_key_2,TLweKey * tlwe_key_1);

};

#endif
