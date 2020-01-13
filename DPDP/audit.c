#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/bn.h>

#include"audit.h"
#include"filehandle.h"
#include"keys.h"
#include"rank_skiplist.h"
#include"proof.h"


void* mem_set(void* b, int c, size_t len) {
    char* p = (char*)b;
    for (size_t i = 0; i != len; ++i) 
    {
        if (i != len-1)
          p[i] = 0;
        else
          p[i] = c;
    }
    return b;
}

bool isvalueinarray(int val, int *arr, int size){
    int i;
    for (i=0; i < size; i++) {
        if (arr[i] == val)
            return true;
    }
    return false;
}

struct query* challenge(int num_query, unsigned long long m, struct parameters* params){

  BIGNUM* e = BN_new();
  e = params->e;
  int random ;
  int arr[num_query];

  struct query* q = (struct query*)malloc(sizeof(struct query)*num_query);
  int count = 0;
  do{
       random = rand() % m  ;
       if(isvalueinarray(random, arr, num_query)== false)
       {
         arr[count] = random;
         count++;
       }
  }while(count < num_query);

  for(int k = 0;k < num_query ; k++)
  {
     q[k].i = arr[k];
     q[k].a = BN_new();       
     BN_rand_range(q[k].a,e);
    
     char* s = BN_bn2hex(q[k].a);  
  }
 return q;
} 


struct T* prove(struct file* f, struct query* q , int num_query, struct parameters* params, SkipList& S){

   BIGNUM* r1   = BN_new();
   BIGNUM* r2   = BN_new();
   BIGNUM* data = BN_new();
   BIGNUM* r3   = BN_new();
   BIGNUM* e    = BN_new();

   e = params->e; 
  
   BN_CTX *ctx  = BN_CTX_new();
   BN_CTX_start(ctx);

   struct file_vector* fv;
   struct file_block* fb;
   struct tag_t* tb;

   struct tag_t* tag;
   struct block* b;
   int index;   

   struct T* response = (struct T*)malloc(sizeof(struct T));
   struct T1* t1 = (struct T1*)malloc(sizeof(struct T1));
   struct T2* t2 = (struct T2*)malloc(sizeof(struct T2));
   
  // Generating T2
   struct tag_list* t_list = (struct tag_list*)malloc(sizeof(struct tag_list));
   t_list->t = (struct tag_t*)malloc(sizeof(struct tag_t)*num_query);

   struct proof_list* pi = (struct proof_list*)malloc(sizeof(struct proof_list));
   pi->pl = (struct proof_vector*)malloc(sizeof(struct proof_vector)* num_query);

   struct proof_vector* p_v;
   struct proof* pf;
   unsigned long long m = params->m;

   for(int i = 0;i< num_query; i++){

      // generating tag list
       index = q[i].i;
       fv = f->vecs+index;      
       tb = fv->tag;
 
       tag = t_list->t+i;
       tag->t = tb->t;

      //generating skiplist proof 
       p_v = S.gen_proof(q[i].i+1,m);

       pi[i].pl = p_v;
       pf = p_v->pv+i;
   }

     t2->ti = t_list;
     t2->pi = pi;
     printf("T2 generated \n");



      //Generating T1 = M = summation(ajmij)  where aj = random values in Fe sent by client as part of the challenge and mij = data blocks

     struct block* vector = (struct block*)malloc(sizeof(struct block));
     b = vector;
     BN_zero(r2);
     for(int i = 0;i <num_query;i++){
         index = q[i].i;
         fv = f->vecs+index;  

         for(int j = 0; j < params->n; j++){
            fb = fv->blocks+j;
            char* temp = (char*)malloc(BLK_SIZE);
            temp = (char*)fb->data;
            if(temp == NULL){
             printf("No data to process\n");
            }
           else{
             char* str = hexstring((unsigned char*)temp, BLK_SIZE);
             BN_hex2bn(&data ,str); 
           }      
            printf("Running T1 example...%d\n",j);
           BN_mul(r1, data, q[i].a, ctx);
         }
         
         BN_add(r2, r2, r1); 
      }
     

     BN_mod(r3,r2,params->N,ctx);
     b->data = BN_new();
     b->data = BN_dup(r3);
     t1->M = b;
     
     printf("T1 generated \n");

     response->t1 = t1;
     response->t2 = t2;

     return response;

}


int verify(struct query* q, struct T* t, struct parameters* params , int num_query, const char* root){

  int output = 0 ;

  unsigned long long m = params->m;
  unsigned long long n = params->n;

  struct T1* t1 = t->t1;
  struct T2* t2 = t->t2;  

  struct tag_list* t_list = t2->ti;
  struct block* block = t1->M;

   /**
   Check 2: check for all the proofs
   **/

  int flag1 = 1;
  
  struct proof_list* plist = t2->pi;
  struct proof_vector* p_v;
  struct proof* pf;

  for(int i = 0;i<num_query;i++){
    
    p_v = plist[i].pl;
    if(ListVerifyRead(q[i].i+1, root, p_v, m) == 1){
      continue;
    }
    else{
      flag1 = 0;
      break;
    }
  }

  printf("The value of flag1 is %d\n",flag1);

 // Check 1: check the audit response 

  int flag2 = 1;
  BIGNUM *g = BN_new();
  BN_hex2bn(&g,params->g);

  BIGNUM *T  = BN_new();
  BIGNUM *r1 = BN_new();
  BIGNUM *r2 = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  BN_CTX_start(ctx);

  BN_mod_exp(T,g,block->data,params->N,ctx);            // T = g^M mod N
  char * Tchar = BN_bn2hex(T);
  printf("The calculated T is %s\n", Tchar);

  printf("T calculated \n");
  BIGNUM *T_prime = BN_new();
  BIGNUM *a = BN_new();
  
  struct tag_t* tag;
  BN_one(r2);

  for(int i = 0;i < num_query; i++){

    printf("in %d\n",i);
    tag = t_list->t+i;
    BN_mod_exp(r1,tag->t,q[i].a,params->N,ctx);
   
     printf("now in %d\n",i);
    BN_mod_mul(r2,r2,r1,params->N,ctx);
    char *r2char = BN_bn2hex(r2);
     printf("The calculated r2 is %s\n", r2char);    
  }

  BN_mod(T_prime,r2,params->N,ctx);
   char *Tprimechar = BN_bn2hex(T_prime);
     printf("The calculated Tprime is %s\n", Tprimechar);
 

  if(BN_cmp(T,T_prime) != 0){
      flag2 = 0;
   }

   printf("The value of flag2 is %d\n",flag2);
 

  output = (flag1 & flag2);
  return output;


}

/*struct query* challenge(int num_query, unsigned long long m, struct public_key* pubkey)
{
  BIGNUM* e = BN_new();
  e = pubkey->e;
  int random ;
  int arr[num_query];

  struct query* q = (struct query*)malloc(sizeof(struct query)*num_query);
  int count = 0;
  do{
       random = rand() % m  ;
       if(isvalueinarray(random, arr, num_query)== false)
       {
         arr[count] = random;
         count++;
       }
  }while(count < num_query);

  for(int k = 0;k < num_query ; k++)
  {
     q[k].i = arr[k];
     q[k].nu = BN_new();       
     BN_rand_range(q[k].nu,e);
    
     char* s = BN_bn2hex(q[k].nu);  
  }
 return q;
} 





struct T* prove(struct file* f, struct query* q , int num_query, struct public_key* pubkey, SkipList& S)
{

  BIGNUM* e = BN_new();
  e = pubkey->e;
  
  BN_CTX *ctx = BN_CTX_new();
  BN_CTX_start(ctx);

  struct file_vector* fv;
  struct file_block* fb;

  struct tag_t* t;
  int index;

  clock_t start, end;
  double skiplist_proofgen_time;

 //Step 1: Calculate s = summation (nu*si) mod e and Calculate s_prime = summation(nu*si - s)/ e


 BIGNUM* s       = BN_new();
 BIGNUM* s_prime = BN_new();

 BIGNUM* r       = BN_new();
 BIGNUM* sum     = BN_new();
 BIGNUM* y_s     = BN_new();
 BIGNUM* rem     = BN_new();

 BN_zero(sum);

 struct tag_list* t_list = (struct tag_list*)malloc(sizeof(struct tag_list));
 t_list->t = (struct tag_t*)malloc(sizeof(struct tag_t)*num_query);

 struct tag_t* tag;

 for(int i = 0;i <num_query;i++)
 {
   index = q[i].i;
   fv = f->vecs+index;
   t = fv->tag;

   tag = t_list->t+i;
 
   tag->s = t->s;
   tag->x = t->x;

   BN_mul(r,q[i].nu, t->s,ctx);
   BN_add(sum, sum, r);
 }
   
  BN_mod(s,sum,e,ctx);

  BN_sub(s_prime,sum,s);

  char* str = BN_bn2hex(s);
  char* str1 = BN_bn2hex(s_prime);

 //Step 2: Compute ui = [vi ei] by augumenting vector vi with unit coefficient vector ei.
  


  struct u* aug_vec = (struct u*)malloc(sizeof(struct u));

  aug_vec->v = (struct vector_v*)malloc(sizeof(struct vector_v)*num_query);
  
  struct vector_v* vec;
  struct block* b;

  unsigned long long n = pubkey->n;
  unsigned long long m = pubkey->m;

  for(int i = 0;i<num_query; i++)
  { 
    fv = f->vecs+q[i].i;

    vec = aug_vec->v+i;

    vec->blk = (struct block*)malloc(sizeof(struct block)*(n+m)); 

    for(int j = 0;j< (n+m);j++)
    {
      b = vec->blk+j;
      b->data = BN_new();

      fb = fv->blocks+j;
      char* temp = (char*)malloc(BLK_SIZE);
    
      if(j<n)
       {
         temp = (char*)fb->data; 
         if(temp == NULL)
         {
            BN_zero(b->data);
         }
         else
         {
            char* str = hexstring((unsigned char*)temp, BLK_SIZE);
            BN_hex2bn(&b->data ,str);       
         }
      }
      else
       {  

          if(j == (n+(q[i].i)))
            BN_one(b->data);
          else
            BN_zero(b->data);
        }  
      }
   } 


 //    Step 3: Compute w = summation(nu*u)mod e and Compute w_prime = summation(nu*ui - w)/ e

    BIGNUM* w_t = BN_new();
    BIGNUM* w_p = BN_new();
    BIGNUM* a   = BN_new();
    BIGNUM* r1   = BN_new();
    BIGNUM* r2   = BN_new();
   
    struct vector_v* w = (struct vector_v*)malloc(sizeof(struct vector_v));  
    w->blk = (struct block*)malloc(sizeof(struct block)*(n+m));

    struct vector_v* w_prime = (struct vector_v*)malloc(sizeof(struct vector_v));
    w_prime->blk = (struct block*)malloc(sizeof(struct block)*(n+m));

    struct block* blk_b;
    struct block* blk_w;
  
    struct vector_v* ptr ;
    struct block* ptr_b;

    for(int i = 0;i < n+m ;i++)
    {
      blk_b = w->blk+i;
      blk_w = w_prime->blk+i;

      blk_b->data = BN_new();
      blk_w->data = BN_new();

      BN_zero(w_t);

      for(int j = 0;j<num_query;j++)
       {          
          ptr = aug_vec->v+j;
          ptr_b = ptr->blk+i; 

          BN_mul(a, ptr_b->data, q[j].nu, ctx);
          BN_add(w_t, w_t, a);        
       }
      
         BN_mod(r1,w_t,e,ctx);
         blk_b->data = BN_dup(r1);

         BN_sub(w_p, w_t , r1);
         blk_w->data = BN_dup(w_p);
    }


 //Step 4: Compute x using the given formula 
     
  BIGNUM* p  = BN_new();
  BIGNUM* r3 = BN_new();
  BIGNUM* r4 = BN_new();
  BIGNUM* r5 = BN_new();
  BIGNUM* r6 = BN_new();

  BN_one(r3);
 
  for(int i = 0; i < num_query;i++)
  {
    index = q[i].i;
    fv = f->vecs+index;
    t = fv->tag;

    BN_mod_exp(p ,t->x,q[i].nu ,pubkey->N, ctx);
    BN_mod_mul(r3, r3, p, pubkey->N, ctx);
  }

 BIGNUM* g  = BN_new();
 BIGNUM* wp = BN_new();
 BIGNUM* prod1 = BN_new();
 BIGNUM* prod2 = BN_new();
 BIGNUM* result = BN_new();
 BN_one(prod1);
 BN_one(prod2);  

  int count = 0;
  int j = 0, k = 0;
  struct list_ptr* primes = pubkey->list;
  struct prime_list* pl;


  for(int i = 0 ; i< (n+m+1); i++)
  {
    if (i == 0)
    {
      pl = primes->plist+0;
      BN_hex2bn(&g,pl->elem);
      BN_mod_exp(r4, g, s_prime, pubkey->N, ctx);
      
    }
    else if (i>0 && i<n+1)
    { 
       blk_w = w_prime->blk+count;
       pl = primes->plist+(1+j);     
       BN_hex2bn(&g,pl->elem);

       BN_mod_exp(r5, g, blk_w->data , pubkey->N, ctx);
       BN_mod_mul(prod1,prod1, r5, pubkey->N, ctx);
       j++;
       count++;
    }
   else
    {
       blk_w = w_prime->blk+count;
       pl = primes->plist+(pubkey->n+1+k);    
       BN_hex2bn(&g,pl->elem);
       BN_mod_exp(r6, g, blk_w->data , pubkey->N, ctx);
       BN_mod_mul(prod2,prod2, r6, pubkey->N, ctx);
       k++;
       count++;
    }
 
  }
  
  BN_mod_mul(r4,r4,prod1, pubkey->N, ctx);
  BN_mod_mul(r4,r4,prod2, pubkey->N, ctx);

  BN_mod_inverse(rem, r4, pubkey->N, ctx);

  BN_mod_mul(result, r3, rem, pubkey->N, ctx);
 
 struct tag_t* w_tag = (struct tag_t*)malloc(sizeof(struct tag_t));

 w_tag->s = s;
 w_tag->x = result;

 struct vector_v* y = (struct vector_v*)malloc(sizeof(struct vector_v));
 y->blk = (struct block*)malloc(sizeof(struct block)* n);

 struct block* y_b;

 for(int i = 0; i<n ;i++)
 {
    y_b = y->blk+i;
    blk_b = w->blk+i;

    y_b->data = BN_new();
    y_b->data = BN_dup(blk_b->data);
 }

 struct T* response = (struct T*)malloc(sizeof(struct T));

 struct T1* t1 = (struct T1*)malloc(sizeof(struct T1));
 t1->y = y;
 t1->t = w_tag;

 struct T2* t2 = (struct T2*)malloc(sizeof(struct T2));
 
 t2->ti = t_list;

 struct proof_list* pi = (struct proof_list*)malloc(sizeof(struct proof_list));
 pi->pl = (struct proof_vector*)malloc(sizeof(struct proof_vector)* num_query);

 struct proof_vector* p_v;
 struct proof* pf;

 for(int i = 0;i< num_query; i++)
 {
   p_v = S.gen_proof(q[i].i+1,m);

   pi[i].pl = p_v;
   pf = p_v->pv+i;
 
 }

  t2->pi = pi;

 response->t1 = t1;
 response->t2 = t2;

 return response;
}



int verify(struct query* q, struct T* t, struct public_key* pubkey , int num_query, const char* root)
{
  int output = 0 ;

  unsigned long long m = pubkey->m;
  unsigned long long n = pubkey->n;

  struct T1* t1 = t->t1;
  struct T2* t2 = t->t2;  

  struct tag_t* tag = t1->t;;

  struct vector_v* y = t1->y;
  struct block* y_b;


  struct vector_v* w = (struct vector_v*)malloc(sizeof(struct vector_v));  // create an empty vector w
  w->blk = (struct block*)malloc(sizeof(struct block)*(n + m));
 
  struct block* b; 
  int f ;
  for(int i = 0; i< n+m; i++)
  {
     b = w->blk+i;  
     b->data = BN_new();
     f = 1;
     if(i < n)
     {
       y_b = y->blk+i;
       b->data =  BN_dup(y_b->data); 
     }
    else
    {
       for(int j = 0;j < num_query;j++)
       {
         if(i == (n + q[j].i))
         { 
            b->data = BN_dup(q[j].nu);
            f = 0;       
         }
       }
       
       if(f == 1)
       {
           BN_zero(b->data);   
       }
    }
  }



// Check 1: check the audit response 
  
 BN_CTX *ctx = BN_CTX_new();
 BN_CTX_start(ctx);


 BIGNUM* g = BN_new();
 BIGNUM* wp = BN_new();
 BIGNUM* prod1 = BN_new();
 BIGNUM* prod2 = BN_new();
 BIGNUM* x = BN_new();
 BIGNUM* r2 = BN_new();
 BIGNUM* r3 = BN_new();
 BIGNUM* r4 = BN_new();
 BIGNUM* r5 = BN_new();
 BIGNUM* r6 = BN_new();
 BN_one(prod1);
 BN_one(prod2); 

 struct list_ptr* list = pubkey->list;
 struct prime_list* prime;
 int count = 0;
 int j = 0;
 int k = 0;


 clock_t s1, e1;
  long double eq_time;

 s1 = clock();


 for(int i = 0 ; i< (n+m+1); i++)
  { 
     if (i == 0)
     {
       prime = list->plist+0;

       BN_hex2bn(&g,prime->elem);

       BN_mod_exp(r4, g, tag->s, pubkey->N, ctx);        

     }
    else if (i>0 & i<n+1)
    {
       b = w->blk+count;

       prime = list->plist+(1+j);

       BN_hex2bn(&g,prime->elem);       

       BN_mod_exp(r5, g, b->data , pubkey->N, ctx);
       BN_mod_mul(prod1,prod1, r5, pubkey->N, ctx);
            
       j++;
       count++;

    } 
   else if(i >= n+1)
    {   

       b = w->blk+count;
       char* s = hexstring((unsigned char*)b->data,32);

       prime = list->plist+(pubkey->n+1+k);
       BN_hex2bn(&g,prime->elem);     
   
       BN_mod_exp(r6, g, b->data , pubkey->N, ctx); 
       BN_mod_mul(prod2,prod2, r6, pubkey->N, ctx);
     
       k++;
       count++;

    }
  }


  BN_mod_mul(r3,r4,prod1, pubkey->N, ctx);
  BN_mod_mul(r2,r3,prod2, pubkey->N, ctx);
  
  int flag3 = 1;
  if(BN_cmp(r2,  tag->x) != 0)
     flag3 = 0;
    
// Check 2: checks if sbar = s

 int flag2 = 1;
 BIGNUM* sbar = BN_new();
 BIGNUM* r    = BN_new();
 BIGNUM* sum  = BN_new();


 BN_zero(sum);
 struct tag_list* ti = t2->ti;


 for(int i = 0;i< num_query; i++)
 {
   tag = ti->t+i;
   BN_mul(r, tag->s, q[i].nu, ctx);
   BN_add(sum, sum, r);
 } 

  BN_mod(sbar, sum, pubkey->e,ctx);
  
  tag = t1->t;
  if(BN_cmp(sbar,tag->s) != 0)
   {
      flag2 = 0;
   }
   
 char* str = BN_bn2hex(sbar);
 char* str1 = BN_bn2hex(tag->s);
 

 // Check 3: check for all the proofs


  int flag1 = 1;
  
  struct proof_list* plist = t2->pi;
  struct proof_vector* p_v;
  struct proof* pf;

  for(int i = 0;i<num_query;i++)
  {
    
    p_v = plist[i].pl;
    if(ListVerifyRead(q[i].i+1, root, p_v, m) == 1 ) 
    {
      continue;
    }
    else
    {
      flag1 = 0;
      break;
    }
    
  }

 output = ( flag1 & flag2 & flag3);
 return output;
  
}

*/

 
