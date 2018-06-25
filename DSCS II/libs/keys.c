#include<stdint.h>
#include<string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>

#include<filehandle.h>
#include<bls.h>
#include<keys.h>

#define _DEFAULT_SOURCE 1


struct parameters* generate_params(unsigned long long num_vecs,unsigned long long num_blocks)
{
 /* Step 1: generate pairing 
  */

   struct parameters* params = (struct parameters*)malloc(sizeof(struct parameters));

   struct pairing_s *pairing;
   pairing = (struct pairing_s*)malloc(sizeof(pairing_t));

   pairing_init_set_buf(pairing, a1_param, strlen(a1_param));

   params->pairing = pairing;



 /*
   Step 2: generate a list of generators from group G1
 */

   struct list_ptr* ptr = (struct list_ptr*)malloc(sizeof(struct list_ptr));
   ptr->elist = (struct elem_list*)malloc(sizeof(struct elem_list)*(num_blocks)); 

   struct elem_list* item;
   element_t g;

   for(unsigned long i = 0; i < num_blocks; i++)
   {
     item = ptr->elist+i;
     
     element_init_G1(g, pairing);
     element_init_G1(item->elem, pairing);
     element_random(g);
    
     element_set(item->elem , g);
     element_clear(g);
   }

  params->list_elem = ptr;
 

 /*
   Step 3: generate h (system parameter) from G2
 */

  element_init_G2(params->h, pairing);
  element_random(params->h);

 /*
   Step 4: generate alpha
 */
   
   element_init_Zr(params->alpha,pairing);
   element_random(params->alpha);

  /*
   Step 5: generate z = h^alpha
  */

   element_init_G2(params->z, pairing);
   element_pow_zn(params->z, params->h , params->alpha);

  mpz_t fid ;
  mpz_init(fid);

  pbc_mpz_randomb(fid, 112);
  unsigned long long id = mpz_get_ui(fid);
 
  params->id = id;
  params->m = num_vecs;
  params->n = num_blocks;

  return params;

}

struct keypair* generate_key_pair(struct parameters* params)
{ 

  struct keypair* kpair = NULL;
  kpair = (struct keypair*)malloc(sizeof(struct keypair));

  if(!kpair)
  {
   printf("Unable to allocate memory to keypair\n");
   return NULL;
  }

  struct private_key* pvtkey = NULL;
  pvtkey = (struct private_key*)malloc(sizeof(struct private_key));

  if(!pvtkey)
  {
    printf("Unable to allocate memory to private key\n");
    free(kpair);
    return NULL;
  }

  struct public_key* pubkey = NULL;
  pubkey = (struct public_key*)malloc(sizeof(struct public_key));

  if(!pubkey)
  {
    printf("Unable to allocate memory to private key\n");
    free(kpair);
    return NULL;
  }


  pvtkey->pairing = (struct pairing_s*)malloc(sizeof(pairing_t));
  pubkey->pairing = (struct pairing_s*)malloc(sizeof(pairing_t));

  element_init_same_as(pvtkey->alpha , params->alpha);
  element_init_same_as(pubkey->h , params->h);
  element_init_same_as(pubkey->z, params->z);


  pvtkey->pairing =  params->pairing;
  pubkey->pairing = params->pairing;

  element_set(pvtkey->alpha , params->alpha);
  element_set(pubkey->h , params->h);
  element_set(pubkey->z, params->z);

  pubkey->list = params->list_elem;
  pubkey->m = params->m;
  pubkey->n = params->n;
  pubkey->id = params->id;

  kpair->pvt_key = pvtkey;
  kpair->pub_key  = pubkey;

 return kpair;
 
}


void set_tags(struct file* file_info,struct parameters* params)
{
  struct list_ptr* elems = params->list_elem;
  struct elem_list* ptr_g;

  element_t v;
  struct element_s prod[1];
  struct element_s  temp[1];
  struct element_s* new_elem;
  char* string;
  mpz_t integer;
  struct tag_t* tag;
  unsigned long long id = params->id;

  for(unsigned long long i = 0; i < params->m ; i++)
  {
     struct file_vector* fvector;  
     fvector = file_info->vecs + i;
 
     tag = fvector->tag;
     tag->pairing = params->pairing;
     element_init_G1(tag->sigma,params->pairing);

     new_elem = bls_hash_int(id, tag->index, params->pairing);  // new_elem = H(fid || i)

     struct file_block* fblock;
 
     element_init_G1(prod,params->pairing);

     for(unsigned long long j = 0 ; j < params->n ;j++)
     {
        fblock = fvector->blocks + j;
        ptr_g  = elems->elist+j;

        element_init_same_as(temp, ptr_g->elem);
        element_set(temp,ptr_g->elem);

        string = hexstring((unsigned char*)(fblock->data),(int)fblock->blk_size);

        element_init_Zr(v, params->pairing);
        mpz_init_set_str(integer,string,16);
        element_set_mpz(v,integer);

        element_pow_zn(temp, temp, v); 

        element_mul(prod, prod, temp);
     }

     element_mul(new_elem, new_elem, prod);
     element_pow_zn(new_elem, new_elem, params->alpha); 
   
     element_set(tag->sigma , new_elem);
     element_clear(prod);
  }

}












