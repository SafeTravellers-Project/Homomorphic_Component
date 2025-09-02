#include "scaled_tlwe_keyswitch_functions.h"

EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey(
  TLweKeySwitchKey* result,
  const LweKey* in_key,
  const TLweKey* out_key)
{
  const int n=result->n;
  const int basebit=result->basebit;
  const int t=result->t;

  CreateKeySwitchKey_fromArray(
    result->ks,
    out_key, out_key->params->alpha_min,
    in_key->key, n, t, basebit);
}

// EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_fromRaw(
//   TLweKeySwitchKey* result,
//   const int* ks0_raw,
//   const int n,
//   const int t,
//   const int basebit,
//   const TLweParams* params)
// {
//   // Initialize the key-switching key
//   result->n = n;
//   result->basebit = basebit;
//   result->t = t;
//   result->out_params = params;

//   // Allocate memory for the key-switching key
//   result->ks = new TLweSample*[n];
//   for (int i = 0; i < n; ++i) {
//     result->ks[i] = new TLweSample[t];
//     for (int j = 0; j < t; ++j) {
//       new (&result->ks[i][j]) TLweSample(params);
//     }
//   }

//   // Fill the key-switching key with the provided raw values
//   for (int i = 0; i < n; ++i) {
//     for (int j = 0; j < t; ++j) {
//       for (int k = 0; k < params->N; ++k) {
//         result->ks[i][j].a[k] = ks0_raw[(i * t + j) * params->N + k];
//       }
//       result->ks[i][j].b = ks0_raw[(i * t + j) * params->N + params->N];
//       result->ks[i][j].current_variance = ks0_raw[(i * t + j) * params->N + params->N + 1];
//     }
//   }
// }

/*
* ID key switch key creation from a general key-switching key
*/

//template<typename TORUS>
EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_Id(
  TLweKeySwitchKey* ksOut,
  const TLweKeySwitchKey* ksIn,
  const int tlwe_size)
{
  // Parameters
  const TLweParams * params = ksIn->out_params;
  const int n = ksIn->n;
  const int basebit=ksIn->basebit;
  const int t=ksIn->t;

  // Offset
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  // Temp sample
  TLweSample * temp = new_TLweSample(params);

  for (int i = 0; i < n; i++)
  {
    for (int p = 0; p < t; p++)
    {
      tLweClear(&ksOut->ks[i][p], params);
      for (int j = 0; j < tlwe_size; j++)
      {
        TLweFunctions::TLweMulByXai(temp, j, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
        tLweAddTo(&ksOut->ks[i][p], temp, params);
      }
    }
  }

  // Clean up
  delete temp;
}

//template<typename TORUS>
EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_Id(
  TLweKeySwitchKey* ksOut1,
  TLweKeySwitchKey* ksOut2,
  const TLweKeySwitchKey* ksIn,
  const int tlwe_size)
{
  // Parameters
  const TLweParams * params = ksIn->out_params;
  const int n = ksIn->n;
  const int basebit=ksIn->basebit;
  const int t=ksIn->t;

  // Offset
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  // Temp sample
  TLweSample * temp = new_TLweSample(params);

  for (int i = 0; i < n; i++)
  {
    for (int p = 0; p < t; p++)
    {
      // Clear the output
      tLweClear(&ksOut1->ks[i][p], params);
      tLweClear(&ksOut2->ks[i][p], params);

      for (int j = 0; j < tlwe_size/2; j++)
      {
        TLweFunctions::TLweMulByXai(temp, j, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
        tLweAddTo(&ksOut1->ks[i][p], temp, params);
      }
      // Here we could remove this part and replace it with a multiplication ksOut2 = ksOut1 * X^(tlwe_size/2)
      for (int j = tlwe_size/2; j < tlwe_size; j++)
      {
        TLweFunctions::TLweMulByXai(temp, j, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
        tLweAddTo(&ksOut2->ks[i][p], temp, params);
      }
    }
  }

  // Clean up
  delete temp;
}


//template<typename TORUS>
EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_Id1(
  TLweKeySwitchKey* ksOut,
  const TLweKeySwitchKey* ksIn,
  const int tlwe_size,
  const int array_size)
{
  // Parameters
  const TLweParams * params = ksIn->out_params;
  const int n = ksIn->n;
  const int basebit=ksIn->basebit;
  const int t=ksIn->t;

  // Offset
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  //Temp int
  int t_size = tlwe_size/array_size;;
  // Temp sample
  TLweSample * temp = new_TLweSample(params);

  for (int i = 0; i < n; i++)
  {
    for (int p = 0; p < t; p++)
    {
      // Clear the output
      for (int j = 0 ; j < array_size ; j++){
          tLweClear(&(ksOut+j)->ks[i][p], params);
          //std::cout << "Done clearing for " << j+1 << '\n';
      }
      for (int j = 0; j < t_size; j++)
      {
        TLweFunctions::TLweMulByXai(temp, j, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
        tLweAddTo(&ksOut->ks[i][p], temp, params);
      }
      for (int j=1; j < array_size ; j++)
      {
        TLweFunctions::TLweMulByXai(&(ksOut+j)->ks[i][p], j*t_size, &ksOut->ks[i][p], params);
      }
      //tLweClear(&ksOut1->ks[i][p], params);
      //tLweClear(&ksOut2->ks[i][p], params);
      /*for (int j = 0; j < array_size ; j++)
      {
        for (int k = j*t_size ; k < (j+1)*t_size ; k++)
      {if(k==j*t_size)
          TLweFunctions::TLweMulByXai(&(ksOut+j)->ks[i][p],k,&ksIn->ks[i][p], params);
          else{
          TLweFunctions::TLweMulByXai(temp, k, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(&(ksOut+j)->ks[i][p], temp, params);
        }
        }
        */
      //  std::cout << "Done for the " << j+1 << "th key. " << '\n';

      //}
    }
  }

  // Clean up
//  delete temp;
}

//template<typename TORUS>
EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_Id4(
  TLweKeySwitchKey* ksOut1,
  TLweKeySwitchKey* ksOut2,
  TLweKeySwitchKey* ksOut3,
  TLweKeySwitchKey* ksOut4,
  const TLweKeySwitchKey* ksIn,
  const int tlwe_size,
  const int array_size)
{
  // Parameters
  const TLweParams * params = ksIn->out_params;
  const int n = ksIn->n;
  const int basebit=ksIn->basebit;
  const int t=ksIn->t;

  // Offset
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  //Temp int
  int t_size = tlwe_size/array_size;;
  // Temp sample
  TLweSample * temp = new_TLweSample(params);

  for (int i = 0; i < n; i++)
  {
    for (int p = 0; p < t; p++)
    {
      // Clear the output
      //for (int j = 0 ; j < array_size ; j++){
          tLweClear(&ksOut1->ks[i][p], params);
          tLweClear(&ksOut2->ks[i][p], params);
          tLweClear(&ksOut3->ks[i][p], params);
          tLweClear(&ksOut4->ks[i][p], params);
          //std::cout << "Done clearing for " << j+1 << '\n';
      //}
      //tLweClear(&ksOut1->ks[i][p], params);
      //tLweClear(&ksOut2->ks[i][p], params);

      for (int j = 0; j < tlwe_size/array_size; j++)
      {
        TLweFunctions::TLweMulByXai(temp, j, &ksIn->ks[i][p], params);                       // KS_{i,p} * X^j
        tLweAddTo(&ksOut1->ks[i][p], temp, params);
      }
      // Here we could remove this part and replace it with a multiplication ksOut2 = ksOut1 * X^(tlwe_size/2)
      TLweFunctions::TLweMulByXai(&ksOut2->ks[i][p], tlwe_size/array_size, &ksOut1->ks[i][p], params);
      TLweFunctions::TLweMulByXai(&ksOut3->ks[i][p], 2*tlwe_size/array_size, &ksOut1->ks[i][p], params);
      TLweFunctions::TLweMulByXai(&ksOut4->ks[i][p], 3*tlwe_size/array_size, &ksOut1->ks[i][p], params);

    }
  }

  // Clean up
  delete temp;
}


/**
* Creates the key-switching key
*/
//template<typename TORUS>
EXPORT void TLweKeySwitchKeyFunctions::CreateKeySwitchKey_fromArray(
  TLweSample** result,
  const TLweKey* out_key,
  const double out_alpha,
  const int* in_key,
  const int n,
  const int t,
  const int basebit)
{
  #pragma omp parallel for
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<t;j++)
    {
      Torus32 x=in_key[i]*(1<<(32-(j+1)*basebit));
      tLweSymEncryptT(&result[i][j],x,out_alpha,out_key);
    }
  }
}
