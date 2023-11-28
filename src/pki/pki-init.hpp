#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "../profiles/profiles.hpp"
namespace gpki
{
void pki_init();
void pki_init(const char *profile_name);
void pki_init(const char *profile_name, const char *base_dir);
}
