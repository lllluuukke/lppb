#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>

#include<wand/magick_wand.h>

#include"template.h"

#define PATH_TO_TEMPLATES "./templates"

static void
print_template_info(Template *t)
{
    int i;
    // Filename, dimension, number of holes
    fprintf(stdout, "%s: %dx%d\n%d holes found\n", t->filename, t->width,
            t->height, t->hole_count);
    // Hole sizes and locations
    for(i = 0; i<t->hole_count; i++)
        fprintf(stdout, "    Start @ (%d, %d),\tsize %dx%d\n",
                t->hole_pos[i][0], t->hole_pos[i][1],
                t->hole_size[i][0], t->hole_size[i][1]);

    // Indicate first/last template
    if(t->was_first && t->was_last)
        fprintf(stdout, "This is the only template.\n\n");
    else if(t->was_first)
        fprintf(stdout, "This is the first template.\n\n");
    else if(t->was_last)
        fprintf(stdout, "This is the last template.\n\n");
    else
        fprintf(stdout, "\n");

    // Recurse
    if(!t->was_last)
        print_template_info(t->next);
}

static void
get_holes_from_alpha(Template *t, int **alpha_map)
{
    t->hole_count= 0;

    int i, j;
    for(i = 0; i<t->height-1; i++)
    {
        for(j = 0; j<t->width-1; j++)
        {
            // Start (upper left corner) of the hole
            if(((i == 0 && j == 0) || //start on top-left corner of image
                (i == 0 && j > 0 && alpha_map[i][j-1]) || //start on top edge
                (i > 0 && j == 0 && alpha_map[i-1][j]) || //start on left edge
                (i > 0 && j > 0 && alpha_map[i-1][j] &&
                 alpha_map[i][j-1])) //start elsewhere
                    && !alpha_map[i][j]) //actual start point
            {
                if(!t->hole_count)
                {
                    t->hole_pos = malloc(sizeof(int *));
                    t->hole_size= malloc(sizeof(int *));
                }
                else
                {
                    t->hole_pos =
                        realloc(t->hole_pos, sizeof(int *)*(t->hole_count+1));
                    t->hole_size =                                          
                        realloc(t->hole_size, sizeof(int *)*(t->hole_count+1));
                }

                int *pos = malloc(sizeof(int)*2);
                pos[0] = i;
                pos[1] = j;
                t->hole_pos[t->hole_count] = pos;

                // Getting width and height of the hole
                int *size = malloc(sizeof(int)*2);
                size[0] = 0;
                size[1] = 0;
                while(!alpha_map[i][j+size[0]])
                {
                    size[0]++;
                    if(size[0]+j == t->width)
                        break;
                }
                while(!alpha_map[i+size[1]][j])
                {
                    size[1]++;
                    if(size[1]+i == t->height)
                        break;
                }
                t->hole_size[t->hole_count] = size;

                t->hole_count++;
            }
        }
    }
}

static void
scan_holes(Template *t, MagickWand *wand)
{
    int **alpha_map = malloc(sizeof(int *)*t->width);
    PixelWand *pw = NewPixelWand();

    // Map alpha value of every pixel
    int i, j;
    for(i = 0; i<t->height; i++)
    {
        int *alpha_line = malloc(sizeof(int *)*t->width);
        for(j = 0; j<t->width; j++)
        {
            MagickGetImagePixelColor(wand, j, i, pw);
            alpha_line[j] = (int)PixelGetAlpha(pw);
            ClearPixelWand(pw);
        }
        alpha_map[i] = alpha_line;
    }

    DestroyPixelWand(pw);

    // Finding rectangular holes in a template
    get_holes_from_alpha(t, alpha_map);
    
    for(i = 0; i<t->height; i++)
        free(alpha_map[i]);
    free(alpha_map);
}

static void
get_template_info(Template *this)
{
    MagickWand *wand = NULL;
    MagickWandGenesis();
    wand = NewMagickWand();
    MagickReadImage(wand, this->filename);

    this->width = (int)MagickGetImageWidth(wand);
    this->height = (int)MagickGetImageHeight(wand);

    scan_holes(this, wand);

    wand = DestroyMagickWand(wand);
    MagickWandTerminus();
}

static Template
*create_link(char **name, Template *this, int count)
{
    this->filename = malloc(strlen(name[0])+1);
    strcpy(this->filename, name[0]);
    free(name[0]);

    get_template_info(this);

    this->was_first = FALSE;
    this->was_last = FALSE;

    if(count == 1)
        return this;
    else
    {
        Template *new = malloc(sizeof(Template));
        this->next = new;
        new->prev = this;
        return create_link(name+1, new, count-1);
    }
}

static char
**list_templates(char **name, int *count_ext)
{
    int count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(PATH_TO_TEMPLATES);
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, "..") > 0)
            {
                name = realloc(name, sizeof(char*)*(count+1));
                name[count] = malloc((strlen(dir->d_name)+11)*sizeof(char));
                strcpy(name[count], "templates/");
                strcat(name[count], dir->d_name);
                /*fprintf(stdout, "#%d: %s\n", count, name[count]);*/
                count++;
            }
        }

        closedir(d);
    }

    *count_ext = count;
    return name;
}

Template
*init_templates()
{
    /* Read template file names from ./templates */
    int num= 0;
    char **names = malloc(sizeof(char*));

    names = list_templates(names, &num);
    if(num == 0)
        fprintf(stderr, "ERROR: No template file found.\n");

    /* TEST: populate image filenames */
    Template *first = (Template *)malloc(sizeof(Template));
    Template *last = create_link(names, first, num);

    if(num == 1)
    {
        first->was_first = TRUE;
        first->was_last = TRUE;
    }
    else
    {
        first->was_first = TRUE;
        first->was_last = FALSE;
        last->was_first = FALSE;
        last->was_last = TRUE;
    }

    print_template_info(first);

    free(names);

    return first;
}

void
destroy_templates(Template *t)
{
    if(!t->was_last)
        destroy_templates(t->next);
    
    free(t->filename);

    int i;
    for(i = 0; i<t->hole_count; i++)
    {
        free(t->hole_pos[i]);
        free(t->hole_size[i]);
    }
    free(t->hole_pos);
    free(t->hole_size);
}

int
main(int argc, char **argv)
{
    Template *first = init_templates();
    destroy_templates(first);
    free(first);

    return 0;
}

