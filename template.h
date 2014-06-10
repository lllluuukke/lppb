#ifndef _TEMPLATE_H_
#define _TEMPLATE_H

#include<stdlib.h>

typedef enum {FALSE, TRUE} bool;

typedef struct _template Template;

struct _template
{
    char *filename;

    int width;
    int height;

    int hole_count;
    int **hole_pos;
    int **hole_size;
    
    bool was_first;
    bool was_last;

    Template *prev;
    Template *next;
};

Template *init_templates();

void destroy_templates(Template *t);

#endif

