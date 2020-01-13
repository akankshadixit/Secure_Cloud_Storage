#include<stdio.h>
#include <time.h>


#include"filehandle.h"
#include"keys.h"

int main()
{
 clock_t start, end;
 long double cpu_time_used;
 unsigned long long m,n;
 printf("Enter the value of m\n");
 scanf("%llu",&m);
 printf("Enter the value of n\n");
 scanf("%llu",&n);

 struct parameters* params;
 params = generate_params(m,n);

 const char* filename = "test.txt";
 struct file* f;
 f = get_file_vectors(filename);
  start = clock();

 set_tags(f,params);
 end = clock();

cpu_time_used = ((long double) (end - start)) / CLOCKS_PER_SEC;
printf("THE CPU tme used is : %Lf", cpu_time_used);
 return 0;
}
