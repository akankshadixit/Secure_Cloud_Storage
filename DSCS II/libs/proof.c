#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<string.h>


#include<proof.h>
#include<filehandle.h>
#include<port.h>



struct proof* ListAuthRead(int index, const char* filename, struct public_key* pubkey)
{
  printf("Sending query for %dth element to server....\n",index);
  struct proof* p = (struct proof*) malloc(sizeof(struct proof));

  struct file* f = import_file(filename, pubkey);
  struct file_vector* fv = f->vecs + index;
  struct tag_t* tag = fv->tag;
  printf("tag index: %llu\n",tag->index);
 
  p->fvector = fv;
  p->tag = tag;

  return p;
}



int VerifyRead(int index, struct proof* p, struct parameters* params)
{
  int response;
  struct file_vector* fv = p->fvector;
  struct tag_t* tag = p->tag;

  struct list_ptr* elems = params->list_elem;
  struct elem_list* ptr_g;

  element_t v;
  struct element_s prod[1];
  struct element_s  temp[1];
  struct element_s* new_elem;
  char* string;
  mpz_t integer;
  unsigned long long id = params->id;
  struct file_block* fblock;

  
  new_elem = bls_hash_int(id, tag->index, params->pairing);  // new_elem = H(fid || i)
  element_init_G1(prod,params->pairing);
printf("here\n");
  for(unsigned long long j = 0 ; j < params->n ;j++)
  {
      fblock = fv->blocks + j;
      ptr_g  = elems->elist + j;

      element_init_same_as(temp, ptr_g->elem);
      element_set(temp,ptr_g->elem);
//printf("here3\n");
      string = hexstring((unsigned char*)(fblock->data),(int)fblock->blk_size);
//printf("here4\n");
      element_init_Zr(v, params->pairing);
      mpz_init_set_str(integer,string,16);
//printf("here5\n");
      element_set_mpz(v,integer);
//printf("here6\n");
      element_pow_zn(temp, temp, v); 
//printf("here7\n");
      element_mul(prod, prod, temp);
   }

     element_mul(new_elem, new_elem, prod);
     element_pow_zn(new_elem, new_elem, params->alpha); 
     printf("here1\n");
   response = element_cmp(new_elem, tag->sigma);
printf("here2\n");
   return response;

}


void performUpdate(const char* filename, struct proof* p, struct public_key* pubkey)
{
  struct file* f = import_file(filename, pubkey);

  printf("%llu\n", f->nr_vectors);

  struct file_vector* fv, *fv1;
  struct file_block* fb, *fb1;
  struct tag_t* ft , *ft1;

  struct file* fupd = (struct file*)malloc(sizeof(struct file));
  fupd->vecs = (struct file_vector*)malloc(sizeof(struct file_vector)*(f->nr_vectors+1));
 
  fupd->nr_vectors = f->nr_vectors+1; 

  for(int i = 0; i < f->nr_vectors + 1 ; i++)
  {
    fv = fupd->vecs+i;
    
    if( i < f->nr_vectors)
     {
      fv1 = f->vecs+i;
      ft1 = fv1->tag;
     }
    else
     {
      fv1 = p->fvector;
      ft1 = p->tag;
     }
 
    fv->blocks = (struct file_block*)malloc(sizeof(struct file_block)*(pubkey->n));
    fv->nr_blocks = fv1->nr_blocks;
    fv->tag = (struct tag_t*)malloc(sizeof(struct tag_t));
    ft = fv->tag;

    ft->pairing = ft1->pairing;
    element_init_G1(ft->sigma,pubkey->pairing);
    element_set(ft->sigma, ft1->sigma);
    ft->index = i;

    for(int j = 0;j< pubkey->n ;j++)
     {
     
       fb  = fv->blocks + j;
       fb1 = fv1->blocks + j;

       fb->data = (char*)malloc(BLK_SIZE);
       fb->data = fb1->data;
       fb->blk_size = fb1->blk_size;

     }

  }

  export_file(fupd, filename);

}







