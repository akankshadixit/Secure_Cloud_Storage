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

   //struct tag_list* t_list = (struct tag_list*)malloc(sizeof(struct tag_list));
  // t_list->t = (struct tag_t*)malloc(sizeof(struct tag_t)*num_query);

   element_set1(mul);

   for(int i = 0; i <num_query;i++)
   {
     index = q[i].i;
     fv = f->vecs+index;
     t = fv->tag;
    // tag = t_list->t+i;

    // tag->pairing = t->pairing;

    // element_init_same_as(tag->sigma, t->sigma);
    // element_set(tag->sigma, t->sigma);

    // tag->index   = t->index;

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

//printf("the value for ws are as follows\n\n");
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

  

  /**
    Step 5: Compute skiplist proof           
 

  clock_t start, end;
  double skiplist_proofgen_time;

  struct proof_list* pi = (struct proof_list*)malloc(sizeof(struct proof_list));
   pi->pl = (struct proof_vector*)malloc(sizeof(struct proof_vector)* num_query);

  // printf("The size is : %ld, %ld\n", sizeof(struct proof), sizeof(proof_list));

   struct proof_vector* p_v;
   //struct proof* pf;

   start = clock();

   for(int i = 0;i< num_query; i++)
   {
     p_v = S.gen_proof(q[i].i+1,m);
     pi[i].pl = p_v;
    // pf = p_v->pv+i;
  // printf("here in audit\n");
  // printf("%s\n", pf->label);
 
   }

   end = clock();
   skiplist_proofgen_time = ((double) (end - start) ) / CLOCKS_PER_SEC;
   printf("Time taken for proofgen of skiplist for %d queried vectors is %f\n", num_query,skiplist_proofgen_time);

**/

  struct T* response = (struct T*)malloc(sizeof(struct T));

  element_init_same_as(response->t , w_tag);
  element_set(response->t , w_tag);

  response->y = y;

  //struct T1* t1 = (struct T1*)malloc(sizeof(struct T1));
  //t1->y = y;
  

  
 // struct T2* t2 = (struct T2*)malloc(sizeof(struct T2)); 
  //t2->ti = t_list;
 // t2->pi = pi;

  //response->t1 = t1;
  //response->t2 = t2;

  return response;

}





int verify(struct query* q, struct T* t, struct public_key* pubkey , int num_query)
{

  int output = 0 ;

  unsigned long long m = pubkey->m;
  unsigned long long n = pubkey->n;

 // struct T1* t1 = t->t1;
 // struct T2* t2 = t->t2;  

  element_t tag ;
  element_init_same_as(tag, t->t);
  element_set(tag, t->t);

  struct vector_v* y = t->y;
  struct block* y_b;


  struct vector_v* w = (struct vector_v*)malloc(sizeof(struct vector_v));  
  w->blk = (struct block*)malloc(sizeof(struct block)*(n + m));

 // printf("creating a vector w here ..\n");
  
  struct block* b; 
  int f ;
  for(int i = 0; i< n+m; i++)
  {
   //  printf("%d\n",i);
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
   Check 2: check tag



   struct tag_list* ti = t2->ti;
   struct tag_t* tc;
   int index;
   element_t a;
   element_t mul;


   element_init_Zr(mul, pubkey->pairing);
   element_init_Zr(a, pubkey->pairing);
   element_set1(mul);

   for(int i = 0; i<num_query ; i++)
   {
     tc = ti->t+i;
     element_pow_zn(a, tc->sigma , q[i].nu);
     element_mul(mul , mul , a);
   }

   int flag2 ;
   flag2 = element_cmp(mul,tag);
**/
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

  //clock_t s1, e1;
  //long double eq_time;

 //s1 = clock();

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

 //e1 = clock();
//eq_time =  ((long double) (e1 - s1) ) / CLOCKS_PER_SEC;
 //printf("Time taken for eq verification is %Lf sec\n",eq_time);

printf("flag3 is %d\n", flag3);


 /**
   Check 1: check for all the proofs


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
      continue;
    }   
  }

printf("flag 1 : %d\n", flag1);
**/

  output = (!flag3);

  return output;

}
