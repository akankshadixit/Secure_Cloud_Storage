#include<stdio.h>
#include<stdlib.h>

#include<port.h>
#include<filehandle.h>
#include<keys.h>
#include<sha256.h>
#include<rank_skiplist.h>


SkipList& export_skiplist(SkipList& s,const char* filename)
{
 srand((unsigned)time(0)); 
 unsigned long long m,n;
 
 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);

 struct parameters* params;
 params = generate_params(m,n);

 struct file* file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);

 struct file_vector* fv;
 

 for(unsigned long long i = 0;i<file_struct->nr_vectors;i++)
{
  fv = file_struct->vecs+i;
  s.insert(i,fv->tag);
}

}


static void write_tag(struct tag_t* tag, FILE* fp)
{
    char* s_tag = (char*)malloc(FIELD_ELEM_LEN);
    char* x_tag = (char*)malloc(TAG_LEN);

    s_tag =  BN_bn2hex(tag->s);
    x_tag =  BN_bn2hex(tag->x);
    
    fwrite(s_tag,1,FIELD_ELEM_LEN,fp);
    fwrite(x_tag,1,TAG_LEN,fp);

    free(s_tag);
    free(x_tag);
      
}


static struct tag_t* read_tag(FILE* fp)
{
  struct tag_t* tag = (struct tag_t*)malloc(sizeof(struct tag_t));

  char* s_tag = (char*)malloc(FIELD_ELEM_LEN);
  char* x_tag = (char*)malloc(TAG_LEN);

  fread(s_tag,1,FIELD_ELEM_LEN,fp);
  fread(x_tag,1,TAG_LEN,fp);

  BN_hex2bn(&(tag->s), s_tag);
  BN_hex2bn(&(tag->x), x_tag);
  
  return tag;
}



void export_file(struct file* file_struct,const char* filename)
{
  FILE *fp = fopen(filename,"wb");
  struct file_vector* fv;
  struct file_block* fb;

  fwrite( &(file_struct->nr_vectors),sizeof(unsigned long long),1,fp);
  for(uint64_t i = 0;i< file_struct->nr_vectors;i++)
  {
    fv = file_struct->vecs+i;

    fwrite(&(fv->nr_blocks),sizeof(unsigned long long),1,fp);
    write_tag(fv->tag,fp);
    for(uint64_t j = 0;j<fv->nr_blocks;j++)
    {   
      fb = fv->blocks+j;
      fwrite(&(fb->blk_size),sizeof(unsigned long long),1,fp);
      fwrite(&(fb->data),1,fb->blk_size,fp);
    }
  } 
  fclose(fp);
}


struct file* import_file(const char* filename)
{
  FILE *fp = fopen(filename,"rb");  
  struct file* file_struct = (struct file*)malloc(sizeof(struct file));

  fread(&(file_struct->nr_vectors),sizeof(unsigned long long),1,fp);
printf("no of vectors are : %llu\n", file_struct->nr_vectors);
  file_struct->vecs = (struct file_vector*)malloc(sizeof(struct file_vector)*
                                                   (file_struct->nr_vectors));
  struct file_vector* fv;
  struct file_block* fb;

  for(uint64_t i = 0 ; i < file_struct->nr_vectors; i++)
  {
    fv = file_struct->vecs+i;

    fread(&(fv->nr_blocks),sizeof(unsigned long long),1,fp);
    fv->blocks = (struct file_block*)malloc(sizeof(struct file_block)*
                                            (fv->nr_blocks));
    fv->tag = read_tag(fp);
    for(uint64_t j = 0;j < fv->nr_blocks;j++)
    {
      fb = fv->blocks+j;
      
      fread(&(fb->blk_size),sizeof(unsigned long long),1,fp);
      fb->data = (unsigned char*)malloc(fb->blk_size);
      fread(&(fb->data),1,fb->blk_size,fp);
    }
  }
 printf("here\n");  
  fclose(fp);
 
  return file_struct;
}



