/*
 * Copyright notice...
 */

#ifndef _CTWM_R_LAYOUT_H
#define _CTWM_R_LAYOUT_H

#include "r_structs.h"


RLayout *RLayoutNew(RAreaList *monitors);
void RLayoutFree(RLayout *self);

RLayout *RLayoutCopyCropped(RLayout *self, int left_margin, int right_margin,
                            int top_margin, int bottom_margin);

RLayout *RLayoutSetMonitorsNames(RLayout *self, char **names);

RArea RLayoutGetAreaAtXY(RLayout *self, int x, int y);
RArea RLayoutGetAreaIndex(RLayout *self, int index);
RArea RLayoutGetAreaByName(RLayout *self, const char *name, int len);
RArea RLayoutBigArea(RLayout *self);

void RLayoutFindTopBottomEdges(RLayout *self, RArea *area, int *top,
                               int *bottom);
int RLayoutFindBottomEdge(RLayout *self, RArea *area);
int RLayoutFindTopEdge(RLayout *self, RArea *area);
void RLayoutFindLeftRightEdges(RLayout *self, RArea *area, int *left,
                               int *right);
int RLayoutFindLeftEdge(RLayout *self, RArea *area);
int RLayoutFindRightEdge(RLayout *self, RArea *area);

int RLayoutFindMonitorBottomEdge(RLayout *self, RArea *area);
int RLayoutFindMonitorTopEdge(RLayout *self, RArea *area);
int RLayoutFindMonitorLeftEdge(RLayout *self, RArea *area);
int RLayoutFindMonitorRightEdge(RLayout *self, RArea *area);

RArea RLayoutFullHoriz(RLayout *self, RArea *area);
RArea RLayoutFullVert(RLayout *self, RArea *area);
RArea RLayoutFull(RLayout *self, RArea *area);
RArea RLayoutFullHoriz1(RLayout *self, RArea *area);
RArea RLayoutFullVert1(RLayout *self, RArea *area);
RArea RLayoutFull1(RLayout *self, RArea *area);

void RLayoutPrint(RLayout *self);

#endif  /* _CTWM_R_LAYOUT_H */