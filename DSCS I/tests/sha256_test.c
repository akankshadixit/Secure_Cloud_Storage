#include<stdio.h>
#include<stdlib.h>

#include<sha256.h>

int main()
{
  sha256_hash_t buffer[32];

  unsigned char* msg = "Indian Statistical Institute";
  

  sha256(buffer,msg,8*28);

  char* string = hexstring((unsigned char*)buffer,32);
  printf("%s",string);

}
