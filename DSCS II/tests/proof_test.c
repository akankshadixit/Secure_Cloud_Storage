#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<keys.h>
#include<filehandle.h>
#include<proof.h>
#include<port.h>

int main()
{

  srand(time(NULL));
  const char* filename = "test.txt";
  const char* writefile = "file.dat";
  unsigned long long m,n;
  
  printf("Enter the number of vectors \n");
  scanf("%llu",&m);
  printf("Enter the number of blocks \n");
  scanf("%llu",&n);

  struct parameters* params;
  params = generate_params(m,n);
  struct keypair* kp = generate_key_pair(params);
  struct public_key* pubkey = kp->pub_key;
  

  struct file* file_struct = get_file_vectors(filename);
  set_tags(file_struct,params);
  export_file(file_struct,writefile);

  int random_index = rand() % m;
  
  struct proof* pv = ListAuthRead(random_index, writefile, pubkey);
 
  int output = VerifyRead(random_index, pv, params);
  printf("Output : %d\n", output);
   
  performUpdate(writefile, pv, pubkey);
 
  return 0;
}
