#include<stdio.h>
#include<stdlib.h>

#include<filehandle.h>
#include<keys.h>
#include<sha256.h>
#include<rank_skiplist.h>


int main()
{
 srand((unsigned)time(0)); 
 SkipList s;
 unsigned long long m,n;

 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);

 struct parameters* params;
 params = generate_params(m,n);

 const char* filename = {"test.txt"};
 struct file* file_struct = get_file_vectors(filename);
 set_tags(file_struct,params);

 struct file_vector* fv;
 

 for(unsigned long long i = 0;i<file_struct->nr_vectors;i++)
{
  fv = file_struct->vecs+i;
  s.insert(i,fv->tag);
}
 s.print();
 s.remove(2);
 s.print();
 const char* root = s.root();
 printf("%s\n",root);
 return 0;

}



