#ifndef SCALED_TLWE_KEYSWITCH_FUNCTIONS_H
#define SCALED_TLWE_KEYSWITCH_FUNCTIONS_H

#include "scaled_tlwe_functions.h"
#include "scaled_tlwe_keyswitch.h"
#include "tfhe.h"
    
struct TLweKeySwitchKeyFunctions{
    static void CreateKeySwitchKey(TLweKeySwitchKey* result,
      const LweKey* in_key,
      const TLweKey* out_key);

    // static void CreateKeySwitchKey_fromRaw(TLweKeySwitchKey* result,
    //     const int* ks0_raw,
    //     const int n,
    //     const int t,
    //     const int basebit,
    //     const TLweParams* params);  

    // ID key switch key creation from a general key-switching key
    // This is made for a single input of the keyswitch_id
    static void CreateKeySwitchKey_Id(TLweKeySwitchKey* ksOut,
      const TLweKeySwitchKey* ksIn,
      const int tlwe_size);

    // ID key switch key creation from a general key-switching key
    // This is made for two inputs of the keyswitch_id
    static void CreateKeySwitchKey_Id(TLweKeySwitchKey* ksOut1,
      TLweKeySwitchKey* ksOut2,
      const TLweKeySwitchKey* ksIn,
      const int tlwe_size);

    static void CreateKeySwitchKey_Id1(
        TLweKeySwitchKey* ksOut,
        const TLweKeySwitchKey* ksIn,
        const int tlwe_size,
        const int array_size);

    static void CreateKeySwitchKey_Id4(
          TLweKeySwitchKey* ksOut1,
          TLweKeySwitchKey* ksOut2,
          TLweKeySwitchKey* ksOut3,
          TLweKeySwitchKey* ksOut4,
          const TLweKeySwitchKey* ksIn,
          const int tlwe_size,
          const int array_size);

    static void CreateKeySwitchKey_fromArray(
      TLweSample** result,
      const TLweKey* out_key,
      const double out_alpha,
      const int* in_key,
      const int n,
      const int t,
      const int basebit);
      };


      #endif