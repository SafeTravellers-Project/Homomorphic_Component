#include "fhe_argmax.h"

/*
* Create the delta matrix from the labels matrix
*/

//template<typename TORUS>
LweSample *** fheArgmax::compute_DeltaMatrix(LweSample ** lweSummedLabels, int d, LweBootstrappingKeyFFT * boot_key_fft, const LweParams * init_lwe_params, const LweParams * out_lwe_params, int print_prog)
{
  LweSample *** lweDeltas = (LweSample ***) malloc(sizeof(LweSample **)*d);
  for (int i = 0; i < d; i++)
  {
    lweDeltas[i] = (LweSample **) malloc(sizeof(LweSample *)*d);
    for (int j = 0; j < d; j++)
      lweDeltas[i][j] = new_LweSample(out_lwe_params);
  }

  // Here the modulus is set in the case where we can sum everything once: d < m
  // We have m ~ 65
  int delta_modulus = 4*d-4;
  Torus32 positive = modSwitchToTorus32(1, delta_modulus);
  Torus32 negative = modSwitchToTorus32(-1, delta_modulus);

  // Set this one to 1/modulus for rotation purposes
  LweSample * lweRotation = new_LweSample(out_lwe_params);
  lweNoiselessTrivial(lweRotation, positive, out_lwe_params);

  // To store the difference of two values
  LweSample * lweDifference = new_LweSample(init_lwe_params);

  // For progression percentage
  int pos;
  float progress;
  int total_loops;
  int nb_loops;
  const int barWidth = 70;

  // For progression percentage
  total_loops = (int) (d*d - d)/2;
  nb_loops = 0;

  for (int i = 0; i < d; i++)
  {
    for (int j = 0; j < i; j++)
    {
      // Compute label_i - label_j
      lweCopy(lweDifference, lweSummedLabels[i], init_lwe_params);
      lweSubTo(lweDifference, lweSummedLabels[j], init_lwe_params);

      // If label_i < label_j, then label_i - label_j < 0 and delta_{i,j} = 1/modulus
      // Otherwise delta_{i,j} = -1/modulus
      tfhe_bootstrap_woKS_FFT(lweDeltas[i][j], boot_key_fft, negative, lweDifference);

      // To print the progression
      if (print_prog == 1)
      {
        nb_loops += 1;
        progress = (float) nb_loops / total_loops;
        std::cout << "[";
        pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
      }
    }
  }
  if (print_prog == 1)
    std::cout << std::endl;   // For progression percentage

  // Fill the other half of the matrix with the opposite
  for (int i = 0; i < d; i++)
  {
    for (int j = i+1; j < d; j++)
      lweNegate(lweDeltas[i][j], lweDeltas[j][i], out_lwe_params);
  }

  // Add 1/modulus (now values are 2/modulus and 0)
  for (int i = 0; i < d; i++)
  {
    for (int j = 0; j < d; j++)
    {
      if (i != j)
        lweAddTo(lweDeltas[i][j], lweRotation, out_lwe_params);
    }
  }

  delete lweRotation;
  delete lweDifference;

  /*
  * Return
  */

  return lweDeltas;
}

//template<typename TORUS>
void fheArgmax::delete_DeltaMatrix(LweSample *** lweDeltas, int d)
{
  for (int i = d-1; i >= 0; i--)
  {
    for (int j = 0; j < d; j++)
      delete lweDeltas[i][j];
    free(lweDeltas[i]);
  }
}

//template<typename TORUS>
LweSample * fheArgmax::argmax_from_DeltaMatrix(LweSample *** lweDeltas, int d, LweBootstrappingKeyFFT * boot_key_fft2, const LweParams * init_lwe_params, const LweParams * out_lwe_params)
{
  /**
  * Initializing tfhe shells
  */

  // The TFHE argmax results
  LweSample * lweArgmax = new_LweSample_array(d, init_lwe_params);

  // The TFHE summed deltas
  LweSample ** lweSummedDeltas = (LweSample **) malloc(sizeof(LweSample *)*d);
  for (int i = 0; i < d; i++)
    lweSummedDeltas[i] = new_LweSample(out_lwe_params);

  // Set this one to 1/modulus for rotation purposes
  LweSample * lweRotation = new_LweSample(out_lwe_params);

  /**
  * Summing the deltas together into lweSummedDeltas
  */

  // Here the modulus is set in the case where we can sum everything once: d < m
  // We have m ~ 65
  int delta_modulus = 4*d-4;
  Torus32 positive = modSwitchToTorus32(1, delta_modulus);
  Torus32 negative = modSwitchToTorus32(-1, delta_modulus);
  lweNoiselessTrivial(lweRotation, positive, out_lwe_params);

  // First, initialize with 1/modulus
  for (int j = 0; j < d; j++)
    lweCopy(lweSummedDeltas[j], lweRotation, out_lwe_params);

  // Start summing
  for (int i = 0; i < d; i++)
  {
    for (int j = 0; j < d; j++)
    {
      if (i != j)
        lweAddTo(lweSummedDeltas[j], lweDeltas[i][j], out_lwe_params);

      // if i = j then do nothing
    }
  }

  /**
  * At this point we need to bootstrap every one of them again
  */

  // Here we just need the modulus to be 4
  int final_modulus = 4;
  positive = modSwitchToTorus32(1, final_modulus);
  negative = modSwitchToTorus32(-1, final_modulus);

  for (int i = 0; i < d; i++)
    tfhe_bootstrap_woKS_FFT(lweArgmax+i, boot_key_fft2, positive, lweSummedDeltas[i]);

  /**
  * Rotation for aestetic purposes
  */

  // Set this one to 1/modulus for rotation pruposes
  lweNoiselessTrivial(lweRotation, negative, init_lwe_params);
  for (int i = 0; i < d; i++)
    lweAddTo(lweArgmax+i, lweRotation, init_lwe_params);

  /*
  * Clean up
  */

  // Summed deltas
  for (int i = d-1; i >= 0; i--)
    delete lweSummedDeltas[i];
  free(lweSummedDeltas);

  // The rotation ciphertext
  delete lweRotation;

  /*
  * Return
  */

  return lweArgmax;
}
