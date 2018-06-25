#include<stdio.h>
#include<filehandle.h>


#include<keys.h>

int main()
{
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

set_tags(f,params);

return 0;
}
