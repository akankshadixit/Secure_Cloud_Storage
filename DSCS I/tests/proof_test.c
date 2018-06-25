#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<rank_skiplist.h>
#include<keys.h>
#include<filehandle.h>
#include<port.h>
#include<proof.h>

int main()
{
  int searchKey;
  unsigned long long m,n;
  struct proof_vector* pi;
  
  SkipList s;
  int response;
  const char* filename = "test.txt";   //file present with the server
 
  printf("Enter the number of vectors \n");
  scanf("%llu",&m);
  printf("Enter the number of blocks \n");
  scanf("%llu",&n);

  export_skiplist(s, filename);
  printf("Enter the searchkey for which proof has to be generated.....\n");
  scanf("%d",&searchKey);
  ListAuthRead(searchKey,m);

  pi = s.gen_proof(searchKey,m);
  struct proof_vector* new_proof = s.gen_proof(searchKey,m);
  const char* root = s.root();

  response = ListVerifyRead(searchKey,root,pi,m);
  
  if(response == 1)
   printf("Matching !!\n");
  else
   printf("Not Matching...\n");
  

  int level = 3;
  struct parameters* params;
  params = generate_params(m,n);

  struct file* file_struct = get_file_vectors(filename);
  set_tags(file_struct,params);

  struct file_vector* fv;
  fv = file_struct->vecs+1;

  const char* new_dm_insert = upd_root_insert(new_proof,level, fv->tag);

  s.modify(searchKey, fv->tag);
 
  struct proof_vector* insert_proof = s.gen_proof(searchKey+1,m);    //element is inserted at first position
  int out = ListVerifyRead(searchKey+1, new_dm_insert, insert_proof, m);
 
return 0;

}
