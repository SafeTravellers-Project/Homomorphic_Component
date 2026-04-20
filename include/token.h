#pragma once
#include <openssl/sha.h>
#include <string>
#include <seal/seal.h>
#include <sstream>

class TokenGenerator {
public:
    TokenGenerator();
    ~TokenGenerator();

    std::string generateToken(const std::string& personalData, const seal::SecretKey& secretKey);

private:
    // No private members
};