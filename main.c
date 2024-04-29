#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./stb_image_resize.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

int pixelate(const char *input_filename, const char *output_filename, int new_width, int new_height)
{
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

int main(int argc, char **argv)
{
    //  check args
    if (argc < 5)
    {
        fprintf(stderr, "Usage: <input file> <output file> <new width> <new height>\n");
        return 1;
    }

    // preprocess args
    const char *input_filename = argv[1];
    const char *output_filename = argv[2];
    int new_width = atoi(argv[3]);
    int new_height = atoi(argv[4]);

    // pixelate image
    gtk_init(&argc, &argv);
    GtkWidget *window;
    GtkWidget *button;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "closeApp", G_CALLBACK(closeApp), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 30);
    button = gtk_button_new_with_label("close");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(closeApp), "button");
    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_show_all(window);
    gtk_main();
    return pixelate(input_filename, output_filename, new_width, new_height);
}