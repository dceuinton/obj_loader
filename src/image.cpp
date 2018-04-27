// Project Header
#include "image.h"

// stb_image Header
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --------------------------------------------------------------------------------
// Image Functions
// --------------------------------------------------------------------------------

// Load an image from file
unsigned char* loadImage(const char *filename, int &width, int &height, int &n) {
	// Force RGBA 
	int force_channels = 4;

	// Load image
	unsigned char *image = stbi_load(filename, &width, &height, &n, force_channels);

	// Channels forced to 4
	n = 4;

	// Check result
	if(!image) {
		// Print error message
		std::cerr << "Error: could not load image: " << filename << std::endl;
		
		// Return error
		return NULL;
	}

	// Temporary buffer
	unsigned char *t = new unsigned char[width*n];

	// Flip image vertically
	for(int iy = 0; iy < height/2; iy++) {
		// Copy row iy into temporary buffer
		memcpy(t, &image[iy*width*n], width*n);

		// Copy row ((height-1)-iy) into row iy
		memcpy(&image[iy*width*n], &image[((height-1)-iy)*width*n], width*n);

		// Copy temporary buffer into row ((height-1)-iy)
		memcpy(&image[((height-1)-iy)*width*n], t, width*n);
	}

	// Delete temporary buffer
	delete[] t;

	// Check dimensions are power-of-2
	if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
		// Print warning message
		std::cerr << "Warning: image " << filename << " is not power-of-2 dimensions" << std::endl;
	}

	// Print log message
	std::cout << "Loaded: " << filename << std::endl;

	// Return image
	return image;
}