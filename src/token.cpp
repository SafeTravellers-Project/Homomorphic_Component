#include <iostream>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include "tfhe.h"
#include "token.h"


#include "seal/seal.h"
#include <sstream>


TokenGenerator::TokenGenerator() {
    // // No initialization needed
}

TokenGenerator::~TokenGenerator() {
    // // No cleanup neededcd
}

std::string TokenGenerator::generateToken(const std::string& personalData, const seal::SecretKey& secretKey) {
    
    // Serialize the secret key to a string
    std::stringstream keyStream;
    secretKey.save(keyStream);
    std::string keyString = keyStream.str();

    // Combine personal data and serialized key
    std::string combinedData = personalData + keyString;

    // Hash the combined data using SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)combinedData.c_str(), combinedData.length(), hash);

    // Convert to hex string
    std::stringstream ss;
    for (unsigned char c : hash) {
        ss << std::hex << (int)c;
    }
    return ss.str();
}


/*std::string generateToken1(const std::string& personalData, const std::string& secretKey) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    HMAC(EVP_sha256(), secretKey.c_str(), secretKey.length(),
         (unsigned char*)personalData.c_str(), personalData.length(), hash, nullptr);
}
*/

/*
///// An exmaple of how it might work

    std::string personalData = "user@example.com"; // Use a stable identifier
    std::string secretKey = "SuperSecretKey123"; // Keep this secure!

    std::string token = generateToken(personalData, secretKey);
    std::cout << "Generated Token: " << token << std::endl;

*/

