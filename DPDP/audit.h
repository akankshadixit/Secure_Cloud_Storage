#include<openssl/bn.h>

#include"proof.h"
#include"rank_skiplist.h"

/*struct query{

	unsigned long long i;
    BIGNUM* nu;

};

struct u{

	struct vector_v* v;
};

struct vector_v{

	struct block* blk;
};


struct block{

	BIGNUM* data;

};

struct tag_list{

	struct tag_t* t;

};


struct T1{

	struct vector_v* y;
    struct tag_t* t;

};

struct T2{

	struct tag_list* ti;
	struct proof_list* pi;

};

struct T{

	struct T1* t1;
	struct T2* t2;

};
*/


struct query{

	unsigned long long i;
    BIGNUM* a;

};

struct block{

	BIGNUM* data;

};
	
struct tag_list{

	struct tag_t* t;

};


struct T1{

	struct block* M;

};

struct T2{

	struct tag_list* ti;
	struct proof_list* pi;

};

struct T{

	struct T1* t1;
	struct T2* t2;

};

//struct query* challenge(int , unsigned long long, struct public_key*);

struct query* challenge(int , unsigned long long, struct parameters*);
struct T* prove(struct file* ,struct query*, int, struct parameters*,SkipList&);
int verify(struct query* , struct T* , struct parameters* , int , const char*);

//struct T* prove(struct file* ,struct query*, int, struct public_key*,SkipList&);

//int verify(struct query* , struct T* , struct public_key* , int , const char*);







