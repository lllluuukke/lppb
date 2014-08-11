#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"gui.h"

guint image_total = 0;
guint merged_total = 0;

gchar working_dir[256];

static void
get_path()
{
    if(getcwd(working_dir, sizeof(working_dir)) != NULL)
        fprintf(stdout, "Current working directory: %s\n", working_dir);
    else
        fprintf(stderr, "Error reading current directory\n");
}

int
main(int argc,
     char **argv)
{
    get_path();

    gui_main(argc, argv);

    return 0;
}

