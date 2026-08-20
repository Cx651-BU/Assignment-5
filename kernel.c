#include "loader.h"
#include <stdlib.h>
#include <string.h>

struct pixel mul(struct pixel p1, float scalar) {
    return (struct pixel){r: p1.r * scalar, g: p1.g * scalar, b: p1.b * scalar};
}

struct pixel add(struct pixel p1, struct pixel p2) {
    return (struct pixel){r: p1.r + p2.r, g: p1.g + p2.g, b: p1.b + p2.b};
}

struct image* apply_kernel(struct image* img, int* kernel, int ksize, float normalize) {

}

