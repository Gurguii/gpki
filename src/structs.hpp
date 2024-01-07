#pragma once
#include <string>
struct Profile {
  int id;
  std::string name{};
  std::string source_dir{};
};

struct Subject {
  std::string country;
  std::string state;
  std::string location;
  std::string organisation;
  std::string cn;
  std::string email;
  std::string serial;
};
// #define SUBJECT_TEMPLATE "/C=%s/ST=%s/L=%s/O=%s/CN=%s/emailAddress=%s"
struct Entity {
  Subject subject;
  std::string serial;
  std::string cert_path;
  std::string key_path;
  std::string csr_path;
  int profile_id;
  std::string subj_oneliner() {
    return "'/C=" + subject.country + "/ST=" + subject.state +
           "/L=" + subject.location + "/O=" + subject.organisation +
           "/CN=" + subject.cn + "/emailAddress=" + subject.email + "'";
  };
};

enum class X509_EXT : int {
  none = 0x00,
#define X509_NONE X509_EXT::none
  ca = 0x01,
#define X509_CA X509_EXT::ca
  client = 0x03,
#define X509_CLIENT X509_EXT::client
  server = 0x05
#define X509_SERVER X509_EXT::server
};
