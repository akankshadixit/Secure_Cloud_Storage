#ifndef PORT_H
#define PORT_H


#include<stdio.h>
#include<stdlib.h>

#include<rank_skiplist.h>

#define TAG_LEN 265
#define FIELD_ELEM_LEN 15


SkipList& export_skiplist(SkipList&,const char*);

struct file* import_file(const char*);
void export_file(struct file*,const char*);


#endif
