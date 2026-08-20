#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"


int compare_images(struct image* img1, struct image* img2, int error_thresh) {
    if(img1->width != img2->width || img2->height != img1->height) {
        return 0;
    }

    for(int i = 0; i < img1->width*img1->height; i++) {
        if(abs(img1->pixels[i].r - img2->pixels[i].r) > error_thresh || abs(img1->pixels[i].g - img2->pixels[i].g) > error_thresh || abs(img1->pixels[i].b - img2->pixels[i].b) > error_thresh) {
            printf("Pixel Mismatch: {%d,%d,%d} != {%d,%d,%d}\n",img1->pixels[i].r,img1->pixels[i].g,img1->pixels[i].b,img2->pixels[i].r,img2->pixels[i].g,img2->pixels[i].b);
            return 0;
        }
    }
    return 1;
}

int test_identity() {
    struct image* image = malloc(sizeof(struct image));
    image->width = 640;
    image->height = 426;

    loadimage("images/sky.bmp",image);

    struct image* ref = malloc(sizeof(struct image));
    ref->width = 640;
    ref->height = 426;

    loadimage("reference/identity.bmp",ref);

    int kernel[3][3] = {{0,0,0},{0,1,0},{0,0,0}};

    struct image* out = apply_kernel(image, (int*) kernel, 3, 1);

    return compare_images(ref, out,1);
}

int test_blur() {
    struct image* image = malloc(sizeof(struct image));
    image->width = 640;
    image->height = 426;

    loadimage("images/sky.bmp",image);

    struct image* ref = malloc(sizeof(struct image));
    ref->width = 640;
    ref->height = 426;

    loadimage("reference/blur.bmp",ref);

    int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};

    struct image* out = apply_kernel(image, (int*) kernel, 3, 1.0/9);

    return compare_images(ref, out,1);
    
}

int test_mmap() {

    struct image* image = malloc(sizeof(struct image));
    image->width = 640;
    image->height = 426;
    image->pixels = malloc(sizeof(struct pixel) * image->width * image->height);

    loadimage("images/sky.bmp",image);

    saveimage_mmap("test.bin", image);

    struct image* out = malloc(sizeof(struct image));
    out->width = 640;
    out->height = 426;
    
    loadimage_mmap("test.bin", out);
    
    return compare_images(image, out,0);
}

int test_blur_mmap() {
    struct image* image = malloc(sizeof(struct image));
    image->width = 640;
    image->height = 426;
    image->pixels = malloc(sizeof(struct pixel) * image->width * image->height);

    loadimage("images/sky.bmp",image);
    saveimage_mmap("sky.bin", image);

    struct image* img2 = malloc(sizeof(struct image));
    img2->width = 640;
    img2->height = 426;
    loadimage_mmap("sky.bin", img2);

    struct image* ref = malloc(sizeof(struct image));
    ref->width = 640;
    ref->height = 426;

    loadimage("reference/blur.bmp",ref);

    int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};

    struct image* out = apply_kernel(img2, (int*) kernel, 3, 1.0/9);

    return compare_images(ref, out,1);
    
}


int run_test(char * test_name, int (*test_func)()) {
    int test_res = test_func();
    printf("Test %-25s: %d/1\n", test_name, test_res);
    return test_res;
}

int main(int argc, char **argv){
    if(argc != 2) {
        printf("ERROR: expected format ./test_one <test_num>\n");
        return -1;
    }
    int passed = 0;
    int test_num = atoi(argv[1]);
    
    char *test_list[] = {"Test Identity","Test Blur", "Test MMap", "Test Blur/MMap"};
    int (*test_func[])() = {&test_identity, &test_blur, &test_mmap, &test_blur_mmap};

    passed += run_test(test_list[test_num], test_func[test_num]);
    int tests_ran = 1;

    printf("Total: %d/%d\n", passed, tests_ran);
    return passed;
}