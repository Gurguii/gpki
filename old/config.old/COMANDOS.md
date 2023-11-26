## Commands to create PKI with openssl and custom cnf file in this directory
### CA
#### Create key + self-signed certificate
```bash
openssl req -new -x509 -config ca.cnf -out ca/ca-crt.pem -keyout ca/ca-key.pem -nodes
```
## SERVER
### ALTERNATIVE 1
#### Create server key + certificate signing request
```bash
openssl req -new -config sv.cnf -keyout sv/sv-key.pem -out sv/sv-crt.pem -nodes 
```
#### Issue Server certificate
```bash
openssl x509 -req -in sv/sv-csr.pem -CA ca/ca-crt.pem -CAkey ca/ca-key.pem -CAcreateserial -extfile sv.cnf -extensions v3_crt -out sv/sv-crt.pem
```
### ALTERNATIVE 2
#### Straight generate both key + certificate with 1 command
```bash
openssl req -new -x509 -newkey <algorithm>:<keysize> -config sv.cnf -keyout sv/sv-key.pem -out sv/sv-crt.pem -nodes -CA ca/ca-crt.pem -CAkey ca/ca-key.pem -set_serial <serial>
```
## CLIENT
### ALTERNATIVE 1
#### Create client key + certificate signing request
```bash
openssl req -new -config cl.cnf -keyout cl/cl-key.pem -out cl/cl-crt.pem -nodes 
```
#### Issue Client certificate
```bash
openssl x509 -req -in cl/cl-csr.pem -CA ca/ca-crt.pem -CAkey ca/ca-key.pem -CAcreateserial -extfile cl.cnf -extensions v3_crt -out cl/cl-crt.pem
```
### ALTERNATIVE 2
#### Straight generate both key + certificate with 1 command
```bash
openssl req -new -x509 -newkey <algorithm>:<keysize> -config cl.cnf -keyout cl/cl-key.pem -out cl/cl-crt.pem -nodes -CA ca/ca-crt.pem -CAkey ca/ca-key.pem -set_serial <serial>
```
***Note: -CAcreateserial option is not available in `ALTERNATIVE 2`, but we can use -set_serial <serial> and keep track ourselves, or even give reserve certain serial numbers for certain roles/groups***

### Extra security
#### Create dhparam  
```bash
openssl dhparam -out <out-file> <keysize>
```
#### Create openvpn static key
```bash
openvpn --genkey tls-auth <out-file>
```
### Utils  
#### Create .pfx12 file
openssl pkcs12 -export -in <certificate> -inkey <key> -out <outfile>  
#### Check key contents  
openssl rsa -noout -text -in <key>
#### Check certificate contents
openssl x509 -noout -text -in <crt>
#### Check certificate signing request contents  
openssl req -noout -text -in <csr>
