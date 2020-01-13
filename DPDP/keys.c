#include<stdlib.h>
#include<openssl/rand.h>
#include<stdbool.h>
#include<string.h>
#include<stdint.h>


#include"filehandle.h"
#include"keys.h"
#include"sha256.h"
#include"rank_skiplist.h"
#define _DEFAULT_SOURCE 1


/*struct parameters* generate_params(unsigned long long num_vecs,unsigned long long num_blocks)
{
 // Step 1: Generate two large primes p and q (1536 bits)  
  //         and generate n= pq (3072 bits)
 //


 struct parameters* params = (struct parameters*)malloc(sizeof(struct parameters));

 BIGNUM *p = BN_new();
 BIGNUM *q = BN_new();
 BIGNUM *n = BN_new();
 BIGNUM *e = BN_new();
 BIGNUM *x = BN_new();
 BIGNUM *d = BN_new();
 

 BN_CTX *ctx = BN_CTX_new();
 BN_CTX_start(ctx);

 BIGNUM *r = BN_CTX_get(ctx); 

 int rc;
  
 BN_generate_prime_ex(p,1024,1,NULL,NULL,NULL);     //generate safe prime p
 BN_generate_prime_ex(q,1024,1,NULL,NULL,NULL);     //generate safe prime q

 params->p = BN_dup(p);
 params->q = BN_dup(q);

 //Step 2: Generate a random prime e (113 bits)  and x = (p-1)(q-1) such that gcd(e,x) = 1
 

 BIGNUM *p_minus_one = BN_new();
 BIGNUM *q_minus_one = BN_new();
 BIGNUM *value_one = BN_new();

 BN_one(value_one);

 BN_sub(p_minus_one,p,value_one);
 BN_sub(q_minus_one,q,value_one);
 
 BN_mul(x,p_minus_one,q_minus_one,ctx);
 

 BN_generate_prime_ex(e,113,0,NULL,NULL,NULL);      // e = 113 bits
 BN_mul(n, p, q, ctx);                              //generate n = pq
 params->N = BN_dup(n);

 while(true)
 {
   BN_gcd(r,x,n,ctx);
   if(BN_is_one(r))
    break;
  
  BN_generate_prime_ex(e,113,0,NULL,NULL,NULL);     
 }
 
  params->e = BN_dup(e);

 // Step 3: Calculate unique d such that ed = 1(mod x)
 
  if(!BN_mod_inverse(d,e,x,ctx))
  {
   printf("\nOOPS : Could not find multiplicative inverse!\n");
   printf("\nTrying again...");
   generate_params(num_vecs,num_blocks);
  }

  params->d = BN_dup(d);

 //Step 4: Pick a list of elements randomly from Z*n   such that {g,g1,....gn,h1,....hm}
  



  unsigned long long count = 0;  
  struct list_ptr* ptr = (struct list_ptr*)malloc(sizeof(struct list_ptr));
  BIGNUM *rand_num = BN_new();
  
  ptr->plist = (struct prime_list*)malloc(sizeof(struct prime_list)*(num_vecs+num_blocks+1));  

  struct prime_list* str ;


  for(unsigned long i = 0; count < (num_vecs+num_blocks+1);i++)
 {
   rc = BN_rand_range(rand_num,n);
   if(rc != 1)
   {
    printf("Random number not generated\n");
    return NULL;
   }

   BN_gcd(r,rand_num,n,ctx);

   if(BN_is_one(r) == 1)
   { 
   
    str = ptr->plist+count;

    str->elem = (char*)malloc(2048);

    str->elem = BN_bn2hex(rand_num);
    
    count++;
   }
  }
  
  params->list_elem = ptr;
  params->m = num_vecs;
  params->n = num_blocks;

  return params;
}



char* hexstring(unsigned char* bytes, int len)
{
    char* buffer    = (char*)malloc(sizeof(char)*((2*len)+1));
    char* temp      = buffer;
    for(int i=0;i<len;i++) {
        sprintf(temp,"%02x",bytes[i]);
        temp += 2;
    }
    *temp = '\0';
    return buffer;
}
*/
struct parameters* generate_params(unsigned long long num_vecs,unsigned long long num_blocks){

   /* Step 1: Generate two large primes p and q (1536 bits)  
  *         and generate n= pq (3072 bits)
 */


 struct parameters* params = (struct parameters*)malloc(sizeof(struct parameters));

      BIGNUM *p = BN_new();
      BIGNUM *q = BN_new();
      BIGNUM *n = BN_new();
      BIGNUM *e = BN_new();
      

      BN_CTX *ctx = BN_CTX_new();
      BN_CTX_start(ctx);

      BIGNUM *r = BN_CTX_get(ctx); 

      int rc;
        
      BN_generate_prime_ex(p,1024,1,NULL,NULL,NULL);     //generate safe prime p
      BN_generate_prime_ex(q,1024,1,NULL,NULL,NULL);     //generate safe prime q

      BN_generate_prime_ex(e,113,0,NULL,NULL,NULL);      // e = 113 bits
      params->e = BN_dup(e);

      params->p = BN_dup(p);
      params->q = BN_dup(q);

      BN_mul(n, p, q, ctx);                              //generate n = pq
      params->N = BN_dup(n);

      BIGNUM *rand_num = BN_new();
      rc = BN_rand_range(rand_num,n);
      if(rc != 1){

        printf("Random number not generated\n");
        return NULL;
      }

      BN_gcd(r,rand_num,n,ctx);

      if(BN_is_one(r) == 1){ 
      
          params->g = (char*)malloc(2048);
          params->g = BN_bn2hex(rand_num);
          printf("G is : %s\n",params->g);
      }

     params->m = num_vecs;
     params->n = num_blocks;

     return params;
}


/*struct keypair* generate_key_pair(struct parameters* params, const char* root)
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

  
 pvtkey->p = params->p;
 pvtkey->q = params->q;
 
 pubkey->N = params->N;
 pubkey->e = params->e;
 pubkey->list = params->list_elem;
 pubkey->m = params->m;
 pubkey->n = params->n;
 pubkey->dm = root;

 kpair->pvt_key = pvtkey;
 kpair->pub_key  = pubkey;

 return kpair;
 
}


void set_tags(struct file* file_obj,struct parameters* params)
{
   struct list_ptr* primes = params->list_elem;
   struct prime_list* ptr_g;
   struct prime_list* ptr_h;

   BIGNUM* g   = BN_new();
   BIGNUM* h   = BN_new();
   BIGNUM* G   = BN_new(); 
   BIGNUM* v   = BN_new();
   BIGNUM* s   = BN_new();
   BIGNUM* G_s = BN_new();
   BIGNUM* g_v = BN_new();
   BIGNUM* r   = BN_new();
   BIGNUM* x   = BN_new();
   BIGNUM* prod = BN_new();
  
   BN_CTX *ctx = BN_CTX_new();
   BN_CTX_start(ctx);

   ptr_g = primes->plist+0;
   BN_hex2bn(&G,ptr_g->elem);

   struct file_vector* fv;
   struct file_block* fb;
  
   for(unsigned long long i = 0;i < file_obj->nr_vectors;i++)
   {

     fv = file_obj->vecs+i;
     fv->tag = (struct tag_t*)malloc(sizeof(struct tag_t));

     //select si for each vector
     BN_rand_range(s,params->e);

     //select hi for each vector

     ptr_h = primes->plist+(params->n+1+i);
     BN_hex2bn(&h,ptr_h->elem);

     //G^s 
     BN_mod_exp(G_s,G,s,params->N,ctx);  

     BN_one(prod);
    
     for(unsigned long long j=0;j< fv->nr_blocks;j++) 
     {
       fb = fv->blocks+j;
       char *data = (char*)malloc(sizeof(char)*fb->blk_size);
       data = (char*)fb->data;

       char* s = hexstring((unsigned char*)data,BLK_SIZE);
       ptr_g = primes->plist+(j+1);

       BN_hex2bn(&v, s);
       BN_hex2bn(&g,ptr_g->elem);
 
       BN_mod_exp(g_v,g,v,params->N,ctx);      //g^v
       BN_mod_mul(prod,prod,g_v,params->N,ctx);  

     }

     BN_mod_mul(r,prod,G_s, params->N, ctx);
     BN_mod_mul(x,r,h,params->N,ctx);


     fv->tag->x = BN_dup(x);
     fv->tag->s = BN_dup(s); 
 
   } 

}
*/

void set_tags(struct file* file_obj,struct parameters* params){

   BIGNUM* g   = BN_new();
   BIGNUM* b   = BN_new();
   BIGNUM* g_b = BN_new();

   BN_CTX *ctx = BN_CTX_new();
   BN_CTX_start(ctx);

   BN_hex2bn(&g,params->g);
   struct file_vector* fv;
   struct file_block* fb;
   char* tag;

   for(unsigned long long i = 0;i < file_obj->nr_vectors;i++){

     fv = file_obj->vecs+i;
     fv->tag = (struct tag_t*)malloc(sizeof(struct tag_t));

     for(unsigned long long j=0;j< fv->nr_blocks;j++){

       fb = fv->blocks+j;
       char *data = (char*)malloc(sizeof(char)*fb->blk_size);
       data = (char*)fb->data;

       char* s = hexstring((unsigned char*)data,BLK_SIZE);
       BN_hex2bn(&b, s);
       BN_mod_exp(g_b,g,b,params->N,ctx);      //g^b
     }
     
     fv->tag->t = BN_dup(g_b);
     tag = BN_bn2hex(g_b);
     //printf("The tag generated for vector %u",i);
   }

}