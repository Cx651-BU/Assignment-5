#ifndef IMAGE_DEFINED
#define IMAGE_DEFINED

struct image {
    struct pixel* pixels;
    int width;
    int height;
};

struct pixel {
    int r;
    int g;
    int b;
};

#endif