
#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <string>
namespace fs = std::experimental::filesystem;
//std::experimental::
#include "dataIO.h"
using namespace std;
using namespace seal;




VectorValidation dataIO::validateAndNormalize(
    std::vector<float>& vec,        // in/out: modified in place if needed
    int precision,
    uint32_t modulus)
{
    static long int pow10[12] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 
        10000000, 100000000, 1000000000, 10000000000, 100000000000
    };

    VectorValidation result;
    result.is_valid = true;
    int size = vec.size();
    long int scale     = pow10[precision - 1];       // s      = 100
    long int scale_sq  = pow10[2 * (precision - 1)]; // s²     = 10,000
    int64_t  half_p    = (int64_t)modulus / 2;       // p/2    = 30,000

    // -------------------------------------------------------
    // Step 1: Compute raw norm and max component
    // -------------------------------------------------------
    double norm_sq = 0.0;
    float  max_abs = 0.0f;
    for (int i = 0; i < size; i++) {
        norm_sq += (double)vec[i] * vec[i];
        if (std::abs(vec[i]) > max_abs)
            max_abs = std::abs(vec[i]);
    }
    float raw_norm = (float)std::sqrt(norm_sq);
    result.raw_norm = raw_norm;

    // -------------------------------------------------------
    // Step 2: Check if raw components overflow p when scaled
    // -------------------------------------------------------
    // Each raw component scaled: vec[i] * s must fit in [-p/2, p/2]
    float max_scaled_component = max_abs * scale;
    result.max_component_scaled = max_scaled_component;
    if (max_scaled_component > (float)half_p) {
        result.is_valid = false;
        result.error_msg = "Raw component overflow: max_scaled=" 
                         + std::to_string(max_scaled_component)
                         + " > p/2=" + std::to_string(half_p);
        // Rescale so max component fits within p/2 with 20% margin
        float safe_scale = (float)(half_p * 0.8) / max_abs;
        std::cout << "  Rescaling raw vector by " << safe_scale 
                  << " (was scale=" << scale << ")" << std::endl;
        for (int i = 0; i < size; i++)
            vec[i] *= (safe_scale / scale); // adjust to be re-multiplied by scale later
    }

    // -------------------------------------------------------
    // Step 3: Check square sum for readPlaintextSquare
    // -------------------------------------------------------
    // sum = s² * Σvᵢ² = scale_sq * norm_sq
    double square_sum = scale_sq * norm_sq;
    result.square_sum_scaled = (float)square_sum;
    if (square_sum > (double)half_p) {
        result.is_valid = false;
        result.error_msg += " | Square sum overflow: sum=" 
                          + std::to_string(square_sum)
                          + " > p/2=" + std::to_string(half_p);
        // Force L2-normalize so norm becomes 1.0
        // After normalization: square_sum = scale_sq * 1.0 = 10,000 << p/2=30,000
        std::cout << "  L2-normalizing vector (norm was " << raw_norm << ")" << std::endl;
        for (int i = 0; i < size; i++)
            vec[i] /= raw_norm;
        // Recompute norm_sq after normalization
        norm_sq = 1.0;
        square_sum = scale_sq * norm_sq;
    }

    // -------------------------------------------------------
    // Step 4: Check cosine inner product bound
    // -------------------------------------------------------
    // After normalization, max inner product = scale_sq * 1.0 = 10,000
    // This is always safe if Step 3 passed, but verify explicitly
    double max_inner_product = scale_sq * norm_sq; // = scale_sq if normalized
    if (max_inner_product > (double)half_p) {
        result.is_valid = false;
        result.error_msg += " | Cosine inner product overflow: max=" 
                          + std::to_string(max_inner_product)
                          + " > p/2=" + std::to_string(half_p);
    }

    // -------------------------------------------------------
    // Step 5: Final report
    // -------------------------------------------------------
    std::cout << "[validateAndNormalize]"
              << " norm=" << raw_norm
              << " | max_comp_scaled=" << max_scaled_component
              << " | square_sum=" << result.square_sum_scaled
              << " | p/2=" << half_p
              << " | status=" << (result.is_valid ? "OK" : "FIXED")
              << std::endl;
    if (!result.error_msg.empty())
        std::cout << "  Issues fixed: " << result.error_msg << std::endl;

    return result;
}


std::vector<float> dataIO::loadRawVector(std::string path, int size)
{
    std::vector<float> vec(size);
    FILE* f = fopen(path.c_str(), "r");
    if (!f) {
        std::cerr << "ERROR: Could not open file: " << path << std::endl;
        return {};
    }
    for (int i = 0; i < size; i++) {
        if (fscanf(f, "%f ", &vec[i]) != 1) {
            std::cerr << "ERROR: Failed to read float at index " << i << std::endl;
            fclose(f);
            return {};
        }
      if (i < 10)
        cout << "Coefficient: " << i << " : read from file: " << vec[i] << '\n';
    }
    fclose(f);
    return vec;
}

void dataIO::readPlaintext_Int_woMod(seal::Plaintext * plaintext, std::string path, uint32_t modulus)
{
  FILE * f;
  // f = fopen("../data/model", "r");
  const char * char_path = path.c_str();
  f = fopen(char_path, "r");

  // Size of the vector (number of coefficients to read)
  int size;
  fscanf(f, "%d\n", &size);

  // Get an appropriate plaintext for that size
  plaintext->resize(size);
  plaintext->set_zero();

  // Temporary variable to store a coefficient
  int32_t intTemp;

  for (int i = 0; i < size; i++)
  {
    fscanf(f, "%d ", &intTemp);
    while (intTemp < 0)
      intTemp += modulus;
    *plaintext->data(i) = intTemp;
  }
  fclose(f);
}

/* This function makes a plaintext with the "value" stored in the final coefficient of the plaintext and rest others are set to 0 */
void dataIO::makePlaintext(seal::Plaintext * plaintext, int32_t value, int size, uint32_t modulus)
{
  // Get an appropriate plaintext for that size
  plaintext->resize(size);
  plaintext->set_zero();
  // Temporary variable to store a coefficient
  int32_t intTemp = value ;
  *plaintext->data(size-1) = intTemp;
  }



void dataIO::readPlaintext(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert)
{
  // For a quick power of 10
  static long int pow10[12] =
  {
      1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
  };
  // FILE * f;
  // const char * char_path = path.c_str();
  // f = fopen(char_path, "r");
  
  // // Check if file opened successfully
  // if (f == NULL) {
  //   std::cerr << "ERROR: Could not open file: " << path << std::endl;
  //   return;
  // }
  
  std::vector<float> vec = loadRawVector(path, size);
    if (vec.empty()) return;
    validateAndNormalize(vec, precision, modulus);  // fixes in place if needed

  // Get an appropriate plaintext for that size
  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();
  //cout<< "In readPlaintext, the size is: " << size << '\n';
  // Temporary variable to store a coefficient
  float floatTemp;
  int32_t intTemp;
  int coef_place;
  //int32_t max = 0;
  //cout << "Multiplying the floats with  " << pow10[precision-1] << endl;
  
  for (int i = 0; i < size; i++)
  {
    floatTemp = vec[i];
    // int result = fscanf(f, "%f ", &floatTemp);
    // if (result != 1) {
    //   std::cerr << "ERROR: Failed to read float at index " << i << " from file: " << path << std::endl;
    //   fclose(f);
    //   return;
    // }
    if (floatTemp > 0)
      intTemp = (int) (floatTemp * pow10[precision-1] + 0.5);   // The +0.5 is there for rounding and not truncating
    else
      intTemp = (int) (floatTemp * pow10[precision-1] - 0.5);   // The -0.5 is there for rounding and not truncating

    if (abs(intTemp) > modulus)
    {
      std::cout << "WARNING: the coefficient is bigger than the modulus: " << intTemp << '\n';
      return;
    }
    while (intTemp < 0)
      intTemp += modulus;

   // if (intTemp > max)
    //  max = intTemp;

    //if (i < 10)
     // cout << " Coefficient: " << i << " : read from file and converted to int: " << intTemp << '\n';  

    if (invert == 0)
      coef_place = i;
    else
      coef_place = pt_size - i - 1;
    *plaintext->data(coef_place) = intTemp;
  }
 // cout << "The max coefficient is: " << max << '\n';
//  fclose(f);
}


void dataIO::readPlaintextSquare(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus)
{
  // For a quick power of 10
  static long int pow10[12] =
  {
      1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
  };

  // FILE * f;
  // const char * char_path = path.c_str();
  // f = fopen(char_path, "r");
  
  // // Check if file opened successfully
  // if (f == NULL) {
  //   std::cerr << "ERROR: Could not open file: " << path << std::endl;
  //   return;
  // }

  std::vector<float> vec = loadRawVector(path, size);
    if (vec.empty()) return;
    validateAndNormalize(vec, precision, modulus);  // fixes in place if needed
  // Get an appropriate plaintext for that size
  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();

  // Temporary variable to store a coefficient
  float floatTemp,floatTemp2;
  int64_t intTemp;

  // The variable where we will store the sum
  uint32_t sum = 0;
  //*result =0 ;

  //cout<< "In readPlaintext, the size is: " << size << '\n';

  for (int i = 0; i < size; i++)
  {
    // int result = fscanf(f, "%f ", &floatTemp);
    // if (result != 1) {
    //   std::cerr << "ERROR: Failed to read float at index " << i << " from file: " << path << std::endl;
    //   fclose(f);
    //   return;
    // }
    floatTemp = vec[i];
    floatTemp2 = floatTemp*floatTemp;
    // Here we wil multiply by the square of the precision
    if (floatTemp2 > 0)
      intTemp = (int) (floatTemp2 * pow10[2*(precision-1)] + 0.5);   // The +0.5 is there for rounding and not truncating
    else
      intTemp = (int) (floatTemp2 * pow10[2*(precision-1)] - 0.5);   // The +0.5 is there for rounding and not truncating

   // if (i < 5)
   //   cout << " sq Coefficient: " << i << " : read from file and converted to int: " << intTemp << '\n';  

    sum += intTemp;
  }

  if (sum > (int) modulus/2)
  {
    std::cout << "WARNING: the sum of the squares of the coefficients is bigger than the modulus: " <<   '\n';
  //  std::cout << "The sum: " << sum << '\n';
    return;
  }
 // cout << "The sum has been added: " << sum << '\n';
  *plaintext->data(pt_size-1) = sum;
  //fclose(f);
}


void dataIO::readPlaintextCosine(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert)
{
  // For a quick power of 10
  static long int pow10[12] =
  {
      1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000
  };

  // FILE * f;
  // const char * char_path = path.c_str();
  // f = fopen(char_path, "r");
  
  // // Check if file opened successfully
  // if (f == NULL) {
  //   std::cerr << "ERROR: Could not open file: " << path << std::endl;
  //   return;
  // }

std::vector<float> vec = loadRawVector(path, size);
    if (vec.empty()) return;
    validateAndNormalize(vec, precision, modulus);  // fixes in place if needed

  // Get an appropriate plaintext for that size
  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();

  // Temporary variable to store a coefficient
  float floatTemp[size],floatTemp2;
  //int32_t intTemp[size];
  float float_arr[size];
  float Temp_float_sum=0;
  // The variable where we will store the sum
  int32_t finalint[size];
  int coef_place;

  //cout<< "In readPlaintextCosine, the size is: " << size << '\n';
  for (int i = 0; i < size; i++)   // First computing the \sum a_i^2  or \sum b_i^2
  {
    // int result = fscanf(f, "%f ", &floatTemp[i]);
    // if (result != 1) {
    //   std::cerr << "ERROR: Failed to read float at index " << i << " from file: " << path << std::endl;
    //   fclose(f);
    //   return;
    // }
    floatTemp[i] = vec[i];
    floatTemp2 = floatTemp[i]*floatTemp[i];
    
    Temp_float_sum += floatTemp2;

  }
  float sqrt_sum = sqrt(Temp_float_sum);
  if (sqrt_sum == 0)
  {
    std::cout << "WARNING: the sum of the squares of the coefficients is zero, cannot compute cosine similarity. " <<   '\n';
    return;
  }
  for (int i =0 ; i < size ;i++){  // now computing the a_i/\sum a_i^2 or b_i/\sum b_i^2
    {
    float_arr[i] = (floatTemp[i]/(float) sqrt_sum);}
    if (float_arr[i] > 0)
      finalint[i] = (int) (float_arr[i] * pow10[precision-1] + 0.5);   // The +0.5 is there for rounding and not truncating
    else
      finalint[i] = (int) (float_arr[i] * pow10[precision-1] - 0.5);   // The -0.5 is there for rounding and not truncating

    if (abs(finalint[i]) > (int) modulus/2)
    {
      std::cout << "WARNING: the coefficient is bigger than the modulus: " << finalint[i] << '\n';
      return;
    }

    while (finalint[i] < 0)
      finalint[i] += modulus;

    if (invert == 0)
      coef_place = i;
    else
      coef_place = pt_size - i - 1;

    *plaintext->data(coef_place) = finalint[i];
    }
  //fclose(f);

}

void dataIO::addPlaintext(seal::Plaintext * plaintext1, seal::Plaintext * plaintext2,seal::Plaintext * destination, int size, uint32_t modulus) //std::string path, int size, int precision, uint32_t modulus)
{
  destination->resize(size);
  destination->set_zero();

  int intTemp1, intTemp2;
  intTemp1 = *plaintext1->data(size-1);
  intTemp2 = *plaintext2->data(size-1);
  std::cout << "priting in add_ptx "<< intTemp1 <<" " << intTemp2<< '\n';
  *destination->data(size-1) = intTemp1 +intTemp2;

}

void dataIO::mulPlaintext(seal::Plaintext * plaintext1, seal::Plaintext * plaintext2,seal::Plaintext * destination, int size, uint32_t modulus) //std::string path, int size, int precision, uint32_t modulus)
{
  destination->resize(size);
  destination->set_zero();
  uint32_t p = modulus ;
  int intTemp1, intTemp2,prodTemp,sum;
  sum =0;
  int flag1 =0;
  int flag2 =0;
  //std::cout << "Sum step wise:" << '\n';
  for (int i =0 ;i < size; i++){
      prodTemp=0;
      intTemp1 = *plaintext1->data(i);
      intTemp2 = *plaintext2->data(size-1-i);
      if (intTemp1 > p/2)
        {intTemp1 = p-intTemp1; flag1=1;}
      if (intTemp2 > p/2)
        {intTemp2 = p-intTemp2; flag2=1;}
      //std::cout << intTemp1 << " " << intTemp2 << '\n';
      if ((flag1 == 0 && flag2==0) || (flag1 == 1 && flag2==1))
        prodTemp = (-2)*intTemp1*intTemp2;
      else   prodTemp = 2*intTemp1*intTemp2;
      sum = sum + prodTemp;
      //std::cout << sum << '\n';
  }

  *destination->data(size-1) = sum;

}

//this function takes in a file and then encrypts the plaintext read from the file into 3 types of ciphertexts we need and saves them to path_to
void dataIO::EncryptF2F(std::string path_to, std::string path_f, seal::Encryptor & encryptor, int vector_size, int precision, uint32_t modulus, int invert)
{

Plaintext val;
Plaintext valSquare;
Plaintext valCos;

cout << "Reading the plaintext from file..." << '\n';

dataIO::readPlaintext(&val, path_f, vector_size, precision, modulus, invert);  
dataIO::readPlaintextSquare(&valSquare, path_f, vector_size, precision, modulus);
dataIO::readPlaintextCosine(&valCos, path_f, vector_size, precision, modulus,invert);


Ciphertext SEALCipher;
Ciphertext SEALCipherSquare;
Ciphertext SEALCosineCipher;

cout << "Encrypting the plaintexts..." << '\n';
encryptor.encrypt(val, SEALCipher);
encryptor.encrypt(valSquare, SEALCipherSquare);
encryptor.encrypt(valCos, SEALCosineCipher);
cout << "Encryption done!" << '\n';

//cout << "The path is " << path_to << '\n';
//Writing the ciphertexts to file
filebuf bin_sealciph_handler;
bin_sealciph_handler.open(path_to + "ciph", std::ios::out | ios::binary);
 if (!bin_sealciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for writing cipher!");
 }
std::ostream os(&bin_sealciph_handler);
SEALCipher.save(os);
bin_sealciph_handler.close();

filebuf bin_sealsqciph_handler;
bin_sealsqciph_handler.open(path_to + "sqciph", std::ios::out | ios::binary);
 if (!bin_sealsqciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for writing cipher square!");
}
std::ostream os2(&bin_sealsqciph_handler);
SEALCipherSquare.save(os2);
bin_sealsqciph_handler.close();

filebuf bin_sealcosineciph_handler;
bin_sealcosineciph_handler.open(path_to+"cosineciph", std::ios::out | ios::binary);
 if (!bin_sealcosineciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for writing cipher cosine!");
 }
std::ostream os3(&bin_sealcosineciph_handler);
SEALCosineCipher.save(os3);
bin_sealcosineciph_handler.close();

}

void dataIO::Read_F(std::string path,
seal::SEALContext &context,seal::Ciphertext &outCipher,seal::Ciphertext &outCipherSquare,seal::Ciphertext &outCosine)
{
  filebuf bin_sealciph_handler;
//  cout<< "The path is " << path << '\n';
bin_sealciph_handler.open(path+"ciph", ios::in | ios::binary);
istream is2(&bin_sealciph_handler);
 if (!bin_sealciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for reading cipher!");
 }
outCipher.load(context,is2);
bin_sealciph_handler.close();

filebuf bin_sealsqciph_handler;
bin_sealsqciph_handler.open(path+"sqciph", ios::in | ios::binary);
istream is3(&bin_sealsqciph_handler);
if (!bin_sealsqciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for reading cipher square!");
 }
outCipherSquare.load(context,is3);  
bin_sealsqciph_handler.close();


filebuf bin_sealcosineciph_handler;
bin_sealcosineciph_handler.open(path+"cosineciph", ios::in | ios::binary);
istream is4(&bin_sealcosineciph_handler);
if (!bin_sealcosineciph_handler.is_open()) {
     throw std::runtime_error("Error opening file for reading cosine cipher!");
 }
outCosine.load(context,is4);  
bin_sealcosineciph_handler.close();

}
