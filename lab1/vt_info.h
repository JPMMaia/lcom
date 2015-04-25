#ifndef __VT_INFO_H
#define __VT_INFO_H

/** @defgroup vt_info vt_info
 * @{
 *
 * Functions related to the configuration of the video controller in text mode
 */

/** @name Video Text Mode Information */
/**@{*/
typedef struct {

	/** @name VRAM info */
	/**@{*/
	unsigned long vram_size;    	/**< @brief VRAM size in 16-bit words*/
	void * vram_base;		/**< @brief VRAM base physical address. */
	/**@}*/
	/** @name Screen parameters */
	/**@{*/
	unsigned scr_width;		/**< @brief No of columns of the screen */
	unsigned scr_lines;		/**< @brief No of lines of the screen */
	/**@}*/
} vt_info_t;
/**@}*/

/**
 * @brief Returns information on the VRAM and the screen dimensions
 *
 * Initializes vt_info_t structure passed as an address with
 *  the information on the VRAM and the screen dimensions
 *
 * @param vi_p address of vt_info_t structure to be initialized
 */
void vt_info_get(vt_info_t *vi_p);

/**
 * @brief Displays information on the VRAM and the screen dimensions.
 *
 * @param vi_p address of vt_info_t structure to be displayed
 */
void vt_info_display(vt_info_t *vi_p);


 /** @} end of vt_info */

#endif /* __VT_INFO_H */
