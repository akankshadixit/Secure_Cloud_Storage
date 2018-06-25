
#ifndef RANK_SKIPLIST_H
#define RANK_SKIPLIST_H

#include <iostream>
#include <stdio.h>
#include <vector>

#include<proof.h>


using namespace std;


struct node{
          
    int level;
    int rnk;
    int bit;
    const char* rhash;
    const char* dhash;
    const char* hash ;
    node* rgt;
    node* dwn;

    node():rnk(0),bit(0),rgt(0),dwn(0),level(0) 
    {}
};


const char* set_hash(const char* , const char*);
const char* set_level_hash(int,int ,int , const char* , const char* );
char* hexstring(unsigned char* bytes, int len);

class SkipList{

private:

  int coinToss() const;
  bool canGoBelow(node *x, int key); 
  
  static node* makeNode();
  const char* get_root(); 
  void calc_rank();
  void calc_hash();
 
  
  float probability;
  int maxLevel;

// head and tail towers
  vector<node*> head;
  vector<node*> nil;

public:

   SkipList();
  ~SkipList();

   vector<node*> update(int searchKey);
   unsigned long long insert(unsigned long long searchKey,struct tag_t*);

   void insert_new(unsigned long long searchKey, struct tag_t* tag, int level);
   void modify(int searchKey,struct tag_t*);
   void remove(int searchKey);
   const char* root();
   node* search(int searchKey);
  
   SkipList& get_skiplist(SkipList&, struct file*);
   struct proof_vector* gen_proof(int,unsigned long long);

   void print() const;
  
};


#endif
