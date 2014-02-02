/*  PMChaos.   (C) Copyright Matthew Austern, 1993.

    Permission granted to use, distribute, and modify, provided that this
    notice remains intact.  If you distribute a modified version, you must
    identify your modifications as such.
*/

/* Prototypes for functions in graph.c */

/* Must #include os2.h before including this file. */

#define MaxGraphLabelLength 100

struct GraphInfo {
  long WindowWidth, WindowHeight; /* Full size of the window. */
  double xMin, xMax, yMin, yMax; /* Limits of the graph. */
  long xLeftMargin, yBotMargin;	 /* Blank space by the side of graph. */
  long xRightMargin, yTopMargin; /*  Not symmetric, because of axis labels. */
  double xConvFactor, yConvFactor; /* Convert from graph to window coords. */
  double xFirstTick, yFirstTick, xTickInc, yTickInc; /* Where to put ticks. */
  long MajorTickSize, HalfTickSize, MinorTickSize; /* Size of ticks. */
  char xLabel[MaxGraphLabelLength], yLabel[MaxGraphLabelLength];
  long xLabelWidth, yLabelWidth;
  unsigned short FontPointSize;
  long FontHeight;
  long FontWidth;
};

void DrawAxes(HPS hps, struct GraphInfo *G);
void ClipToGraph (HPS PS, struct GraphInfo *G);
void UndoClipping(HPS);
void SizeAxes(HPS hps,
	      long Width, long Height,
	      double xMin, double xMax, double yMin, double yMax,
	      char *xLabel, char *yLabel,
	      struct GraphInfo *G);

#include "inline.h"

INLINE POINTL toPoint (double x, double y, struct GraphInfo *G)
{
  POINTL result;
  result.x = (long) (G->xLeftMargin + (x - G->xMin) * G->xConvFactor);
  result.y = (long) (G->yBotMargin + (y - G->yMin) * G->yConvFactor);
  return result;
}

INLINE void toXY (POINTL P, double *x, double *y, struct GraphInfo *G)
{
  *x = G->xMin + (P.x - G->xLeftMargin) / G->xConvFactor;
  *y = G->yMin + (P.y - G->yBotMargin) / G->yConvFactor;
}
