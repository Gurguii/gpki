#pragma once
#include "gpki.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

enum class ACTION {
#define GPKI_ACTION uint8_t
  none = 0x00,
#define ACTION_NONE static_cast<uint8_t>(ACTION::none)
  init_pki = 0x01,
#define ACTION_INIT_PKI static_cast<uint8_t>(ACTION::init_pki)
  build_ca = 0x03,
#define ACTION_BUILD_CA static_cast<uint8_t>(ACTION::build_ca)
  build_server = 0x05,
#define ACTION_BUILD_SERVER static_cast<uint8_t>(ACTION::build_server)
  build_client = 0x07
#define ACTION_BUILD_CLIENT static_cast<uint8_t>(ACTION::build_client)
};

class Globals {
public:
  /* Metainfo */
  static std::string base_dir;
  static std::string profiles_file;
  static std::string config_dir;

  /* Generic variables */
  static std::string profile_name;
  static std::vector<std::string> subopts;

  /* PKI variables */
  static int keysize;

  // ACTIONS
  static uint8_t action;

  // OPTIONS

  /* Methods */
  static void Initialize();
};
