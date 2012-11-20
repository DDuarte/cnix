#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void* vg_init(unsigned short mode);

/**
 * @brief Fills the screen with the input color
 *
 * @param color color to fill the screen with
 * @return 0 on success, non-zero upon failure
 */
int vg_fill(unsigned long color);

/**
 * @brief Sets input pixel with input color
 *
 * Sets the color of the pixel at the specified position to the input value,
 *  by writing to the corresponding location in VRAM
 *
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param color color to set the pixel
 * @return 0 on success, non-zero otherwise
 */
int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color);

/**
 * @brief Returns the color of the input pixel
 *
 * Returns the color of the pixel at the specified position,
 *  by reading to the corresponding location in VRAM
 *
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
long vg_get_pixel(unsigned long x, unsigned long y);


/**
 * @brief Draw line segment with specifed end points and color
 *
 * Draws a line segment with the specified end points and the input color,
 *  by writing to VRAM
 *
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_line(unsigned long xi, unsigned long yi,
                   unsigned long xf, unsigned long yf, unsigned long color);

/**
 * @brief Draw filled rectangle with the specifed end points and color
 *
 * Draws a filled rectangle with the specified end points and the input color,
 *  by writing to VRAM
 *
 * @param x1 horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)
 * @param y1 vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param x2 horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)
 * @param y2 vertical coordinate of "last" endpoint, starts at 0 (top pixel)
 * @param color color of the line segment to draw and rectangle fill
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_rectangle(unsigned long x1, unsigned long y1, unsigned long x2,
        unsigned long y2, unsigned long color);

/**
 * @brief Draw circle with the specifed center point, radius and line color
 *
 * Draws a a circle with the specified center point, radius and line color,
 *  by writing to VRAM
 *
 * @param x0 horizontal coordinate of the circle's center
 * @param y0 vertical coordinate of the circle's center
 * @param radius radius (in pixel) of the circle
 * @param color color of the line segment to draw and rectangle fill
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_circle(int x0, int y0, int radius, unsigned long color);

/**
 * @brief Copies the virtual buffer to the physical buffer
 */
void vg_swap_buffer(void);

/**
 * @brief Assembly implemented function that copies size bytes of the src char* to the dst char*
 */
void _vg_swap_buffer(char* src, char* dst, unsigned int size);

/**
 * @brief Converts a RGB color to the color supported by VBE
 * @param r Red component of the color
 * @param g Green component of the color
 * @param b Blue component of the color
 * @return Computed color
 */
unsigned long vg_color_rgb(unsigned long r, unsigned long g, unsigned long b);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

 /** @} end of video_gr */

#endif /* __VIDEO_GR_H */
