# SS Cryptography - README

## Description

Schmidt-Samoa algorithm generates a public and private key using keygen, and then whatever message, etc you want to send will be encrypted. With the private key, the message can be decrypted to its actual message.

## Program Compilation

To compile/build the entire program, enter: make

## How to run this program

### Keygen 

Generates the pair of SS public and private keys.

To run keygen.c enter: (./keygen [-command line options]). 

The command line options:

- `-h` gets program usage and help

- `-b` Minimum bits needed for public key n

- `-v` Display verbose program output

- `-i` Miller-Rabin iterations for testing primes

- `-n` Public key file

- `-d` Private key file

- `-s` Random seed for testing

### Encrypt

Encrypts the data or input file using ss encrypt 

To run encrypt.c enter: (./encrypt [-command line options]). 

The command line options:

- `-h` gets program usage and help

- `-v` Display verbose program output

- `-i`  Input file of data to encrypt

- `-o` Output file for encrypted data

- `-n` Public key file

### Decrypt

Decrypts the encrypted data using ss decrypt to get the intended message/content

To run decrypt.c enter: (./decrypt [-command line options]). 

The command line options:

- `-h` gets program usage and help

- `-v` Display verbose program output

- `-i`  Input file of data to decrypt

- `-o` Output file for decrypted data

- `-n` Private key file
