# COMANDOS

## Before getting started

Gotta make sure that every directory/file in [ CA_default ] do exist
Based on the 'gopenssl.cnf' config:

```bash
mkdir /mnt/pki
cd /mnt/pki
mkdir certs private crl database issued keys req serial
touch database/index.txt
echo 01 > serial/serial
echo 1000 > crl/crlnumber
```

## CA

### Create CA key + self signed cert

```bash
openssl req -config gopenssl.cnf -new -x509 -out ca-crt.pem -keyout ca-key.pem -noenc
```

## SERVER

### Create server key + req

```bash
openssl req -config gopenssl.cnf -new -out sv-csr.pem -keyout sv-key.pem -noenc 
```

### Sign request with CA and create certificate with server x509 extensions

```bash
openssl ca -config gopenssl.cnf --notext -batch -in sv-csr.pem -out sv-crt.pem -extfile x509/server 
```

## CLIENT

### Create client key + req

```bash
openssl req -config gopenssl.cnf -newkey rsa:2048 -outform PEM -out req.pem -keyout key.pem -noenc
```

### Sign request with CA and create certificate with client x509 extensions

```bash
openssl ca -config gopenssl.cnf --notext -batch -in cl-csr.pem -out cl-crt.pem -extfile x509/client
```

***Note: when signing with the CA remove --notext | -batch to get prompted for confirmation when signing/commiting to database***

## CRL (certificate revoke list )

**When creating a crl, the info will be taken from the ***pki/database/index.txt*** and the crl number will be inside ***pki/crl/crlnumber*****

### Generate crl

```bash
openssl ca -gencrl -config ./gopenssl.cnf -out pki/crl/first-crl.pem
```

### Revoke a certificate  

This will take the information from database/index.txt to see which certificates to add as revoked in the resulting crl  

```bash
openssl ca -config gopenssl.cnf -revoke pki/crt/client1.crt 
```

***Note: after a new certificate is revoked, a new crl has to be generated plus the server crl key must be updated with the created one***

## Extra security

### Generate dhparam (used by server only)

```bash
openssl dhparam -out <outfile> <keysize>
```

### Generate openvpn static keys (used by both server and client)

```bash
openvpn --genkey <tls-crypt|tls-auth|secret> <outfile>
```
