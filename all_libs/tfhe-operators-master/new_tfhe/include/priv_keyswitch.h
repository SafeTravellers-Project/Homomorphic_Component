#ifndef PRIV_KEYSWITCH_H
#define PRIV_KEYSWITCH_H

#include <assert.h>
#include <iostream>

#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
#include "tfhe_core.h"

//template<typename TORUS>
struct PrivKeySwitchKey{
    int32_t n;        ///< length of the input key: s'
    int32_t new_n;    ///< length of the input key + 1 to account for b
    int32_t t; ///< decomposition length
    int32_t p; ///< number of inputs
    int32_t basebit; ///< log_2(base)
    int32_t base; ///< decomposition base: a power of 2
    const TLweParams* out_params;      // Params of the output key s
    TLweSample* ks0_raw;               // Array that contains all of the TRLWE samples linearly of size n*t*p
    TLweSample*** ks;                  // The triple TRLWE array: n*t*p

    PrivKeySwitchKey(int32_t n, int32_t t, int32_t p, int32_t basebit, const TLweParams* out_params, TLweSample* ks0_raw);
    ~PrivKeySwitchKey();
    PrivKeySwitchKey(const PrivKeySwitchKey&) = delete;
    void operator=(const PrivKeySwitchKey&) = delete;

    static void init_obj(
        PrivKeySwitchKey* obj,
        int32_t n,
        int32_t t,
        int32_t p,
        int32_t basebit,
        const TLweParams* out_params);
    static void destroy_obj(PrivKeySwitchKey* obj);

    /*
    * Creating a private key switching key with function f(x) = - K * x and p=1
    */

    static void CreateKeySwitchKey_OutKey(
      PrivKeySwitchKey * result,
      const LweKey* in_key,
      const TLweKey* out_key);

    /*
    * Creating a private key switching key with function f(x) = 1 * x and p=1
    */

    static void CreateKeySwitchKey_Id(
      PrivKeySwitchKey * result,
      const LweKey* in_key,
      const TLweKey* out_key);

    // /**
    // * Creates a Key Switching Key between the two keys
    // */
    // static void CreateKeySwitchKey(
    //   PrivKeySwitchKey* result,
    //   const LweKey* in_key,
    //   const TLweKey* out_key);

    /**
    * The key-switching process given a certain private key-switching key
    */
    static void KeySwitch(
      TLweSample * result,
      PrivKeySwitchKey* privKS,
      const TLweParams * in_params,   // The input parameters
      LweSample * samples);
};

//template struct PrivKeySwitchKey<Torus32>;
//template struct PrivKeySwitchKey<Torus64>;

#endif
