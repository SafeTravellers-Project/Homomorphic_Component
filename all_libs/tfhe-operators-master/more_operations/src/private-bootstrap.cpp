#include "private-bootstrap.h"
#include "tfhe.h"
#include "scaled_tfhe.h"

/*
*********************************** ANY TORUS *******************************************
*/

/*
* Call a bootstrapping operation given an encrypted test vector
*/

//template<typename TORUS>
void PBootFFT::extendedBoot(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    TLweSample* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // Bootstrapping rotation and extraction
  PBootFFT::blindRotateAndExtract_FFT(lweOut, testvect, bk->bkFFT, barb, bara, n, bk_params);

  // Clean up
  delete[] bara;
}

//template<typename TORUS>
void PBootFFT::boot_wo_extract(TLweSample* tlweOut,
    LweBootstrappingKey * bk,
    TLweSample* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // tlweOut = X^{2N-barb}*testvec
  if (barb!=0)
    TLweFunctions::TLweMulByXai(tlweOut, Nx2-barb, testvect, accum_params);
  else
    tLweCopy(tlweOut, testvect, accum_params);

  // Blind rotation
  tfhe_blindRotate(tlweOut, bk->bk, bara, n, bk_params);

  // Clean up
  delete[] bara;
}


//template<typename TORUS>
void PBootFFT::boot_wo_extract(TLweSample* tlweOut,
    LweBootstrappingKeyFFT * bk,
    TLweSample* testvect,
    LweSample * lweIn)
{
  // Get the parameters
  const TGswParams* bk_params = bk->bk_params;
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* in_params = bk->in_out_params;
  const int N=accum_params->N;
  const int Nx2= 2*N;
  const int n = in_params->n;

  // Create bara and barb
  int* bara = new int[N];
  int barb = modSwitchFromTorus32(lweIn->b,Nx2);
  for (int i=0; i<n; i++) {
    bara[i]=modSwitchFromTorus32(lweIn->a[i],Nx2);
  }

  // tlweOut = X^{2N-barb}*testvec
  if (barb!=0)
    TLweFunctions::TLweMulByXai(tlweOut, Nx2-barb, testvect, accum_params);
  else
    tLweCopy(tlweOut, testvect, accum_params);

  // Blind rotation
  tfhe_blindRotate_FFT(tlweOut, bk->bkFFT, bara, n, bk_params);

  // Clean up
  delete[] bara;
}

//template<typename TORUS>
void PBootFFT::MuxRotate_FFT(TLweSample* result,
  const TLweSample* accum,
  const TGswSampleFFT* bki,
  const int barai,
  const TGswParams* bk_params)
{
  // ACC = BKi*[(X^barai-1)*ACC]+ACC
  // temp = (X^barai-1)*ACC
  tLweMulByXaiMinusOne(result, barai, accum, bk_params->tlwe_params);
  // temp *= BKi
  tGswFFTExternMulToTLwe(result, bki, bk_params);
  // ACC += temp
  tLweAddTo(result, accum, bk_params->tlwe_params);
}

// template<typename TORUS>
// void PBootFFT::blindRotate_FFT(TLweSample* accum,
//   const TGswSampleFFT* bkFFT,
//   const int* bara,
//   const int n,
//   const TGswParams* bk_params)
// {
//   TLweSample* temp = new_TLweSample(bk_params->tlwe_params);
//   TLweSample* temp2 = temp;
//   TLweSample* temp3 = accum;
//   // For swapping purposes
//   TLweSample* temp4 new_TLweSample(bk_params->tlwe_params)
//
//   for (int i=0; i<n; i++)
//   {
//     const int barai=bara[i];
//     if (barai==0) continue; //indeed, this is an easy case!
//
//     PBootFFT::MuxRotate_FFT(temp2, temp3, bkFFT+i, barai, bk_params);
//
//     // Make it quicker here ??
//     TLweFunctions::Copy(temp4, temp2, bk_params->tlwe_params);
//     TLweFunctions::Copy(temp2, temp3, bk_params->tlwe_params);
//     TLweFunctions::Copy(temp3, temp4, bk_params->tlwe_params);
//     // swap(temp2,temp3);
//   }
//
//   if (temp3 != accum)
//     TLweFunctions::Copy(accum, temp3, bk_params->tlwe_params);
//
//   // Clean up
//   delete_TLweSample(temp);
// }

//template<typename TORUS>
void PBootFFT::blindRotateAndExtract_FFT(LweSample* result,
    TLweSample* v,
    const TGswSampleFFT* bk,
    const int barb,
    const int* bara,
    const int n,
    const TGswParams* bk_params)
{
  // Parameters
  const TLweParams* accum_params = bk_params->tlwe_params;
  const LweParams* extract_params = &accum_params->extracted_lweparams;
  const int N = accum_params->N;
  const int _2N = 2*N;

  // Accumulator
  TLweSample* acc = new_TLweSample(accum_params);

  // acc = X^{2N-barb}*v
  if (barb!=0)
    TLweFunctions::TLweMulByXai(acc, _2N-barb, v, accum_params);
  else
    tLweCopy(acc, v, accum_params);

  // Blind rotation

  // PBootFFT::blindRotate_FFT(acc, bk, bara, n, bk_params);
  tfhe_blindRotate_FFT(acc, bk, bara, n, bk_params);

  // Extraction
  tLweExtractLweSample(result, acc, extract_params, accum_params);

  // Clean up
  delete_TLweSample(acc);
}
 
/*
*********************************** SCALED HALF TORUS Private boot*******************************************
*/

/*
* Exteneded Sign
THIS IS USELESS
* Importantly, the scale is with regard to the [-1/4,1/4] range
*/
//template<typename TORUS>
void HTPBootFFT::boot_Sign(LweSample * lweOut,
    LweBootstrappingKeyFFT * bk,
    TLweKeySwitchKey * key_switch_key,
    LweSample * lweIn,
    LweSample * lweSign)
{
  // Get parameters
  const TLweParams* accum_params = bk->accum_params;
  const LweParams* lwe_params = &accum_params->extracted_lweparams;
  const int N=accum_params->N;
  const int Nx2 = N*2;

  /*
  * Create the encrypted test vector
  */
  TLweSample * testvect = new_TLweSample(accum_params);

  // For this create the appropriate TLWE array
  LweSample * lweTestvect = new_LweSample_array(N,lwe_params);

  // Fill it with the appropriate values
  for (int i=0;i<N/2;i++)
  {
    lweCopy(lweTestvect+i, lweSign, lwe_params);
    // lweNoiselessTrivial(slweTestvect+i, 0, slweSign->scale, lwe_params);
  }


  for (int i=N/2;i<N;i++)
  {
    // Here we might have a problem in terms of error analysis and leakage... If so, we need to get some noise into this..
    lweNoiselessTrivial(lweTestvect+i, 0, lwe_params);
  }

  // Call the identity public functional keyswitch
  TLweFunctions::KeySwitch_Id(testvect, key_switch_key, lweTestvect, N);

  // Call the bootstrapping with the test vector
  PBootFFT::extendedBoot(lweOut, bk, testvect, lweIn);

  // CLean up
  delete_TLweSample(testvect);
  delete_LweSample_array(N,lweTestvect);
}
