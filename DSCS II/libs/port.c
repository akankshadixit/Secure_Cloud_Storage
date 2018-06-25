#include<stdio.h>
#include<stdlib.h>

#include<port.h>
#include<filehandle.h>
#include<keys.h>
#include<sha256.h>



static void write_tag(struct tag_t* tag, FILE* file)
{
    unsigned char* data = (unsigned char*)malloc(G1_LEN_COMPRESSED);
    element_to_bytes_compressed(data,tag->sigma);
    fwrite(data,1,G1_LEN_COMPRESSED,file);
    fwrite(&(tag->index),sizeof(unsigned long long),1,file);
    free(data);
}


static struct tag_t* read_tag(FILE* file, struct public_key* pubkey)
{
    unsigned char* data = (unsigned char*)malloc(G1_LEN_COMPRESSED);
    struct tag_t* tag = (struct tag_t*)malloc(sizeof(struct tag_t));

    pairing_init_set_buf(pubkey->pairing,a1_param,strlen(a1_param));
    fread(data,1,G1_LEN_COMPRESSED,file);

    element_init_G1(tag->sigma,pubkey->pairing);
    element_from_bytes_compressed(tag->sigma,data);
    fread(&(tag->index),sizeof(unsigned long long),1,file);

    return tag;
}



void export_file(struct file* file_struct,const char* filename)
{
  FILE *fp = fopen(filename,"wb");
  struct file_vector* fv;
  struct file_block* fb;

  fwrite(&(file_struct->nr_vectors),sizeof(unsigned long long),1,fp);

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


struct file* import_file(const char* filename, struct public_key* pubkey)
{
  FILE *fp = fopen(filename,"rb");
  struct file* file_struct = (struct file*)malloc(sizeof(struct file));

  fread(&(file_struct->nr_vectors),sizeof(unsigned long long),1,fp); 
  file_struct->vecs = (struct file_vector*)malloc(sizeof(struct file_vector)*
                                                   (file_struct->nr_vectors));
  struct file_vector* fv;
  struct file_block* fb;

  for(uint64_t i = 0 ; i<file_struct->nr_vectors;i++)
  {
    fv = file_struct->vecs+i;

    fread(&(fv->nr_blocks),sizeof(unsigned long long),1,fp);
    fv->blocks = (struct file_block*)malloc(sizeof(struct file_block)*
                                            (fv->nr_blocks));
    fv->tag = read_tag(fp, pubkey);

    for(uint64_t j = 0;j < fv->nr_blocks;j++)
    {
      fb = fv->blocks+j; 
    
      fread(&(fb->blk_size),sizeof(unsigned long long),1,fp);
      fb->data = (unsigned char*)malloc(fb->blk_size);
      fread(&(fb->data),1,fb->blk_size,fp);

    }
  }
  
  fclose(fp);
  return file_struct;
}

