#pragma once

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps
 */

typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

/**
 *
 */
typedef struct {
    unsigned short type; /**< @brief specifies the file type */
    unsigned int size; /**< @brief specifies the size in bytes of the bitmap file */
    unsigned int reserved; /**< @brief reserved; must be 0 */
    unsigned int offset; /**< @brief specifies the offset in bytes from the bitmapfileheader to the bitmap bits */
} BitmapFileHeader;

/**
 *
 */
typedef struct {
    unsigned int size; /**< @brief specifies the number of bytes required by the struct */
    int width; /**< @brief specifies width in pixels */
    int height; /**< @brief specifies height in pixels */
    unsigned short planes; /**< @brief specifies the number of color planes, must be 1 */
    unsigned short bits; /**< @brief specifies the number of bit per pixel */
    unsigned int compression; /**< @brief specifies the type of compression */
    unsigned int imageSize; /**< @brief size of image in bytes */
    int xResolution; /**< @brief number of pixels per meter in x axis */
    int yResolution; /**< @brief number of pixels per meter in y axis */
    unsigned int nColors; /**< @brief number of colors used by the bitmap */
    unsigned int importantColors; /**< @brief number of colors that are important */
} BitmapInfoHeader;

/**
 * Represents a Bitmap
 */
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
} Bitmap;

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws an unscaled, unrotated bitmap at the given position
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment image alignment
 */
void drawBitmap(Bitmap* bitmap, int x, int y, Alignment alignment);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bitmap bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/** @}* end of Bitmap */
