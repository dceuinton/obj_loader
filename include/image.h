#ifndef IMAGE_H
#define IMAGE_H

// System Headers
#include <iostream>

// --------------------------------------------------------------------------------
// Image Functions
// --------------------------------------------------------------------------------

// Load an image from file
unsigned char* loadImage(const char *filename, int &x, int &y, int &n);

#endif // IMAGE_H