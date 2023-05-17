//
//  rangen.h
//  sortbench
//
//  Created by Mark Riordan on 5/7/23.
//

#ifndef rangen_h
#define rangen_h

#include <stdint.h>

struct md5_context {
    // state
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    // number of bits, modulo 2^64 (lsb first)
    unsigned int count[2];
    // input buffer
    unsigned char input[64];
    // current block
    unsigned int block[16];
};

#define MD5_HASH_SIZE 16

struct md5_digest {
    unsigned char bytes[MD5_HASH_SIZE];
};

struct myRandomContext {
    md5_context     md_context;
    md5_digest      md_digest;
    int64_t         md_seed;
    int             md_bytes_left;
};

void mySetRandomSeed(myRandomContext *myctx, uint64_t seed);
unsigned char myNextRandomByte(myRandomContext *myctx);

#endif /* rangen_h */
