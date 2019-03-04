// (C) 2019 by folkert van heusden, released under AGPL v3.0

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

bool read_JPEG_memory(const char *const file, int *w, int *h, unsigned char **pixels)
{
	bool ok = true;
	FILE *fh = fopen(file, "r");
	if (!fh) {
		printf("Cannot open file\n");
		return false;
	}

	struct jpeg_decompress_struct info;

	struct jpeg_error_mgr err;
	info.err = jpeg_std_error(&err);

	jpeg_create_decompress(&info);

	jpeg_stdio_src(&info, fh);
	jpeg_read_header(&info, true);

	jpeg_start_decompress(&info);

	*pixels = NULL;
	*w = info.output_width;
	*h = info.output_height;

	if (info.num_components != 3) {
		printf("JPEG: unexpected number of color components (%u) - RGB is required\n", info.num_components);
		return false;
	}

	unsigned long dataSize = *w * *h * info.num_components;

	*pixels = (unsigned char *)valloc(dataSize);
	while(info.output_scanline < *h) {
		unsigned char *rowptr = *pixels + info.output_scanline * *w * info.num_components;

		jpeg_read_scanlines(&info, &rowptr, 1);
	}

	jpeg_finish_decompress(&info);    

	fclose(fh);

	jpeg_destroy_decompress(&info);

	return ok;
}
