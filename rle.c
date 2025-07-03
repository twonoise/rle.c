// gimptool-2.0 --install rle.c

// Credits: Copyright David Neary, 2004; Copyright Nathan Osman, 2012; jpka, 2017

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <iso646.h>
#define eq ==
#define shl <<
#define shr >>
#define mod %

int write_rle(const gchar * filename, gint drawable_id, int color_depth)
{
    GimpDrawable * drawable;
    gint bpp;
    GimpPixelRgn rgn;
    long int data_size;
//    void * image_data;
    uint8_t * image_data;
    size_t output_size;
    uint8_t * raw_data;
    FILE * file;

//    uint8_t rle_width;
//    uint8_t rle_height;
    uint rle_width; // while it is one byte, width = 256 also used when processing.
    uint rle_height; // It becomes 0 at header. But we can't proccessing with width/height = 0.
	

	long int i;

	unsigned int x, y; //, pixel;
	unsigned int colors;
	unsigned int color, prev_color;
	unsigned int colorlist[256];
	int err;
//	char s[256];
	int already_have;
	int max_colors;
	int rle, rle_max, delta;

unsigned int get_indexed_color_grayscale(unsigned int x, unsigned int y, unsigned int maxc) {
	unsigned int pixel;
	unsigned int color_index;

	pixel = (x + (drawable->width * y)) * bpp;
// pixel+3 is 0=transparent, ff=occuped
// 	image_data[pixel] + image_data[pixel+1]*256 + image_data[pixel+2]*256*256

// Currently, only grayscale is supported. Anybody need color?	
//	if (maxc eq 0) {
//		color_index = 0; // failsafe
//	} else {
		color_index = (int)(image_data[pixel] * (maxc-0) / 256); // Using red channel only
//	}
	return (color_index); 
}


	if ((color_depth < 0) or (color_depth > 4)) { 
	        free(raw_data);
	        return 0;
	}

	max_colors = 1 shl color_depth + 1;

    // Get the drawable
    drawable = gimp_drawable_get(drawable_id);

    // Get the BPP
    bpp = gimp_drawable_bpp(drawable_id);

    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        FALSE, FALSE);

    // Determine the size of the array of image data to get
    // and allocate it.
    data_size = drawable->width * drawable->height * bpp;
    image_data = malloc(data_size);

    // Get the image data
    gimp_pixel_rgn_get_rect(&rgn,
                            (guchar *)image_data,
                            0, 0,
                            drawable->width,
                            drawable->height);

    // We have the image data, now encode it.
/*    output_size = rleEncodeRGB((const uint8_t *)image_data,
                                drawable->width,
                                drawable->height,
                                drawable->width * 3,
                                color_depth,
                                &raw_data);*/

	if ((drawable->width eq 0) or (drawable->height eq 0)) { 
	        free(raw_data);
	        return 0;
	}

	rle_width = drawable->width;
	if (rle_width > 256) rle_width = 256;
	rle_height = drawable->height;
	if (rle_height > 256) rle_height = 256;


// First of all, we calculate color quantity Unfortunately, GIMP does not expose indexed-color API.
// TODO calculate also transparency (RGBA) here ?
// for RGBA, bpp automatically be 4, else 3.

// colorlist = malloc (65536);

	err = 0;
	colors = 0;

	for (y = 0; y < rle_height; y++) { // process only 256*256 px or less, don't care outside this region.
		for (x = 0; x < rle_width; x++) {
//			pixel = (x + (drawable->width * y)) * bpp;
//// pixel+3 is 0=transparent, ff=occuped
//			color = image_data[pixel] + image_data[pixel+1]*256 + image_data[pixel+2]*256*256;
			color = get_indexed_color_grayscale (x, y, max_colors);
			already_have = 0;
			for (i = 0; i < colors; i++) {
				if (color eq colorlist[i]) {
					already_have = 1;
					break; 
				}
			}	
			if (already_have eq 0) { //new color
				colorlist[colors] = color;
				colors = colors + 1;
				if (colors > max_colors) {
// normally this should be simple GIMP plugin error dialogue.
// But no ready-to-use tested and working example of it. So file output used.
					raw_data = malloc (1024);
					sprintf(raw_data, 
						"Too many colors: %u or more, max is %u (or %u with transparency bit).\n"
						"Use Image->Mode->Indexed.", colors, max_colors, max_colors - 1);
					output_size = strlen(raw_data);
					err = 1;
					goto done; // http://stackoverflow.com/questions/9695902/how-to-break-out-of-nested-loops
				}
			}
		}
	}
done:


if (err eq 0) {

	if (colors eq 0) {
	        free(raw_data);
	        return 0;
	}

// Now we will force more colors than we really calculate, using 'bpp' value entered by user:
// to make set of images unified for same context/palette, etc.
// Say we have set of 17-color images, and add image that is just 50% gray rectangle, which determined as 1-color only.
// Or it have pixels of 25% and 75% gray only (two color), which will be decoded to black and white,
// if we do not increase colors.

// Images will be always assumed as full contrast. So 50% dimmed image will have only 50% colors used, 
// not full dynamic range of colors as for other color indexers, because we do not have palette.


// at this point, 'colors' already checked to be no greater than 'max_colors'.

// is this required? We have user-requested 'max_colors' already.
/*	if (colors > 9) colors = 17;
	else if (colors > 5) colors = 9;
	else if (colors > 3) colors = 5; */



void write_rle_byte(void) {
	if (color_depth eq 0) {
		raw_data[i] = rle;
	} else {
		delta = (color - prev_color) mod max_colors;
		raw_data[i] = rle + (delta shl (8-color_depth));
	}
	i = i + 1;
}


	if (colors eq 1) { // special - filled with only one color, or fully transparent
		output_size = 8;
		raw_data = malloc (output_size);
		raw_data[7] = 128 + colorlist[0]; // first and only color
	} else {
		raw_data = malloc (1024*1024); // TODO how to do it correctly?
//		raw_data[7] = bpp; // debug
		raw_data[7] = colorlist[0]; // first color

		rle = 0;
		rle_max = (1 shl (8-color_depth)) - 1 - 1; // 0 (2 color) should be 254, 1 (3 color) is 126, 2 (5 color) is 62...
		i = 8;

		for (y = 0; y < rle_height; y++) { // process only 256*256 px or less, don't care outside this region.
			for (x = 0; x < rle_width; x++) {
 				if ((x eq 0) and (y eq 0)) {
					prev_color = get_indexed_color_grayscale (x, y, max_colors);
				} else {
					color = get_indexed_color_grayscale (x, y, max_colors);
					if (color not_eq prev_color) {
						write_rle_byte();
						prev_color = color;
						rle = 0;
					}
				}
				rle = rle + 1;
				if (rle > rle_max) {
					rle = rle_max;
					write_rle_byte();
	 				if (not ((x eq rle_width) and (y eq rle_height))) { // not last pixel
						rle = 0;
// currently try with incorrect color: (will wirk for 1-bit)
						write_rle_byte();
						rle = 1;
					}
				}
			}
		}
		write_rle_byte();
		output_size = i;
	}

//	if (rle_width eq 256) rle_width = 0; // 0 is 256 px, because zero px is not possible
//	if (rle_height eq 256) rle_height = 0; // 0 is 256 px, because zero px is not possible
    raw_data[0] = 'R';
    raw_data[1] = 'L';
    raw_data[2] = 'E';
    raw_data[3] = '0';
    raw_data[4] = rle_width bitand 0xFF; // In header, 0 is 256 px, because zero px is not possible
    raw_data[5] = rle_height bitand 0xFF;
    raw_data[6] = max_colors;
//    raw_data[7] = bpp;

} // if (err eq 0)

    // Free the image data
    free(image_data);

    // Detach the drawable
    gimp_drawable_detach(drawable);

    // Make sure that the write was successful
//    if(output_size == FALSE)
//    {
//        free(raw_data);
//        return 0;
//    }


    // Open the file
    file = fopen(filename, "wb");
    if(!file)
    {
        free(raw_data);
        return 0;
    }

    // Write the data and be done with it.
    fwrite(raw_data, output_size, 1, file);
    free(raw_data);
    fclose(file);

    return 1;
}

extern const char SAVE_PROCEDURE[];
extern const char BINARY_NAME[];

// Response structure
struct rle_data {
    int       * response;
    GtkObject * color_depth_scale;
    float     * color_depth;
};

void on_response(GtkDialog * dialog,
                 gint response_id,
                 gpointer user_data)
{
    // Basically all we want to do is grab the value
    // of the slider and store it in user_data.
    struct rle_data * data = user_data;
    GtkHScale * hscale = GIMP_SCALE_ENTRY_SCALE(data->color_depth_scale);
    gdouble returned_color_depth;

    // Get the value
    returned_color_depth = gtk_range_get_value(GTK_RANGE(hscale));
    *(data->color_depth) = returned_color_depth;

    // Quit the loop
    gtk_main_quit();

    if(response_id == GTK_RESPONSE_OK)
        *(data->response) = 1;
}

int export_dialog(float * color_depth)
{
    int response = 0;
    struct rle_data data;
    GtkWidget * dialog;
    GtkWidget * vbox;
    GtkWidget * label;
    GtkWidget * table;
    GtkObject * scale;

    // Create the dialog
    dialog = gimp_export_dialog_new("rle",
                                    BINARY_NAME,
                                    SAVE_PROCEDURE);

    // Create the VBox
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       vbox, TRUE, TRUE, 2);
    gtk_widget_show(vbox);

    // Create the label
    label = gtk_label_new("The options below allow you to customize\nthe rle image that is created.");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 2);
    gtk_widget_show(label);

    // Create the table
    table = gtk_table_new(1, 3, FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 6);
    gtk_widget_show(table);

    // Create the scale
    scale = gimp_scale_entry_new(GTK_TABLE(table), 0, 0,
                                 "Bits Per Pixel:",
                                 150, 0,
                                 0.0f, 0.0f, 4.0f, 1.0f, 1.0f,
                                 0, TRUE, 0.0f, 0.0f,
                                 "Sets number of colors, see text below.",
                                 NULL);

/*    scale = gimp_scale_entry_new(GTK_TABLE(table), 0, 0,
                                 "color_depth:",
                                 150, 0,
                                 90.0f, 0.0f, 100.0f, 1.0f, 10.0f,
                                 0, TRUE, 0.0f, 0.0f,
                                 "color_depth for encoding the image",
                                 NULL);*/

    label = gtk_label_new(
"There is 2-color RLE (simplest possible), 3, 5, 9, 17 colors (2^BPP+1).\n"
"No palette support (yet?), so you assume colors yourself.\n"
"One extra color (compared to 2^n) can be used for transparency.\n"
"File format is: header and data flow. Header is 8 bytes: \n"
"'RLE0'(w)(h)(c)(sf), where: 0 is format version;\n"
"w is 1-byte width (0 is 256 px), h is 1-byte height (0 is 256 px),\n"
"c is 1-byte color depth (2, 3, 5, 9, 17),\n"
"sf is 1-byte 0bFSSSSSSS, S=start color (0..c-1), and F=1 means it is\n"
"the only color in image, it is filled with S color (or fully\n"
"transparent). Then no data is followed. Else,\n"
"data is flow of lengths of equal pixel lines, ending with 0xFF.\n"
"For 2 colors, and start color 0, data flow 05 10 15 255 means\n"
"5 black pixels, 10 white, 15 black. Remaining image area\n"
"considered transparent (no writing to videoRAM, skip).\n"
"No any byte is allowed to be 0xFF before end, include header:\n"
"so 255 px width or height is not possible.\n"
"Line of 255 equal pixels (need to be 0xFF in stream) replaced to\n"
"something like 0xFE 0x00 0x01, which is same at decoding.\n"
"For 3 colors, data is flow of: 0bCLLLLLLL: L=length 0..126, C=color delta.\n"
"(again, L=127 is not possible and will be replaced with equivalent.)\n"
"For 5 colors, data is flow of: 0bCCLLLLLL; etc.\n"
"When length is 0, color delta is ignored.\n"
"When something is not described here, use simplest stuff / KISS rule."
);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 2);
    gtk_widget_show(label);

    // Connect to the response signal
    data.response      = &response;
    data.color_depth_scale = scale;
    data.color_depth       = color_depth;

    g_signal_connect(dialog, "response", G_CALLBACK(on_response),   &data);
    g_signal_connect(dialog, "destroy",  G_CALLBACK(gtk_main_quit), NULL);

    // Show the dialog and run it
    gtk_widget_show(dialog);
    gimp_dialog_run(GIMP_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    return response;
}

int read_rle(const gchar * filename)
{
    FILE * file;
    long int filesize;
//    void * data,
//         * image_data;
//    void * data;
    uint8_t * data;
    uint8_t * image_data;
    int width, height, bpp;
    gint32 new_image_id,
           new_layer_id;
    GimpDrawable * drawable;
    GimpPixelRgn rgn;

	int color, color_depth, max_colors, start_color, fill;
	int i, p, rle;

unsigned int decode_indexed_color_grayscale(unsigned int c, unsigned int maxc) { // result used as gray RGB (each is equivalent 0..255)
	int color;
	color = (int)(c * 255 / (maxc-1));
	return (color); 
}

    // Try to open the file
    file = fopen(filename, "rb");
    if(!file)
        return -1;

    // Get the file size
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Now prepare a buffer of that size
    // and read the data.
    data = malloc(filesize);
    fread(data, filesize, 1, file);

    // Close the file
    fclose(file);

    // Perform the load procedure and free the raw data.
//////    image_data = rleDecodeRGB(data, filesize, &width, &height);

// decode RLE

	if (filesize < 8)
	        return -1;

	if (data[0] not_eq 'R') 
	        return -1;
	if (data[1] not_eq 'L') 
	        return -1;
	if (data[2] not_eq 'E') 
	        return -1;
	if (data[3] not_eq '0') 
	        return -1;

	if (data[4] eq 255) 
	        return -1;
	if (data[5] eq 255) 
	        return -1;
	if (data[6] eq 255) 
	        return -1;
	if (data[7] eq 255) 
	        return -1;

	width = data[4]; 
	if (width eq 0) 
	        width = 256;

	height = data[5];
	if (height eq 0) 
	        height = 256;

	max_colors = data[6];
	if (max_colors > 17) 
	        return -1;

	start_color = data[7] and 0x7f;
	if (start_color > 17) 
	        return -1;

	fill = (data[7] shr 7) and 0x01;


	bpp = 3; // 4 for transparency

	image_data = malloc(width * height * bpp);

	color = start_color;

int current_pixel;

	if (fill eq 1) {
		for (i = 0; i < (width * height); i++) { 
			image_data[i*bpp] = decode_indexed_color_grayscale(color, max_colors);
			image_data[i*bpp+1] = decode_indexed_color_grayscale(color, max_colors);
			image_data[i*bpp+2] = decode_indexed_color_grayscale(color, max_colors);
		}
	} else {
		current_pixel = 0;
		for (i = 8; i < filesize ; i++) { 
			rle = data[i];
			if (rle eq 255) 
				return -1;
			for (p = 0; p < rle; p++) {
				image_data[current_pixel*bpp] = decode_indexed_color_grayscale(color, max_colors);
				image_data[current_pixel*bpp+1] = decode_indexed_color_grayscale(color, max_colors);
				image_data[current_pixel*bpp+2] = decode_indexed_color_grayscale(color, max_colors);
				current_pixel = current_pixel + 1;
				if (current_pixel > width * height) goto overflow;
			}
			if (color eq 1) color = 0; else color = 1; // BW only TODO!
		}
		if (current_pixel < width * height) { // underflow
// same as for fill! unify it!
// for test, we use RED pixels.
			for (i = current_pixel; i < (width * height); i++) { 
				image_data[i*bpp] = 255; // decode_indexed_color_grayscale(color, max_colors);
				image_data[i*bpp+1] = 0; // decode_indexed_color_grayscale(color, max_colors);
				image_data[i*bpp+2] = 0; // decode_indexed_color_grayscale(color, max_colors);
			}
		}
	}

overflow:


    free(data);

    // Check to make sure that the load was successful
    if(!image_data)
        return -1;

    // Now create the new RGBA image.
    new_image_id = gimp_image_new(width, height, GIMP_RGB);

    // Create the new layer
    new_layer_id = gimp_layer_new(new_image_id,
                                  "Background",
                                  width, height,
                                  GIMP_RGB_IMAGE,
                                  100,
                                  GIMP_NORMAL_MODE);

    // Get the drawable for the layer
    drawable = gimp_drawable_get(new_layer_id);

    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        width, height,
                        TRUE, FALSE);

    // Now FINALLY set the pixel data
    gimp_pixel_rgn_set_rect(&rgn,
                            image_data,
                            0, 0,
                            width, height);

    // We're done with the drawable
    gimp_drawable_flush(drawable);
    gimp_drawable_detach(drawable);

    // Free the image data
    free((void *)image_data);

    // Add the layer to the image
    gimp_image_add_layer(new_image_id, new_layer_id, 0);

    // Set the filename
    gimp_image_set_filename(new_image_id, filename);

    return new_image_id;
}



const char LOAD_PROCEDURE[] = "file-rle-load";
const char SAVE_PROCEDURE[] = "file-rle-save";
const char BINARY_NAME[]    = "file-rle";

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
    static const GimpParamDef load_arguments[] =
    {
        { GIMP_PDB_INT32,  "run-mode",     "Interactive, non-interactive" },
        { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
        { GIMP_PDB_STRING, "raw-filename", "The name entered" }
    };

    static const GimpParamDef load_return_values[] =
    {
        { GIMP_PDB_IMAGE, "image", "Output image" }
    };

    static const GimpParamDef save_arguments[] =
    {
        { GIMP_PDB_INT32,    "run-mode",     "Interactive, non-interactive" },
        { GIMP_PDB_IMAGE,    "image",        "Input image" },
        { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
        { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
        { GIMP_PDB_STRING,   "raw-filename", "The name entered" },
        { GIMP_PDB_FLOAT,    "color_depth",      "color_depth of the image (0 <= color_depth <= 100)" }
    };
//}

    gimp_install_procedure(LOAD_PROCEDURE,
                           "Loads images in the rle file format",
                           "Loads images in the rle file format",
                           "jpka",
                           "jpka",
                           "2017",
                           "rle image",
                           NULL,
//    "INDEXED" ,
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(load_arguments),
                           G_N_ELEMENTS(load_return_values),
                           load_arguments,
                           load_return_values);

    // Install the save procedure
    gimp_install_procedure(SAVE_PROCEDURE,
                           "Saves files in the rle image format",
                           "Saves files in the rle image format",
                           "jpka",
                           "jpka",
                           "2017",
                           "rle image",
                           "RGB*",
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);

    // Register the load handlers
    gimp_register_file_handler_mime(LOAD_PROCEDURE, "image/rle");
    gimp_register_load_handler(LOAD_PROCEDURE, "rle", "");

    // Now register the save handlers
    gimp_register_file_handler_mime(SAVE_PROCEDURE, "image/rle");
    gimp_register_save_handler(SAVE_PROCEDURE, "rle", "");

}

void run(const gchar * name,
         gint nparams,
         const GimpParam * param,
         gint * nreturn_vals,
         GimpParam ** return_vals)
{
    // Create the return value.
    static GimpParam return_values[2];
    *nreturn_vals = 1;
    *return_vals  = return_values;

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

    // Check to see if this is the load procedure
    if(!strcmp(name, LOAD_PROCEDURE))
    {
        int new_image_id;

        // Check to make sure all parameters were supplied
        if(nparams != 3)
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        // Now read the image
        new_image_id = read_rle(param[1].data.d_string);

        // Check for an error
        if(new_image_id == -1)
        {
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
            return;
        }

        // Fill in the second return value
        *nreturn_vals = 2;

        return_values[1].type         = GIMP_PDB_IMAGE;
        return_values[1].data.d_image = new_image_id;
    }
    else if(!strcmp(name, SAVE_PROCEDURE))
    {
        gint32 image_id, drawable_id;
        int status = 1;
        float color_depth;
        GimpExportReturn export_ret;

        // Check to make sure all of the parameters were supplied
        if(nparams != 6)
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        image_id    = param[1].data.d_int32;
        drawable_id = param[2].data.d_int32;

        // Try to export the image
        gimp_ui_init(BINARY_NAME, FALSE);
        export_ret = gimp_export_image(&image_id,
                                       &drawable_id,
                                       "rle",
                                       GIMP_EXPORT_CAN_HANDLE_RGB);

        switch(export_ret)
        {
            case GIMP_EXPORT_EXPORT:
            case GIMP_EXPORT_IGNORE:

                // Now get the settings
                if(!export_dialog(&color_depth))
                {
                    return_values[0].data.d_status = GIMP_PDB_CANCEL;
                    return;
                }

                status = write_rle(param[3].data.d_string,
                                    drawable_id, color_depth);
                gimp_image_delete(image_id);

                break;
            case GIMP_EXPORT_CANCEL:
                return_values[0].data.d_status = GIMP_PDB_CANCEL;
                return;
        }

        if(!status)
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    }
    else
        return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
}
