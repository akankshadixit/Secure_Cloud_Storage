#ifndef KEYS_H
#define KEYS_H

#include<openssl/bn.h>

struct list_ptr{

    struct prime_list* plist;

};

struct prime_list{
     
     char* elem;
};

struct parameters{
     BIGNUM *p;
     BIGNUM *q;
     BIGNUM *N;
     BIGNUM *e;
     BIGNUM *d;

     struct list_ptr* list_elem;
     unsigned long long m;
     unsigned long long n;

};

struct private_key{
     BIGNUM *p;
     BIGNUM *q;
};

struct public_key{
    BIGNUM *N;
    BIGNUM *e;
    struct list_ptr* list;
    const char* dm;    
    unsigned long long m;
    unsigned long long n;

};

struct keypair{
    struct private_key* pvt_key;
    struct public_key* pub_key;
};

struct parameters* generate_params(unsigned long long, unsigned long long);

void set_tags(struct file*, struct parameters*);

struct keypair* generate_key_pair(struct parameters*,const char* root);

#endif

