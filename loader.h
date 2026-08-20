#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"


#pragma pack(push, 1)  // Ensure structure is packed



typedef struct {
    uint16_t type;              // Magic identifier: 0x4d42
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;              // Offset to image data in bytes
} BMPHeader;

typedef struct {
    uint32_t size;                // Header size in bytes
    uint32_t width, height;       // Width and height of image
    uint16_t planes;            // Number of color planes
    uint16_t bits;              // Bits per pixel
    uint32_t compression;         // Compression type
    uint32_t imagesize;           // Image size in bytes
    uint32_t xresolution, yresolution;
    uint32_t ncolors;             // Number of colors
    uint32_t importantcolors;     // Important colors
} BMPInfoHeader;

#pragma pack(pop)  // End packed structure


int loadimage_mmap(char* filename, struct image* image);
int saveimage_mmap(char* filename, struct image* image);

int loadimage(char* filename, struct image* image);
int saveimage(char* filename, struct image* image);