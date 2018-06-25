#include<stdio.h>
#include<stdlib.h>
#define _DEFAULT_SOURCE 1

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


#include<audit.h>
#include<keys.h>
#include<filehandle.h>
#include<sha256.h>


int main()
{

 srand((unsigned)time(NULL));
 unsigned long long m,n;
 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);
 
 
 struct parameters* params;
 params = generate_params(m,n);
 const char* filename = "test.txt";
 
 struct file* file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);

 struct keypair* kp = generate_key_pair(params);
 struct public_key* pubkey = kp->pub_key;

 int num_query = 2;

 struct query* q = challenge(num_query, m, pubkey);

 int response;

 struct T* t = prove(file_struct, q , num_query,pubkey);
 response =  verify(q , t , pubkey , num_query);
 
 if(response == 1)
 printf("AUDIT PASSED\n");
 else
 printf("AUDIT FAILED\n");


}




