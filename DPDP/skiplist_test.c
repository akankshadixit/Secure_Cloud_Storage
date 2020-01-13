#include <stdio.h>
#include<stdlib.h>
#include <time.h>


#include"rank_skiplist.h"
#include"sha256.h"
#include"keys.h"
#include"filehandle.h"


int main()
{
 srand((unsigned)time(0)); 
 clock_t s_file, end_file, s_tag, end_tag, s_skip, end_skip;
 long double file_time, tag_time, skip_time;
 SkipList s;
 unsigned long long m,n;


 const char* filename = {"test.txt"};
 s_file = clock();
 struct file* file_struct = get_file_vectors(filename);
 end_file = clock();

 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);

 struct parameters* params;

 s_tag = clock();
 params = generate_params(m,n);
 set_tags(file_struct,params);
 end_tag = clock();

 struct file_vector* fv;
 
 s_skip = clock();
 for(unsigned long long i = 0;i<file_struct->nr_vectors;i++){
  
  fv = file_struct->vecs+i;
  s.insert(i,fv->tag);
}
end_skip = clock();
 s.print();
 //s.remove(2);
 //s.print();
 const char* root = s.root();
 printf("%s\n",root);

file_time = ((long double) (end_file - s_file)) / CLOCKS_PER_SEC;
tag_time  = ((long double) (end_tag - s_tag)) / CLOCKS_PER_SEC;
skip_time = ((long double) (end_skip - s_skip)) / CLOCKS_PER_SEC;
printf("THE total time is : %Lf", file_time + tag_time + skip_time);
 return 0;

}



