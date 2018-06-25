#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/bn.h>

#include<filehandle.h>


int main()
{
  const char* filename = "test.txt";
  
  struct file* f = get_file_vectors(filename);

  printf("The number of vectors are:%llu\n",f->nr_vectors);
  struct file_vector fv;

  for(int i = 0; i< f->nr_vectors;i++)
  {
    fv = f->vecs[i];
    
   printf("The number of blocks in vector %d are:%llu\n",i,f->vecs[i].nr_blocks);
   printf("Vector: %d: \n",i);
   
   for(int j=0;j<fv.nr_blocks;j++) 
   {
     struct file_block fb = f->vecs[i].blocks[j];

     printf("Block: %d: %llu\n",j,fb.blk_size);

     for(int k=0;k<fb.blk_size;k++)
      { 
        unsigned char* byte = (unsigned char*)fb.data+k;
        printf("%02x",*byte);
      }   
     printf("\n");
   }
   printf("\n");
  }

  free(f->vecs->blocks);
  free(f->vecs);
  free(f);
  return 0;
}
