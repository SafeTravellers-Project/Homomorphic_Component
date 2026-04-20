#pragma once
#include <stdlib.h>
#include <fstream>
#include "string.h"
#include <random>
//SEAL
#include "seal/seal.h"
//TFHE
#include "tfhe.h"
#include "scaled_tfhe.h"
#include "more_tfhe.h"



struct UserKeyGen
{
    static void generateUserKeys();
};

// struct UserID
// {
//     static void generateID();
// };