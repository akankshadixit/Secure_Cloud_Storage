#include<stdio.h>
#include<stdlib.h>
#include<openssl/bn.h>

#include<audit.h>
#include<keys.h>
#include<filehandle.h>
#include<sha256.h>
#include<rank_skiplist.h>


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

 const char* filename = "test.txt";
 
 struct file* file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);
 struct file_vector* fv;

 for(unsigned long long i = 0;i < file_struct->nr_vectors; i++)
 {
   fv = file_struct->vecs+i;
   s.insert(i,fv->tag);
 }

 const char* root = s.root();
 struct keypair* kp = generate_key_pair(params,root);
 struct public_key* pubkey = kp->pub_key;

 int num_query = 2;
 int response;
 
 struct query* q = challenge(num_query, m, pubkey);
 struct T* t = prove(file_struct, q , num_query,pubkey, s);
 response =  verify(q , t , pubkey , num_query, root );

 if(response == 1)
   printf("AUDIT PASSED!! \n");
 else
   printf("AUDIT FAILED\n");

}



