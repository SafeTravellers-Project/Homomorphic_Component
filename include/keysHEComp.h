#pragma once
#include <stdlib.h>
#include <fstream>
#include "params.h"
//TFHE
#include "tfhe.h"

struct HEKeyGen
{
    static void generateHEkeys();
};