#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pbc/pbc.h>

#include<audit.h>
#include<filehandle.h>
#include<keys.h>
#include<rank_skiplist.h>
#include<proof.h>
#include<bls.h>


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



struct query* challenge(int num_query, unsigned long long m, struct public_key* pubkey)
{
  struct pairing_s* pairing = pubkey->pairing;
  
  int random ;
  int arr[num_query];

  struct query* q = (struct query*)malloc(sizeof(struct query)*num_query);
  int count = 0;
  do{
       random = rand() % m  ;
       if(isvalueinarray(random, arr, num_query) == false)
       {
         arr[count] = random;
         count++;
       }
  }while(count < num_query);

  for(int k = 0;k < num_query ; k++)
  {
     q[k].i = arr[k];

     element_init_Zr(q[k].nu, pairing);      
     element_random(q[k].nu);
  }
 return q;
} 


struct T* prove(struct file* f, struct query* q , int num_query, struct public_key* pubkey)
{

  struct file_vector* fv;
  struct file_block* fb;

 /**
   Step 1: Compute ui = [vi ei] by augumenting vector vi with unit
           coefficient vector ei.
  **/

  struct u* aug_vec = (struct u*)malloc(sizeof(struct u));                      // vector ui = aug_vec
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

    for(int j = 0;j < (n+m);j++)
    {
      b = vec->blk+j;
      element_init_Zr(b->data,pubkey->pairing);
      fb = fv->blocks+j;
      char* temp = (char*)malloc(BLK_SIZE);
    
      if(j<n)
       {
         temp = (char*)fb->data; 
         if(temp == NULL)
         {
            element_set0(b->data);
         }
         else
         {
            char* str = hexstring((unsigned char*)temp, BLK_SIZE);
            unsigned char* buff = (unsigned char*)malloc(33);
            memcpy( buff, str, sizeof(buff));
            element_from_bytes(b->data, buff);      
         }
      }
      else
       {  

          if(j == (n+(q[i].i)))
             element_set1(b->data);
          else
             element_set0(b->data);
        }
      }
   } 


 /**
    Step 2: Compute tag list and tag to be sent           
 **/

   struct tag_t* t;
   int index;
   element_t b1;
   element_t mul;

   element_init_Zr(mul, pubkey->pairing);
   element_init_Zr(b1, pubkey->pairing);
 
   element_t w_tag;
   element_init_Zr(w_tag, pubkey->pairing);

   element_set1(mul);

   for(int i = 0; i <num_query;i++)
   {
     index = q[i].i;
     fv = f->vecs+index;
     t = fv->tag;

     element_pow_zn(b1, t->sigma , q[i].nu);
     element_mul(mul , mul , b1);
   
   }

   element_set(w_tag ,mul);

 /**
    Step 3: Compute w = summation(nu*u)mod e           
 **/

    struct vector_v* w = (struct vector_v*)malloc(sizeof(struct vector_v));  
    w->blk = (struct block*)malloc(sizeof(struct block)*(n+m));

    struct vector_v* ptr ;
    struct block* ptr_b;
    struct block* blk_b;
   
    element_t sum;
    element_t a;
    element_init_Zr(sum, pubkey->pairing);
    element_init_Zr(a, pubkey->pairing);


    for(int i = 0;i < n+m ;i++)
    {
      blk_b = w->blk+i;
      element_init_Zr(blk_b->data , pubkey->pairing);

      element_set0(sum);
      for(int j = 0;j<num_query;j++)
       {          
          ptr = aug_vec->v+j;
          ptr_b = ptr->blk+i; 

          element_mul(a , ptr_b->data , q[j].nu);
          element_add(sum, sum , a);      
       }
  
      element_set(blk_b->data, sum);
    }

/**
    Step 4: Compute y           
 **/


   struct vector_v* y = (struct vector_v*)malloc(sizeof(struct vector_v));
   y->blk = (struct block*)malloc(sizeof(struct block)* n);
 
   struct block* y_b;
 
   for(int i = 0; i<n ;i++)
   {
      y_b = y->blk+i;

      blk_b = w->blk+i;

      element_init_Zr(y_b->data, pubkey->pairing);

      element_set(y_b->data, blk_b->data);
   }

  


  struct T* response = (struct T*)malloc(sizeof(struct T));

  element_init_same_as(response->t , w_tag);
  element_set(response->t , w_tag);

  response->y = y;
  return response;

}





int verify(struct query* q, struct T* t, struct public_key* pubkey , int num_query)
{

  int output = 0 ;

  unsigned long long m = pubkey->m;
  unsigned long long n = pubkey->n;  

  element_t tag ;
  element_init_same_as(tag, t->t);
  element_set(tag, t->t);

  struct vector_v* y = t->y;
  struct block* y_b;


  struct vector_v* w = (struct vector_v*)malloc(sizeof(struct vector_v));  
  w->blk = (struct block*)malloc(sizeof(struct block)*(n + m));
  
  struct block* b; 
  int f ;
  for(int i = 0; i< n+m; i++)
  {
     b = w->blk+i;
     
     element_init_Zr(b->data , pubkey->pairing);
     f = 1;
     if(i < n)
     {
       y_b = y->blk+i;
       element_set(b->data, y_b->data); 

     }
    else
    {
       for(int j = 0;j < num_query;j++)
       {
         if(i == (n + q[j].i))
         { 
            element_set(b->data, q[j].nu);
            f = 0;       
         }
       }
       
       if(f == 1)
       {
          element_set0(b->data);

       }
    }
  }



 

/**
   Check 3: check equality
**/

  int flag3;
  element_t temp1, temp2 , a;
  element_t prod1 , prod2 , prod;

  element_init_Zr(prod, pubkey->pairing);
  element_init_Zr(prod1, pubkey->pairing);
  element_init_Zr(prod2, pubkey->pairing);
  element_init_GT(temp1, pubkey->pairing);
  element_init_GT(temp2, pubkey-> pairing);
  element_init_Zr(a, pubkey->pairing);

  element_pairing(temp1, tag, pubkey->h);

  struct list_ptr* list = pubkey->list;
  struct elem_list* item;
 
  struct element_s* ret;                                                             
  char int_str[22];
  int value = 1;
  int len;
  unsigned long long pow ;
  unsigned long long concat;

  element_set1(prod1);
  element_set1(prod2);

  for(unsigned long long i = 0 ; i < n+m ;i++)
  {
    b = w->blk+i;
    if(i<n)
    {
       item = list->elist+i;
      
       element_pow_zn(a, item->elem , b->data);
       element_mul(prod1, prod1, a);
   
    }
    else
    {
       pow = 10;
       while(value >= pow)
        pow *= 10;

       concat =  (pubkey->id * pow) + value; 
       
       len = sprintf(int_str,"%llu",concat);
       ret = bls_hash((void*)int_str,len, pubkey->pairing);

       element_pow_zn(a, ret , b->data);
       element_mul(prod2, prod2, a);
       value++ ;
    }

  }
 
  element_mul(prod, prod1, prod2);
  element_pairing(temp2, prod, pubkey->z);
  flag3 = element_cmp(temp1, temp2);

  output = (!flag3);

  return output;

}
