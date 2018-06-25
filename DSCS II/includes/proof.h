
#ifndef PROOF_H
#define PROOF_H


#include<stdio.h>
#include<stdlib.h>
#include<keys.h>

struct proof{

       struct file_vector* fvector;
       struct tag_t* tag;

};


struct proof* ListAuthRead(int, const char*, struct public_key*);

int ListVerifyRead(int,const char*,struct proof*,unsigned long long);

int VerifyRead(int, struct proof* , struct parameters*);

void performUpdate(const char* , struct proof*, struct public_key*);

#endif
