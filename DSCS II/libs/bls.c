#include <pbc/pbc.h>
#include <string.h>

#include <sha256.h>
//#include <keys.h>
#include <bls.h>



struct element_s* bls_hash(void* data, int length, pairing_t pairing)
{
    struct element_s* new_elem = (struct element_s*)malloc(sizeof(element_t));
    element_init_G1(new_elem, pairing);
    element_from_hash(new_elem,data,length);
    return new_elem;
}

struct element_s* bls_hash_int(unsigned long long fid, unsigned long long value, pairing_t pairing)
{
    struct element_s* ret;                                                             
    char int_str[22]; 
    unsigned long long pow = 10;

    while(value >= pow)
        pow *= 10;

    unsigned long long concat =  (fid * pow) + value; 
    //printf("concat is %llu\n", concat);

    int len = sprintf(int_str,"%llu",concat);
    ret = bls_hash((void*)int_str,len,pairing);
    return ret; 
    
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
