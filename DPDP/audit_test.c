#include<stdio.h>
#include<stdlib.h>
#include<openssl/bn.h>

#include"audit.h"
#include"keys.h"
#include"filehandle.h"
#include"sha256.h"
#include"rank_skiplist.h"
#include"proof.h"


int main()
{
 srand((unsigned)time(NULL));
 unsigned long long m,n;
 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);
 SkipList s;
 
 struct parameters* params;
 params = generate_params(m,n);

 printf("Parameters generated \n");
 const char* filename = "test.txt";
 
 struct file* file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);

 printf("Tags Generated \n");
 struct file_vector* fv;

 for(unsigned long long i = 0;i < file_struct->nr_vectors; i++)
 {
   fv = file_struct->vecs+i;
   s.insert(i,fv->tag);
 }

 const char* root = s.root();

 printf("SkipList generated \n");
 //struct keypair* kp = generate_key_pair(params,root);
 //struct public_key* pubkey = kp->pub_key;

 int num_query = 2;
 int response;
 
 struct query* q = challenge(num_query, m, params);
 printf("Query generated \n");
 struct T* t = prove(file_struct, q , num_query,params, s);
 printf("Proof generated \n");
 response =  verify(q , t , params , num_query, root );

 if(response == 1)
   printf("AUDIT PASSED!! \n");
 else
   printf("AUDIT FAILED\n");

}



