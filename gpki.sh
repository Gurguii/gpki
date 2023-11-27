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
# [dir]  $profile/pki/serial             -> serial related files 
# [file] $profile/pki/serial/serial      -> next serial number to use, by default starts with 01
# [file] $profile/$profile.pkiconf       -> info about the profile installation
# [dir]  $profile/logs

# Global variables #
dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
dir_config="$dir/config"
dir_templates="$config_dir/templates"
dir_x509="$config_dir/x509"
file_profiles="$dir/.profiles"
declare -A profiles
####################

# Global variables given by user #
args=()           
argc=0
profile_name="" # profile name required to do most stuff
pkiconf=""      # profile .pkiconf location
common_name=""  # common name to use for certificate creation operations  
create_pack=0
##################################

###################### BEGINNING OF HELP FUNCTIONS #########################
function Usage()
{
  cat << EOF
Usage: gpki.sh <action> <options>

[ ACTIONS ]
init-pki | build-ca | build-server | build-client | remove-profile | remove-all-profiles

[ OPTIONS ]
not added yet

- Create new profile -
./gpki.sh init-pki /home/gurgui/[profile]

- Create new entities -
./gpki.sh build-ca     [profile] [cn]
./gpki.sh build-server [profile] [cn]
./gpki.sh build-client [profile] [cn]

- Create packs -
NOT IMPLEMENTED ./gpki.sh create-pack  [profile] [cn] [options]

- Removing profiles -
./gpki.sh remove-profile [profile]
./gpki.sh remove-all-profiles
EOF
}

function help_build-ca()
{
  printf "build-ca help function\n"
}

function help_build-server()
{
  printf "build-server help function\n"
}

function help_build-client()
{
  printf "build-client help function\n"
}

declare -A helpmsg
helpmsg["build-ca"]=$(help_build-ca)
helpmsg["build-server"]=$(help_build-server)
helpmsg["build-client"]=$(help_build-client)
####################### END OF HELP FUNCTIONS #############################

function DefaultSubjectValuesWithCustomCN()
{
  # Params
  # $1 - CN
  printf "/C=ES/ST=CANARIAS/L=LAS PALMAS/O=MARIWANOS/CN=$1/emailAddress=noemail"
}

function CheckProfile()
{
  # $1 - profile name
  local profile_name="$1"
  # returns 1 if profile exists else 0
  while read entry; do
    name=$(echo $entry | cut -d: -f1)
    if [ "$name" == "$profile_name" ]; then
      printf "%s" "$name"
      break
    fi
  done < "$file_profiles"
}

function CheckProfilePath()
{
  # $1 - profile path
  local profile_path="$1"
  # returns 1 if path exists else 0
  while read entry; do
    path=$(echo $entry | cut -d: -f2)
    source "$path"
    if [ "$profile_source_dir" == "$profile_path" ]; then
      printf "%s" "$profile_name"
      break
    fi
  done < "$file_profiles"
}

function RemoveProfile()
{
  local profile_name="$1"
  printf "Deleting profile '%s'\n" "$profile_name"
  printf "WARNING about to run 'rm -rf %s', continue? [Y/N]: " "$profile_source_dir"
  read -r -p "" ans
  if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
      rm -rf "$profile_source_dir"
      newprofile="$file_profiles.new"
      touch "$newprofile"

      while read line; do 
        name=$(echo $line | cut -d: -f1)
        if [[ "${name,,}" != "${profile_name,,}" ]]; then
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
}

function InitPKI()
{
  # This function initializes a new PKI profile
  # what means creating the directory structure,
  # $profile.pkiconf file, adecuate the gopenssl.cnf 
  # template, etc. 
   
  # [Mandatory]
  # $1 -> absolute path to new profile directory
  # [Optional]
  # $2 -> name to give to profile - default path's dirname
  
  local profile_name="$1"
  while [ -z "$profile_name" ]; do
    read -r -p "Profile name: " profile_name
  done

  local path="$2"

  while [ -n "${profiles[$profile_name]}" ] ; do
    printf "Profile '%s' already exist, change profile name? [Y/N] " "$profile_name"  
    read -r -p "" ans                                                                 
    if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then                                 
      read -r -p "New profile name: " profile_name                                    
    else
      return 1
    fi                                                                                
  done
  
  while [ "${path:0:1}" != "/" ]; do
    read -r -p "Path to create new PKI structure [absolute path]: " path
  done
 
  local pathcheck=$(CheckProfilePath "$path")
  while [ -n "$pathcheck" ]; do
    printf "Path '%s' in use by profile '%s', what to do?\n'd' - delete 'r' - rename 'any other' - exit" "$path" "$pathcheck"
    read -r -p "" ans
    case "${ans,,}" in
      "d")
        RemoveProfile "$path"
        break
        ;;
      "r")
        read -r -p "New path [absolute]: " path 
        ;;
      *)
        break
        ;;
    esac
    pathcheck=$(CheckProfilePath "$path")
  done 

  if [ -e "$path" ]; then
    printf "Path '%s' already exists, remove? [Y/N] " "$path"
    read -r -p "" ans
    if [ ${ans,,} == "y" || ${ans,,} == "yes" ]; then
      rm -rf "$path"
    else 
      return 0
    fi
  fi

  # Create pki structure
  mkdir -p "$path/pki" "$path/pki/ca" "$path/pki/certs" "$path/pki/crl" "$path/pki/database" "$path/pki/keys"\
   "$path/pki/reqs" "$path/pki/serial" "$path/logs" 

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

# Profile stuff
profile_source_dir=$path
profile_name=$profile_name

# Openssl configutation file
openssl_config_file=$path/config/gopenssl.cnf

# PKi paths
certs=$path/pki/certs
keys=$path/pki/keys
ca=$path/pki/ca
reqs=$path/pki/reqs
serial=$path/pki/serial

# Contains files with extensions for each certificate (client/server)
x509_dir=$path/config/x509

# Contains template files used to create packs or configuration files
templates=$path/config/templates

# Logs
logs=$path/logs
EOF
  # Add info about this new profile to .profiles file 
  printf "[info] Adding new entry '%s:%s' to '%s'\n" "$profile_name" "$path/config/.pkiconf" "$file_profiles"
  printf "%s:%s\n" "$profile_name" "$path/config/.pkiconf" >> "$file_profiles"
  printf "[done] New profile '%s' added\n" "$profile_name"
}

function BuildCA()
{
  if [ -z "$common_name" ]; then
    local key="$ca/ca-key.pem"
    local crt="$ca/ca-crt.pem"
    openssl req -config "$openssl_config_file" -new -x509 -out "$crt" -keyout "$key" -nodes
  else
    local key="$ca/$common_name-key.pem"
    local crt="$ca/$common_name-crt.pem"
    openssl req -config "$openssl_config_file" -new -x509 -out "$crt" -keyout "$key" -subj "/CN=$common_name" -nodes 
  fi
  printf "[info] CA created:\nca certificate -> '%s'\nca key -> '%s'\n" "$crt" "$key"
}

function BuildServer()
{
  if [ -n "$common_name" ]; then
    local request="$reqs/$common_name-csr.pem" 
    local key="$keys/$common_name-key.pem"     
    local cert="$certs/$common_name-crt.pem"   
    subject=$(DefaultSubjectValuesWithCustomCN "$common_name")
    openssl req -new -config "$openssl_config_file" -out "$request" -keyout "$key" -subj "$subject" -nodes
  else
    local nserial=$(cat "$serial/serial" 2>/dev/null) 
    local request="$reqs/$nserial-csr.pem"            
    local key="$keys/$nserial-key.pem"                
    local cert="$certs/$nserial-crt.pem"              
    openssl req -new -config "$openssl_config_file" -out "$request" -keyout "$key" -nodes
  fi
  openssl ca -config "$openssl_config_file" -in "$request" -out "$cert" -extfile "$x509_dir/server" 
}

function BuildClient()
{
  if [ -n "$common_name" ]; then
    local request="$reqs/$common_name-csr.pem" 
    local key="$keys/$common_name-key.pem"     
    local cert="$certs/$common_name-crt.pem"   
    subject=$(DefaultSubjectValuesWithCustomCN "$common_name")
    openssl req -new -config "$openssl_config_file" -out "$request" -keyout "$key" -subj "$subject" -nodes
  else
    local nserial=$(cat "$serial/serial" &>/dev/null)
    local request="$reqs/$nserial-csr.pem"
    local key="$keys/$nserial-key.pem"
    local cert="$certs/$nserial-crt.pem"
    openssl req -new -config "$openssl_config_file" -out "$request" -keyout "$key" -nodes 
  fi 
  openssl ca -config "$openssl_config_file" -in "$request" -out "$cert" -extfile "$x509_dir/client" 
}

function RemoveAllProfiles()
{
  read -r -p "WARNING This will delete every PKI profile inside $file_profiles, continue? [Y/N]: " ans
  if [[ ${ans,,} == "y" || ${ans,,} == "yes" ]]; then
    while read line; do
      path=$(echo $line | cut -d: -f2)
      source "$path"
      rm -rf "$profile_source_dir" && printf "[Deleted] '%s'\n" "$profile_source_dir"
    done < "$file_profiles"
    > "$file_profiles"
  fi
  printf "Done, all PKI profiles were deleted\n"
  return 0
}

function CreatePack()
{
  while [ -z "$common_name" ]; do
    clear
    printf "== Create pack ==\n"
    read -r -p "Common name: " common_name
  done
}

function Parse()
{
  local scriptname="$0"

  # Remove script name from arguments
  shift

  # TODO - if first argument is 'help' check next one and give specific help
  # e.g ./gpki.sh help build-ca would print options about build-ca command specifically, examples etc.

  # Check sudo privileges
  #if (( $EUID != 0 )); then
  #  printf "[!] Need sudo privileges\n"
  #  exit 1
  #fi
  if [ $# -lt 1 ]; then
    Usage
    return 0
  fi

  # Check if user asking for general help msg
  for arg in $@; do
    if [[ "${arg,,}" == "-h" || "${arg,,}" == "--help" ]]; then
      Usage
      return 0
    fi
  done

  if [ ! -e "$file_profiles" ]; then                         
    touch "$file_profiles"                                   
  else                                                       
    # Load .profiles file into global variable $profiles     
    while read line; do                                      
      name=$(echo $line | cut -d: -f1)                       
      path=$(echo $line | cut -d: -f2)                       
      profiles[$name]="$path"                                
    done < "$file_profiles"                                  
  fi                                                         

  # Action will in essence be the function 
  # to call, and each function will make use
  # of whatever variables they use
  action="$1"

  case "$action" in
    "remove-all-profiles")
      RemoveAllProfiles ${@:2}
      return $?
      ;;
    "init-pki")
      InitPKI ${@:2}
      return $?
      ;;
  esac
    
  # Every option down here requires a profile name minimum plus some optional extra args, so we can set the global variables
  # profile_name and opts to reduce code duplication by not checking it in every function
  profile_name="$2"
  args=(${@:2})
  argc=${#args[@]}
  
  if [ -z "$profile_name" ]; then
    printf "[info] Profile name is mandatory\n"
    printf "Example -> %s %s [profile]\n" "$scriptname" "$@"
    printf "[help] %s help %s\n" "$scriptname" "$@"
    return 1
  fi
  pkiconf="${profiles[$profile_name]}" 

  if [ -z "$pkiconf" ]; then
    printf "[info] Can't find profile '%s' in '%s'\n" "$profile_name" "$file_profiles"
    # Print action-specific help message
    printf "%s\n" "${helpmsg[$action]}"
    return 1
  fi
  
  # Parse more generic options to reduce code duplication
  for(( i=0; i<argc; ++i )); do
    opt="${args[$i]}"
    case "$opt" in
      "-cn" | "--common-name" )
        common_name="${args[++i]}"
        unset args[$i]
        unset args[--i]
        ;;
      "-pack" | "--create-pack" )
        create_pack=1
        ;;
      *)
        ;;
    esac
  done

  # Import pkiconf
  source "$pkiconf"

  case "$action" in
    "build-ca")
      BuildCA
      ;;
    "build-server")
      BuildServer
      ;;
    "build-client")
      BuildClient
      ;;
    "remove-profile")
      RemoveProfile ${@:2}
      ;;
    *)
      Usage
      ;;
  esac
  return $?
}

function Main()
{
  Parse $@ 
  return $?
}

Main $# $@
