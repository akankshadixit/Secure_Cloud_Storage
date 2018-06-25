#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include<openssl/bn.h>

#include<rank_skiplist.h>
#include<sha256.h>
#include<keys.h>
#include<filehandle.h>

using namespace std;

//================================ Private Member Functions============================

bool SkipList::canGoBelow(node *x, int searchKey)
{
  int dkey;

  if(x->dwn == 0)
  return false;
  else
  {
   dkey = x->dwn->rnk;
   if(searchKey <= dkey)
   return true;
   else
   return false;
  }
}



vector<node*>SkipList::update(int searchKey)
{
  vector<node*> result(maxLevel);

  node *x;
  int key;
  x = head[maxLevel-1];
  // handles the case when first element is being inserted
  if(searchKey == 1)
     {
       for(int i = maxLevel-1; i>= 0; i--)
        {
          x = head[i];
          result[i] = x;
        }
     }
   else
      {
       key = searchKey;
       for(int i = maxLevel-1; i>=0; i--)
        {
          while( x->rgt->rnk != 0 && x->rgt != 0 && (canGoBelow(x,key) == false) && key != 1) 
          {
            key = key - (x->rnk - x->rgt->rnk);
            x = x->rgt;
          }
          result[i] = x;

           if(i>0)
           {
             x = x->dwn;
           }
         }
      }

   return result;
}



char* hexstring(unsigned char* bytes, int len)
{
    char* buffer    = (char*)malloc(sizeof(char)*((2*len)+1));
    char* temp      = buffer;
    for(int i=0;i<len;i++) {
        sprintf(temp,"%02x",bytes[i]);
        temp += 2;
    }
    *temp = '\0';
    return buffer;
}


const char* set_hash(const char* a, const char* b)
{
    int len = strlen(a) + strlen(b);
    char* result = (char*)malloc (sizeof(char)*(2*(len)+1));

    strcpy(result,a);
    strcat(result,b);

    sha256_hash_t buffer[32];
    sha256(buffer,result,8*(len));
    char* string = hexstring((unsigned char*)buffer,32);

    return string;

}


const char* set_level_hash(int level,int rank,int bit, const char* x , const char* y)
{ 
   int len = strlen(x) + strlen(y);
   char* result = (char*)malloc (sizeof(char)*(2*(len)+10));


   char int_str[32];
   sprintf(int_str,"%d", level);
   strcpy(result, int_str);

   sprintf(int_str,"%d", rank);
   strcat(result,int_str);

   sprintf(int_str,"%d", bit);
   strcat(result,int_str);

   strcat(result,x);
   strcat(result,y);
  
   sha256_hash_t buffer[32];
   sha256(buffer,result,8*(len+10));
   char* string = hexstring((unsigned char*)buffer,32);
   return string;
}



void SkipList::calc_hash()
{
  node* x;

  for(int i = 0;i<maxLevel ; i++)
  {
    vector<node*> list;
    x = head[i];
    const char* a = "";
    while(x->rgt !=0)
    {
     list.push_back(x);
     x = x->rgt;
    }
    if(i == 0)
    {      
      for(int j = list.size()-1; j>=0;j--)
       {

        if(list[j] == head[i])
         {
           if(list[j]->bit == 0)
           {
             head[i]->rhash = a;
             head[i]->hash = set_level_hash(head[i]->level, head[i]->rnk, head[i]->bit, head[i]->dhash, head[i]->rhash);
           }
           else
           { 
             head[i]->rhash = head[i]->rgt->hash;
             head[i]->hash = set_level_hash(head[i]->level, head[i]->rnk, head[i]->bit, head[i]->dhash, head[i]->rhash);       
           }
         }
        else
         { 
           if(list[j]->bit == 0)
           {
             
              list[j]->rhash = a;
              list[j]->hash = set_level_hash(list[j]->level, list[j]->rnk, list[j]->bit,list[j]->dhash,list[j]->rhash);   
           }
	   else
           {
              list[j]->rhash = list[j]->rgt->hash;
              list[j]->hash = set_level_hash(list[j]->level, list[j]->rnk, list[j]->bit,list[j]->dhash,list[j]->rhash); 
           }
          
         }

       }    
    }
   else
    {
      for(int j = list.size()-1; j>=0;j--)
       {
	  list[j]->dhash = list[j]->dwn->hash;

          if(list[j]->bit == 0)
          {
             list[j]->rhash = a;           
	     list[j]->hash = set_level_hash(list[j]->level, list[j]->rnk, list[j]->bit, list[j]->dhash,list[j]->rhash);                 
          }
	  else
  	  {
             list[j]->rhash = list[j]->rgt->hash; 
	     list[j]->hash = set_level_hash(list[j]->level, list[j]->rnk, list[j]->bit, list[j]->dhash,list[j]->rhash);          
          }
        
       }
    }
  }
}



void SkipList::calc_rank()
{

 node *x ;
 for(int i = 0; i<maxLevel; i++)
  {
    vector<node*> list;
    x = head[i];

    while(x->rgt != 0)
    {
       list.push_back(x);
       x = x->rgt;
    }

    if(i == 0)
    {
      for(int j = list.size()-1; j>=0;j--)
      {
         if(list[j]->bit == 0)
         {
           if(list[j] == head[i])
           {
            list[j]->rnk = 0;
           }
           else
           {
             list[j]->rnk = 1;
           }
         }
         else
         {
           if(list[j]->rgt->rnk == 0)
           {
             list[j]->rnk = 1;
           }
           else if(list[j] == head[i])
           {
             list[j]->rnk = list[j]->rgt->rnk;
           }
           else
           {
             list[j]->rnk = list[j]->rgt->rnk + 1;
           }
         }
      }

    }
    else
    {
      for(int j = list.size()-1; j>=0; j--)
      {

        if(list[j]->bit == 0)
        {
          list[j]->rnk = list[j]->dwn->rnk;  
        }
       else
        {
          list[j]->rnk = list[j]->rgt->rnk + list[j]->dwn->rnk;
        }
      }
    }

  }

}


int SkipList::coinToss() const
	{
		int flip = 1 + rand() % 2;    // 1 = heads , 2 = tails
		return flip;
	}


node* SkipList::makeNode()
{

   return new node;
}



const char* SkipList:: get_root() 
{
  node* x;
  x = head[15];
  return x->hash;
}




//============================Public Member Functions===================


SkipList::SkipList():
 probability(0.5), maxLevel(16){

     head.resize(maxLevel, 0);
     nil.resize(maxLevel,0);

     const char* a = "";
    
     for(int i = 0; i < maxLevel ;i++)
     {
        head[i] = new node;
        nil[i] = new node;

 	head[i]->level = i;
	nil[i]->level = i;
        nil[i]->hash = set_hash(a,a);
        nil[i]->rhash = set_hash(a,a);

        head[i]->rgt = nil[i];
        head[i]->bit = 1;
     
       
        if(i == 0)
        {
	 nil[i]->dhash = set_hash(a,a);
         head[i]->dhash = set_hash(a,a);      
	}

        if(i>0)
        {
          head[i]->dwn = head[i-1];
          nil[i] ->dwn = nil[i-1];
  
	  nil[i]->dhash = nil[i]->dwn->hash;        
        }
           
      }
}

SkipList::~SkipList()
{
   node* temp, *d, *r;
   d = head[maxLevel-1];

   while(d != head[0])
   {
     r = d;
     temp = r;
     r = r->rgt;
     d = d->dwn;
     delete temp;
   }
   delete d;
   delete r;
}

void SkipList::insert_new(unsigned long long searchKey, struct tag_t* tag, int level)
{
   unsigned long long key = searchKey ;  //+1 removed for insertion after the passed searchkey
   vector<node*> result = update(key);
   node *ptr, *x;
   const char* a = "";

   for(int i = 0;i<level;i++)
   {
    ptr = makeNode();
      
      x = result[i]->rgt;
      result[i]->rgt = ptr;
      ptr->rgt = x;
      ptr->bit = result[i]->bit;
      result[i]->bit = 1;
      ptr->level = i;       

     //insert leaf node hash for tag
      if(i == 0)
      {
    
       const char* S = BN_bn2hex(tag->s);
       const char* X = BN_bn2hex(tag->x);

       const char* z = set_hash(S,X);
       ptr->dhash = z;             

      }
      
    //join down pointers
      if(i>0)
      {
        x = result[i-1]->rgt;
        ptr->dwn = x;
        result[i-1]->bit = 0;
      }
   }

calc_rank(); 
calc_hash();

}

unsigned long long SkipList::insert(unsigned long long searchKey,struct tag_t* tag)
{
   unsigned long long key = searchKey + 1;
   vector<node*> result = update(key);
   node *ptr, *x;
   int coin = 1 ;
   unsigned long long i = 0;
   const char* a = "";
unsigned long long count = 0;
  do
   {
      ptr = makeNode();
      count++;
      x = result[i]->rgt;
      result[i]->rgt = ptr;
      ptr->rgt = x;
     
      ptr->bit = result[i]->bit;
      result[i]->bit = 1;
      ptr->level = i;  

     //insert leaf node hash for tag
      if(i == 0)
      {
    
       const char* S = BN_bn2hex(tag->s);
       const char* X = BN_bn2hex(tag->x);

       const char* z = set_hash(S,X);
       ptr->dhash = z;             

      }
      
    //join down pointers
      if(i>0)
      {
        x = result[i-1]->rgt;
        ptr->dwn = x;
        result[i-1]->bit = 0;
      }
  
      i++;
      coin = coinToss();
    }while(coin == 1);


calc_rank(); 
calc_hash();
return count;
}



const char* SkipList:: root()
{
 const char* root;
 root = get_root();
 return root;
}


void SkipList::modify(int searchKey, struct tag_t* tag)
{
  const char* S = BN_bn2hex(tag->s);
  const char* X = BN_bn2hex(tag->x);
  node * x = search(searchKey+1);
  x->hash = set_hash(S,X);
}



node* SkipList::search(int searchKey)
{
  node *x;
  int key;
  x = head[maxLevel-1];

  if(searchKey == 1)
  return(head[0]->rgt);
  else
  {
       key = searchKey + 1;
       for(int i = maxLevel-1; i>=0; i--)
        {
          while( x->rgt->rnk != 0 && x->rgt != 0 && (canGoBelow(x,key) == false) && key != 1) //&& key != 1
          {
            key = key - (x->rnk - x->rgt->rnk);
            x = x->rgt;
          }

          if(i>0)
          {
            x = x->dwn;
          }
        }
       return x;
  }
}



void SkipList::remove(int searchKey)
{
 vector<node*> prev = update(searchKey+1);
 vector<node*> cur = update(searchKey+2);

 for(int i = 0;i<maxLevel;i++)
 {
  if(prev[i] != cur[i])
  {
    prev[i]->rgt = cur[i]->rgt;
    prev[i]->bit = cur[i]->bit;
    free(cur[i]);
  }
 }
  calc_rank();
  calc_hash();
}


void SkipList::print() const
{
 node *ptr;

   for(int i = 0; i < maxLevel ;i++)
   {
    ptr = head[i];
    cout<<"LEVEL: "<<i<<endl;

   while(ptr->rgt != 0)
   {
     cout<<"Level: "<<ptr->level<<"  Bit: "<<ptr->bit<<"  Hash:  "<<ptr->hash<<"  Rank:  "<<ptr->rnk<<"  Right: "<<ptr->rgt<<"  Down: "<<ptr->dwn<<endl;
     ptr = ptr->rgt;
   }
    cout<<endl;
   }
    cout<<endl;
}


SkipList& SkipList:: get_skiplist(SkipList& s, struct file* file_struct)
{
 srand((unsigned)time(0)); 
 unsigned long long m,n;
 SkipList s;
 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);

 struct parameters* params;
 params = generate_params(m,n);

 file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);

 struct file_vector* fv;
 

 for(unsigned long long i = 0;i<file_struct->nr_vectors;i++)
{
  fv = file_struct->vecs+i;
  s.insert(i,fv->tag);
}

return s;
}


struct proof_vector* SkipList::gen_proof(int searchKey, unsigned long long m)
{
  struct proof_vector* p_v = (struct proof_vector*)malloc(sizeof(struct proof_vector));
  p_v->pv = (struct proof*)malloc(sizeof(struct proof)*2*(maxLevel + m));

  struct proof* p;

  node* x;
  int key;
  int j = 0;
  p = p_v->pv+j;
  x = head[maxLevel-1];

  if(searchKey == 1)
   { 

      for(int i = maxLevel-1; i>= 0; i--)
       {

          x = head[i];

           p->rnk = x->rgt->rnk;        
           p->label = x->hash;
	   p->rlabel = x->rhash;
	   p->dlabel = x->dhash;
	   p->dz = 0;
	   p->bit = x->bit;
	   p->level = i;
           j++;
           p = p_v->pv+j;
      }
   }
 else
      {

       key = searchKey; 
       for(int i = maxLevel-1; i>=0; i--)
        {
          while( x->rgt->rnk != 0 && x->rgt != 0 && (canGoBelow(x,key) == false) && key != 1) 
          {  

             if(i==0)
             {
                  p->rnk = x->rgt->rnk;
             }
             else
             {
                  p->rnk = x->dwn->rnk;
	     }         
            
             p->label = x->hash;
	     p->rlabel = x->rhash;
	     p->dlabel = x->dhash;
	     p->dz = 1;
	     p->bit = x->bit;
	     p->level = i;
 	     j++;
             p = p_v->pv+j;
             key = key - (x->rnk - x->rgt->rnk);
             x = x->rgt;

          }

           
           p->label = x->hash;
	   p->rlabel = x->rhash;
	   p->dlabel = x->dhash;
           p->rnk = x->rgt->rnk;
	   p->dz = 0;
	   p->bit = x->bit;
	   p->level = i;
           j++;
           p = p_v->pv+j;
          
           if(i>0)
           {
            x = x->dwn;  
           }
      }
   }


p->dz = 2;   //to find size of array.

return p_v;
}


