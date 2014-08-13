#ifndef _DSETS_H
#define _DSETS_H

#include<glib.h>

void init_dsets(gint64* ds, gint64 length);

gint64 find(gint64* ds, gint64 index);

void merge(gint64* ds, gint64 a, gint64 b);

gint64 size(gint64* ds, gint64 index);

#endif

