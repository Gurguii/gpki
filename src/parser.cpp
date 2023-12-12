#include "globals.hpp"
#include "profiles/profiles.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

/* As reference
 * ./gpki [action] [profile-name] [options]
 * ./gpki build-ca myvpnserver -cn myCA
 * ./gpki init-pki */

/*
 * [ ACTIONS ]
 * IMPLEMENTED - init-pki | build-ca | build-server | build-client
 * NOT IMPLEMENTED - revoke-cert | update-crl | create-pack
 * */

namespace gpki {
int Parse(int argc, const char **&_args) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.push_back(_args[i]);
  }

  if (args.size() == 0) {
    // No arguments supplied
    return -1;
  }

  // Check if user asking for help
  for (const std::string &opt : args) {
    if (opt == "-h" || opt == "--help") {
      return -1;
    }
  }

  // Check if profiles file exists
  if (!std::filesystem::exists(Globals::profiles_file)) {
    std::cout << "Creating file '" << Globals::profiles_file << "'\n";
    // File does not exist, create it
    std::ofstream(Globals::profiles_file, std::ios::app);
    if (!std::filesystem::exists(Globals::profiles_file)) {
      std::cerr << "[error] couldn't create missing .profiles file -> "
                << Globals::profiles_file;
      return -1;
    } else {
      std::cout << Globals::profiles_file << " file created\n";
    }
  }

  // Initialize Profiles static class
  Profiles::Initialize();

  if (args.size() == 1) {
    int rcode = -1;
    // Check if options have an action that does not require profile name
    // (init-pki)
    for (const std::string &st : args) {
      // I always forget :)
      if (st == "init-pki" || st == "pki-init") {
        Globals::action = ACTION_INIT_PKI;
        rcode = 0;
      }
    }
    return rcode;
  }

  // Store desired action + profile
  std::string_view action = args[0];
  std::string_view profile = args[1];

  // Check profile
  if (!Profiles::Find(profile)) {
    // Profile not found
    std::cerr << "[error] Profile '" << profile << "' not found\n";
    return -1;
  }

  Globals::profile_name = profile;
  // Set action
  Globals::action = ((action == "build-ca")       ? ACTION_BUILD_CA
                     : (action == "build-server") ? ACTION_BUILD_SERVER
                     : (action == "build-client") ? ACTION_BUILD_CLIENT
                                                  : ACTION_NONE);
  // ./gpki <action> <profile> [options]
  // action will always be args[1] and profile args[2]
  int pos = 0;
  return 0;
}
} // namespace gpki
