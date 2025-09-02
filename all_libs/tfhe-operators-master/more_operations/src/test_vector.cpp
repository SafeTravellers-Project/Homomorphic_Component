#include "test_vector.h"

// ------------------------- FULL TORUS -----------------------------------

// f(x) = x

void FTestVector::id(TorusPolynomial* testvect,
    const TLweParams* tlwe_params,
    int plaintext_size,
    int rescale)
{FTestVector::id
  int temp_chunk_size;

  int plaintext_sizex2 = 2*plaintext_size;

  // Get the rest
  int rest = N%plaintext_size;

  // The real chunk size, can be chunk_size+1
  int real_chunk_size;

  // The input/output values
  double doubleIn;
  double doubleOut;

  // Fill in every chunk with size chunk_size+1 for the first "rest" chunks
  int index=0;
  for (int i=0;i<plaintext_size;i++)
  {
    // Initialize input
    doubleIn = (double) i/plaintext_sizex2;
     // The function is entered here
    doubleOut = doubleIn;
    // Rescaling
    doubleOut = doubleOut/rescale;

    if (i < rest)
      real_chunk_size = chunk_size+1;
    else
      real_chunk_size = chunk_size;

    for (size_t j = 0; j < real_chunk_size; j++)
    {
      testvect->coefsT[index] = dtot32(doubleOut);
      index++;
    }
  }
}

// f(x) = x - 1/4

void FTestVector::sub_fourth(TorusPolynomial* testvect,
    const TLweParams* tlwe_params,
    int plaintext_size,
    int rescale)
{
  // Get parameters
  const int N=tlwe_params->N;

  // Get the size of the chunks
  int chunk_size = N/plaintext_size;
  int temp_chunk_size;

  int plaintext_sizex2 = 2*plaintext_size;

  // Get the rest
  int rest = N%plaintext_size;

  // The real chunk size, can be chunk_size+1
  int real_chunk_size;

  // The input/output values
  double doubleIn;
  double doubleOut;

  // Fill in every chunk with size chunk_size+1 for the first "rest" chunks
  int index=0;
  for (int i=0;i<plaintext_size;i++)
  {
    // Initialize input
    doubleIn = (double) i/plaintext_sizex2;
     // The function is entered here
    doubleOut = doubleIn - 0.25;
    // Rescaling
    doubleOut = doubleOut/rescale;

    if (i < rest)
      real_chunk_size = chunk_size+1;
    else
      real_chunk_size = chunk_size;

    for (size_t j = 0; j < real_chunk_size; j++)
    {
      testvect->coefsT[index] = dtot32(doubleOut);
      index++;
    }
  }
}

// ------------------------- HALF TORUS -----------------------------------

// f(x) = x

void HTestVector::id(TorusPolynomial* testvect,
    const TLweParams* tlwe_params,
    int plaintext_size,
    int rescale)
{
  // Get parameters
  const int N=tlwe_params->N;

  // Get the size of the chunks
  int chunk_size = N/plaintext_size;
  int temp_chunk_size;

  int plaintext_sizex2 = 2*plaintext_size;

  // Get the rest
  int rest = N%plaintext_size;

  // The real chunk size, can be chunk_size+1
  int real_chunk_size;

  // The input/output values
  double doubleIn;
  double doubleOut;

  // Fill in every chunk with size chunk_size+1 for the first "rest" chunks
  int index=0;
  for (int i=0;i<plaintext_size;i++)
  {
    // Initialize input
    doubleIn = (double) (i - plaintext_size/2)/plaintext_sizex2 ;    // in [-1/4,1/4]
     // The function is entered here
    doubleOut = doubleIn;
    // Rescaling
    doubleOut = doubleOut/rescale;

    if (i < rest)
      real_chunk_size = chunk_size+1;
    else
      real_chunk_size = chunk_size;

    for (size_t j = 0; j < real_chunk_size; j++)
    {
      testvect->coefsT[index] = dtot32(doubleOut);
      index++;
    }
  }
}
