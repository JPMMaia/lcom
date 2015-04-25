#ifndef __VIDEO_TXT_H
#define __VIDEO_TXT_H



/** @defgroup video_txt video_txt
 * @{
 *
 * Functions for outputing data to screen in text mode
 */

/**@name Defines for character codes to draw single border frames
 */

/**@def VERT_BAR
 * CP437 code for vertical bar
 *
 * @def HOR_BAR
 * CP437 code for horizontal bar
 *
 * @def LL_CORNER
 * CP437 code for lower-left corner symbol
 *
 * @def UL_CORNER
 * CP437 code for upper-left corner symbol
 *
 * @def LR_CORNER
 * CP437 code for lower-right corner symbol
 *
 * @def UR_CORNER
 * CP437 code for upper-right corner symbol
 *
 */
#define VERT_BAR 	0xB3
#define HOR_BAR 	0xC4
#define LL_CORNER 	0xC0
#define UL_CORNER	0xDA
#define LR_CORNER	0xD9
#define UR_CORNER	0xBF

/**@name Module's external functions
 */

/**
 * @brief Initializes the video module in text mode
 *
 * Maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen
 *
 * @param vi_p address of vt_info_t structure with the necessary information
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
char *vt_init(vt_info_t *vi_p);

/**
 * @brief Fills the screen with the input character, with the specified attribute
 *
 * @param ch character to fill the screen with
 * @param attr attribute of character to fill the screen with
 */
void vt_fill(char ch, char attr);

/**
 * @brief Blanks screen
 *
 */
void vt_blank();

/**
 * @brief Prints a character on the screen
 *
 * Prints a character, with the specified attribute, at the specified position
 * on the screen in text mode, by writing to the corresponding location in VRAM
 *
 * @param ch character to print
 * @param attr attribute of character to print
 * @param r row where to print (row 0 is the first one)
 * @param c column where to print (column 0 is the first one)
 * @return 0 upon success, non-zero upon failure
 */
int vt_print_char(char ch, char attr, int r, int c);

/**
 * @brief Prints a string on the screen
 *
 * Prints a string, with the specified attribute, at the specified position in
 * the screen in text mode, by writing to the corresponding location in VRAM
 *
 * @param str string to print
 * @param attr attribute of character to print
 * @param r row where to print (row 0 is the first one)
 * @param c column where to print (column 0 is the first one)
 * @return 0 upon success, non-zero upon failure
 */
int vt_print_string(char *str, char attr, int r, int c);

/**
 * @brief Prints an integer on the screen
 *
 * Prints an integer, with the specified attribute, at the specified position in the screen
 * in text mode, by writing to the corresponding location in VRAM
 *
 * @param n integer number to print
 * @param attr attribute of character to print
 * @param r row where to print (row 0 is the first one)
 * @param c column where to print (column 0 is the first one)
 * @return 0 upon success, non-zero upon failure
 */
int vt_print_int(int n, char attr, int r, int c);

/**
 * @brief Draws a frame on the screen
 *
 * Draws a frame, with the specified dimensions and attribute, at the specified position
 * in the screen in text mode, by writing to the corresponding location in VRAM
 *
 * @param width frame width in number of columns
 * @param height frame heigth in number of lines
 * @param attr attribute of frame to draw
 * @param r row where to print top left corner (row 0 is the first one)
 * @param c column where to print top left corner (column 0 is the first one)
 * @return 0 upon success, non-zero upon failure
 */
int vt_draw_frame(int width, int height, char attr, int r, int c);

/** @} */
#endif /* __VIDEO_TXT_H */
