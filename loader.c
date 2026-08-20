#include "loader.h"
#include <sys/mman.h>
#include <errno.h>


int loadimage_mmap(char* filename, struct image* image) {
	return 0;
}

int saveimage_mmap(char* filename, struct image* image) {
	return 0;
}

int loadimage(char* filename, struct image* image) {
    int fd = open(filename, O_RDONLY);
	uint32_t x, y;
	BMPHeader header;
	BMPInfoHeader infoHeader;

	if (fd == -1) return -1;

	header.type = 0;

	read(fd, &header, sizeof(BMPHeader));
	read(fd, &infoHeader, sizeof(BMPInfoHeader));

	if (header.type != 0x4D42 || infoHeader.bits != 24) {
        printf("corrupted header\n");
		close(fd);
		return -1;
	}
	int padding = (4 - (infoHeader.width * 3) % 4) % 4;

	lseek(fd, header.offset, SEEK_SET);

	/* Start from the last row */
	y = infoHeader.height - 1;

	image->pixels = malloc(sizeof(struct pixel) * image->width * image->height);

	do {
		for (x = 0; x < infoHeader.width; x++) {
			unsigned char color[3];
			read(fd, color, sizeof(unsigned char) * 3);
			image->pixels[x + y * image->width].r = color[0];
            image->pixels[x + y * image->width].g = color[1];
            image->pixels[x + y * image->width].b =  color[2];
		}
		lseek(fd, padding, SEEK_CUR);
	} while (y-- > 0); /* The post-increment here is important not
			    * to miss the last row. */

	close(fd);
	return 0;
}

int saveimage(char* filename, struct image* image) {
    /* Create if the file does not exist, overwrite otherwise. Set
	 * file permissions: 0644 */
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	int x, y;

	if (fd == -1) return 1;

	BMPHeader header = { 0x4D42, 54 + image->width * image->height * 3, 0, 0, 54 };
	BMPInfoHeader infoHeader = { 40, image->width, image->height, 1, 24, 0,
				     image->width * image->height * 3, 0, 0, 0, 0 };

	write(fd, &header, sizeof(BMPHeader));
	write(fd, &infoHeader, sizeof(BMPInfoHeader));

	int padding = (4 - (image->width * 3) % 4) % 4;

	/* Start by serializing the last row. */
	y = image->height - 1;
	do {
		for (x = 0; x < image->width; x++) {
			struct pixel p = image->pixels[x + y * image->width];
			unsigned char color[3] = {((uint32_t) p.r) & 0xFF,
						    ((uint32_t) p.g) & 0xFF,
						   ((uint32_t) p.b) & 0xFF};
			write(fd, color, sizeof(unsigned char) * 3);
		}
		for (int i = 0; i < padding; i++) {
			unsigned char pad = 0;
			write(fd, &pad, 1);
		}
	} while ( y-- > 0); /* The post-increment here is important
			    * not to miss the last row. */

	close(fd);
	return 0;
}