#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include<stdio.h>
#include<pbc/pbc.h>

//   A file is broken into m vectors each comprising of n blocks as shown below.

//   Vector 0 : |_0_|_1_|_2_|_3_|_4_|_|_n_| 
//   Vector 1 : |_0_|_1_|_2_|_3_|_4_|_|_n_|
//   ...
//   Vector m : |_0_|_1_|_2_|_3_|_4_|_|_n_|

#define BLK_SIZE 14         

struct file{

     struct file_vector* vecs;   //an array of vectors
     unsigned long long nr_vectors;
};

struct file_vector{

     struct file_block* blocks;  //an array of blocks
     unsigned long long nr_blocks;
     struct tag_t* tag;
};

struct file_block{

    unsigned long long blk_size;
    void* data;
};
  
struct tag_t{

    struct pairing_s* pairing;      // The pairing (G1, G2, GT, e, si)
    element_t sigma;               // The element sigma in Zr
    unsigned long long index;     // The index of the vector in the file
};


static unsigned long long get_nr_blocks(size_t);
struct file* get_file_vectors(const char *);

#endif

