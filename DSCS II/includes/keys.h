
#ifndef KEYS_H
#define KEYS_H


#include<pbc/pbc.h>




struct list_ptr{

    struct elem_list* elist;
};

struct elem_list{

   element_t elem;

};

struct parameters{
     
     struct pairing_s* pairing;
     struct list_ptr* list_elem;  
     element_t h;
     element_t alpha;
     element_t z;
     unsigned long long m;
     unsigned long long n;
     unsigned long long id;

};

struct private_key{

  struct pairing_s* pairing;
  element_t alpha;

};

struct public_key{

    struct pairing_s* pairing;
    struct list_ptr* list;
    element_t h;                // a generator in G2 i.e system parameter
    element_t z;                //z = h^alpha  
    unsigned long long m;
    unsigned long long n;
    unsigned long long id;

};

struct keypair{

    struct private_key* pvt_key;
    struct public_key* pub_key;
};


struct parameters* generate_params(unsigned long long, unsigned long long);

void set_tags(struct file*, struct parameters*);

struct keypair* generate_key_pair(struct parameters*);



#endif
