#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./stb_image_resize.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

GtkWidget *label;
char *filename = NULL;
GtkWidget *widthEntry;  // Global entry widget for width
GtkWidget *heightEntry; // Global entry widget for height
GtkWidget *outputEntry;

int pixelate(const char *input_filename, const char *output_filename, int new_width, int new_height)
{
    printf("Input filename: %s\n", input_filename);
    printf("Output filename: %s\n", output_filename);
    printf("New width: %d\n", new_width);
    printf("New height: %d\n", new_height);
    int width, height, channels;

    // reading the image file
    unsigned char *data = stbi_load(input_filename, &width, &height, &channels, 0);
    if (data == NULL)
    {
        fprintf(stderr, "Error in loading the image\n");
        return 1;
    }

    // setting the dimensions of the new image
    int resize_width = new_width;
    int resize_height = (height * new_width) / width;

    // allocating memory for pixelated image
    unsigned char *resized_data = (unsigned char *)malloc(resize_width * resize_height * channels);
    if (resized_data == NULL)
    {
        stbi_image_free(data);
        fprintf(stderr, "Failed to allocate memory for resized data.\n");
        return 1;
    }

    // resize the image to pixelate it
    stbir_resize_uint8_generic(data, width, height, 0, resized_data, resize_width, resize_height, 0, channels, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_BOX, STBIR_COLORSPACE_SRGB, NULL);

    // allocating memory for the output image
    unsigned char *final_data = (unsigned char *)malloc(width * height * channels);
    if (final_data == NULL)
    {
        free(resized_data);
        stbi_image_free(data);
        fprintf(stderr, "Failed to allocate memory for final data.\n");
        return 1;
    }
    // resize pixelated image back to original dimensions using nearest-neighbor to preserve the pixelated effect
    stbir_resize_uint8_generic(resized_data, resize_width, resize_height, 0, final_data, width, height, 0, channels, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_BOX, STBIR_COLORSPACE_SRGB, NULL);

    // save the pixelated image as png
    stbi_write_png(output_filename, width, height, channels, final_data, width * channels);

    // free allocated meory
    stbi_image_free(data);
    free(resized_data);
    free(final_data);
    return 0;
}

void closeApp(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

void onSelect(GtkWidget *widget, gpointer data)
{
    GtkWidget *box;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint result;
    box = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(data), action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    result = gtk_dialog_run(GTK_DIALOG(box));
    if (result == GTK_RESPONSE_ACCEPT)
    {

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(box);
        filename = gtk_file_chooser_get_filename(chooser);
        gchar *new_text = g_strdup_printf("Image Source: %s", filename);

        // Update label text with the formatted string
        gtk_label_set_text(GTK_LABEL(label), new_text);

        // closeApp(GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(widget))), NULL);
    }
    gtk_widget_destroy(box);
}

void onPixelate(GtkWidget *widget, gpointer data)
{
    const gchar *width_str = gtk_entry_get_text(GTK_ENTRY(widthEntry));
    const gchar *height_str = gtk_entry_get_text(GTK_ENTRY(heightEntry));
    const gchar *output_filename = gtk_entry_get_text(GTK_ENTRY(outputEntry));
    int new_width = atoi(width_str);
    int new_height = atoi(height_str);

    // pixelate the image using pixelate function
    pixelate(filename, output_filename, new_width, new_height);
    closeApp(NULL, NULL);
}

int main(int argc, char **argv)
{
    //  check args
    // if (argc < 5)
    // {
    //     fprintf(stderr, "Usage: <input file> <output file> <new width> <new height>\n");
    //     return 1;
    // }

    // preprocess args
    // const char *input_filename = argv[1];
    // const char *output_filename = argv[2];
    // int new_width = atoi(argv[3]);
    // int new_height = atoi(argv[4]);

    // pixelate image
    gtk_init(&argc, &argv);

    // create main window
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *closeButton;
    GtkWidget *pixelateButton;
    GtkWidget *vbox;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(closeApp), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 30);

    // create buttons and label
    button = gtk_button_new_with_label("Select file");
    closeButton = gtk_button_new_with_label("Quit App");
    pixelateButton = gtk_button_new_with_label("Pixelate");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(onSelect), window);
    g_signal_connect(G_OBJECT(closeButton), "clicked", G_CALLBACK(closeApp), window);
    g_signal_connect(G_OBJECT(pixelateButton), "clicked", G_CALLBACK(onPixelate), window);
    label = gtk_label_new("");

    // user inputs
    widthEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widthEntry), "New Image Width");
    heightEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(heightEntry), "New Image Height");
    outputEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(outputEntry), "Output File Name");

    // create box to hold buttons
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), closeButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), widthEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), heightEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), outputEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), pixelateButton, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();
    g_free(filename);

    return 0;
}