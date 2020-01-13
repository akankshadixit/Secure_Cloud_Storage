
#ifndef PROOF_H
#define PROOF_H


#include<stdio.h>
#include<stdlib.h>

#include"rank_skiplist.h"

struct proof_list{

	struct proof_vector* pl;
};

struct proof_vector{

	struct proof* pv;

};

struct proof{
	
	int dz;
	int rnk;
	int level;
	int bit;
	const char* label;
    const char* rlabel;
	const char* dlabel;
};



void ListAuthRead(int,unsigned long long);
int ListVerifyRead(int,const char*,struct proof_vector*,unsigned long long);

//const char* upd_root_insert(struct proof_vector*, int level, struct tag_t* tag);
//const char* upd_root_del(struct proof_vector*, struct proof_vector*);

#endif
