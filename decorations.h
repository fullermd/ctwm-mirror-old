/*
 * Window decoration bits
 */

#ifndef _CTWM_DECORATIONS_H
#define _CTWM_DECORATIONS_H


void ComputeTitleLocation(TwmWindow *tmp);
void CreateWindowTitlebarButtons(TwmWindow *tmp_win);
void ComputeWindowTitleOffsets(TwmWindow *tmp_win, unsigned int width,
                               Bool squeeze);
void CreateHighlightWindows(TwmWindow *tmp_win);
void DeleteHighlightWindows(TwmWindow *tmp_win);
void CreateLowlightWindows(TwmWindow *tmp_win);

void PaintTitle(TwmWindow *tmp_win);
void PaintTitleButtons(TwmWindow *tmp_win);
void PaintTitleButton(TwmWindow *tmp_win, TBWindow  *tbw);


/* Used in decorations.c and add_window.c for building images */
#define gray_width 2
#define gray_height 2
static unsigned char gray_bits[] = {
	0x02, 0x01
};
static unsigned char black_bits[] = {
	0xFF, 0xFF
};


#endif /* _CTWM_DECORATIONS_H */
