#include "database/database.hpp"
#include "globals.hpp"
#include "pki/entities.hpp"
#include "profiles/profiles.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace gpki;

std::unordered_map<std::string_view, int (*)()> ACTIONS{
    {"build-ca", pki::build_ca},
    {"build-client", pki::build_client},
    {"build-server", pki::build_server},
    {"init-pki", profiles::create},
    {"remove-profile",profiles::remove}};
/*
 * @return -1:error 0:good
 * */
int parse(int argc, const char **args) {
  if (argc == 0) {
    return -1;
  }

  std::vector<std::string> _args(args, args + argc);
  for (auto &st : _args) {
    if (st == "-h" || st == "--help") {
      return -1;
    }
  }

  // Initialize database
  database::initialize();
  // Check if user wants to create a new profile
  for (auto &st : _args) {
    if (st == "init-pki") {
      // create new profile
      setaction(profiles::create);
      return 0;
    }
  }
  if(argc == 1){
    seterror("profile name must be given\n");
    return -1;
  }
  std::string_view action = _args[0];
  std::string_view profile_name = _args[1];

  // Check if action exists
  if (ACTIONS.find(action) == ACTIONS.end()) {
    seterror("Action '" + std::string(action) + "' doesn't exist");
    std::cout << "== Available actions ==\n";
    for (auto &action : ACTIONS) {
      std::cout << action.first << " | ";
    }
    std::cout << "\n";
    return -1;
  }

  // Check if profile exists
  if (!database::profiles::exists(profile_name)) {
    seterror("Profile '" + std::string(profile_name) + "' doesn't exist");
    return -1;
  }
  
  // Set action
  setaction(ACTIONS[action]);
  // Populate global struct Profile with current profile 
  database::profiles::get(profile_name, PROFILE_PTR);

  // Remove first 2 values from _args which where [action] [profile_name]
  _args.erase(_args.begin(), _args.begin() + 2);
  _args.push_back("\0");

  // E.g -> ./gpki build-ca test --keysize 2048
  // at this point, _args will have {"--keysize","2048"}
  // PROFILE will already be populated with the right profile
  // ACTION will be pointing to the right function

  for (int i = 0; i < _args.size(); ++i) {
    std::string_view opt = _args[i];
    if (opt == "-keysize") {
      globals::pki::key_size = strtol(&_args[++i][0], nullptr, 10);
    } else if (opt == "-inform") {
      globals::pki::input_format = _args[++i];
    } else if (opt == "-outform") {
      globals::pki::output_format = _args[++i];
    } else if (opt == "-days") {
      globals::pki::certificate_days = strtol(&_args[++i][0], nullptr, 10);
    } else if (globals::action_ptr == nullptr) {
      // shouldn't never happen since we check if action exists before
      seterror("action set to null\n");
      return -1;
    }
  }
  return 0;
  // Everything went well if the program reached this point
};
