#include"dsets.h"

void
init_dsets(gint64* ds, gint64 length)
{
    for(gint64 i = 0; i < length; i++)
        *(ds+i) = -1;
}

gint64
find(gint64* ds, gint64 index)
{
    if(*(ds+index) < 0)
        return index;

    return *(ds+index) = find(ds, *(ds+index));
}

void
merge(gint64* ds, gint64 a, gint64 b)
{
    gint64 r1 = find(ds, a);
    gint64 r2 = find(ds, b);

    if(r1 == r2)
        return;

    if(*(ds+r1) <= *(ds+r2))
    {
        *(ds+r1) += *(ds+r2);
        *(ds+r2) = r1;
    }
    else
    {
        *(ds+r2) += *(ds+r1);
        *(ds+r1) = r2;
    }
}

gint64
size(gint64* ds, gint64 index)
{
    return -1*(*ds+index);
}

