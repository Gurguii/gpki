#!/bin/bash

# [ PKI profile structure ]
# [dir]  $profile/x509                   -> x509 v3 extensions for client/server
# [dir]  $profile/templates              -> openvpn/openssl template configuration files
# [dir]  $profile/packs                  -> client packs
# [dir]  $profile/pki/ca                 -> CA certificate and key
# [dir]  $profile/pki/certs              -> issued certificates
# [dir]  $profile/pki/crl                -> issued crl's
# [file] $profile/pki/crl/crlnumber      -> crl number 
# [dir]  $profile/pki/database           -> info about issued certificates and status
# [file] $profile/pki/database/index.txt -> info about certificates and status (revoked/verified)
# [dir]  $profile/pki/keys               -> private keys
# [dir]  $profile/pki/serial
# [file] $profile/pki/serial/serial      -> stores next serial number to use, by default starts with 01
# [file] $profile/$profile.pkiconf       -> stores info about the profile installation

# Global variables #
dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
dir_config="$dir/config"
dir_templates="$config_dir/templates"
dir_x509="$config_dir/x509"
file_profiles="$dir/.profiles"
declare -A profiles
####################

# Global variables given by user #
##################################

function Usage()
{
  cat << EOF
Usage: gpki.sh <action> <options>

[ ACTIONS ]
init-pki | build-ca | build-server | build-client | remove-profile | remove-all-profiles

[ OPTIONS ]

[ EXAMPLES ]
./gpki.sh init-pki /home/gurgui/mynewpki
./gpki.sh build-ca mynewpki myCA
./gpki.sh build-server mynewpki mySV
./gpki.sh build-client mynewpki client1
EOF
}
function RemoveProfile()
{
  # Params
  # $1 -> Profile name
  if [ -z "$1" ]; then
    printf "gpki.sh remove-profile <profilename>"
    return
  fi

  local path=""
  while read line; do 
    name=$(echo $line | cut -d: -f1)
    if [[ "${name,,}" == "${1,,}" ]]; then
      # Got profile
      path=$(echo $line | cut -d: -f2)
    fi
  done < "$file_profiles"

  if [ -z "$path" ]; then
    printf "Couldn't find profile '%s' in '%s'\n" "$1" "$file_profiles"
  else
    printf "About to run 'rm -rf %s', continue? [Y/N]: " "$path"
    read -r -p "" ans
    if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
      rm -rf "$path"
      newprofile="$file_profiles.new"
      touch "$newprofile"

      while read line; do 
        name=$(echo $line | cut -d: -f1)
        if [[ "${name,,}" != "${1,,}" ]]; then
          echo "$line" >> "$newprofile"
        fi
      done < "$file_profiles"
         
      if [ -e "$newprofile" ]; then
        printf "Updating .profiles file\n"
        rm "$file_profiles"
        mv "$newprofile" "$file_profiles"
      fi
    else
      printf "Not removing anything, exiting...\n"
      return 
    fi
  fi
}
function InitPKI()
{
  # This function initializes a new PKI profile
  # what means creating the directory structure,
  # $profile.pkiconf file, adecuate the gopenssl.cnf 
  # template 
   
  # Params
  # [Mandatory]
  # $1 -> absolute path to new profile directory
  # [Optional]
  # $2 -> name to give to profile - default path's dirname
  local path="$1"
  local name="$2"
  local profile_name=""

  if [ -z "$path" ]; then
    read -r -p "Path to create new PKI structure: " path
  fi

  if [ -z "$name" ]; then
    profile_name=$(basename $path)
  else
    profile_name="$name"
  fi

  if [ ! -z ${profiles[$profile_name]} ]; then
    printf "Profile '%s' already exists on path '%s', remove? [Y/N] " "$profile_name" "${profiles[$profile_name]}"
    read -r -p "" ans
    if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
      printf "Removing profile $profile_name\n"
      RemoveProfile "$profile_name"
      # rm -rf "${profiles[$profile_name]}"
    fi
  fi

  if  [ -e "$path" ]; then
    printf "Path '%s' does exist, remove? [Y/N]: " "$path"
    read -r -p "" ans
    if ! [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
      printf "Cannot continue, exiting ...\n"
      return
    fi
    rm -rf "$path"
  fi 

  # Create pki structure
  mkdir -p "$path/pki" "$path/pki/ca" "$path/pki/certs" "$path/pki/crl" "$path/pki/database" "$path/pki/keys"\
   "$path/pki/req" "$path/pki/serial" 

  echo 01 > "$path/pki/serial/serial"
  echo 1000 > "$path/pki/crl/crlnumber"
  touch "$path/pki/database/index.txt"

  # Adapt gopenssl.cnf file to the new pki
  cp -rf "$dir_config" "$path/config"
  sed -i "s#GPKI_BASEDIR#$path/pki#" "$path/config/gopenssl.cnf"
  
  # Create the "$path/config/.$profile_name.pkiconf" file
  cat << EOF >> "$path/config/.pkiconf"
# Auto generated config file by gpki
# Date: $(date '+%d-%m-%Y at %H:%M')
# Author: Airán 'Gurguii' Gómez

profile_source_dir=$path
profile_name=$profile_name
openssl_config_file=$path/config/gopenssl.cnf
certs=$path/pki/certs
keys=$path/pki/keys
ca=$path/pki/ca
reqs=$path/pki/reqs
x509_dir=$path/config/x509
serial=$path/pki/serial
EOF
  # Add info about this new profile to .profiles file 
  printf "[info] Adding new entry '%s:%s' to '%s'\n" "$profile_name" "$path/config/.pkiconf" "$file_profiles"
  printf "%s:%s\n" "$profile_name" "$path/config/.pkiconf" >> "$file_profiles"
  printf "[done] New profile '%s' added\n" "$profile_name"
}

function BuildCA()
{
  # Params
  # [Mandatory] $1 - profile name 
  # [Optional]  $2 - CA common name
  
  if [[ -z "$1" ]]; then
    printf "%s/gpki.sh build-ca <profile_name>* <common_name> [optional]\n" "$dir"
    return
  fi

  local profile="$1"
  local common_name="$2"
  local pkiconf="${profiles[$profile]}"

  if [ -z "$pkiconf" ]; then
    printf "Profile '%s' not found in '%s'\n" "$profile" "$file_profiles"
  fi
  
  # Import .pkiconf file
  source "$pkiconf"

  if [ -z "$pkiconf" ]; then 
    printf "Profile '%s' not in '%s'\n" "$profile" "$file_profiles"
    return
  fi
  
  key_path="$ca/ca-key.pem"
  crt_path="$ca/ca-crt.pem"

  if [ -z "$common_name" ]; then
    openssl req -config "$openssl_config_file" -new -x509 -out "$crt_path" -keyout "$key_path" -nodes
  else
    openssl req -config "$openssl_config_file" -new -x509 -out "$crt_path" -keyout "$key_path" -subj "/CN=$common_name" -nodes 
  fi
  
  printf "ca key -> '%s'\nca certificate -> '%s'\n" "$ca/ca-crt.pem" "$ca/ca-key.pem"
}

function BuildServer()
{
  # Params
  # [Mandatory] $1 - profile_name 
  # [Optional] $2 - common_name
  local profile="$1"
  local common_name="$2"
  local nserial=""

  if [ -z "$profile" ]; then
    printf "./gpki.sh build-server <profile_name>* <common_name>"
    exit 1
  fi
  
  local pkiconf="${profiles[$profile]}"
 
  if [ -z "$pkiconf" ]; then
    printf "Profile '%s' does not exist\n" "$profile"
    exit 1
  fi
  
  source "$pkiconf" 
  
  nserial=$(cat "$serial/serial")

  local request="$reqs/$nserial-csr.pem"
  local key="$keys/$nserial-key.pem"
  local cert="$certs/$nserial-crt.pem"
   
  if [ -z "$common_name" ]; then
    openssl req -new -x509 -config "$openssl_config_file" -out "$request" -keyout "$key" -nodes
  else
    openssl req -new -x509 -config "$openssl_config_file" -out "$request" -keyout "$key" -subj "/CN=$common_name" -nodes
  fi

  openssl ca -config "$openssl_config_file" -in "$request" -out "$cert" -extfile "$x509_dir/server" 
}

function BuildClient()
{
  # Params
  # [Mandatory] $1 - profile_name 
  # [Optional] $2 - common_name
  
  local profile="$1"
  local common_name="$2"
  local nserial=""

  if [ -z "$profile" ]; then
    printf "./gpki.sh build-client <profile_name>* <common_name>"
    exit 1
  fi
  
  local pkiconf="${profiles[$profile]}"
  if [ -z "$pkiconf" ]; then
    printf "Profile '%s' does not exist\n" "$profile"
    exit 1
  fi
  source "$pkiconf" 
  nserial=$(cat "$serial/serial")
  local request="$reqs/$common_name-csr.pem"
  local key="$keys/$common_name-key.pem"
  local cert="$certs/$common_name-crt.pem"
  
  if [ -z "$common_name" ]; then
    openssl req -new -x509 -config "$openssl_config_file" -out "$request" -keyout "$key" -nodes
  else
    openssl req -new -x509 -config "$openssl_config_file" -out "$request" -keyout "$key" -subj "/CN=$common_name" -nodes
  fi
  openssl ca -config "$openssl_config_file" -in "$request" -out "$cert" -extfile "$x509_dir/server" 
}

function RemoveAllProfiles()
{
  read -r -p "This will delete every PKI profile inside $file_profiles, continue? [Y/N]: " ans
  if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
    while read line; do
      path=$(echo $line | cut -d: -f2)
      source "$path"
      rm -rf "$profile_source_dir" && printf "[Deleted] '%s'\n" "$profile_source_dir"
    done < "$file_profiles"
    > "$file_profiles"
  fi
  printf "Done, all PKI profiles were deleted\n"
}

function Parse()
{
  local name
  local path
  # Given args must be 2 minimum (action + param)
  if (( $# < 2 )); then 
    Usage
    exit 1
  fi
  
  # Remove script name from arguments
  shift
  
  # Check sudo privileges
  if (( $EUID != 0 )); then
    printf "[!] Need sudo privileges\n"
    exit 1
  fi

  args=($@)
  # Check if user asking for help msg
  for i in ${args[@]}; do
    if [[ "$i" == "-h" || "$i" == "--help" ]]; then
      Usage
      exit 1
    fi
  done
  
  # Load .profiles file into $profiles
  while read line; do
    name=$(echo $line | cut -d: -f1)
    path=$(echo $line | cut -d: -f2)
    profiles[$name]="$path"
  done < "$file_profiles"
  
  # Action will in essence be the function 
  # to call, and each function will make use
  # of whatever variables they use
  action=${args[0]}

  case "$action" in
    "init-pki")
      # implemented
      InitPKI ${args[@]:1}
      ;;
    "build-ca")
      # implemented
      BuildCA ${args[@]:1}
      ;;
    "build-server")
      # not implemented
      BuildServer ${args[@]:1}
      ;;
    "build-client")
      # not implemented
      BuildClient ${args[@]:1}
      ;;
    "remove-profile")
      # implemented 
      RemoveProfile ${args[@]:1}
      ;;
    "remove-all-profiles")
      # implemented
      RemoveAllProfiles
      ;;
    *)
      Usage
      exit 1
      ;;
  esac
}

function Main()
{
  Parse $@ 
}


Main $# $@
