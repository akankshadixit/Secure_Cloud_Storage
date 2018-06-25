#ifndef PORT_H
#define PORT_H


#include<stdio.h>
#include<stdlib.h>

#include<bls.h>

#define G1_LEN_COMPRESSED 131
#define G1_LEN 260
#define ZR_LEN 128


struct file* import_file(const char*,struct public_key*);
void export_file(struct file*,const char*);


#endif
