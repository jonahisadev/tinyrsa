# TinyRSA

I wrote an RSA implementation in college in C++ with a relatively obscure big
integer library. I wanted to revisit that program and reimplement it with
GMP in C.

This program was not written to be super safe or efficient and is just a proof
of concept implementation to showcase the math behind the algorithm. The original
program was used as a demonstration.

## Build

You can easily build this yourself since it's just one source file, but I have
provided a Makefile for the lazy among us:

```bash
$ make
```

## Usage

```bash
$ tinyrsa [gen|encrypt|decrypt]
```

* `gen`: Generate a public/private key pair. Common sizes are 1024, 2048, 4096.
* `encrypt`: Encrypt a (small) message. The output is the encrypted message based
on the public key.
* `decrypt`: Paste in an encrypted message and the output will use the private
key to decrypt your message.

## Libraries

* [GMP](https://gmplib.org/) 6.3.0 (GPLv3)
