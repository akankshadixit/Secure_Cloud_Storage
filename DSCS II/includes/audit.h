#include<pbc/pbc.h>

#include<proof.h>
#include<rank_skiplist.h>

struct query{

	unsigned long long i;
        element_t nu;

};

struct u{

	struct vector_v* v;
};

struct vector_v{

	struct block* blk;
};


struct block{

	element_t data;

};

struct tag_list{

	struct tag_t* t;

};


struct T{

	struct vector_v* y;
        element_t t;

};


struct query* challenge(int , unsigned long long, struct public_key*);

struct T* prove(struct file* ,struct query*, int, struct public_key*);

int verify(struct query* , struct T* , struct public_key* , int);







