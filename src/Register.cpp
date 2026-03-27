#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <time.h>
#include <filesystem>

// SEAL
#include "seal/seal.h"

// TFHE
#include "tfhe.h"

#include "utils.h"
#include "dataIO.h"
#include "token.h"
#include "keysUser.h"

using namespace std;
using namespace seal;
namespace fs = std::filesystem;

static bool looks_like_embedding_file(const fs::path &p)
{
  // Fast filters first
  std::error_code ec;
  if (!fs::is_regular_file(p, ec) || ec) return false;

  // Skip very large files (e.g., KSK.data) to avoid accidental binary parsing
  auto sz = fs::file_size(p, ec);
  if (!ec && sz > 50ULL * 1024ULL * 1024ULL) { // 50 MiB
    return false;
  }

  // Check first line for the expected header
  std::ifstream in(p);
  if (!in) return false;
  std::string line;
  if (!std::getline(in, line)) return false;

  // Trim CRLF
  while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();

  return line == "EMBEDDING BEGINS";
}

int main(int argc, char *argv[])
{
  try {
    if (argc < 3) {
      std::cerr << "Usage: " << argv[0]
      << " <Input Registration Biometric folder> <Output Registration Biometric folder>\n";
      return 1;
    }

    // Generating User Keys
    cout << "Generating User Keys..." << endl;
    UserKeyGen::generateUserKeys();

    // Resolve data directory relative to the executable location (works no matter where you run it from)
    fs::path exe_path = fs::weakly_canonical(fs::path(argv[0]));
    fs::path exe_dir  = exe_path.parent_path();
    fs::path data_dir = fs::weakly_canonical(exe_dir / ".." / "data");

    // Read the SEAL parameters from the file
    fs::path sealparam_path = data_dir / "System_Parameters" / "sealparam.txt";
    std::ifstream bin_sealparam_handler(sealparam_path, std::ios::in | std::ios::binary);
    if (!bin_sealparam_handler) {
      std::cerr << "Seal param file did not open: " << sealparam_path << std::endl;
      return 1;
    }

    EncryptionParameters parms(scheme_type::bfv);
    parms.load(bin_sealparam_handler);
    bin_sealparam_handler.close();

    SEALContext context(parms);

    // Load public key
    fs::path sealpubk_path = data_dir / "Traveller" / "sealpubK.txt";
    PublicKey init_public_key;
    std::filebuf bin_sealpubK_handler;
    if (!bin_sealpubK_handler.open(sealpubk_path, ios::in | ios::binary)) {
      std::cerr << "Seal public key did not open: " << sealpubk_path << std::endl;
      return 1;
    }
    {
      istream is(&bin_sealpubK_handler);
      init_public_key.load(context, is);
    }
    bin_sealpubK_handler.close();

    Encryptor init_encryptor(context, init_public_key);

    // Some necessary parameters for the Biometrics
    const int N = (int)parms.poly_modulus_degree();
    const uint64_t p = parms.plain_modulus().value();
    const int precision = 3;
    const int vector_size = 1024;
    if (vector_size > N) {
      std::cerr << "The size of N has to be greater than that of the Biometrics vectors\n";
      return 1;
    }

    cout << "Reading the Biometrics, encrypting and registering" << endl;

    fs::path input_dir  = fs::path(argv[1]);
    fs::path output_dir = fs::path(argv[2]);

    // Check if input folder exists
    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
      std::cerr << "Error: Input folder does not exist or is not a directory: " << input_dir << std::endl;
      return 1;
    }

    // Ensure output folder exists
    std::error_code ec;
    fs::create_directories(output_dir, ec);
    if (ec) {
      std::cerr << "Error: Cannot create output folder: " << output_dir << " (" << ec.message() << ")\n";
      return 1;
    }

    struct BioItem { std::string name; fs::path fullpath; };
    std::vector<BioItem> biometric_files;

    for (const auto &entry : fs::directory_iterator(input_dir)) {
      const fs::path pth = entry.path();
      if (!looks_like_embedding_file(pth)) {
        continue;
      }
      biometric_files.push_back({ pth.filename().string(), pth });
    }

    if (biometric_files.empty()) {
      std::cerr << "Error: No embedding files found in input folder: " << input_dir << std::endl;
      std::cerr << "Hint: files must start with a first line: 'EMBEDDING BEGINS'\n";
      return 1;
    }

    cout << "Found " << biometric_files.size() << " biometric file(s) in " << input_dir << ":\n";
    for (const auto &item : biometric_files) {
      cout << "  - " << item.name << "\n";
    }
    cout << endl;

    for (const auto &item : biometric_files) {
      cout << "Processing: " << item.name << endl;

      // Create an output sub-directory per biometric file (so EncryptF2F writes /ciph, /sqciph, /cosineciph)
      fs::path out_bio_dir = output_dir / (item.name + "_enc");
      fs::create_directories(out_bio_dir, ec);
      if (ec) {
        throw std::runtime_error("Cannot create output directory: " + out_bio_dir.string() + " (" + ec.message() + ")");
      }

      // Ensure trailing slash because EncryptF2F uses path_to + "ciph"
      std::string out_prefix = out_bio_dir.string();
      if (!out_prefix.empty() && out_prefix.back() != '/') out_prefix.push_back('/');

      dataIO::EncryptF2F(out_prefix, item.fullpath.string(), init_encryptor,
                         vector_size, precision, (uint32_t)p, 0);

      cout << "  ✓ Successfully encrypted and registered\n";
    }

    cout << "***************************************************\n";
    cout << "*     Successfully Registered User Biometrics     *\n";
    cout << "***************************************************\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << "\n";
    return 1;
  }
}
