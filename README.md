# Assignment 5: Observing the OS, Memory Usage

In this week’s assignment you will write a handful of matrix operations on images. 

Specifically, you will be implementing a simple **convolution** of a kernel onto an image.

These matrix operations, "convolutions", are akin to those performed in AI tasks that can process image input and allow the computer to “see” image data. 

Image data however can be extremely large, and this process is memory intensive. Having multiple processes work on these images will introduce contention on our system’s memory resources. This week you will measure the operating system under high memory-usage conditions to expose the inner-workings of virtual memory. 

You will measure process's memory usage as well as trigger a page fault! 

## Part 1: Image Kernels

An image kernel is a small matrix that can be used to apply various operations on an image. With an image kernel we can do edge-detection, blur an image, sharpen and image and more!

The process of applying a kernel to an image is known as **convolution**.

The following pseudo-code describes the operation of convolution:

```
for each image row in input image:
    for each pixel in image row:

        set accumulator to zero

        for each kernel row in kernel:
            for each element in kernel row:

                if element position  corresponding* to pixel position then
                    multiply element value  corresponding* to pixel value
                    add result to accumulator
                endif

         set output image pixel to accumulator * normalization
```

Consider for example we have a 4x4 image, a 3x3 kernel and 4x4 output:

```
Image:
01 02 03 04
05 06 07 08
09 10 11 12
```

```
Output:
01 02 03 04
05 06 07 08
09 10 11 12
```

```
kernel:
01 02 03
04 05 06
07 08 09
```

Kernel 05 is the **origin** of the kernel. When calculating an output value, we perform the following calculation:

`output07 = (kernel01 * image02 + kernel02 * image03 + kernel03 * image04 + kernel05 * image06 + kernel05 * image07 + kernel06 * image08 + kernel07 * image10 + kernel08 * image11 + kernel09 * image12) * normalization`.

### Edge Case

We also must consider what to do when the kernel would be multiplied by a pixel that is off the edge of the image. In our case, let's consider any pixel that is not on the image to be "black" with a value of {0,0,0}.

> [!IMPORTANT]
> Task: Implement the apply_kernel function that applies the image kernel to a loaded image.

When you complete this task you should pass `./test 0` and `./test 1`:

```shell
$ make
gcc -c -o cli.o cli.c -I. -lm
gcc -c -o kernel.o kernel.c -I. -lm
gcc -c -o loader.o loader.c -I. -lm
gcc -o cli cli.o kernel.o loader.o -I. -lm
gcc -c -o test.o test.c -I. -lm
gcc -o test test.o kernel.o loader.o -I. -lm
$ ./test 0
Test Test Identity            : 1/1
Total: 1/1
$ ./test 1
Test Test Blur                : 1/1
Total: 1/1
```

Make sure your implementation also avoids memory leaks! Running **valgrind** on your cli executable should return 0 errors.

```shell
$ valgrind ./cli images/sky.bmp 640 426 out.bmp
==61442== Memcheck, a memory error detector
==61442== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==61442== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==61442== Command: ./cli images/sky.bmp 640 426 out.bm
==61442== 
filepath: images/sky.bmp width: 640 height 426
==61442== 
==61442== HEAP SUMMARY:
==61442==     in use at exit: 0 bytes in 0 blocks
==61442==   total heap usage: 5 allocs, 5 frees, 6,544,416 bytes allocated
==61442== 
==61442== All heap blocks were freed -- no leaks are possible
==61442== 
==61442== For lists of detected and suppressed errors, rerun with: -s
==61442== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Part 2: Memory Contention

How much memory does an instance of your program use?
Run multiple copies of this process. What is the memory usage?


> [!IMPORTANT]
> Task: Run `source bench.sh 1` and inspect the `log-normal-1.log` file. Write your answer in `questions.txt` in bytes. Label your answer `(1)`.
> Modify the command that is tested in `source bench.sh 1` such that your kernel program uses at least 1GB of memory.


## Part 3: Virtual Memory + mmap

We have noticed that loading in image data into memory is expensive! Let's try to reduce the amount of memory our processes are using by having them **share** an image in memory with `mmap`.

Thus, we want to support 3 additional modes in our cli:
- `convert` - converts a bmp format image to binary format, ready to mmap.
- `uconvert` - "unconverts" a binary format image to bmp
- `mmap` - runs the kernel with mmap input


Using `mmap` will have two nice benefits: the input image will no longer be shared and the output 

> [!IMPORTANT]
> Task: Complete the implementation of `loadimage_mmap` and `saveimage_mmap` in `loader.c`.
> Task: Modify the cli program to parse and correctly run the convert,uconvert, and mmap modes.

After you implement this functionality you should be able to pass `./test 2` and `./test 3`:

```shell
$ ./test 2
Test Test MMap                : 1/1
Total: 1/1
```

You will need to have the `smem` util installed:

```sudo apt install smem```

> [!IMPORTANT]
> Task: Run `source batch.sh 5` again and check the memory usage reported. 
> Task: Run `source batch-mmap.sh 5` again and check the memory usage reported.
> Run `python3 plot.py log-mmap-5.out log-normal-5.out` to view a memory usage graph.
> Describe what you observe about the processes' memory usage as well as execution time in `questions.txt`. Closely look at the "image loading" phase of execution. What do you notice the difference is? Label your answer `(2)`.

## Part 4: Page Faults

Let's monitor and report the number of page faults that occurs when our program runs. 

```
/usr/bin/time -v ./cli kernel images/sky.bmp 640 426 out.bmp
```

Notice, we have two types of page faults reported. Major and Minor page faults.

> [!IMPORTANT]
> Task: Describe the difference between major/minor page faults in `questions.txt`. Label your answer `(3)`.

Let's try to tri
```Major (requiring I/O) page faults: 0```

Make a new cli command that triggers additional major page faults.

How can you write a program that does this?

> [!IMPORTANT]
> Task: Complete the `fault` mode to call `generate_pagefault` which should generate a **major** pagefault.


**Hint:** You may be able to use `saveimage_mmap` to generate your pagefault!

Once you are done, try running: 
```
/usr/bin/time -v ./cli fault x x x x
```

You should now see at least 1 major page fault.

