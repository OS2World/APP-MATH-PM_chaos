/*  PMChaos.   (C) Copyright Matthew Austern, 1993.

    Permission granted to use, distribute, and modify, provided that this
    notice remains intact.  If you distribute a modified version, you must
    identify your modifications as such.
*/

#define INCL_PM
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"

/* Precompute some information we need for drawing the axes.  We need to
   do this every time the range changes, or every time the window is resized,
   or its font changes, but not every time it's redrawn. 

   NOTE: I assume that (0,0), in window coordinates, is the lower left corner
   of the window.
*/

				/* Set size of the current font. */
static void ScaleFont (HPS PS, unsigned short size)
{
  SIZEF FontSize;
  
  FontSize.cx = FontSize.cy = MAKEFIXED(size, 0);
  DosEnterCritSec();
  GpiSetCharBox(PS, &FontSize);
  DosExitCritSec();
}


static int GetFont(HPS PS, unsigned short size, long ID)
{
  FATTRS Attrib;		/* Font attributes. */

  Attrib.usRecordLength = sizeof(FATTRS);
  Attrib.fsSelection = Attrib.lMatch = Attrib.idRegistry = Attrib.fsType = 0;
  Attrib.usCodePage = 850;	/* Standard code page. */
  Attrib.lMaxBaselineExt = Attrib.lAveCharWidth = 12; /* Size of font. */
  Attrib.fsFontUse = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;
  strcpy(Attrib.szFacename, "Times New Roman Bold");

  if (GpiCreateLogFont(PS, 0, ID, &Attrib) != FONT_MATCH) {
    GpiDeleteSetId(PS, ID);
    return 0;
  }
  else {
    DosEnterCritSec();
    GpiSetCharSet(PS, ID);
    DosExitCritSec();
    ScaleFont(PS, size);
    return 1;
  }
}
 
  
static long TextHeight (HPS PS, char *Text)
{
  POINTL Corners[TXTBOX_COUNT];
  GpiQueryTextBox(PS,
		  (long) strlen(Text), Text,
		  TXTBOX_COUNT,Corners);
  return Corners[TXTBOX_TOPLEFT].y - Corners[TXTBOX_BOTTOMLEFT].y;
}
 
static long TextWidth (HPS PS, char *Text)
{
  POINTL Corners[TXTBOX_COUNT];
  GpiQueryTextBox(PS,
		  (long) strlen(Text), Text,
		  TXTBOX_COUNT,Corners);
  return Corners[TXTBOX_TOPRIGHT].x - Corners[TXTBOX_TOPLEFT].x;
}

/* This function returns the value that should be given to ScaleFont to
   get back a font of the current size.  It assumes that the CharBox is
   square, i.e., that we're not stretching or squishing the fonts. 
*/
static unsigned short CurrentFontSize (HPS PS)
{
  SIZEF theCharBox;

  DosEnterCritSec();
  GpiQueryCharBox(PS, &theCharBox);
  DosExitCritSec();
  return (unsigned short) (theCharBox.cx / 0x10000);
				/* theCharBox.cx is a fixed-point number. */
				/*  The numerical value we want is this  */
				/*  number divided by 65536. */
}
  

void SizeAxes(HPS hps,
              long Width, long Height,
              double xMin, double xMax, double yMin, double yMax,
	      char *xLabel, char *yLabel,
              struct GraphInfo *G)
{
  double xrange, yrange, x_log, y_log, x_mantissa, y_mantissa;
  double x, y; 
  unsigned long maxTextHeight, maxXTextWidth, maxYTextWidth;
  unsigned long TotalTextWidth, TotalTextHeight; /* Space taken up by text. */
  char buffer[100];
  int nXTicks, nYTicks;		/* Number of ticks on each axis. */
  FONTMETRICS theFont;

  G->xMin = xMin;		/* Copy information about window to the */
  G->xMax = xMax;		/*  GraphInfo structure. */
  G->yMin = yMin;
  G->yMax = yMax;
  G->WindowWidth = Width;
  G->WindowHeight = Height;
  
  strncpy(G->xLabel, xLabel, MaxGraphLabelLength); /* Save axis labels. */
  strncpy(G->yLabel, yLabel, MaxGraphLabelLength);
  G->xLabel[MaxGraphLabelLength-1] = G->yLabel[MaxGraphLabelLength-1] = 0;

  xrange = xMax-xMin;       
  yrange = yMax-yMin;
  if (xrange <= 0 || yrange <= 0) /* Negative range makes no sense. */
    return;
  

  /* Decide where to put the ticks.  Rule: let L be the mantissa of
     the range.  If L is greater then 2, put major ticks at integers; 
     otherwise put them at tenth-integers. */
  x_log = log10(xrange);
  y_log = log10(yrange);
  x_mantissa   = pow(10, x_log - floor(x_log));
  y_mantissa   = pow(10, y_log - floor(y_log));

  G->xTickInc = pow(10, floor(x_mantissa > 2 ? x_log : x_log - 1));
  G->yTickInc = pow(10, floor(y_mantissa > 2 ? y_log : y_log - 1));

  G->xFirstTick = G->xTickInc * ceil(G->xMin/G->xTickInc);
  G->yFirstTick = G->yTickInc * ceil(G->yMin/G->yTickInc);

  nXTicks = floor((G->xMax - G->xFirstTick) / G->xTickInc) + 1;
  nYTicks = floor((G->yMax - G->yFirstTick) / G->yTickInc) + 1;

  /* Decide on the font size.  Rule: if we can, then just use the size of
     the default font.  However, make sure it fits.  Find out how many
     x labels and y labels we're putting on, and make sure the font is small
     enough so they'll fit in the window.  

     For y labels this is easy: if there are nYTicks labels on the y 
     axis, then the height of the font must be smaller than
     WindowHeight/(nYTicks+3), where the 3 is because of the margins.  
     
     For x labels, we need to know the width of each label.  We'll just
     find the width of the largest label, and then multiply by the number
     of labels.

     Note that the x margin is also a bit tricky.  It must be wide enough
     to hold the labels on the y axis; it must also, however, be wide 
     enought to hold half of a label on the x axis.  (What if there's
     a tick at the far left of the graph, after all?)
  */

  G->FontPointSize = CurrentFontSize(hps); /* Default font size for labels. */
  GetFont(hps, G->FontPointSize, 1); /* Change to font with the right size. */

  maxXTextWidth = maxYTextWidth = 0;
  for (x = G->xFirstTick; x < xMax; x += G->xTickInc) {
    sprintf(buffer, "%lg \0", x);
    maxXTextWidth = max(maxXTextWidth, TextWidth(hps, buffer));
  }
  for (y = G->yFirstTick; y < G->yMax; y += G->yTickInc) {
    sprintf (buffer, "%lg\0", y);
    maxYTextWidth = max(maxYTextWidth, TextWidth(hps, buffer));
  }
  
  GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &theFont);
  TotalTextHeight = theFont.lMaxBaselineExt * (nYTicks+4);
  TotalTextWidth  = maxXTextWidth * (nXTicks+2)
                  + max(2*maxYTextWidth, maxXTextWidth);
				/* We know how much space we need at the */
				/*  default font size; readjust the font */
				/*  as necessary to fit in available space. */
  if (TotalTextHeight > G->WindowHeight || TotalTextWidth > G->WindowWidth) {
    G->FontPointSize =
      min((G->FontPointSize * G->WindowHeight) / TotalTextHeight,
	  (G->FontPointSize * G->WindowWidth)  / TotalTextWidth);
      ScaleFont(hps, G->FontPointSize);
  }

  /* Now decide on the size of the margins.  We have to leave enough space 
     around the graph for the labels. */
                                /* First, check height of the x labels. */
  maxTextHeight = maxYTextWidth = 0;
  for (x = G->xFirstTick; x < xMax; x += G->xTickInc) {
    sprintf (buffer, "%lg\0", x);
    maxTextHeight = max(maxTextHeight, TextHeight(hps, buffer));
    maxXTextWidth = max(maxXTextWidth, TextWidth(hps, buffer));
  }
                                /* Do the same for width of the y labels. */
  maxXTextWidth = 0;
  for (y = G->yFirstTick; y < G->yMax; y += G->yTickInc) {
    sprintf (buffer, "%lg\0", y);
    maxYTextWidth = max(maxYTextWidth, TextWidth(hps, buffer));
  }
				/* Look at font metrics again, and  */
				/*  squirrel away some of the information. */
  GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &theFont);
  G->FontHeight = theFont.lLowerCaseAscent;
  G->FontWidth  = theFont.lAveCharWidth;
				/* Finally, return the margin information. */ 
  G->xRightMargin = max(maxYTextWidth, maxXTextWidth/2) + 2*G->FontWidth;
  G->yTopMargin   = maxTextHeight  * 5/4;
  G->xLeftMargin  = G->xRightMargin + 2*G->FontHeight; /* Extra space for */
  G->yBotMargin   = G->yTopMargin   + 2*G->FontHeight; /*  the axis names. */

  G->xConvFactor = (G->WindowWidth - G->xLeftMargin- G->xRightMargin) / xrange;
  G->yConvFactor = (G->WindowHeight- G->yBotMargin - G->yTopMargin) / yrange;

  G->MajorTickSize = (G->FontHeight*4)/5;
  G->HalfTickSize  =  G->FontHeight/2;
  G->MinorTickSize =  G->FontHeight/4;

  G->xLabelWidth = TextWidth(hps, G->xLabel);
  G->yLabelWidth = TextWidth(hps, G->yLabel);

  GpiDeleteSetId(hps, 1);	/* We don't need the font again until it's */
				/*  actually time to display the text. */
}

static void MakeXTick(HPS hps, int x, int y, int length)
{
  POINTL Position;
  Position.x = x;
  Position.y = y;
  GpiMove(hps, &Position);

  Position.y += length;
  GpiLine(hps, &Position);

  Position.y = y;
  GpiMove(hps, &Position);
}

static void MakeYTick(HPS hps, int x, int y, int length) 
{
  POINTL Position;
  Position.x = x;
  Position.y = y;
  GpiMove(hps, &Position);

  Position.x += length;
  GpiLine(hps, &Position);

  Position.x = x;
  GpiMove(hps, &Position);
}

/* Display a string at (x,y).  Leaves current position unchanged. */
static void DrawText(HPS PS, char *text, long x, long y)
{
  POINTL Pos;			/* Where to start drawing string. */
  long TextLength;

  Pos.x = x;
  Pos.y = y;
  TextLength = (long) strlen(text);
  GpiCharStringAt(PS, &Pos, TextLength, text);
}

void DrawAxes(HPS hps, struct GraphInfo *G)
{
  long SavedPS;
  double x, y;
  long WinX, WinY;		/* x and y in Gpi coordinates. */
  double xMinorTickInc, yMinorTickInc, xFirstMinorTick, yFirstMinorTick;
  int xMajorTickIndex, yMajorTickIndex;
  char label[100];		/* Text that we put at a major tick. */
  long LabelWidth;		/* Size of text, in Gpi coordinates. */
  POINTL LowerLeftCorner, UpperRightCorner;
  static GRADIENTL VertAngle = {0, 1}; /* Used to rotate text. */
  int i;

  SavedPS = GpiSavePS(hps);
  GetFont(hps, G->FontPointSize, 1); /* Use correctly scaled font. */
                                    /* Draw the 0 axes. */
  GpiSetLineType(hps, LINETYPE_DOT);
  GpiSetColor(hps, CLR_PALEGRAY);
  if (G->yMin<0 && G->yMax>0) {
    POINTL Left, Right;
    Left.x  = G->xLeftMargin;
    Right.x = G->WindowWidth - G->xRightMargin;
    Left.y  = (long) (G->yBotMargin - G->yMin * G->yConvFactor);
    Right.y = Left.y;
    GpiMove(hps, &Left);
    GpiLine(hps, &Right);
  }
  if (G->xMin<0 && G->xMax>0) {
    POINTL Top, Bottom;
    Bottom.y = G->yBotMargin;
    Top.y    = G->WindowHeight - G->yTopMargin;
    Bottom.x = (long) (G->xLeftMargin - G->xMin * G->xConvFactor);
    Top.x    = Bottom.x;
    GpiMove(hps, &Bottom);
    GpiLine(hps, &Top);
  }
                                /* Draw the box. */
  LowerLeftCorner.x  = G->xLeftMargin;
  LowerLeftCorner.y  = G->yBotMargin;
  UpperRightCorner.x = G->WindowWidth  - G->xRightMargin;
  UpperRightCorner.y = G->WindowHeight - G->yTopMargin;
  GpiSetColor(hps, CLR_DEFAULT);
  GpiSetLineType(hps, LINETYPE_SOLID);
  GpiMove(hps, &LowerLeftCorner);
  GpiBox(hps, DRO_OUTLINE, &UpperRightCorner, 0, 0); 

                                /* Hardest part: draw and label the ticks. */
  yMinorTickInc  = G->yTickInc/10; /* Do the y axis. */
  yMajorTickIndex= (long) (floor((G->yFirstTick - G->yMin)/yMinorTickInc));
  yFirstMinorTick= G->yFirstTick - yMinorTickInc * yMajorTickIndex;
  for (i=0, y=yFirstMinorTick; y < G->yMax; i++, y += yMinorTickInc) {
    if (((i-yMajorTickIndex) % 10) == 0) { /* It's a major tick. */
      WinY = (long) (G->yBotMargin + (y - G->yMin) * G->yConvFactor);
      MakeYTick(hps, G->xLeftMargin, WinY, G->MajorTickSize);   
      if (fabs(y) < yMinorTickInc*1.e-6)
        strcpy(label, "0");
      else
        sprintf(label, "%lg\0", y);
      LabelWidth = TextWidth(hps, label);
      DrawText(hps, label,
	       G->xLeftMargin - LabelWidth - G->FontWidth,
	       WinY - G->FontHeight/2);
   }
    else if (((i - yMajorTickIndex) % 5) == 0) { /* It's a half tick. */
      MakeYTick (hps,
                 G->xLeftMargin,
		 (int)(G->yBotMargin + (y-G->yMin) * G->yConvFactor),
                 G->HalfTickSize);
    }
    else {                          /* It's a minor tick. */
      MakeYTick (hps,
                 G->xLeftMargin,
		 (int)(G->yBotMargin + (y-G->yMin) * G->yConvFactor),
                 G->MinorTickSize);
    }
  }

  xMinorTickInc  = G->xTickInc/10; /* Do the x axis. */
  xMajorTickIndex= (int) (floor((G->xFirstTick - G->xMin)/xMinorTickInc));
  xFirstMinorTick= G->xFirstTick - xMinorTickInc * xMajorTickIndex;
  for (i=0, x=xFirstMinorTick; x < G->xMax; i++, x += xMinorTickInc) {
    if (((i-xMajorTickIndex) % 10) == 0) { /* It's a major tick. */
      WinX = (long) (G->xLeftMargin + (x - G->xMin) * G->xConvFactor);
      MakeXTick(hps, WinX, G->yBotMargin, G->MajorTickSize);
      if (fabs(x) < xMinorTickInc*1.e-6)
        strcpy(label, "0");
      else
        sprintf(label, "%lg\0", x);
      LabelWidth = TextWidth(hps, label);
      DrawText(hps, label,
	       WinX - LabelWidth/2, G->yBotMargin - 3*G->FontHeight/2);
    }
    else if (((i - xMajorTickIndex) % 5) == 0)  /* It's a half tick. */
      MakeXTick (hps,
                 (int)(G->xLeftMargin + (x-G->xMin) * G->xConvFactor),
		 G->yBotMargin,
                 G->HalfTickSize); 
    else        
      MakeXTick (hps,
                 (int)(G->xLeftMargin + (x-G->xMin) * G->xConvFactor),
		 G->yBotMargin,
                 G->MinorTickSize); 
  }

				/* Draw the axis names. */
  GpiSetBackColor(hps, CLR_PALEGRAY);
  GpiSetBackMix(hps, BM_OVERPAINT);
  GpiSetColor(hps, CLR_DARKBLUE);
  DrawText(hps,			/* The x axis. */
	   G->xLabel,
	   G->xLeftMargin + (G->WindowWidth- G->xLeftMargin- G->xRightMargin)/2
	                  - G->xLabelWidth/2,
	   G->FontHeight);

  GpiSetCharAngle(hps, &VertAngle); /* Rotate the text by 90 degrees.  Note */
				    /*  that this won't work unless we've */
				    /*  got an outline font. */
  DrawText(hps,
	   G->yLabel,
	   G->FontHeight * 7/4,
	   G->yBotMargin + (G->WindowHeight - G->yBotMargin - G->yTopMargin)/2
	                 - G->yLabelWidth/2);

  GpiDeleteSetId(hps, 1);	/* Get rid of font we've used. */
  GpiRestorePS(hps, SavedPS);	/* Get rid of changes we've made to 
				/*   the PS parameters. */
}

void ClipToGraph (HPS PS, struct GraphInfo *G)
{
  POINTL LowerLeftCorner, UpperRightCorner;
  RECTL ClipRectangle;
  HRGN ClipRegion, oldClipRegion;

  LowerLeftCorner.x  = G->xLeftMargin;
  LowerLeftCorner.y  = G->yBotMargin;
  UpperRightCorner.x = G->WindowWidth  - G->xRightMargin;
  UpperRightCorner.y = G->WindowHeight - G->yTopMargin;

  GpiConvert(PS, CVTC_PAGE, CVTC_DEVICE, 1L, &LowerLeftCorner);    
  GpiConvert(PS, CVTC_PAGE, CVTC_DEVICE, 1L, &UpperRightCorner);    

  ClipRectangle.xLeft  = LowerLeftCorner.x;
  ClipRectangle.xRight = UpperRightCorner.x;
  ClipRectangle.yBottom= LowerLeftCorner.y;
  ClipRectangle.yTop   = UpperRightCorner.y;

  ClipRegion = GpiCreateRegion(PS, 1, &ClipRectangle);
  GpiSetClipRegion(PS, ClipRegion, &oldClipRegion);
  GpiDestroyRegion(PS, oldClipRegion);
}

void UndoClipping (HPS PS)
{
  HRGN oldClipRegion;

  GpiSetClipRegion(PS, 0, &oldClipRegion);
  GpiDestroyRegion(PS, oldClipRegion);
}
