#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include<glib.h>
#include<gtk/gtk.h>

#include"icon.h"

#define VALID "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

extern char working_dir;

/* Paths */
gchar template_path[256], accept_path[256], noo_path[256], person_path[256];
gchar hole_path[256], person[100];
gchar candidate_path[3][256], merged_path[256], script_path[256];

/* Coordinates for DnD on template */
guint x_shift = 97;

gint template_w = 1080;
gint template_h = 720;

gint hole_x[3] = {64, 34, 540};
gint hole_y[3] = {96, 796, 796};

gint hole_sizes_w[3] = {900, 450, 450};
gint hole_sizes_h[3] = {600, 300, 300};

static GtkWidget *lastname_entry;
static GtkWidget *dialog;
static GtkWidget *notebook;
static GtkWidget *image;
static GtkWidget *candidates[3];
static GtkWidget *template_fixed_hole[3];

extern guint image_total;
extern guint merged_total;

static guint per_person;
static guint merged[3] = {0, 1, 2};
static gchar info_buffer[20];
static GtkWidget *info_label;

static void
get_path(gchar relative[256],
         gchar absolute[256])
{
    if(realpath(relative, absolute) != NULL)
        fprintf(stdout, "Relative: %s  Absolute: %s\n", relative, absolute);
    else
        fprintf(stderr, "Error reading directory %s\n", relative);
}

static gboolean
quitit(GtkWidget   *wig,
       GdkEventKey *event,
       gpointer    data)
{
    if(event->state & GDK_CONTROL_MASK && event->keyval == 'd' )
        gtk_main_quit();

    return FALSE;
}

static void
update_info_label()
{
    g_snprintf(info_buffer, 19, "Picture %1d out of 3", per_person+1);
    gtk_label_set_text(GTK_LABEL(info_label), info_buffer);
}

/**************************Malnipulate lastname begin*************************/
static void
create_dir(guint16 length,
           gchar   *lastname)
{
    gchar buf[100];
    gint ret_val;

    g_snprintf(buf, length+16, "mkdir captures/%s",
               g_ascii_strup(lastname, length));
    g_print("exec: %s\n", buf);

    ret_val = system(buf);
    g_print("result: %d\n", ret_val);
}

static gint
add_record(guint16 length,
           gchar   *lastname)
{
    gchar *lastname_corrected;

    if(length == 0)
        return 1;

    lastname_corrected = g_ascii_strup(lastname, length);
    lastname_corrected = g_strcanon(lastname, VALID, '_');

    length = strlen(lastname_corrected);

    create_dir(length, lastname_corrected);

    g_snprintf(person, length+11, "captures/%s/",
               g_ascii_strup(lastname_corrected, length));

    
    get_path(person, person_path);

    return 0;
}
/***************************Malnipulate lastname end**************************/
/*****************************Drag and Drop START*****************************/
enum {
    TARGET_STRING,
    TARGET_IMAGE
};

static GtkTargetEntry target_list[] = {
  {"STRING",     0, TARGET_STRING},
  {"text/plain", 0, TARGET_STRING},
  {"image/jpeg", 0, TARGET_IMAGE}
};

static guint n_targets = G_N_ELEMENTS(target_list);

static void  
drag_data_received_handl(GtkWidget          *widget,
					     GdkDragContext     *context,
					     gint                x,
					     gint                y,
					     GtkSelectionData   *sel_data,
					     guint               info,
					     guint               time,
                         gpointer            data)
{
    gint i;
    guint candid = GPOINTER_TO_UINT(data);

    g_print("%s to dest %d\n",
            (gchar *)gtk_selection_data_get_data(sel_data), candid);
    i=g_strcmp0((gchar *)gtk_selection_data_get_data(sel_data), "Candidate 1");
    switch(i){
        case -1:
            merged[candid] = 0;
            break;
        case 0:
            merged[candid] = 1;
            break;
        case 1:
            merged[candid] = 2;
            break;
        default:
            break;
    }

    GtkWidget *source_widget;
    GdkPixbuf *source_buf;
    GtkWidget *parent;
    GdkPixbuf *buf;
    GtkWidget *new_widget;

    parent = gtk_widget_get_parent(GTK_WIDGET(widget));
    source_widget = gtk_drag_get_source_widget(context);
    source_widget = gtk_button_get_image(GTK_BUTTON(source_widget));
    source_buf = gtk_image_get_pixbuf(GTK_IMAGE(source_widget));
    buf = gdk_pixbuf_scale_simple(source_buf, hole_sizes_w[candid],
                                  hole_sizes_h[candid], GDK_INTERP_BILINEAR);
    new_widget = gtk_image_new_from_pixbuf(buf);
    g_object_unref(buf);

    gtk_fixed_put(GTK_FIXED(parent), new_widget, hole_x[candid],
                                                 hole_y[candid]);

    gtk_widget_show_all(new_widget);

    gtk_drag_finish (context, TRUE, FALSE, time);
}

static void  
drag_data_get_handl(GtkWidget          *widget,
		             GdkDragContext     *context,
		             GtkSelectionData   *sel_data,
		             guint               info,
		             guint               time,
		             gpointer            data)
{
    guint i = GPOINTER_TO_UINT(data);

    g_print("\nGet data...\n");
    switch(i) {
        case 0:
            gtk_selection_data_set(sel_data, gtk_selection_data_get_target
                                   (sel_data), 8, (guchar *)"Candidate 0", 11);
            break;
        case 1:
            gtk_selection_data_set(sel_data, gtk_selection_data_get_target
                                   (sel_data), 8, (guchar *)"Candidate 1", 11);
            break;
        case 2:
            gtk_selection_data_set(sel_data, gtk_selection_data_get_target
                                   (sel_data), 8, (guchar *)"Candidate 2", 11);
            break;
        default:
            break;
    }
}
  
/*****************************Drag and Drop STOP******************************/

static void
get_pos()
{
    gint i;
    for(i = 0; i<3; i++){
        hole_x[i] = x_shift+hole_x[i]*template_w/1800;
        hole_y[i] = hole_y[i]*template_h/1200;

        hole_sizes_w[i] = 1+hole_sizes_w[i]*template_w/1800;
        hole_sizes_h[i] = 1+hole_sizes_h[i]*template_h/1200;

        /*g_print("hole%d\tpos\tx:%d\ty:%d\t", i, hole_x[i], hole_y[i]);
        g_print("\tsize\tx:%d\ty:%d\n", hole_sizes_w[i], hole_sizes_h[i]);*/
    }
}

static void
create_done()
{
    GtkWidget *done_label;
    GtkWidget *done_page_label;

    PangoFontDescription *fd;
    fd= pango_font_description_from_string("50");

    done_label = gtk_label_new("Your picture has been saved.\n"
                               "\nEnjoy the night.\n\n");
    gtk_label_set_justify(GTK_LABEL(done_label), GTK_JUSTIFY_CENTER);
    gtk_widget_modify_font(done_label, fd);
    
    done_page_label = gtk_label_new("Done");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), done_label,
                             done_page_label);
}

static void
save_script()
{
    gchar script[1024], chmod[256];
    gint length = 2*(strlen(candidate_path[0])+
                     strlen(candidate_path[1])+
                     strlen(candidate_path[2]))+
                     strlen(merged_path)+
                     strlen(script_path);
    g_snprintf(script, length+63,
       "printf '#!/bin/sh\n../../resize.sh 0 %s %s %s\n../../merge.sh %s %s %s %s\n' > %s",
      candidate_path[merged[0]], candidate_path[merged[1]],
      candidate_path[merged[2]], candidate_path[merged[0]],
      candidate_path[merged[1]], candidate_path[merged[2]],
      merged_path, script_path);
    system(script);
    g_snprintf(chmod, strlen(script_path)+10, "chmod +x %s", script_path);
    system(chmod);
}

static void
save_merged(GtkWidget *widget,
            gpointer   data)
{
    save_script();
    gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
    while(gtk_events_pending())
        gtk_main_iteration();
    sleep(5);
    gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
    gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
    gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
    gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
}

static void
create_merge()
{
    guint i;

    get_pos();

    GdkPixbuf *drag_icon_buf;
    GdkPixbuf *hole_default_buf;
    GtkWidget *template;
    GdkPixbuf *template_buf;
    GtkWidget *template_fixed;
    GdkPixbuf *template_fixed_hole_buf[3];
    GtkWidget *candidate_button[3];
    GtkWidget *save_button;
    GtkWidget *save_button_label;
    GtkWidget *candidate_hbox;
    GtkWidget *merge_vbox;
    GtkWidget *merge_page_label;

    PangoFontDescription *fd_button;
    fd_button = pango_font_description_from_string("30");

    drag_icon_buf = gdk_pixbuf_new_from_xpm_data(icon);
    drag_icon_buf = gdk_pixbuf_scale_simple(drag_icon_buf, 84, 60,
                                            GDK_INTERP_BILINEAR);

    hole_default_buf = gdk_pixbuf_new_from_file(hole_path, NULL);

    template_buf = gdk_pixbuf_new_from_file(template_path, NULL);
    template_buf = gdk_pixbuf_scale_simple(template_buf, template_w,
                                           template_h, GDK_INTERP_BILINEAR);
    template = gtk_image_new_from_pixbuf(template_buf);
    template_fixed = gtk_fixed_new();

    for(i = 0; i<3; i++){
        template_fixed_hole_buf[i] = 
              gdk_pixbuf_scale_simple(hole_default_buf, hole_sizes_w[i],
                                      hole_sizes_h[i], GDK_INTERP_BILINEAR);
        template_fixed_hole[i] = 
                gtk_image_new_from_pixbuf(template_fixed_hole_buf[i]);
        g_object_unref(template_fixed_hole_buf[i]);
        gtk_fixed_put(GTK_FIXED(template_fixed), template_fixed_hole[i],
                      hole_x[i], hole_y[i]);
    }
    gtk_fixed_put(GTK_FIXED(template_fixed), template, x_shift, 0);

    candidate_hbox = gtk_hbox_new(FALSE, 0);

    for(i = 0; i<3; i++){
        candidates[i] = gtk_image_new();
        candidate_button[i] = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(candidate_button[i]), candidates[i]);
        gtk_button_set_relief(GTK_BUTTON(candidate_button[i]),GTK_RELIEF_NONE);

        gtk_box_pack_start(GTK_BOX(candidate_hbox),candidate_button[i],
                           TRUE, FALSE, 0);
    }

    save_button = gtk_button_new_with_label("Save!");
    save_button_label = gtk_bin_get_child(GTK_BIN(save_button));
    gtk_widget_modify_font(save_button_label, fd_button);
    gtk_box_pack_start(GTK_BOX(candidate_hbox), save_button, TRUE, FALSE, 0);

    merge_vbox = gtk_vbox_new(FALSE, 15);
    gtk_box_pack_start(GTK_BOX(merge_vbox), template_fixed, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(merge_vbox), candidate_hbox, TRUE, FALSE, 0);
    gtk_box_set_child_packing(GTK_BOX(merge_vbox), template_fixed, FALSE,
                              FALSE, 0, GTK_PACK_START);
    gtk_box_set_child_packing(GTK_BOX(merge_vbox), candidate_hbox, FALSE,
                              FALSE, 0, GTK_PACK_START);

    merge_page_label = gtk_label_new("Merge");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), merge_vbox,
                             merge_page_label);

    for(i = 0; i<3; i++){
        gtk_drag_dest_set(template_fixed_hole[i],
                          GTK_DEST_DEFAULT_ALL, target_list, n_targets,
                          GDK_ACTION_COPY|GDK_ACTION_MOVE);
        g_signal_connect(template_fixed_hole[i], "drag-data-received",
                    G_CALLBACK(drag_data_received_handl), GUINT_TO_POINTER(i));
    }
 
    for(i = 0; i<3; i++){
        gtk_drag_source_set(candidate_button[i], GDK_BUTTON1_MASK, target_list,
                            n_targets, GDK_ACTION_COPY|GDK_ACTION_MOVE);
        g_signal_connect(candidate_button[i], "drag-data-get",
                         G_CALLBACK(drag_data_get_handl), GUINT_TO_POINTER(i));
    }

    for(i = 0; i<3; i++){
        gtk_drag_source_set_icon_pixbuf(candidate_button[i], drag_icon_buf);
    }

    g_signal_connect(save_button, "clicked",
                     G_CALLBACK(save_merged), NULL);
}

static void
prep_merge()
{
    gint i;
    GtkWidget *parent;
    GtkWidget *new_widget;
    GdkPixbuf *image_buf;
    GdkPixbuf *new_buf;

    for(i = 0; i<3; i++){
        image_buf = gdk_pixbuf_new_from_file(candidate_path[i], NULL);
        new_buf = gdk_pixbuf_scale_simple(image_buf, 360, 240,
                GDK_INTERP_BILINEAR);
        g_object_unref(image_buf);
        gtk_image_set_from_pixbuf(GTK_IMAGE(candidates[i]), new_buf);
    }
    g_object_unref(image_buf);
    g_object_unref(new_buf);

    parent = gtk_widget_get_parent(template_fixed_hole[0]);
    image_buf = gdk_pixbuf_new_from_file(noo_path, NULL);
    for(i = 0; i<3; i++){
        new_buf = gdk_pixbuf_scale_simple(image_buf, hole_sizes_w[i],
                                          hole_sizes_h[i],
                                          GDK_INTERP_BILINEAR);
        new_widget = gtk_image_new_from_pixbuf(new_buf);
        g_object_unref(new_buf);
        gtk_fixed_put(GTK_FIXED(parent), new_widget, hole_x[i], hole_y[i]);
        gtk_widget_show_all(GTK_WIDGET(new_widget));
    }
    g_object_unref(image_buf);
}

static void
discard_image(GtkWidget *button,
              gpointer  data)
{
    GdkPixbuf *buf;

    buf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    g_object_unref(buf);

    g_print("\ndiscard image...  ");
    gint ret_val;

    ret_val = system("rm -f capt0000.jpg");
    if(ret_val == 0)
        fprintf(stdout, "Image discarded.\n");
    else
        fprintf(stderr, "Problem discarding capt0000.jpg\n");

    update_info_label();
    gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
}

static void
accept_image(GtkWidget *button,
             gpointer  data)
{
    gchar path[512], merge[512], script[512];
    GdkPixbuf *buf;

    buf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    g_object_unref(buf);

    g_print("\naccept image %d...  ", per_person);

    image_total++;
    g_print("Total: %d\n", image_total);

    gchar move_img[25];

    /* mv capt0000.jpg /capture/PERSON/XXXX.jpg */
    if(image_total < 10){
        g_snprintf(move_img, 25+strlen(person),
                   "mv capt0000.jpg %s000%1d.jpg", person, image_total);
        g_snprintf(path, strlen(person)+10,
                   "%s/000%1d.jpg", person, image_total);
        g_snprintf(merge, 15+strlen(person), "%soutput000%1d.jpg",
                   person, image_total);
        g_snprintf(script, 14+strlen(person), "%soutput000%1d.sh",
                   person, image_total);
    }
    else if(image_total < 100){
        g_snprintf(move_img, 25+strlen(person),
                   "mv capt0000.jpg %s00%2d.jpg", person, image_total);
        g_snprintf(path, strlen(person)+10,
                   "%s/00%2d.jpg", person, image_total);
        g_snprintf(merge, 15+strlen(person), "%soutput00%2d.jpg",
                   person, image_total);
        g_snprintf(script, 14+strlen(person), "%soutput00%2d.sh",
                   person, image_total);
    }
    else if(image_total < 1000){
        g_snprintf(move_img, 25+strlen(person),
                   "mv capt0000.jpg %s0%3d.jpg", person, image_total);
        g_snprintf(path, strlen(person)+10,
                   "%s/0%3d.jpg", person, image_total);
        g_snprintf(merge, 15+strlen(person), "%soutput0%3d.jpg",
                   person, image_total);
        g_snprintf(script, 14+strlen(person), "%soutput0%3d.sh",
                   person, image_total);
    }
    else{
        g_snprintf(move_img, 25+strlen(person),
                   "mv capt0000.jpg %s%4d.jpg", person, image_total);
        g_snprintf(path, strlen(person)+10,
                   "%s/%4d.jpg", person, image_total);
        g_snprintf(merge, 15+strlen(person), "%soutput%4d.jpg",
                   person, image_total);
        g_snprintf(script, 14+strlen(person), "%soutput%4d.sh",
                   person, image_total);
    }
    system(move_img);
    get_path(path, candidate_path[per_person]);

    if(per_person < 2){
        per_person++;
        update_info_label();
        gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
    }
    else{
        get_path(merge, merged_path);
        get_path(script, script_path);
        per_person = 0;
        merged_total++;
        prep_merge();
        gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
    }
}

static void
create_captured()
{
    GtkWidget *discard_button;
    GtkWidget *discard_button_label;
    GtkWidget *accept_button;
    GtkWidget *accept_button_label;
    GtkWidget *image_hbox;
    GtkWidget *image_vbox;
    GtkWidget *image_page_label;

    PangoFontDescription *fd_button;
    fd_button = pango_font_description_from_string("32");

    image = gtk_image_new();

    discard_button = gtk_button_new_with_label("Discard image");
    discard_button_label = gtk_bin_get_child(GTK_BIN(discard_button));
    gtk_widget_modify_font(discard_button_label, fd_button);

    accept_button = gtk_button_new_with_label("This is GOOD!");
    accept_button_label = gtk_bin_get_child(GTK_BIN(accept_button));
    gtk_widget_modify_font(accept_button_label, fd_button);

    image_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(image_hbox), discard_button, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(image_hbox), accept_button, TRUE, FALSE, 0);

    image_vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(image_vbox), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(image_vbox), image_hbox, FALSE, FALSE, 30);

    image_page_label = gtk_label_new("Image");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), image_vbox,
                             image_page_label);

    g_signal_connect(discard_button, "clicked", G_CALLBACK(discard_image),
                     NULL);
    g_signal_connect(accept_button, "clicked", G_CALLBACK(accept_image),
                     NULL);
}

static void
prep_image()
{
    GdkPixbuf *image_buf;
    GdkPixbuf *new_buf;

    image_buf = gdk_pixbuf_new_from_file(accept_path, NULL);
    new_buf = gdk_pixbuf_scale_simple(image_buf, 1275, 850,
                                        GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(GTK_IMAGE(image), new_buf);
    g_object_unref(image_buf);
}

static void
retry_shoot(GtkWidget *button,
            gpointer  data)
{
    GtkWidget *warning;
    warning = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK,
                                     "Please ask the teacher for help.");
    gtk_window_set_title(GTK_WINDOW(warning), "Oops");
    gtk_window_set_position(GTK_WINDOW(warning), GTK_WIN_POS_CENTER);
    gtk_dialog_run(GTK_DIALOG(warning));
    gtk_widget_destroy(warning);
}

static void
take_a_shot(GtkWidget *button,
            gpointer  data)
{
    GtkWidget *label = data;
    gint cd;
    gchar cd_buf[2];

    for(cd = 3; cd > 0; cd--){
        g_snprintf(cd_buf, 2, "%1d", cd);
        gtk_label_set_text(GTK_LABEL(label), cd_buf);
        while(gtk_events_pending())
            gtk_main_iteration();
        sleep(1);
    }
    gtk_label_set_text(GTK_LABEL(label), "Wait...");
    while(gtk_events_pending())
        gtk_main_iteration();

    gint ret_val;
    system("rm -f capt0000.jpg");
    ret_val = system("gphoto2 --quiet --capture-image-and-download");
    gtk_label_set_text(GTK_LABEL(label), "");
    if(ret_val == 0){
        prep_image();
        gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
    }
    else{
        retry_shoot(button, NULL);
    }
}

static void
create_shoot()
{
    GtkWidget *shoot_button;
    GtkWidget *shoot_button_label;
    GtkWidget *countdown_label;
    GtkWidget *shoot_vbox;
    GtkWidget *shoot_page_label;

    PangoFontDescription *fd_button, *fd_label, *fd_info;
    fd_button= pango_font_description_from_string("80");
    fd_label= pango_font_description_from_string("200");
    fd_info = pango_font_description_from_string("40");

    shoot_button = gtk_button_new_with_label("Take a picture");
    shoot_button_label = gtk_bin_get_child(GTK_BIN(shoot_button));
    gtk_widget_modify_font(shoot_button_label, fd_button);
    gtk_widget_set_size_request(shoot_button, 300, 300);
    
    g_snprintf(info_buffer, 19, "Picture %1d out of 3", per_person+1);
    info_label = gtk_label_new(info_buffer);
    gtk_widget_modify_font(GTK_WIDGET(info_label), fd_info);

    countdown_label = gtk_label_new("");
    gtk_widget_modify_font(countdown_label, fd_label);

    shoot_vbox = gtk_vbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(shoot_vbox), countdown_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(shoot_vbox), shoot_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(shoot_vbox), info_label, FALSE, FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(shoot_vbox), 100);

    shoot_page_label = gtk_label_new("Shoot");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), shoot_vbox,
                             shoot_page_label);

    g_signal_connect(shoot_button, "clicked", G_CALLBACK(take_a_shot),
                     countdown_label);
}

static void
warn_empty_lastname(GtkWidget *button,
                      gpointer  data)
{
    GtkWidget *warning;
    warning = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK,
                                     "Please enter your last name.");
    gtk_window_set_title(GTK_WINDOW(warning), "Warning");
    gtk_window_set_position(GTK_WINDOW(warning), GTK_WIN_POS_CENTER);
    gtk_dialog_run(GTK_DIALOG(warning));
    gtk_widget_destroy(warning);
}

static gint
record_lastname(GtkWidget *button,
                gpointer  data)
{
    gint ret_val;

    ret_val = add_record(gtk_entry_get_text_length(GTK_ENTRY(lastname_entry)),
                    (gchar *)gtk_entry_get_text(GTK_ENTRY(lastname_entry)));

    if(ret_val != 0){
        if(ret_val == 1)
            warn_empty_lastname(button, NULL);
        return 1;
    }

    gtk_entry_set_text(GTK_ENTRY(lastname_entry), "");

    update_info_label();
    gtk_notebook_next_page(GTK_NOTEBOOK(notebook));

    return 0;
}

static gboolean
record_lastname_kp(GtkWidget   *entry,
                   GdkEventKey *event,
                   gpointer    data)
{
    /* GDK_KEY_Return or 65293 or FF0D */
    if(event->keyval == 65293)
        record_lastname(entry, data);

    return FALSE;
}

static void
create_welcome()
{
    GtkWidget *welcome_label;
    GtkWidget *lastname_label;
    GtkWidget *proceed_button;
    GtkWidget *proceed_label;
    GtkWidget *info_label;
    GtkWidget *lastname_hbox;
    GtkWidget *lastname_vbox;
    GtkWidget *name_page_label;
    
    PangoFontDescription *fd_welcome, *fd_button;
    fd_welcome = pango_font_description_from_string("36");
    fd_button = pango_font_description_from_string("26");

    welcome_label = gtk_label_new("\n\nWelcome to Luke's photobooth.\n\n"
                                  "Please enter your last name.\n"
                                  "(First 6 letters ONLY!)");
    gtk_label_set_justify(GTK_LABEL(welcome_label), GTK_JUSTIFY_CENTER);
    gtk_widget_modify_font(welcome_label, fd_welcome);
    
    lastname_label = gtk_label_new("Last Name: ");
    gtk_widget_modify_font(lastname_label, fd_welcome);
    
    lastname_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(lastname_entry), 20);
    gtk_entry_set_text(GTK_ENTRY(lastname_entry), "");
    gtk_entry_set_max_length(GTK_ENTRY(lastname_entry), 6);
    gtk_widget_modify_font(lastname_entry, fd_welcome);

    proceed_button = gtk_button_new_with_label("Continue");
    proceed_label = gtk_bin_get_child(GTK_BIN(proceed_button));
    gtk_widget_modify_font(proceed_label, fd_button);

    info_label = gtk_label_new("Careful: the name you entered here will be used to claim you pictures.");

    lastname_hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(lastname_hbox), lastname_label, FALSE,FALSE,10);
    gtk_box_pack_start(GTK_BOX(lastname_hbox), lastname_entry, FALSE,FALSE,10);
    gtk_box_pack_start(GTK_BOX(lastname_hbox), proceed_button, FALSE,FALSE,10);

    lastname_vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(lastname_vbox), welcome_label, FALSE,FALSE, 80);
    gtk_box_pack_start(GTK_BOX(lastname_vbox), lastname_hbox, FALSE,FALSE, 30);
    gtk_box_pack_start(GTK_BOX(lastname_vbox), info_label, FALSE, FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(lastname_vbox), 100);

    name_page_label = gtk_label_new("Name");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lastname_vbox,
                             name_page_label);

    g_signal_connect(lastname_entry, "key_press_event",
                     G_CALLBACK(record_lastname_kp), NULL);
    g_signal_connect(proceed_button, "clicked",
                     G_CALLBACK(record_lastname), NULL);
}

void
gui_main(int argc, char **argv)
{
    gchar temp[256] = "./template.jpg";
    gchar acce[256] = "./capt0000.jpg";
    gchar hole[256] = "./dnd.jpg";

    get_path(temp, template_path);
    get_path(acce, accept_path);
    get_path(hole, noo_path);
    get_path(hole, hole_path);

    GtkWidget *root_window;

    gtk_disable_setlocale();
    setlocale(LC_ALL, "C");

    gtk_init(&argc, &argv);

    root_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(root_window), FALSE);
    gtk_window_set_position(GTK_WINDOW(root_window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(root_window), 1280, 1024);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);

    create_welcome();
    create_shoot();
    create_captured();
    create_merge();
    create_done();

    gtk_container_add(GTK_CONTAINER(root_window), notebook);

    gtk_widget_show_all(root_window);

    gtk_widget_grab_focus(GTK_WIDGET(lastname_entry));

    g_signal_connect(root_window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(root_window, "key_press_event",
                     G_CALLBACK(quitit), NULL);

    gtk_main();
}

