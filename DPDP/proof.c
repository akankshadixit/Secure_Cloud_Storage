#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<string.h>
#include<openssl/bn.h>

#include"proof.h"
#include"filehandle.h"
#include"rank_skiplist.h"



void ListAuthRead(int searchKey,unsigned long long m)
{
  printf("Sending query for %dth element to server....\n",searchKey);
}


int get_size(struct proof_vector* pv)
{
  struct proof* p;
  int size = 0;
  p = pv->pv+0;

  for(int i = 0;p->dz!= 2;)
   {   
     size = i+1;
     i++;
     p = pv->pv+i;
   }
 
return size;

}

int ListVerifyRead(int searchKey, const char* root, struct proof_vector* p_v, unsigned long long m)
{
  int size;
  const char* str ;
  const char* a = "";

  struct proof* p;
  size = get_size(p_v);

  struct proof* prev;

  for(int i = size-1 ; i >=0 ; i--)
  {  
      p = p_v->pv+i;
      prev = p_v->pv+(i+1);
     
      if(p->level == 0)
       { 
          if(strcmp(p->dlabel,set_hash(a,a)) == 0)
           {
             if(p->bit == 0)
               p->rnk = 0;
             else
               p->rnk = p->rnk;
           }
          else
           {
             if(p->bit == 0)
                p->rnk = 1;
             else
                p->rnk = p->rnk + 1;
           }
        }
      else
       {
          if(p->dz == 0)
          {
            if(p->bit == 0)
               p->rnk = prev->rnk;
            else
               p->rnk = p->rnk + prev->rnk;
          }
         else
          {
            if(p->bit == 0)
               p->rnk = p->rnk;
            else
               p->rnk = p->rnk + prev->rnk;
          }
       }   
 }


   int k = size-1;
   p = p_v->pv+k;

   while(p->level == 0)
   {
     k--;
     p = p_v->pv+k;

   } 


   str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, p->rlabel );

   for(int i = k-1 ;i>=0 ;i--)
   {
      p = p_v->pv+i;

      if(p->dz == 0)
      str = set_level_hash(p->level, p->rnk, p->bit ,str, p->rlabel);
      else
      str = set_level_hash(p->level, p->rnk, p->bit , p->dlabel,str); 
   }

 if(strcmp(str,root) != 0)
 {
  return 0;
 }
 else
 {
  return 1;
 }
}




/*

const char* upd_root_insert(struct proof_vector* p_v , int level , struct tag_t* tag)
{

  const char* S = BN_bn2hex(tag->s);
  const char* X = BN_bn2hex(tag->x);
  const char* z = set_hash(S,X);
  const char* str;
  const char* a = "";
  int size = 0;

  struct proof* p;
  struct proof* pl;
  struct proof* prev;

  size = get_size(p_v);

  struct proof_vector* list = (struct proof_vector*)malloc(sizeof(struct proof_vector));
  list->pv = (struct proof*)malloc(sizeof(struct proof)*level);
  
  for(int i = size-1 , l = 0; l < level ; i--)
  { 
    p = p_v->pv+i;
    pl = list->pv+l;

    if(p->level == l)
    {
       pl->rlabel = p->rlabel;
       pl->bit = p->bit;


     if (l == level-1)
       p->bit = 1;
     else
       p->bit = 0;

    // setting rank values for the new nodes to be inserted
     if(pl->bit == 0 && l == 0)
     {
       pl->rnk = 1;
     }
     else if(pl->bit == 1 && l == 0)
     {
       pl->rnk = p->rnk+1;      
     }
     else if(pl->bit == 0 && l != 0)
     {
       struct proof* prevlist = list->pv+l-1;
       pl->rnk = prevlist->rnk ;      
     }
     else if(pl->bit == 1 && l != 0)
     {
       struct proof* prevlist = list->pv+l-1;
       pl->rnk = prevlist->rnk + p->rnk ;      
     }
   
     if(l == 0)
     pl->dlabel = z;
     else
     {
       struct proof* prevlist = list->pv+l-1;
       pl->dlabel = prevlist->label;
     }

     pl->level = l;

     pl->label = set_level_hash(pl->level , pl->rnk , pl->bit, pl->dlabel, pl->rlabel);

     if(p->bit == 0)
       p->rlabel = a;
     else
       p->rlabel = pl->label;

     l++; 
    }
 }



 for(int i = size-1 ; i >=0 ; i--)
  { 
    p = p_v->pv+i;
    prev = p_v->pv+(i+1);

    if(p->level < level)
    { 
        if(p->level == 0)
         { 
            if(strcmp(p->dlabel,set_hash(a,a)) == 0)
             {
                if(p->bit == 0)
                   p->rnk = 0;
                else
                   p->rnk = prev->rnk;
             }
            else
             {
               if(p->bit == 0)
                 p->rnk = 1;
               else
                { 
                   if(i == size-1)
                      {
                        pl = list->pv+(p->level);
                        p->rnk = pl->rnk + 1;
                      }
                   else
                      p->rnk = prev->rnk+1;
                }
             }
         }
        else
        {
             if(p->dz == 0)
             { 
               if(p->bit == 0)
                  p->rnk = prev->rnk;
               else
                 {
                   pl = list->pv+(p->level);
                   p->rnk = pl->rnk + prev->rnk;               
                 }
             }
            else
             {
               if(p->bit == 0)
                  p->rnk = p->rnk;
               else
                  p->rnk = p->rnk + prev->rnk;
            }
         }
     }
    else
    {
       if(p->level == 0)
       { 
          if(strcmp(p->dlabel,set_hash(a,a)) == 0)
           {
             if(p->bit == 0)
               p->rnk = 0;
             else
               p->rnk = p->rnk;
           }
          else
           {
             if(p->bit == 0)
                p->rnk = 1;
             else
                p->rnk = p->rnk + 1;
           }
        }
      else
       {
          if(p->dz == 0)
          {
            if(p->bit == 0)
               p->rnk = prev->rnk;
            else
               p->rnk = p->rnk + prev->rnk;
          }
         else
          {
            if(p->bit == 0)
               p->rnk = p->rnk;
            else
               p->rnk = p->rnk + prev->rnk;
          }
       }
    }
  }
  

 p = p_v->pv+(size-1);

 str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, p->rlabel);
 int k = size-2;

 p = p_v->pv+k;

 while(p->level == 0)
  {
    if(p->bit == 0)
    {
      str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, a );
    }
    else
    {
      str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, str );
    }
     
    k--;
    p = p_v->pv+k;
  } 

 

   for(int i = k ;i>=0 ;i--)
   {
      p = p_v->pv+i;

      if(p->dz == 0)
        str = set_level_hash(p->level, p->rnk, p->bit ,str, p->rlabel);
      else
      {
        if(p->bit == 0)
        {
	  str = set_level_hash(p->level, p->rnk, p->bit , p->dlabel,a);
        }
	else
 	{
           str = set_level_hash(p->level, p->rnk, p->bit , p->dlabel,str);
        }        
      }
   }

  return str;
}




const char* upd_root_del(struct proof_vector* prev, struct proof_vector* cur)
{
  const char* str;
  const char* a = "";
  struct proof* pr;
  struct proof* pc;

  int sizep = get_size(prev);
  int sizec = get_size(cur);

  pr = prev->pv+(sizep-1);
  pc = cur->pv+(sizec-1);
 
  int sp = sizep-1;
  int sc = sizec-1;

  int l = 0;

   while(pr->label != pc->label)
   {
     pr = prev->pv+sp;
     pc = cur->pv+sc;

      if((pr->level == l) && ( pc->level == l))
      {
         pr->bit = pc->bit;

         if(pc->bit == 0)
            pr->rlabel = a;
         else
            pr->rlabel = pc->rlabel;

        sp--;
        sc--;
      }
      else if((pr->level == l) && (pc->level != l)) 
      {
         sc--;
      }     
      else if((pr->level != l) && (pc->level == l))
      {
         sp--;
      }

      l++;      
   }

  struct proof *p, *pv;

  for(int i = sizep-1 ; i >=0 ; i--)
   { 
     p = prev->pv+i;
     pv = prev->pv+(i+1);

       if(p->level == 0)
       { 
          if(strcmp(p->dlabel,set_hash(a,a)) == 0)
           {
             if(p->bit == 0)
               p->rnk = 0;
             else
               p->rnk = p->rnk;
           }
          else
           {
             if(p->bit == 0)
                p->rnk = 1;
             else
                p->rnk = p->rnk + 1;
           }
        }
      else
       {
          if(p->dz == 0)
          {
            if(p->bit == 0)
               p->rnk = pv->rnk;
            else
               p->rnk = p->rnk + pv->rnk;
          }
         else
          {
            if(p->bit == 0)
               p->rnk = p->rnk;
            else
               p->rnk = p->rnk + pv->rnk;
          }
       }
 }

 p = prev->pv+(sizep-1);
 str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, p->rlabel);

 int k = sizep-2;
 p = prev->pv+k;

  while(p->level == 0)
   {
     if(p->bit == 0)
       str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, a );   
     else
       str = set_level_hash(p->level, p->rnk, p->bit, p->dlabel, str );
     
     k--;
     p = prev->pv+k;
   } 

 

   for(int i = k ;i>=0 ;i--)
   {
      p = prev->pv+i;

      if(p->dz == 0)
        str = set_level_hash(p->level, p->rnk, p->bit ,str, p->rlabel);
      else
      {
        if(p->bit == 0)
	  str = set_level_hash(p->level, p->rnk, p->bit , p->dlabel,a);
	else
          str = set_level_hash(p->level, p->rnk, p->bit , p->dlabel,str);       
      }
  }

 return str;
}

*/



