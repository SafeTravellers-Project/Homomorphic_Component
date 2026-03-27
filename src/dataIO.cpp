
#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
namespace fs = std::experimental::filesystem;
//std::experimental::
#include "dataIO.h"
using namespace std;
using namespace seal;



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
  static long int pow10[12] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000 };

  FILE * f = fopen(path.c_str(), "r");
  if (!f) {
    throw std::runtime_error("Cannot open embedding file: " + path);
  }

  // Read and validate header line
  char header[256];
  if (!fgets(header, sizeof(header), f)) {
    fclose(f);
    throw std::runtime_error("Empty embedding file: " + path);
  }
  header[strcspn(header, "\r\n")] = 0;
  if (std::string(header) != "EMBEDDING BEGINS") {
    fclose(f);
    throw std::runtime_error("Bad embedding header in file: " + path);
  }

  // Get an appropriate plaintext for that size
  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();

  float floatTemp;
  int32_t intTemp;
  int coef_place;

  for (int i = 0; i < size; i++)
  {
    int rc = fscanf(f, "%f", &floatTemp);
    if (rc != 1) {
      fclose(f);
      throw std::runtime_error("Embedding parse error in file: " + path + " at index " + std::to_string(i));
    }

    if (floatTemp > 0)
      intTemp = (int) (floatTemp * pow10[precision-1] + 0.5);
    else
      intTemp = (int) (floatTemp * pow10[precision-1] - 0.5);

    if (abs(intTemp) > modulus)
    {
      fclose(f);
      throw std::runtime_error("WARNING: the coefficient is bigger than the modulus in file: " + path + " value=" + std::to_string(intTemp));
    }
    while (intTemp < 0)
      intTemp += modulus;

    if (invert == 0)
      coef_place = i;
    else
      coef_place = pt_size - i - 1;
    *plaintext->data(coef_place) = intTemp;
  }
  fclose(f);
}


void dataIO::readPlaintextSquare(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus)
{
  // For a quick power of 10
  static long int pow10[12] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000 };

  FILE * f = fopen(path.c_str(), "r");
  if (!f) {
    throw std::runtime_error("Cannot open embedding file: " + path);
  }

  // Read and validate header line
  char header[256];
  if (!fgets(header, sizeof(header), f)) {
    fclose(f);
    throw std::runtime_error("Empty embedding file: " + path);
  }
  header[strcspn(header, "\r\n")] = 0;
  if (std::string(header) != "EMBEDDING BEGINS") {
    fclose(f);
    throw std::runtime_error("Bad embedding header in file: " + path);
  }

  // Get an appropriate plaintext for that size
  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();

  float floatTemp,floatTemp2;
  int64_t intTemp;

  uint32_t sum = 0;

  for (int i = 0; i < size; i++)
  {
    int rc = fscanf(f, "%f", &floatTemp);
    if (rc != 1) {
      fclose(f);
      throw std::runtime_error("Embedding parse error in file: " + path + " at index " + std::to_string(i));
    }
    floatTemp2 = floatTemp*floatTemp;

    if (floatTemp2 > 0)
      intTemp = (int) (floatTemp2 * pow10[2*(precision-1)] + 0.5);
    else
      intTemp = (int) (floatTemp2 * pow10[2*(precision-1)] - 0.5);

    sum += intTemp;
  }

  if (sum > modulus)
  {
    fclose(f);
    throw std::runtime_error("WARNING: sum of squares bigger than modulus in file: " + path);
  }

  *plaintext->data(pt_size-1) = sum;
  fclose(f);
}


void dataIO::readPlaintextCosine(seal::Plaintext * plaintext, std::string path, int size, int precision, uint32_t modulus, int invert)
{
  // For a quick power of 10
  static long int pow10[12] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000 };

  FILE * f = fopen(path.c_str(), "r");
  if (!f) {
    throw std::runtime_error("Cannot open embedding file: " + path);
  }

  // Read and validate header line
  char header[256];
  if (!fgets(header, sizeof(header), f)) {
    fclose(f);
    throw std::runtime_error("Empty embedding file: " + path);
  }
  header[strcspn(header, "\r\n")] = 0;
  if (std::string(header) != "EMBEDDING BEGINS") {
    fclose(f);
    throw std::runtime_error("Bad embedding header in file: " + path);
  }

  int pt_size = 4096;
  plaintext->resize(pt_size);
  plaintext->set_zero();

  std::vector<float> floatTemp(size);
  std::vector<float> float_arr(size);
  std::vector<int32_t> finalint(size);

  float floatTemp2;
  float Temp_float_sum=0;
  int coef_place;

  for (int i = 0; i < size; i++)
  {
    int rc = fscanf(f, "%f", &floatTemp[i]);
    if (rc != 1) {
      fclose(f);
      throw std::runtime_error("Embedding parse error in file: " + path + " at index " + std::to_string(i));
    }
    floatTemp2 = floatTemp[i]*floatTemp[i];
    Temp_float_sum += floatTemp2;
  }

  for (int i =0 ; i < size ;i++){
    float_arr[i] = (floatTemp[i]/(float) sqrt(Temp_float_sum));
    if (float_arr[i] > 0)
      finalint[i] = (int) (float_arr[i] * pow10[precision-1] + 0.5);
    else
      finalint[i] = (int) (float_arr[i] * pow10[precision-1] - 0.5);

    if (abs(finalint[i]) > modulus)
    {
      fclose(f);
      throw std::runtime_error("WARNING: coefficient bigger than modulus in file: " + path + " value=" + std::to_string(finalint[i]));
    }

    while (finalint[i] < 0)
      finalint[i] += modulus;

    if (invert == 0)
      coef_place = i;
    else
      coef_place = pt_size - i - 1;

    *plaintext->data(coef_place) = finalint[i];
  }

  fclose(f);
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

  dataIO::readPlaintext(&val, path_f, vector_size, precision, modulus, invert);
  dataIO::readPlaintextSquare(&valSquare, path_f, vector_size, precision, modulus);
  dataIO::readPlaintextCosine(&valCos, path_f, vector_size, precision, modulus,invert);


  Ciphertext SEALCipher;
  Ciphertext SEALCipherSquare;
  Ciphertext SEALCosineCipher;

  encryptor.encrypt(val, SEALCipher);
  encryptor.encrypt(valSquare, SEALCipherSquare);
  encryptor.encrypt(valCos, SEALCosineCipher);

  //Writing the ciphertexts to file
  filebuf bin_sealciph_handler;
  bin_sealciph_handler.open(path_to + "ciph", std::ios::out | ios::binary);
  if (!bin_sealciph_handler.is_open()) {
    throw std::runtime_error("Error opening file for writing  cipher!");
  }
  std::ostream os(&bin_sealciph_handler);
  SEALCipher.save(os);
  bin_sealciph_handler.close();

  filebuf bin_sealsqciph_handler;
  bin_sealsqciph_handler.open(path_to+"sqciph", std::ios::out | ios::binary);
  if (!bin_sealsqciph_handler.is_open()) {
    throw std::runtime_error("Error opening file for writing cipher square!");
  }
  std::ostream os2(&bin_sealsqciph_handler);
  SEALCipherSquare.save(os2);
  bin_sealsqciph_handler.close();

  filebuf bin_sealcosineciph_handler;
  bin_sealcosineciph_handler.open(string(path_to)+"cosineciph", std::ios::out | ios::binary);
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
  bin_sealciph_handler.open(path+"/ciph", ios::in | ios::binary);
  istream is2(&bin_sealciph_handler);
  if (!bin_sealciph_handler.is_open()) {
    throw std::runtime_error("Error opening file for writing  cipher!");
  }
  outCipher.load(context,is2);
  bin_sealciph_handler.close();

  filebuf bin_sealsqciph_handler;
  bin_sealsqciph_handler.open(path+"/sqciph", ios::in | ios::binary);
  istream is3(&bin_sealsqciph_handler);
  outCipherSquare.load(context,is3);
  bin_sealsqciph_handler.close();


  filebuf bin_sealcosineciph_handler;
  bin_sealcosineciph_handler.open(path+"/cosineciph", ios::in | ios::binary);
  istream is4(&bin_sealcosineciph_handler);
  outCosine.load(context,is4);
  bin_sealcosineciph_handler.close();

}
