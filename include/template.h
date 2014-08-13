#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include<stdlib.h>

#include<glib.h>

typedef struct _template Template;

struct _template
{
    guint8 unique_id;
    gchar* filename;

    guint16 width;
    guint16 height;

    guint8 hole_count;
    guint16** hole_pos;
    guint16** hole_size;
    
    gboolean was_first;
    gboolean was_last;

    Template* prev;
    Template* next;
};

Template* init_templates();

void destroy_templates(Template* t);

#endif

