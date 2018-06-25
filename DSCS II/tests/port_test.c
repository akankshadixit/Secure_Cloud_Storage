#include<stdio.h>
#include<stdlib.h>

#include<port.h>
#include<keys.h>
#include<filehandle.h>

int main()
{

const char* filename = "test.txt";
const char* writefile = "file.dat";

unsigned long long m,n;
printf("Enter the value of m\n");
scanf("%llu",&m);
printf("Enter the value of n\n");
scanf("%llu",&n);

struct parameters* params;
params = generate_params(m,n);

struct keypair* kp = generate_key_pair(params);
struct public_key* pubkey = kp->pub_key;


struct file* file_struct = get_file_vectors(filename);
set_tags(file_struct,params);
export_file(file_struct,writefile);


struct file* f = import_file(writefile, pubkey);
struct file_vector* fv;
struct file_block* fb;


for(int i = 0; i< f->nr_vectors;i++)
  {
    fv = f->vecs+i;
    
   printf("The number of blocks in vector %d are:%llu\n",i,f->vecs[i].nr_blocks);
   printf("Vector: %d: \n",i);
   
   for(int j=0;j< fv->nr_blocks;j++) 
   {
     fb = fv->blocks+j;

     printf("Block: %d: %llu\n",j,fb->blk_size);

     for(int k=0;k < fb->blk_size;k++)
      { 
        unsigned char* byte = (unsigned char*)fb->data+k;
        printf("%02x",*byte);
      }   
     printf("\n");
   }
   printf("\n");
  }

return 0;
}
