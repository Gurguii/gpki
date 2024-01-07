#pragma once
#include "defines.hpp"
#include "structs.hpp"
#include <cstdint>
#include <iostream>
#include <string>


namespace gpki::globals{
static inline std::string lasterror{"no errors"}; // last error encountered
static inline int (*action_ptr)() = nullptr;
}

namespace gpki::globals::pki {
static inline int key_size = 2048;
#define KEY_SIZE gpki::globals::pki::key_size
static inline std::string key_format = "rsa";
#define KEY_FORMAT gpki::globals::pki::key_format
static inline int certificate_days = 30;
#define DAYS gpki::globals::pki::certificate_days
static inline std::string input_format = "pem";
#define IN_FORMAT gpki::globals::pki::input_format
static inline std::string output_format = "pem";
#define OUT_FORMAT gpki::globals::pki::output_format
static inline X509_EXT x509_ext = X509_NONE;
#define X509 gpki::globals::pki::x509_ext
static inline Profile profile;
#define PROFILE gpki::globals::pki::profile
#define PROFILE_PTR &gpki::globals::pki::profile
static inline Entity entity{.subject{.country = "ES",
                                     .state = "CANARIAS",
                                     .location = "LAS PALMAS",
                                     .organisation = "MARIWANOS",
                                     .cn = "",
                                     .email = "default@example.com",
                                     .serial{}}};
#define ENTITY gpki::globals::pki::entity
#define ENTITY_PTR &gpki::globals::pki::entity
#define SUBJECT gpki::globals::pki::entity.subject

} // namespace gpki::globals::pki

namespace gpki::globals::metainfo {
static inline std::string base = CURRENT_PATH;
static inline std::string database = base + SLASH + ".gpki.db";
static inline std::string config = base + SLASH + ".." + SLASH + "config";
} // namespace gpki::globals::metainfo

namespace gpki::globals::behaviour {
static inline int prompt = 1;
}

namespace gpki::globals::tls {
static inline int dhparam_keysize = 1024;
}
