#### [CA] Key + self signed certificate

```bash
openssl req \
-newkey rsa:2048 \
-x509 \
-out test.crt \
-keyout test.key \
-config ca \
-subj '/CN=gurguiCA' \
-days +30 \
-set_serial 7777 \
-outform PEM|DER \
-inform PEM|DER \
-noenc
```

#### [CLIENT/SERVER] Key + CA signed certificate

**note: this does skip creating a certificate request**  

```bash
openssl req \
-newkey rsa:2048 \
-x509 \
-out test.crt \
-keyout test.key \
-config x509/server \
-subj '/CN=server' \
-days +30 \
-set_serial 0001 \
-outform PEM|DER \
-inform PEM|DER \
-CA ca\ca-crt.pem \
-CAkey ca\ca-key.pem \
-noenc
```
