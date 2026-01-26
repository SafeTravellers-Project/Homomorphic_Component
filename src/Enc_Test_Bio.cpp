#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <filesystem>
// SEAL
#include "seal/seal.h"
#include "tfhe.h"

#include "dataIO.h"
#include "token.h"
#include "keysEGate.h"

using namespace std;
using namespace seal;   
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]  << " <Input Raw Test Biometric file>"  << " Output Encrypted Test Biometric folder> " << std::endl;
        return 1;
    }
    srand(time(0));

    // Read the SEAL parameters from the file

cout << "Reading SEAL parameters from file..." << endl;

std::ifstream bin_sealparam_handler("../data/System_Parameters/sealparam.txt", std::ios::in | std::ios::binary);
    if (!bin_sealparam_handler) {
        std::cerr << "Seal param file did not open!" << std::endl;
        return 1;
    }

   // Read the parameters from the file
    EncryptionParameters parms(scheme_type::bfv);
    parms.load(bin_sealparam_handler);
    bin_sealparam_handler.close();

    if (!bin_sealparam_handler.good()) {
        std::cerr << "Error occurred during reading the seal param binary file!" << std::endl;
        return 2;
    }
SEALContext context(parms);

PublicKey init_public_key;
 filebuf bin_sealpubK_handler;
  bin_sealpubK_handler.open("../data/Traveller/sealpubK.txt", ios::in | ios::binary); // this needs to be changed for every traveller. For initial testing we are using a predefined key.
    istream is(&bin_sealpubK_handler);
    init_public_key.load(context,is);
    bin_sealpubK_handler.close();

Encryptor init_encryptor(context, init_public_key);
Evaluator evaluator(context);
/**
  * Set the initial parameters
  */
  const int N_seal = parms.poly_modulus_degree();
  const uint64_t p = parms.plain_modulus().value();  // This needs to be changed as per requirement
  const int precision = 3;

   const int vector_size = 1024;
  if (vector_size > N_seal)
  {
    std::cout << "The size of Poly Modulus Degree has to be greater than that of the Biometrics vectors" << '\n';
    return -1;
  }

cout << "SEAL parameters and keys loaded successfully." << endl;


 std::string folderName_model_f = std::string(argv[1]);
  std::string folderName_model_to = std::string(argv[2]);

  // Check if input folder exists
  if (!fs::exists(folderName_model_f)) {
    std::cerr << "Error: Input folder does not exist: " << folderName_model_f << std::endl;
    return 1;
  }
  
  // Read all files from the input folder
  std::vector<std::string> biometric_files;
  for (const auto & entry : fs::directory_iterator(folderName_model_f)) {
    if (fs::is_regular_file(entry)) {
      biometric_files.push_back(entry.path().filename().string());
    }
  }
  
  if (biometric_files.empty()) {
    std::cerr << "Error: No files found in input folder: " << folderName_model_f << std::endl;
    return 1;
  }
  
  // Display found files
  cout << "Found " << biometric_files.size() << " biometric file(s) in " << folderName_model_f << ":" << endl;
  for (const auto & file : biometric_files) {
    cout << "  - " << file << endl;
  }
  cout << endl;

for (const auto & fileName_model_f : biometric_files) {
    cout << "Processing: " << fileName_model_f << endl;
    std::string fileName_model_to = folderName_model_to + "/" + fileName_model_f + "_enc";
    dataIO::EncryptF2F(fileName_model_to, fileName_model_f, init_encryptor, vector_size, precision, p, 0);
    cout << "  ✓ Successfully encrypted " << endl;
  }

}
