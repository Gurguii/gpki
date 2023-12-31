#include "globals.hpp"
#include "help/usage.cpp"
#include "pki/build_certificates.hpp"
#include "pki/init-pki.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace gpki {
int Parse(int &argc, const char **&_args) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.push_back(_args[i]);
  }

  if (args.size() == 0) {
    // No arguments supplied
    usage();
    return -1;
  }

  // Check if user asking for help
  for (const std::string &opt : args) {
    if (opt == "-h" || opt == "--help") {
      usage();
      return -1;
    }
  }

  // Check if profiles database exists
  if (!std::filesystem::exists(Globals::profiles_db)) {
    std::cout << "[info] Creating file '" << Globals::profiles_db << "'";
    // db file does not exist, create it
    std::ofstream(Globals::profiles_db, std::ios::app);
    if (!std::filesystem::exists(Globals::profiles_db)) {
      std::cerr << " - ERROR\n";
      return -1;
    } else {
      std::cout << " - OK\n";
    }
  }

  // Initialize sqlite3 database
  db::initialize(Globals::profiles_db.c_str());

  // Check if options have an action that does not require profile name
  // (init-pki, profiles-list)
  for (const std::string &st : args) {
    // I always forget :)
    if (st == "init-pki" || st == "pki-init") {
      Globals::action = pki_init;
      Globals::subopts = std::vector<std::string>(args.begin() + 1, args.end());
      return 0;
    } else if (st == "profiles-list") {
      Globals::action = db::display_profiles;
      return 0;
    }
  }

  if (args.size() == 1) {
    usage();
    return -1;
  }

  // Store desired action + profile
  std::string_view action = args[0];
  std::string_view profile = args[1];

  // Check profile
  if (!db::profile_exists(profile)) {
    // Profile not found
    std::cerr << "[error] Profile '" << profile << "' not found\n";
    return -1;
  }
  Globals::subopts = std::vector<std::string>(args.begin() + 2, args.end());
  // used to ensure that Globals::subopts[++i] will
  // never give std::out_of_range exception
  Globals::subopts.push_back("\0");

  // Profile exists, populate global ProfileInfo
  db::populate_ProfileInfo(profile, Globals::profile);

  // Set action
  if (action == "build-ca") {
    Globals::action = build_ca;
  } else if (action == "build-server") {
    Globals::action = build_server;
    Globals::x509_extension = X509_SERVER;
  } else if (action == "build-client") {
    Globals::action = build_client;
    Globals::x509_extension = X509_CLIENT;
  } else if (action == "profile-remove") {
    Globals::action = Globals::DeleteProfile;
  } else if (action == "profile-info") {
    Globals::action = Globals::SelectProfile;
  }

  // Parse subopts
  for (int i = 0; i < Globals::subopts.size(); ++i) {
    std::string_view opt = Globals::subopts[i];
    if (opt[0] != '-' && opt[0] != '\x00') {
      std::cout << "ignoring wrong option " << opt << "\n";
      continue;
    }
    if (opt == "-ks" || opt == "--keysize") {
      Globals::keysize = strtol(&Globals::subopts[++i][0], nullptr, 10);
    } else if (opt == "-alg" || opt == "--key-algorithm") {
      Globals::keyalgorithm = Globals::subopts[++i];
    } else if (opt == "-format" || opt == "--outformat") {
      Globals::outformat = Globals::subopts[++i];
    } else if (opt == "-dh" || opt == "--dhparam") {
      Globals::dhparam_keysize = strtol(&Globals::subopts[++i][0], nullptr, 10);
    } else if (opt == "-tls" || opt == "--tls-size") {
      Globals::tls_keysize = strtol(&Globals::subopts[++i][0], nullptr, 10);
    }
  }
  return 0;
}
} // namespace gpki
