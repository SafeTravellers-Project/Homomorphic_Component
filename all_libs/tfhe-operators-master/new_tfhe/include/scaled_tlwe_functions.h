#ifndef SCALED_TLWE_FUNCTIONS_H
#define SCALED_TLWE_FUNCTIONS_H

#include <stdio.h>
#include <iostream>

// Main TFHE
#include "tfhe_gate_bootstrapping_functions.h"
#include "tfhe_gate_bootstrapping_structures.h"
#include "tfhe.h"
#include "tfhe_core.h"
// I/O
#include "tfhe_io.h"
#include "tfhe_generic_streams.h"
// Scaled
#include "scaled_tlwe_keyswitch.h"

/*
* Generic functions for the scaled TRGSW structure
*/

//template<typename TORUS>
struct TLweFunctions
{
  /**
  * Applies keySwitching
  */
  static void TLweMulByXai(
    TLweSample *result,
    int32_t ai,
    const TLweSample *bk,
    const TLweParams *params);


 static void KeySwitchTranslate_fromArray(
   TLweSample * result,
   const TLweSample ** ks,
   const TLweParams * params,
   const Torus32 ** as,
   const int n,
   const int t,
   const int basebit,
   const int M);

 /**
 * An alternative version with only two inputs
 * Gives the output of the keyswitching operation given the two decomposed inputs as1 and as2
 */
 static void KeySwitchTranslate_fromArray(
   TLweSample * result,
   const TLweSample ** ks,
   const TLweParams * params,
   const Torus32 * as1,
   const Torus32 * as2,
   const int n,
   const int t,
   const int basebit,
   const int tlwe_size);

   /**
   * An alternative version with four inputs
   * Gives the output of the keyswitching operation given the four decomposed inputs as1 and as2, ind1 and ind2
   */
   static void KeySwitchTranslate_fromArray(
     TLweSample * result,
     const TLweSample ** ks,
     const TLweParams * params,
     const Torus32 * as1,
     const Torus32 * as2,
     const Torus32 * ind1,
     const Torus32 * ind2,
     const int n,
     const int t,
     const int basebit,
     const int tlwe_size);

 /**
 * Specialized faster version
 * An alternative version with only two inputs
 * Gives the output of the keyswitching operation given the two decomposed inputs as1 and as2
 */
 static void KeySwitchTranslate_fromArray(
   TLweSample * result,
   const TLweSample ** ks1,
   const TLweSample ** ks2,
   const TLweParams * params,
   const Torus32 * as1,
   const Torus32 * as2,
   const int n,
   const int t,
   const int basebit,
   const int tlwe_size);


   void KeySwitchTranslate_fromArray(
     TLweSample * result,
     const TLweSample ** ks,
     const TLweParams * params,
     const Torus32 * as,
     const int n,
     const int t,
     const int basebit,
     const int array_size,
     const int tlwe_size);
  // static void KeySwitch(
  //     TLweSample* result,
  //     const TLweKeySwitchKey* ks,
  //     const LweSample* sample);

  static void KeySwitchTranslate_fromArray1(
    TLweSample * result,
    const TLweKeySwitchKey * ks,
    const TLweParams * params,
    const Torus32 * as,
    const int n,
    const int t,
    const int basebit,
    const int tlwe_size,
    const int array_size);

    static void KeySwitchTranslate_fromArray2(
      TLweSample * result,
      const TLweKeySwitchKey * ks,
      const TLweParams * params,
      const Torus32 * as,
      const int n,
      const int t,
      const int basebit,
      const int tlwe_size,
      const int array_size
    );

    static void KeySwitchTranslate_fromArray4(
      TLweSample * result,
      const TLweKeySwitchKey * ks1,
      const TLweKeySwitchKey * ks2,
      const TLweKeySwitchKey * ks3,
      const TLweKeySwitchKey * ks4,
      const TLweParams * params,
      const Torus32 * as,
      const int n,
      const int t,
      const int basebit,
      const int tlwe_size,
      const int array_size
    );

  static void KeySwitch_Id(TLweSample * result,
    const TLweKeySwitchKey* ks,
    LweSample * samples,
    const int M);

  /*
  * A keyswitch that takes only two inputs and creates a TRLWE sample with size "tlwe_size"
  *   and filled with the first sample in the first half and the second sample in the second half
  * This tlwe_size is the M from other keyswitch functions
  */
  static void KeySwitch_Id(
    TLweSample * result,
    const TLweKeySwitchKey* ks,
    LweSample * slweIn1,
    LweSample * slweIn2,
    const int tlwe_size);

  /*
  * A faster version with two pre-computed specialized key-switching keys
  * A keyswitch that takes only two inputs and creates a TRLWE sample with size "tlwe_size"
  *   and filled with the first sample in the first half and the second sample in the second half
  * This tlwe_size is the M from other keyswitch functions
  */
  static void KeySwitch_Id(
    TLweSample * result,
    const TLweKeySwitchKey* ks1,
    const TLweKeySwitchKey* ks2,
    LweSample * slweIn1,
    LweSample * slweIn2,
    const int tlwe_size);



    static void KeySwitch_Id_Ind(
      TLweSample * result,
      const TLweKeySwitchKey* ks,
      LweSample * slweIn1,
      LweSample * slweIn2,
      LweSample * slweInd1,
      LweSample * slweInd2,
      const int tlwe_size);

      static void KeySwitch_Id_Ind2(
        TLweSample * result,
        const TLweKeySwitchKey* ks,
        LweSample * slweIn,
        const int tlwe_size,
      const int array_size);

      static void KeySwitch_Id_Ind3(
        TLweSample * result,
        const TLweKeySwitchKey* ks1,
        const TLweKeySwitchKey* ks2,
        const TLweKeySwitchKey* ks3,
        const TLweKeySwitchKey* ks4,
        LweSample * slweIn,
        const int tlwe_size,
        const int array_size);

      static void KeySwitch_Id_Ind4(
          TLweSample * result,
          const TLweKeySwitchKey* ks,
          LweSample * slweIn,
          const int tlwe_size,
        const int array_size);

};

//template struct STLweFunctions<Torus32>;
//template struct STLweFunctions<Torus64>;
//typedef TLweFunctions TLweFunctions
#endif
