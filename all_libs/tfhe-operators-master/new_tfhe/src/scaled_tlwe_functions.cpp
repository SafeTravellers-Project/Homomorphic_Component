#include "scaled_tlwe_functions.h"



/**
* Gives the output of the keyswitching operation given the decomposed inputs in the array as
*/


EXPORT void TLweFunctions::TLweMulByXai(
  TLweSample *result,
  int32_t ai,
  const TLweSample *bk,
  const TLweParams *params) 
     {
    const int32_t k = params->k;
    for (int32_t i = 0; i <= k; i++)
        torusPolynomialMulByXai(&result->a[i], ai, &bk->a[i]);
}

EXPORT void TLweFunctions::KeySwitchTranslate_fromArray(
  TLweSample * result,
  const TLweSample ** ks,
  const TLweParams * params,
  const Torus32 ** as,
  const int n,
  const int t,
  const int basebit,
  const int M)
{
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  TLweSample * temp = new_TLweSample(params);

  //typedef typename TorusUtils::uint32_t uint32_t;

  /*
  / There are n as polynomials (index i)
  / Each of these binary polynomials has M coefficients (index j)
  / We decompose each of them into t binary polynomials (index p)
  /
  */

  // Here we can maybe optimize by building an integer polynomial
  // aij from its coefficients aijp and multiplying it directly
  // to &ks[i][j]. Then just subtract it from result.
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < M; j++)
    {
      // Approximate all the coefficients of the inputs
      const uint32_t aijbar = as[i][j]+prec_offset;

      for (int p = 0; p < t; p++)
      {
        const uint32_t aijp = (aijbar>>(32-(p+1)*basebit)) & mask;

        TLweFunctions::TLweMulByXai(temp, j, &ks[i][p], params);                              // KS_{i,j} * X^p
        tLweSubMulTo(result, aijp, temp, params);                              // KS_{i,j} * aijp * X^p
      }
    }
  }

  // Clean up
  delete temp;
}

/**
* An alternative version with only two inputs
* Gives the output of the keyswitching operation given the two decomposed inputs as1 and as2
*/
//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitchTranslate_fromArray(
  TLweSample * result,
  const TLweSample ** ks,
  const TLweParams * params,
  const Torus32 * as1,
  const Torus32 * as2,
  const int n,
  const int t,
  const int basebit,
  const int tlwe_size)
{
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  TLweSample * temp1 = new_TLweSample(params);
  TLweSample * temp2 = new_TLweSample(params);

  //typedef typename TorusUtils::uint32_t uint32_t;

  /*
  / There are n as polynomials (index i)
  / Each of these binary polynomials has M coefficients (index j)
  / We decompose each of them into t binary polynomials (index p)
  /
  */

  // Here we can maybe optimize by building an integer polynomial
  // aij from its coefficients aijp and multiplying it directly
  // to &ks[i][j]. Then just subtract it from result.
  for (int i = 0; i < n; i++)
  {
    const uint32_t ai1bar = as1[i]+prec_offset;
    const uint32_t ai2bar = as2[i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      const uint32_t ai1p = (ai1bar>>(32-(p+1)*basebit)) & mask;
      const uint32_t ai2p = (ai2bar>>(32-(p+1)*basebit)) & mask;

      // For the first half
      for (int j = 0; j < tlwe_size/2; j++)
      {
        if (j==0)
          tLweCopy(temp2, &ks[i][p], params);                              // KS_{i,p} * X^0
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai1p, temp2, params);                            //  aijp * \sum ( KS_{i,p}* X^j)

      // For the second half
      for (int j = tlwe_size/2; j < tlwe_size; j++)
      {
        if (j==tlwe_size/2)
          TLweFunctions::TLweMulByXai(temp2, j, &ks[i][p], params);                // KS_{i,p} * X^j
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai2p, temp2, params);                           //  aijp * \sum ( KS_{i,p}* X^j)

    }
  }

  // Clean up
  delete temp1;
  delete temp2;
}

/**
* An alternative version with four inputs
* Gives the output of the keyswitching operation given the four decomposed inputs as1 and as2, ind1 and ind2
*/
//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitchTranslate_fromArray(
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
  const int tlwe_size)
{
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  TLweSample * temp1 = new_TLweSample(params);
  TLweSample * temp2 = new_TLweSample(params);

  //typedef typename TorusUtils::uint32_t uint32_t;

  /*
  / There are n as polynomials (index i)
  / Each of these binary polynomials has M coefficients (index j)
  / We decompose each of them into t binary polynomials (index p)
  /
  */

  // Here we can maybe optimize by building an integer polynomial
  // aij from its coefficients aijp and multiplying it directly
  // to &ks[i][j]. Then just subtract it from result.
  for (int i = 0; i < n; i++)
  {
    const uint32_t ai1bar = as1[i]+prec_offset;
    const uint32_t ai2bar = as2[i]+prec_offset;
    const uint32_t ai3bar = ind1[i]+prec_offset;
    const uint32_t ai4bar = ind2[i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      const uint32_t ai1p = (ai1bar>>(32-(p+1)*basebit)) & mask;
      const uint32_t ai2p = (ai2bar>>(32-(p+1)*basebit)) & mask;
      const uint32_t ai3p = (ai3bar>>(32-(p+1)*basebit)) & mask;
      const uint32_t ai4p = (ai4bar>>(32-(p+1)*basebit)) & mask;

      // For the first quarter
      for (int j = 0; j < tlwe_size/4; j++)
      {
        if (j==0)
          tLweCopy(temp2, &ks[i][p], params);                              // KS_{i,p} * X^0
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai1p, temp2, params);                            //  aijp * \sum ( KS_{i,p}* X^j)

      // For the second quarter
      for (int j = tlwe_size/4; j < tlwe_size/2; j++)
      {
        if (j==tlwe_size/4)
          tLweCopy(temp2, &ks[i][p], params);                              // KS_{i,p} * X^0
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai2p, temp2, params);                            //  aijp * \sum ( KS_{i,p}* X^j)

      // For the third quarter
      for (int j = tlwe_size/2; j < 3*tlwe_size/4; j++)
      {
        if (j==tlwe_size/2)
          tLweCopy(temp2, &ks[i][p], params);                              // KS_{i,p} * X^0
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai3p, temp2, params);                            //  aijp * \sum ( KS_{i,p}* X^j)


      // For the fourth quarter
      for (int j = 3*tlwe_size/4; j < tlwe_size; j++)
      {
        if (j==3*tlwe_size/4)
          TLweFunctions::TLweMulByXai(temp2, j, &ks[i][p], params);                // KS_{i,p} * X^j
        else
        {
          TLweFunctions::TLweMulByXai(temp1, j, &ks[i][p], params);                       // KS_{i,p} * X^j
          tLweAddTo(temp2, temp1, params);
        }
      }

      tLweSubMulTo(result, ai4p, temp2, params);                           //  aijp * \sum ( KS_{i,p}* X^j)

    }
  }

  // Clean up
  delete temp1;
  delete temp2;
}

/**
* Specialized fast version that takes only one key-switching keys as inputs and a variable number of input samples
*/

EXPORT void TLweFunctions::KeySwitchTranslate_fromArray1(
  TLweSample * result,
  const TLweKeySwitchKey * ks,
  const TLweParams * params,
  const Torus32 * as,
  const int n,
  const int t,
  const int basebit,
  const int tlwe_size,
  const int array_size
  )
{
  const int base=1<<basebit;
  const int32_t prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;


  // The size of the batch
  const int batch_size = tlwe_size/array_size;
  // temporary variable
  TLweSample * tlweTemp = new_TLweSample(params);
  TLweSample * temp1 = new_TLweSample(params);
  TLweSample * temp2 = new_TLweSample(params);

  // An array of aibar
  uint32_t * aibar = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  uint32_t * aip = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  // UTORUS * aip = (UTORUS*) malloc(sizeof(UTORUS)*array_size);

  for (int i = 0; i < n; i++)
  {
    // For all of the samples
    for (size_t j = 0; j < array_size; j++)
      aibar[j] = as[j*n+i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      // For all of the samples
      for (int j =0 ; j < array_size ; j++)
      {  aip[j] = (aibar[j]>>(32-(p+1)*basebit)) & mask;

        for (int k =j*batch_size ; k < (j+1)*batch_size ; k++){
          if ( k == j*batch_size)
            TLweFunctions::TLweMulByXai(temp2, k, &ks->ks[i][p], params);
          else {
            TLweFunctions::TLweMulByXai(temp1, k, &ks->ks[i][p], params);                       // KS_{i,p} * X^j
            tLweAddTo(temp2, temp1, params);
          }
        }
        tLweSubMulTo(result, aip[j], temp2, params);
      }

      }
    }


  // Clean up
  free(aibar);
  delete_TLweSample(tlweTemp);
  // free(aip);
}






/**
* Specialized fast version
* An alternative version with only two inputs
* Gives the output of the keyswitching operation given the two decomposed inputs as1 and as2
*/
//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitchTranslate_fromArray(
  TLweSample * result,
  const TLweSample ** ks1,
  const TLweSample ** ks2,
  const TLweParams * params,
  const Torus32 * as1,
  const Torus32 * as2,
  const int n,
  const int t,
  const int basebit,
  const int tlwe_size)
{
  const int base=1<<basebit;
  const Torus32 prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;

  //typedef typename TorusUtils::uint32_t uint32_t;

  for (int i = 0; i < n; i++)
  {
    const uint32_t ai1bar = as1[i]+prec_offset;
    const uint32_t ai2bar = as2[i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      // First half
      const uint32_t ai1p = (ai1bar>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, ai1p, &ks1[i][p], params);                 //  aijp * \sum ( KS_{i,p}* X^j)

      // Second half
      const uint32_t ai2p = (ai2bar>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, ai2p, &ks2[i][p], params);                      //  aijp * \sum ( KS_{i,p}* X^j)
    }
  }
}

/**
* Specialized fast version that takes a varibale number of key-switching keys and input samples
*/

EXPORT void TLweFunctions::KeySwitchTranslate_fromArray2(
  TLweSample * result,
  const TLweKeySwitchKey * ks,
  const TLweParams * params,
  const Torus32 * as,
  const int n,
  const int t,
  const int basebit,
  const int tlwe_size,
  const int array_size
  )
{
  const int base=1<<basebit;
  const int32_t prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;


  // The size of the batch
  const int batch_size = tlwe_size/array_size;
  // temporary variable
  TLweSample * tlweTemp = new_TLweSample(params);
  TLweSample * temp1 = new_TLweSample(params);
  TLweSample * temp2 = new_TLweSample(params);

  // An array of aibar
  uint32_t * aibar = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  uint32_t * aip = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  // UTORUS * aip = (UTORUS*) malloc(sizeof(UTORUS)*array_size);

  for (int i = 0; i < n; i++)
  {
    // For all of the samples
    for (size_t j = 0; j < array_size; j++)
      aibar[j] = as[j*n+i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      // For all of the samples
      for (int j =0 ; j < array_size ; j++)
      {
        aip[j] = (aibar[j]>>(32-(p+1)*basebit)) & mask;
        tLweSubMulTo(result, aip[j], &(ks+j)->ks[i][p], params);
      }

      }
    }


  // Clean up
  free(aibar);
  delete_TLweSample(tlweTemp);
  // free(aip);
}

EXPORT void TLweFunctions::KeySwitchTranslate_fromArray4(
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
  )
{
  const int base=1<<basebit;
  const int32_t prec_offset=1<<(32-(1+basebit*t)); //precision
  const int mask=base-1;


  // The size of the batch
  const int batch_size = tlwe_size/array_size;
  // temporary variable
  TLweSample * tlweTemp = new_TLweSample(params);
  TLweSample * temp1 = new_TLweSample(params);
  TLweSample * temp2 = new_TLweSample(params);

  // An array of aibar
  uint32_t * aibar = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  uint32_t * aip = (uint32_t*) malloc(sizeof(uint32_t)*array_size);
  // UTORUS * aip = (UTORUS*) malloc(sizeof(UTORUS)*array_size);

  for (int i = 0; i < n; i++)
  {
    // For all of the samples
    for (size_t j = 0; j < array_size; j++)
      aibar[j] = as[j*n+i]+prec_offset;

    for (int p = 0; p < t; p++)
    {
      // For all of the samples
      aip[0] = (aibar[0]>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, aip[0], &(ks1)->ks[i][p], params);
      aip[1] = (aibar[1]>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, aip[1], &(ks2)->ks[i][p], params);
      aip[2] = (aibar[2]>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, aip[2], &(ks3)->ks[i][p], params);
      aip[3] = (aibar[3]>>(32-(p+1)*basebit)) & mask;
      tLweSubMulTo(result, aip[3], &(ks4)->ks[i][p], params);
      //for (int j =0 ; j < array_size ; j++)
    //  {
    //    aip[j] = (aibar[j]>>(32-(p+1)*basebit)) & mask;
    //    tLweSubMulTo(result, aip[j], &(ks+j)->ks[i][p], params);
    //  }

      }
    }


  // Clean up
  free(aibar);
  delete_TLweSample(tlweTemp);
  // free(aip);
}



//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitch_Id(
  TLweSample * result,
  const TLweKeySwitchKey* ks,
  LweSample * samples,
  const int M)
{
  const TLweParams * params = ks->out_params;
  const int n = ks->n;
  const int basebit=ks->basebit;
  const int t=ks->t;
  const int N = params->N;

  // Notations
  /*
  * c1,..,c_M are the TLWE inputs with c_i = (a_i,b_i)
  * c' = (a',b') is the TRLWE result of the functional keyswitch
  * f is the T^M -> T_N[X] morphism. In this case f is the identity function
  * see the 2018 TFHE paper for the specifics
  */

  // Check that we can actually do the key switch
  if (M > N)
  {
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';
    return;
  }

  // c' = (0,0)
  tLweClear(result, params);

  /*
  * c' = (0, f(b_1,..,b_M))
  * Therefore for the first M coefficients of result->b,
  * we give them the b's from the samples in order
  */
  for (int i = 0; i < M; i++)
    result->b->coefsT[i] = (samples+i)->b;

  /*
  * We create as, an array of exactly n polynomials
  * The first M coefficients of the polynomial #i are filled with
  * the ith value from all the samples taken in order.
  */
  Torus32 ** as = (Torus32 **) malloc(sizeof(Torus32*)*n);
  for (int i = 0; i < n; i++)
    as[i] = (Torus32*) malloc(sizeof(Torus32)*M);
  // const Torus32 ** as = (const Torus32 **) malloc(sizeof(samples[0]->a)*n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < M; j++)
      as[i][j] = (samples+j)->a[i];

  TLweFunctions::KeySwitchTranslate_fromArray(
    result,
    (const TLweSample **) ks->ks,
    params,
    (const Torus32 **) as, n, t, basebit, M);

  /*
  * Update the scale and the polynomial degree
  */
  //result->poly_degree = M-1;

  // Clean up
  free(as);
}

/*
* A keyswitch that takes only two inputs and creates a TRLWE sample with size "tlwe_size"
*   and filled with the first sample in the first half and the second sample in the second half
* This tlwe_size is the M from other keyswitch functions
*/
//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitch_Id(
  TLweSample * result,
  const TLweKeySwitchKey* ks,
  LweSample * slweIn1,
  LweSample * slweIn2,
  const int tlwe_size)
{
  // Parameters
  const TLweParams * params = ks->out_params;
  const int n = ks->n;
  const int basebit=ks->basebit;
  const int t=ks->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  for (int i = 0; i < tlwe_size/2; i++)
    result->b->coefsT[i] = slweIn1->b;
  for (int i = tlwe_size/2; i < tlwe_size; i++)
    result->b->coefsT[i] = slweIn2->b;

  /*
  * We create as, an array of exactly n polynomials of size 2
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
  Torus32 * as1 = (Torus32 *) malloc(sizeof(Torus32)*n);
  Torus32 * as2 = (Torus32 *) malloc(sizeof(Torus32)*n);

  for (int i = 0; i < n; i++)
    as1[i] = slweIn1->a[i];
  for (int i = 0; i < n; i++)
    as2[i] = slweIn2->a[i];

  TLweFunctions::KeySwitchTranslate_fromArray(
    result,
    (const TLweSample **) ks->ks,
    params, as1, as2, n, t, basebit, tlwe_size);

  /*
  * Update the scale and the polynomial degree
  */

 // result->scale = slweIn1->scale;
 // result->poly_degree = tlwe_size-1;

  /*
  * Clean up
  */

  // arrays
  free(as1);
  free(as2);

}

/*
* A faster version with pre-computed specialized key-switching keys
* A keyswitch that takes only two inputs and creates a TRLWE sample with size "tlwe_size"
*   and filled with the first sample in the first half and the second sample in the second half
* This tlwe_size is the M from other keyswitch functions
*/
//template<typename TORUS>
EXPORT void TLweFunctions::KeySwitch_Id(
  TLweSample * result,
  const TLweKeySwitchKey* ks1,
  const TLweKeySwitchKey* ks2,
  LweSample * slweIn1,
  LweSample * slweIn2,
  const int tlwe_size)
{
  // Parameters
  const TLweParams * params = ks1->out_params;
  const int n = ks1->n;
  const int basebit=ks1->basebit;
  const int t=ks1->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  for (int i = 0; i < tlwe_size/2; i++)
    result->b->coefsT[i] = slweIn1->b;
  for (int i = tlwe_size/2; i < tlwe_size; i++)
    result->b->coefsT[i] = slweIn2->b;

  /*
  * We create as, an array of exactly n polynomials of size 2
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
  Torus32 * as1 = (Torus32 *) malloc(sizeof(Torus32)*n);
  Torus32 * as2 = (Torus32 *) malloc(sizeof(Torus32)*n);

  for (int i = 0; i < n; i++)
    as1[i] = slweIn1->a[i];
  for (int i = 0; i < n; i++)
    as2[i] = slweIn2->a[i];

  TLweFunctions::KeySwitchTranslate_fromArray(
    result,
    (const TLweSample **) ks1->ks,
    (const TLweSample **) ks2->ks,
    params, as1, as2, n, t, basebit, tlwe_size);

  /*
  * Update the scale and the polynomial degree
  */

 // result->scale = slweIn1->scale;
  //result->poly_degree = tlwe_size-1;

  /*
  * Clean up
  */

  // arrays
  free(as1);
  free(as2);

}
// a generalized version with any number of inputs and single KS
EXPORT void TLweFunctions::KeySwitch_Id_Ind2(
  TLweSample * result,
  const TLweKeySwitchKey* ks,
  LweSample * slweIn,
  const int tlwe_size,
const int array_size)
{
  // Parameters
  const TLweParams * params = ks->out_params;
  const int n = ks->n;
  const int basebit=ks->basebit;
  const int t=ks->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  int block_size = tlwe_size/array_size;
  for (int j = 0 ; j< array_size ; j++){
    for (int i = j*block_size; i < (j+1)*block_size; i++)
      result->b->coefsT[i] = (slweIn+j)->b;
  }
  /*
  * We create as, an array of exactly n polynomials of size "array_size"
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
    Torus32 * as = (Torus32 *) malloc(sizeof(Torus32)*n*array_size);
    for (int j = 0 ; j < array_size ; j++){
      for( int i = 0; i < n ;i++)
        as[i+j*n] = (slweIn+j)->a[i];
    }
    TLweFunctions::KeySwitchTranslate_fromArray1(
      result,ks, params, as , n, t, basebit, tlwe_size,array_size);
// we send the tlwekeyswitchkey key directly and let keyswitch translate deal with it
    /*
    * Update the scale and the polynomial degree
    */

    //result->scale = slweIn->scale;
    //result->poly_degree = tlwe_size-1;

    /*
    * Clean up
    */

    // arrays
    free(as);
}

// a generalized version with any number of inputs and 4 separate KS
EXPORT void TLweFunctions::KeySwitch_Id_Ind3(
  TLweSample * result,
  const TLweKeySwitchKey* ks1,
  const TLweKeySwitchKey* ks2,
  const TLweKeySwitchKey* ks3,
  const TLweKeySwitchKey* ks4,
  LweSample * slweIn,
  const int tlwe_size,
const int array_size)
{
  // Parameters
  const TLweParams * params = ks1->out_params;
  const int n = ks1->n;
  const int basebit=ks1->basebit;
  const int t=ks1->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  int block_size = tlwe_size/array_size;
  for (int j = 0 ; j< array_size ; j++){
    for (int i = j*block_size; i < (j+1)*block_size; i++)
      result->b->coefsT[i] = (slweIn+j)->b;
  }
  /*
  * We create as, an array of exactly n polynomials of size "array_size"
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
    Torus32 * as = (Torus32 *) malloc(sizeof(Torus32)*n*array_size);
    for (int j = 0 ; j < array_size ; j++){
      for( int i = 0; i < n ;i++)
        as[i+j*n] = (slweIn+j)->a[i];
    }
    TLweFunctions::KeySwitchTranslate_fromArray4(
      result,ks1,ks2,ks3,ks4, params, as , n, t, basebit, tlwe_size,array_size);
// we send the tlwekeyswitchkey key directly and let keyswitch translate deal with it
    /*
    * Update the scale and the polynomial degree
    */

  //  result->scale = slweIn->scale;
  //  result->poly_degree = tlwe_size-1;

    /*
    * Clean up
    */

    // arrays
    free(as);
}


// a generalized version with any number of inputs and KS
EXPORT void TLweFunctions::KeySwitch_Id_Ind4(
  TLweSample * result,
  const TLweKeySwitchKey* ks,
  LweSample * slweIn,
  const int tlwe_size,
const int array_size)
{
  // Parameters
  const TLweParams * params = ks->out_params;
  const int n = ks->n;
  const int basebit=ks->basebit;
  const int t=ks->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  int block_size = tlwe_size/array_size;
  for (int j = 0 ; j< array_size ; j++){
    for (int i = j*block_size; i < (j+1)*block_size; i++)
      result->b->coefsT[i] = (slweIn+j)->b;
  }
  /*
  * We create as, an array of exactly n polynomials of size "array_size"
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
    Torus32 * as = (Torus32 *) malloc(sizeof(Torus32)*n*array_size);
    for (int j = 0 ; j < array_size ; j++){
      for( int i = 0; i < n ;i++)
        as[i+j*n] = (slweIn+j)->a[i];
    }
    TLweFunctions::KeySwitchTranslate_fromArray2(
      result,ks, params, as , n, t, basebit, tlwe_size,array_size);
// we send the tlwekeyswitchkey key directly and let keyswitch translate deal with it
    /*
    * Update the scale and the polynomial degree
    */

  //  result->scale = slweIn->scale;
  //  result->poly_degree = tlwe_size-1;

    /*
    * Clean up
    */

    // arrays
    free(as);
}




// a specialized version with 4 inputs
EXPORT void TLweFunctions::KeySwitch_Id_Ind(
  TLweSample * result,
  const TLweKeySwitchKey* ks,
  LweSample * slweIn1,
  LweSample * slweIn2,
  LweSample * slweInd1,
  LweSample * slweInd2,
  const int tlwe_size)
{
  // Parameters
  const TLweParams * params = ks->out_params;
  const int n = ks->n;
  const int basebit=ks->basebit;
  const int t=ks->t;
  const int N = params->N;

  // Check that we can actually do the key switch
  if (tlwe_size > N)
    std::cerr << "ERROR: Number of inputs for the keyswitch too big" << '\n';

  // c' = (0,0)
  tLweClear(result, params);

  // Fill the b values
  for (int i = 0; i < tlwe_size/4; i++)
    result->b->coefsT[i] = slweIn1->b;
  for (int i = tlwe_size/4; i < tlwe_size/2; i++)
    result->b->coefsT[i] = slweIn2->b;
  for (int i = tlwe_size/2; i < 3*tlwe_size/4; i++)
    result->b->coefsT[i] = slweInd1->b;
  for (int i = 3*tlwe_size/4; i < tlwe_size; i++)
    result->b->coefsT[i] = slweInd2->b;
  /*
  * We create as, an array of exactly n polynomials of size 2
  * The first coefficient of the polynomial #i is filled with
  * the ith value from all the samples taken in order.
  */
  Torus32 * as1 = (Torus32 *) malloc(sizeof(Torus32)*n);
  Torus32 * as2 = (Torus32 *) malloc(sizeof(Torus32)*n);
  Torus32 * ind1 = (Torus32 *) malloc(sizeof(Torus32)*n);
  Torus32 * ind2 = (Torus32 *) malloc(sizeof(Torus32)*n);

  for (int i = 0; i < n; i++)
    as1[i] = slweIn1->a[i];
  for (int i = 0; i < n; i++)
    as2[i] = slweIn2->a[i];
  for (int i = 0; i < n; i++)
    ind1[i] = slweInd1->a[i];
  for (int i = 0; i < n; i++)
    ind2[i] = slweInd2->a[i];


  TLweFunctions::KeySwitchTranslate_fromArray(
    result,
    (const TLweSample **) ks->ks,
    params, as1, as2, ind1,ind2,  n, t, basebit, tlwe_size);

  /*
  * Update the scale and the polynomial degree
  */

 // result->scale = slweIn1->scale;
 // result->poly_degree = tlwe_size-1;

  /*
  * Clean up
  */

  // arrays
  free(as1);
  free(as2);

}
