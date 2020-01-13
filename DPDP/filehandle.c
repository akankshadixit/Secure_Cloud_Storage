#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "filehandle.h"

static FILE* open_file(const char *filename)
{
 if(filename == NULL)
 {
  printf("Null Filename passed to open_file function\n");
  return NULL;
 }


 FILE *fp = fopen(filename,"r");

 if(!fp)
  printf("Unable to open file: %s\n",filename);

 return fp;
}



static size_t get_file_size(FILE *fp)
{

 if(!fp)
 {
  printf("Invalid file pointer passed to function\n");
  return 0;
 }


 size_t filesize;

 fseek(fp, 0L, SEEK_END);
 filesize = ftell(fp);

 fseek(fp, 0L, SEEK_SET);

 if(!filesize)
  printf("Filesize is 0\n");


 return filesize;

}


static unsigned long long get_nr_blocks(size_t filesize)
{
  if(!filesize)
  {
   printf("A file with zero size passed\n");
  }
 
  unsigned long long blocks;

  blocks = (filesize/BLK_SIZE)+1;
  if(filesize % BLK_SIZE == 0)
  blocks--;
 
  return blocks; 

}


struct file* get_file_vectors(const char *filename)
{
 
 if(!filename)
 {
  printf("Invalid Filename passed\n");
  return NULL;
 }

 FILE *fp;
 fp = open_file(filename);

 if(!fp)
 {
   printf("Unable to open the file(%s)\n",filename);
   return NULL;
 }


 size_t          count = 0;
 char*           buffer;
 struct file*    file_info;

 buffer = (char*)malloc(BLK_SIZE);
 
 if(!buffer) 
 {
   printf("Unable to allocate buffer memory for reading the file\n");
   fclose(fp);
   return NULL;
 }

 size_t filesize = get_file_size(fp);

 if(!filesize) 
 {
   printf("Filesize of %s is zero\n",filename);
 }


 file_info = (struct file*)malloc (sizeof(struct file));

 if(!file_info) 
 {
   printf("Unable to allocate memory for file\n");
   free(buffer);
   fclose(fp);
   return NULL;
 }

 unsigned long long num_blocks,n;

 num_blocks = get_nr_blocks(filesize);

 printf("Total number of blocks are: %llu\n",num_blocks);

 if(!num_blocks) 
 {
   printf("Number of blocks are zero\n");
 }

 printf("Enter number of vectors (m) \n");
 scanf("%llu",&file_info->nr_vectors);

 printf("Enter number of blocks in each vector (n)\n");
 scanf("%llu",&n);

 file_info->vecs = (struct file_vector*)
                      malloc(sizeof(struct file_vector)*
                      (file_info->nr_vectors));

 if(!(file_info->vecs))
 {
   printf("Unable to allocate memory for file blocks\n");
   free(buffer);
   free(file_info);
   fclose(fp);
   return NULL;
 }


 for(int i = 0; i < file_info->nr_vectors;i++)
 { 
   struct file_vector *fvector;
   
   fvector = file_info->vecs + i;
        
   fvector->blocks = (struct file_block*)
                        malloc(sizeof(struct file_block)*
                         (n));

   fvector->nr_blocks = 0;

   struct file_block *fblock;

     for(int j = 0;j<n;j++)
     {
        
       fblock = fvector->blocks + j;
                   
         if((count = fread(buffer,1,BLK_SIZE,fp))!=0)
         {

           char *temp;
           temp = (char*)malloc(BLK_SIZE);
            
           memcpy(temp,buffer,count);
           fblock->data = (void*)temp;
           fblock->blk_size = count;
           fvector->nr_blocks++;
         }
        else 
         {
           fblock->data = 0;
	   fblock->blk_size = 0;
         } 

     }
        
 }

 free(buffer);
 return file_info;

}









