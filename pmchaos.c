/*  PMChaos.   (C) Copyright Matthew Austern, 1993.

    Permission granted to use, distribute, and modify, provided that this
    notice remains intact.  If you distribute a modified version, you must
    identify your modifications as such.
*/

/* 
   This is a Complicated program that does something very simple: it plots 
   the "standard map", defined by   
                        J'     = J + K sin(theta)
                        theta' = theta + J', mod(2 Pi).
   K is a constant; K=1 is the onset of stochasticity.                         
   (This is Lichtenberg and Lieberman, eq. 3.1.20.)
   Program is complicated because it's all interface!  I'm showing off PM.
*/

#define INCL_DOS
#define INCL_DOSPROCESS
#define INCL_PM
#define INCL_WINHELP

#include <os2.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "graph.h"		/* Functions that do the actual graphing. */

#include "pmchaos_res.h"        /* Included definitions of resources.  */
#include "color.h"              /*  These six files are definitions of */
#include "kfactor.h"            /*  resources for dialog boxes. */
#include "ranges.h"
#include "delay.h"
#include "curpos.h"
#include "about.h"
#include "inline.h"		/* Defines the macro INLINE, used by Map(). */

#define Pi    3.141592653589793238
#define TwoPi (2*Pi)

struct point {
  double J, theta;
};

INLINE struct point Map (struct point *In, double K)
{
  struct point Out = *In;

  Out.J     += K * sin(Out.theta);
  Out.theta += Out.J;

  if ((Out.theta = fmod(Out.theta, TwoPi)) < 0)
    Out.theta += TwoPi;
/* Uncommenting the next block speeds up the program, but it changes the
   global topology from cylindrical to toroidal.  Essentially, this merges
   the two fixed points into one, so the separatrix in the middle of the
   screen is no longer much of a separatrix.  The actual plots, though,
   still look much the same. */
/*
  if ((Out.J = fmod(Out.J, TwoPi)) < 0)
    Out.J += TwoPi;
*/
  return Out;
}

struct Bitmap {
  HBITMAP theBitmap;
  HPS MemPS;
  HDC MemDC;
  int isValid;
};

static void CreateBitmap (struct Bitmap *B, HWND Win, POINTL *WinSize)
{
  BITMAPINFOHEADER2 BitmapFormat;
  LONG BitmapPlanesBits[2];
  SIZEL PS_Size = {0L, 0L};
  HPS WinPS;
  
  WinPS = WinGetPS(Win);
  GpiQueryDeviceBitmapFormats (WinPS, 2, BitmapPlanesBits);

  memset(&BitmapFormat, 0, sizeof(BITMAPINFOHEADER2));
  BitmapFormat.cbFix = sizeof(BITMAPINFOHEADER2);
  BitmapFormat.cx = (ULONG) WinSize->x;
  BitmapFormat.cy = (ULONG) WinSize->y;
  BitmapFormat.cPlanes = (USHORT) BitmapPlanesBits[0];
  BitmapFormat.cBitCount=(USHORT)BitmapPlanesBits[1];
  B->theBitmap = GpiCreateBitmap(WinPS, &BitmapFormat, 0, 0, 0);
  WinReleasePS(WinPS);

  B->MemDC = DevOpenDC(WinQueryAnchorBlock(Win),
                       OD_MEMORY, "*", 0, 0, 0);
  B->MemPS = GpiCreatePS(WinQueryAnchorBlock(Win),
                         B->MemDC, &PS_Size,
                         PU_HIMETRIC | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
  GpiSetBitmap(B->MemPS, B->theBitmap);
  B->isValid = 1;
}

static void DestroyBitmap (struct Bitmap *B)
{
  if(B->isValid) {
    GpiDestroyPS(B->MemPS);
    DevCloseDC(B->MemDC);
    GpiDeleteBitmap(B->theBitmap);
  }
  B->isValid = 0;
}

static void ResetBitmap (struct Bitmap *B, HWND Win, POINTL *WinSize)
{
  DestroyBitmap(B);
  CreateBitmap(B, Win, WinSize);
  GpiErase(B->MemPS);
}

/* Dialog Procedures.  We have three dialog boxes: setting the K factor,
   setting the range of the graph, and setting the color. */
/* First, the range dialog. */

struct GraphRange {		/* This struct is used when sending  */
  struct point Min;		/*  information to and from the */
  struct point Max;		/*  range dialog. */
};

/* We're going to subclass the entry field window procedure so that the
   user can't type keys that are meaningless for entering numbers.  
   EntryFieldProc and EntryFieldNum are used for double, IntEntryFieldProc
   and EntryFieldInteger are used for non-negative integers.
*/
static PFNWP OldEntryFieldProc;

MRESULT EXPENTRY EntryFieldProc (HWND Win, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  if (msg == WM_CHAR) {
    if ((unsigned long)mp1 & KC_VIRTUALKEY) { /* Process virtual keys. */
      unsigned short theKey = SHORT2FROMMP(mp2);
      if (theKey == VK_SPACE)   /* Filter out space: it's meaningless. */
        return 0;
      else if (theKey == VK_NEWLINE || theKey == VK_ENTER) {
                                /* Notify parent of ENTER or RETURN. */
        WinSendMsg(WinQueryWindow(Win, QW_PARENT), FEF_Enter,
                   MPFROM2SHORT(WinQueryWindowUShort(Win, QWS_ID), 0),
                   0);          /* Our ID is 1st short in mp2. */
        return (MRESULT) 1;
      }
      else                      /* Normal processing for other virtual keys. */
        return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
    }
    else if ((ULONG)mp1 & KC_CHAR && !((ULONG)mp1 & KC_KEYUP)) {
      char theKey;              /* ASCII code of the key. */
      theKey = (char) (SHORT1FROMMP(mp2));
      if (isdigit(theKey) || theKey == 'e' || theKey == 'E' ||
          theKey == '.' || theKey == '+' || theKey == '-')
        return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
      else                      /* Filter out other meaningless characters. */
        return 0;
    }
    else
      return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
  }
  else
    return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
}

MRESULT EXPENTRY IntEntryFieldProc(HWND Win, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  if (msg == WM_CHAR) {
    if ((unsigned long)mp1 & KC_VIRTUALKEY) { /* Process virtual keys. */
      unsigned short theKey = SHORT2FROMMP(mp2);
      if (theKey == VK_SPACE)   /* Filter out space: it's meaningless. */
        return 0;
      else if (theKey == VK_NEWLINE || theKey == VK_ENTER) {
                                /* Notify parent of ENTER or RETURN. */
        WinSendMsg(WinQueryWindow(Win, QW_PARENT), FEF_Enter,
                   MPFROM2SHORT(WinQueryWindowUShort(Win, QWS_ID), 0),
                   0);          /* Our ID is 1st short in mp2. */
        return (MRESULT) 1;
      }
      else                      /* Normal processing for other virtual keys. */
        return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
    }
    else if ((ULONG)mp1 & KC_CHAR && !((ULONG)mp1 & KC_KEYUP)) 
      if (isdigit((char)(SHORT1FROMMP(mp2)))) /* Accept digits. */
        return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
      else                      /* Filter out other meaningless characters. */
        return 0;
    else
      return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
  }
  else
    return (*OldEntryFieldProc)(Win, msg, mp1, mp2);
}


/* Parses the text in an entry field.  If it is a valid number, then
   returns 1 and sets *val to that number; otherwise, pops up a message
   box to yell at the user and returns 0.

   Win is the window handle of the dialog box, and DlgID is the ID of
   the entry field we're looking at.
*/
static int EntryFieldNum (HWND Win, unsigned long DlgID,
                          double Min, double Max,
                          double *val)
{
  char Buffer[100], OutBuffer[200];
  double result;

  WinQueryDlgItemText(Win, DlgID, 100, Buffer);
  if (sscanf(Buffer, "%lg", &result) != 1) {
    sprintf (OutBuffer, "Invalid input: %s\0", Buffer);
    WinMessageBox(HWND_DESKTOP, Win, OutBuffer,
                  "PM Chaos", 0, 
                  MB_ERROR | MB_OK | MB_MOVEABLE);
    return 0;
  }
  else if (result < Min || result > Max) {
    sprintf (OutBuffer, "Input out of range, must be between %.3f and %.3f.\0",
                        Min, Max);
    WinMessageBox(HWND_DESKTOP, Win, OutBuffer,
                  "PM Chaos", 0,
                  MB_ERROR | MB_OK | MB_MOVEABLE);
    return 0;
  }
  else {
    sprintf (OutBuffer, "%.3f\0", result);
    WinSetDlgItemText (Win, DlgID, OutBuffer);
    *val = result;
    return 1;
  }
}

/* Parses the text in an entry field, requiring that it be an integer. */
static int EntryFieldInteger (HWND Win, unsigned long DlgID,
			      int Min, int Max,
			      int *val)
{
  char Buffer[100], OutBuffer[200];
  int result;

  WinQueryDlgItemText(Win, DlgID, 100, Buffer);
  if (sscanf(Buffer, "%d", &result) != 1) {
    sprintf (OutBuffer, "Invalid input: %s\0", Buffer);
    WinMessageBox(HWND_DESKTOP, Win, OutBuffer,
                  "PM Chaos", 0, 
                  MB_ERROR | MB_OK | MB_MOVEABLE);
    return 0;
  }
  else if (result < Min || result > Max) {
    sprintf (OutBuffer, "Input out of range, must be between %d and %d.\0",
                        Min, Max);
    WinMessageBox(HWND_DESKTOP, Win, OutBuffer,
                  "PM Chaos", 0,
                  MB_ERROR | MB_OK | MB_MOVEABLE);
    return 0;
  }
  else {
    sprintf (OutBuffer, "%d\0", result);
    WinSetDlgItemText (Win, DlgID, OutBuffer);
    *val = result;
    return 1;
  }
}

/* Dialog procedure for setting the range of the graph.  Note that the 
   range can't extend past 0 or 2 Pi for either of the axes.
*/

MRESULT EXPENTRY RangeDialog (HWND DlgWin, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  static struct GraphRange NewRange, *OldRange;
  char Buffer[100];

  switch(Msg) {
  case WM_INITDLG:              /* Initialize the dialog. */
    OldRange = (struct GraphRange *) PVOIDFROMMP(mp2);
    NewRange = *OldRange;

                                /* Don't let user type non-numeric values. */
    OldEntryFieldProc = WinSubclassWindow(WinWindowFromID(DlgWin, DID_JMin),
                                          EntryFieldProc);
    WinSubclassWindow(WinWindowFromID(DlgWin, DID_JMax), EntryFieldProc);
    WinSubclassWindow(WinWindowFromID(DlgWin, DID_ThetaMin), EntryFieldProc);
    WinSubclassWindow(WinWindowFromID(DlgWin, DID_ThetaMax), EntryFieldProc);
                                /* Display text in entry fields. */
    sprintf(Buffer, "%.3f\0", NewRange.Min.J);
    WinSetDlgItemText(DlgWin, DID_JMin, Buffer);
    sprintf(Buffer, "%.3f\0", NewRange.Max.J);
    WinSetDlgItemText(DlgWin, DID_JMax, Buffer);
    sprintf(Buffer, "%.3f\0", NewRange.Min.theta);
    WinSetDlgItemText(DlgWin, DID_ThetaMin, Buffer);
    sprintf(Buffer, "%.3f\0", NewRange.Max.theta);
    WinSetDlgItemText(DlgWin, DID_ThetaMax, Buffer);  
                                /* Set focus in an entry field. */
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(DlgWin, DID_JMin));
    return (MRESULT) 1;
  case WM_COMMAND:              /* Process user request. */
    switch(SHORT1FROMMP(mp1)) {
    case DID_Range_Cancel:
      WinDismissDlg(DlgWin, FALSE);
      return (MRESULT) 1;
    case DID_Range_OK:
      if (EntryFieldNum(DlgWin, DID_JMin, 0, TwoPi, &NewRange.Min.J) &&
          EntryFieldNum(DlgWin, DID_JMax, 0, TwoPi, &NewRange.Max.J) &&
          EntryFieldNum(DlgWin, DID_ThetaMin, 0, TwoPi, &NewRange.Min.theta) &&
          EntryFieldNum(DlgWin, DID_ThetaMax, 0, TwoPi, &NewRange.Max.theta))
        if (NewRange.Min.J < NewRange.Max.J &&
            NewRange.Min.theta < NewRange.Max.theta) {
          *OldRange = NewRange; /* Return results to caller. */
          WinDismissDlg(DlgWin, TRUE);
          return (MRESULT) 1;
        }
        else {
          WinMessageBox(HWND_DESKTOP, DlgWin,
                        "Error: Maximum value must be larger than minimum.",
                        "PM Chaos: range", 0,
                        MB_ERROR | MB_OK | MB_MOVEABLE);
          return (MRESULT) 1;
        }
      else
        return (MRESULT) 1;     /* Result was invalid. */
    case DID_Range_Help:
      return (MRESULT) 1;
    default:
    return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
    }
  case FEF_Enter: {             /* User hit return in an entry field. */
    unsigned short TheEntryField; /* Which entry field is it? */
    double dummy;               /* We're just validating the result; we */
                                /*  don't care what it is. */
    TheEntryField = SHORT1FROMMP(mp1);
    EntryFieldNum(DlgWin, TheEntryField, 0, TwoPi, &dummy);
    return (MRESULT) 1;
  }
  default:
    return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
  }
}


/* Dialog procedure for setting the K constant in the standard map.  We're
   using a slider that runs from 0 to 3.  There are 301 ticks in the 
   slider, so the user gets to specify two digits after the decimal point.
*/

MRESULT EXPENTRY KFactorDlg (HWND DlgWin, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  static double *oldK, newK;
  static short Tick;            /* Which tick is the slider at? */
  char Buffer[100];
  unsigned short i;

  switch(Msg) {
  case WM_INITDLG:
    oldK = (double *) PVOIDFROMMP(mp2);
    newK = *oldK;
    Tick = (int) floor(newK*100 + 0.5);

    sprintf(Buffer, "K = %.2f\0", newK); /* Set up the text. */
    WinSetDlgItemText(DlgWin, DID_K_Text, Buffer);
                                /* Initialize the slider.  Label 0,1,2,3,
                                /*  and put ticks at 0.1,0.2,... */
    for (i=0; i<=300; i += 10)
      if (!(i%100))
        WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETTICKSIZE,
                          MPFROM2SHORT(i,11), 0);
      else if (!(i%50))
        WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETTICKSIZE,
                          MPFROM2SHORT(i,7), 0);
      else
        WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETTICKSIZE,
                          MPFROM2SHORT(i,4), 0);

    WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETSCALETEXT,
                      MPFROMSHORT(0), "0.0");
    WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETSCALETEXT,
                      MPFROMSHORT(100), "1.0");
    WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETSCALETEXT,
                      MPFROMSHORT(200), "2.0");
    WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETSCALETEXT,
                      MPFROMSHORT(300), "3.0");

                                /* Set initial slider position. */
    WinSendDlgItemMsg(DlgWin, DID_K_Slider, SLM_SETSLIDERINFO,
                      MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
                      MPFROMSHORT(Tick));

    WinSetFocus (HWND_DESKTOP, WinWindowFromID(DlgWin, DID_K_Slider));
    return (MRESULT) 1;
  case WM_CONTROL:              /* Get control message from slider. */
    if (SHORT1FROMMP(mp1)==DID_K_Slider && SHORT2FROMMP(mp1)== SLN_CHANGE) { 
                                /* Find what tick the slider is at. */
        Tick = (short)WinSendDlgItemMsg(DlgWin, DID_K_Slider,
                                        SLM_QUERYSLIDERINFO,
                                        MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                                     SMA_INCREMENTVALUE),
                                        0);
        newK = Tick/100.;               /* Convert tick number to K value. */
        sprintf(Buffer, "K = %.2f\0", newK); /* Copy it into the text. */
        WinSetDlgItemText(DlgWin, DID_K_Text, Buffer);
      return (MRESULT) 1;
    }
    else
      return (MRESULT) 0;
  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) {
    case DID_K_Cancel:
      WinDismissDlg(DlgWin, FALSE);
      return (MRESULT) 1;
    case DID_K_OK:
      *oldK = newK = Tick/100.;
      WinDismissDlg(DlgWin, TRUE);
      return (MRESULT) 1;
    case DID_K_Help:
      return 0;
    default:
      return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
    }
  default:
    return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
  }
}

/* Dialog to set the color.  mp2 is a pointer to a long containing 
   the color---i.e., CLR_BLACK, etc.
*/

MRESULT EXPENTRY ColorDlg (HWND DlgWin, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  static long ColorNames[] = 
    { CLR_WHITE,    CLR_BLACK,     CLR_BLUE,     CLR_RED,
      CLR_PINK,     CLR_GREEN,     CLR_CYAN,     CLR_YELLOW,
      CLR_PALEGRAY, CLR_DARKGRAY,  CLR_DARKBLUE, CLR_DARKRED,
      CLR_DARKPINK, CLR_DARKGREEN, CLR_DARKCYAN, CLR_BROWN};
  static unsigned long ColorIDs[] =  
    { DID_CLR_WHITE,    DID_CLR_BLACK,     DID_CLR_BLUE,     DID_CLR_RED,
      DID_CLR_PINK,     DID_CLR_GREEN,     DID_CLR_CYAN,     DID_CLR_YELLOW,
      DID_CLR_PALEGRAY, DID_CLR_DARKGRAY,  DID_CLR_DARKBLUE, DID_CLR_DARKRED,
      DID_CLR_DARKPINK, DID_CLR_DARKGREEN, DID_CLR_DARKCYAN, DID_CLR_BROWN};
  int NumColors = 16;
  static long *oldColorName;
  static unsigned long newColorID;
  int i;

  switch(Msg) {
  case WM_INITDLG: {
    oldColorName = (long *) mp2; /* Get old color. */
    newColorID = DID_CLR_WHITE; /* Translate it into ID of a radio button. */
    for (i=0; i<NumColors; i++) 
      if (*oldColorName == ColorNames[i])
        newColorID = ColorIDs[i];
                                /* Check the appropriate radio button, and */
                                /*  set input focus to it. */
    WinSendDlgItemMsg(DlgWin, newColorID, BM_SETCHECK, (void *) 1, (void *) 0);
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(DlgWin, newColorID));
    for (i=0; i<NumColors; i++)	/* Set button colors correctly. */
      WinSetPresParam(WinWindowFromID(DlgWin, ColorIDs[i]),
		      PP_BACKGROUNDCOLORINDEX,
		      sizeof(long),
		      &(ColorNames[i]));
			
    return (MRESULT) 1;
  }

  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) {
    case DID_Color_OK:          /* Find which button is checked. */
      i = (int) WinSendDlgItemMsg(DlgWin,
                                  DID_CLR_WHITE, BM_QUERYCHECKINDEX, 0, 0);
      *oldColorName = ColorNames[i];
      WinDismissDlg(DlgWin, TRUE);
      return (MRESULT) TRUE;
    case DID_Color_Cancel:
      WinDismissDlg(DlgWin, FALSE);
      return (MRESULT) TRUE;
    case DID_Color_Help:
      return 0;
    default:
      return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
    }
  default:
    return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
  }
}

/* Trivial dialog to put up the About box. */
MRESULT EXPENTRY AboutDlg (HWND DlgWin, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  switch(Msg) {
  case WM_INITDLG:
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(DlgWin, DID_About_OK));
    return (MRESULT) TRUE;
  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) {
    case DID_About_OK:
      WinDismissDlg(DlgWin, 1);
      return (MRESULT) TRUE;
    default:
      return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
    }
  default:
    return WinDefDlgProc(DlgWin, Msg, mp1, mp2);
  }
}

/* This next function is also a dialog procedure, but it's rather a
   one-way dialog, and rather a vacuous dialog procedure: it's used to
   show the user the current point.  This is intended to be used as a
   non-modal dialog. 
*/
MRESULT EXPENTRY DisplayPoint (HWND Win, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  switch(Msg) {
  case WM_INITDLG:
    WinSetFocus(HWND_DESKTOP, Win);
    return (MRESULT) 1;

  case WM_UpdatePoint: {
    struct point *thePoint = (struct point *) mp1;
    char Buffer[100];

    sprintf(Buffer, "%.4f", thePoint->J);
    WinSetDlgItemText(Win, DID_JCoord, Buffer);
    sprintf(Buffer, "%.4f", thePoint->theta);
    WinSetDlgItemText(Win, DID_ThetaCoord, Buffer);

    return (MRESULT) 1;
  }
    
  default:
    return WinDefDlgProc(Win, Msg, mp1, mp2);
  }
}
 

/* Finally, a dialog procedure to select just how slow the "slow motion"
   actually is.  The user types in how long we'll wait on each point,
   in units of 1/10 sec.

   We'll subclass the entry field control, just as in RangeDialog.
*/
MRESULT EXPENTRY DelayDlgProc (HWND Win, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  char Buffer[100];
  static unsigned long *Delay; /* Delay, in units of ms. */
  
  switch(Msg) {
  case WM_INITDLG:		/* Initialize the dialog. */
    Delay = (unsigned long *) mp2;

    OldEntryFieldProc = WinSubclassWindow(WinWindowFromID(Win, DID_DelayEntry),
					  IntEntryFieldProc);
    sprintf(Buffer, "%d\0", (int)(*Delay/100.));
    WinSetDlgItemText(Win, DID_DelayEntry, Buffer);

    WinSetFocus(HWND_DESKTOP, WinWindowFromID(Win, DID_DelayEntry));

    return (MRESULT) 1;

  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) {
    case DID_Delay_Cancel:
      WinDismissDlg(Win, FALSE);
      return (MRESULT) 1;

    case DID_Delay_OK: {
      int NumTenths;
      if (EntryFieldInteger(Win, DID_DelayEntry, 0, 32767, &NumTenths)) {
	*Delay = (unsigned long) (NumTenths * 100);
	*Delay = *Delay > 0 ? *Delay : 1;
	WinDismissDlg(Win, TRUE);
      }
      return (MRESULT) 1;
      
    case DID_Delay_Help:
      return (MRESULT) 1;
    }
    default:
      return WinDefDlgProc(Win, Msg, mp1, mp2);
    }
      
  case FEF_Enter: {             /* User hit return in an entry field. */
    unsigned short TheEntryField; /* Which entry field is it? */
    int dummy;			/* We're just validating the result; we */
                                /*  don't care what it is. */
    TheEntryField = SHORT1FROMMP(mp1);
    EntryFieldInteger(Win, TheEntryField, 0, 32767, &dummy);
    return (MRESULT) 1;
  }
  default:
    return WinDefDlgProc(Win, Msg, mp1, mp2);
  }    
}

/***********************************************************************
 * Now the main part of the program.  We're doing this multi-threaded: *
 * when the user selects START, the computation goes along in another  *
 * thread.  This thread draws points on both the window and on the     *
 * shadow bitmap.  Redraws of the window are done in the main thread,  *
 * but this is never anything more than a copy of the shadow bitmap to *
 * the window.                                                         *
 ***********************************************************************/

struct InterThread {
  int isRunning;		/* Are we running?                          */
  int isSlowMotion;		/* Should we pause after each point?        */
  HWND PosWin;			/* Display of coordinates of current point. */
  unsigned long theDelay;	/* If so, for how long?                     */
  int Terminate;		/* Flag: set to 1 when we kill thread 2.    */
  struct point *CurrentPoint;	/* Point where we'll start mapping.         */
  HWND theWindow;		/* Handle of main program window.           */
  struct Bitmap *theBitmap;	/* The shadow bitmap.                       */
  struct GraphInfo *G;		/* Graph range, etc.                        */
  double K;			/* Stochasticity parameter.                 */
  long theColor;		/* Color we're using for the map.           */
  HEV Thread1ToThread2;		/* Semaphores used to synchronize the       */
  HEV Thread2ToThread1;		/*   threads.                               */
};

                                /* Do appropriate semaphore wizardry */
                                /*  to start mapping in thread 2. */
void StartMapping (struct InterThread *Control)
{
  unsigned long PostCount;

  if (!Control->isRunning) {
    if (Control->isSlowMotion)  /* We'll want a position display. */
      WinSetWindowPos(Control->PosWin, HWND_TOP,
		      0, 0, 0, 0,
		      SWP_SHOW | SWP_ACTIVATE | SWP_ZORDER);

    DosPostEventSem(Control->Thread1ToThread2); /* Tell thread 2 to start. */
    DosWaitEventSem(Control->Thread2ToThread1, (ULONG) SEM_INDEFINITE_WAIT);
                                /* Wait until it starts. */
    DosEnterCritSec();
    Control->isRunning = 1;
    DosResetEventSem(Control->Thread2ToThread1, &PostCount);
    DosExitCritSec();
  }
}

void StopMapping (struct InterThread *Control)
{
  unsigned long PostCount;

  if (Control->isRunning) {
    DosPostEventSem(Control->Thread1ToThread2); /* Tell thread 2 to stop. */
    DosWaitEventSem(Control->Thread2ToThread1, (ULONG) SEM_INDEFINITE_WAIT); 
                                /* Wait until it stops. */
    if (Control->isSlowMotion)	/* Get rid of the position display. */
      WinSetWindowPos(Control->PosWin, 0, 0, 0, 0, 0, SWP_HIDE);

    DosEnterCritSec();
    DosResetEventSem(Control->Thread2ToThread1, &PostCount);
    Control->isRunning = 0;
    DosExitCritSec();
  }
}

void KillMappingThread (struct InterThread *Control)
{
  if (Control->isRunning)
    StopMapping(Control);

  Control->Terminate = 1;
  StartMapping(Control);
}

/* Most of this function is completely straightforward: wait until thread 1
   tells us to start, then go into the inner mapping loop until thread 1 tells
   us to stop.  The only slightly tricky aspects: (1) We draw each point
   both on the screen and on the shadow bitmap.  (2) We actually have two
   different inner loops, depending on whether we're doing this normally
   or in slow motion.

   There are two reasons slow motion is a bit tricky.  First: we have to
   pause between points, but we also have to be ready to quit the loop
   instantly, once thread 1 tells us to stop---we can't tie up the message
   queue for several seconds!  Second: in slow motion, we don't just set a
   single pel.  We want to make it obvious which point is the current point.
   So we have to draw a prominent marker, and then erase it when we move on
   to the next point.
*/

/* This small helper function is just like GpiSetMarker, really.  I'm
   just defining my own version because I can't get GpiSetMarker to
   work with a mix mode of FM_INVERT. 
*/
void DrawMark(HPS PS, POINTL *Pos)
{
  static ARCPARAMS MMCircle = {100, 100, 0, 0};	/* Circle with 1mm radius. */
  GpiSetArcParams(PS, &MMCircle);

  GpiMove(PS, Pos);		/* Draw two concentric circles. */
  GpiFullArc(PS, DRO_OUTLINE, MAKEFIXED(2, 0));
  GpiFullArc(PS, DRO_OUTLINE, MAKEFIXED(0, 32768));
}  

void MappingThread (void *ControlArgument)
{
  struct InterThread *ThreadControl;
  struct point thePoint;
  POINTL thePel;
  static SIZEL dummy = {0L, 0L};
  unsigned long PostCount;
  HPS WinPS, MemPS;

  ThreadControl = (struct InterThread *) ControlArgument;

  while(1) {
                                /* Wait for main thread to post semaphore. */
    DosWaitEventSem(ThreadControl->Thread1ToThread2,
		    (ULONG) SEM_INDEFINITE_WAIT);
    DosEnterCritSec();          /* Reset the semaphore. */
    DosResetEventSem(ThreadControl->Thread1ToThread2, &PostCount);      
    DosPostEventSem(ThreadControl->Thread2ToThread1);
                                /* Signal thread 1 that we're running. */
    DosExitCritSec();

    if (ThreadControl->Terminate) { /* See if Thread 1 wants us to stop. */
      DosPostEventSem(ThreadControl->Thread2ToThread1); 
      return;
    }                           /* Now we begin the main mapping loop. 
                                   Continue until the semaphore is posted
                                   again. */
    thePoint = *(ThreadControl->CurrentPoint);
    WinPS    = WinGetPS(ThreadControl->theWindow); /* Get pres. space. */
    MemPS = ThreadControl->theBitmap->MemPS; /* PS for shadow bitmap. */

    GpiSetPS (WinPS, &dummy, PU_HIMETRIC); /* Set units of PS measurement. */
    GpiSetPS (MemPS, &dummy, PU_HIMETRIC); 
    ClipToGraph(WinPS, ThreadControl->G); /* Don't draw outside graph. */
    ClipToGraph(MemPS, ThreadControl->G); 
    GpiSetColor (WinPS, ThreadControl->theColor); 
    GpiSetColor (MemPS, ThreadControl->theColor); 

    if (ThreadControl->isSlowMotion) { /* Do the mapping step by step. */
      GpiSetMix(WinPS, FM_INVERT);
      GpiSetMix(MemPS, FM_INVERT);
      thePel = toPoint(thePoint.J, thePoint.theta, ThreadControl->G);

      DrawMark(WinPS, &thePel); /* Draw the initial point. */
      DrawMark(MemPS, &thePel);
      WinPostMsg(ThreadControl->PosWin, WM_UpdatePoint,
		 &thePoint, 0);

				/* Begin the mapping loop. */
      while (DosWaitEventSem(ThreadControl->Thread1ToThread2,
			     ThreadControl->theDelay) != 0) {
	DrawMark(WinPS, &thePel); /* Erase old point.  Note that we're  */
	DrawMark(MemPS, &thePel); /*  using mix mode of FM_INVERT. */

	GpiSetMix(WinPS, FM_OVERPAINT);
	GpiSetMix(MemPS, FM_OVERPAINT);
	GpiSetPel(WinPS, &thePel); /* Draw old point now, but just as a */
	GpiSetPel(MemPS, &thePel); /*  single pel. */

	thePoint = Map(&thePoint, ThreadControl->K);
	thePel = toPoint(thePoint.J, thePoint.theta, ThreadControl->G);
	GpiSetMix(WinPS, FM_INVERT);
	GpiSetMix(MemPS, FM_INVERT);

	DrawMark(WinPS, &thePel); /* Draw the new point. */
	DrawMark(MemPS, &thePel);
	WinPostMsg(ThreadControl->PosWin, WM_UpdatePoint,
		   &thePoint, 0);
      }
      DrawMark(WinPS, &thePel); /* Erase the mark now that we're done */
      DrawMark(MemPS, &thePel); /*  mapping. */
    }
    else			/* Run at full speed. */
      while (DosQueryEventSem(ThreadControl->Thread1ToThread2, &PostCount),
	     PostCount == 0) {	/* This is the inner mapping loop. */
	thePel = toPoint(thePoint.J, thePoint.theta, ThreadControl->G);
	GpiSetPel(WinPS, &thePel);
	GpiSetPel(MemPS, &thePel);
	thePoint = Map(&thePoint, ThreadControl->K);
      }

    DosEnterCritSec();          /* Semaphore was posted; stop now. */
    DosResetEventSem(ThreadControl->Thread1ToThread2, &PostCount);      
    UndoClipping(MemPS);
    UndoClipping(WinPS);
    WinReleasePS(WinPS);
    *(ThreadControl->CurrentPoint) = thePoint; /* Pass back new value. */
                                /* Signal that we're done. */
    DosPostEventSem(ThreadControl->Thread2ToThread1);
    DosExitCritSec();
  }
}

                                /* Disable a menu item. */
static void MenuDisable (HWND Win, USHORT theItem)
{
  HWND theMenu;
  theMenu = WinWindowFromID(WinQueryWindow(Win, QW_PARENT), FID_MENU);

  WinSendMsg(theMenu, MM_SETITEMATTR,
             MPFROM2SHORT(theItem, 1),
             MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
}

                                /* Enable a menu item. */
static void MenuEnable (HWND Win, USHORT theItem)
{
  HWND theMenu;
  theMenu = WinWindowFromID(WinQueryWindow(Win, QW_PARENT), FID_MENU);

  WinSendMsg(theMenu, MM_SETITEMATTR,
             MPFROM2SHORT(theItem, 1),
             MPFROM2SHORT(MIA_DISABLED, 0));
}

/* Do everything necessary to clear the screen and redraw the axes on it. */
static void ClearScreen (struct Bitmap *theBitmap, HWND Win, POINTL *WinSize,
                         struct GraphRange *theRange, struct GraphInfo *G)
{
  SIZEL dummy = {0L, 0L};
  POINTL GpiSize;               /* Window size in GPI units. */
  
  ResetBitmap(theBitmap, Win, WinSize); /* Clear the shadow bitmap. */
  GpiSetPS(theBitmap->MemPS, &dummy, PU_HIMETRIC);

  GpiSize = *WinSize;		/* Get window size in Gpi coordinates. */
  GpiConvert(theBitmap->MemPS, CVTC_DEVICE, CVTC_PAGE, 1L, &GpiSize); 
    
  SizeAxes(theBitmap->MemPS,  /* Decide on sizing for graph axes. */
           GpiSize.x, GpiSize.y,
           theRange->Min.J, theRange->Max.J,
           theRange->Min.theta, theRange->Max.theta,
	   "J", "Theta",
           G);

  WinInvalidateRegion(Win, 0, 0); /* Get shadow copied to the screen. */
} 


/* given two opposite corners of a rectangle, draw the rectangle.  We draw
     it using FM_INVERT, so that we can erase it with a second call to the
     same function. 
   This function assumes that PS has been set to the correct units and that
     it has already been clipped appropriately.  This is for the sake
     of efficiency: we'll typically want to call it twice in a row (erase the
     old rectangle and draw a new one), so why do the clipping and such twice?
*/

void DrawRect (HPS PS, POINTL *Corner1, POINTL *Corner2)
{
  GpiSetMix (PS, FM_INVERT);
  GpiMove(PS, Corner1);
  GpiBox(PS, DRO_OUTLINE, Corner2, 0, 0); 
}

double fmin (double x, double y)
{
  if (x < y)
    return x;
  else
    return y;
}

double fmax (double x, double y)
{
  if (x > y)
    return x;
  else
    return y;
}

double InRange (double x, double min, double max)
{
  return fmin(max, fmax(x, min));
}

void DestroyHelp (HWND aHelpWindow)
{
  if (aHelpWindow)
    WinDestroyHelpInstance(aHelpWindow);
}


/* Creates a "help instance" for PM Chaos. */
HWND InitHelp(HWND aWindow)
{
  HELPINIT HelpData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  HWND theHelpWindow, aFrame;
  HAB theAnchorBlock;

  HelpData.cb                 =  sizeof(HELPINIT);
  HelpData.phtHelpTable       = (HELPTABLE*)MAKELONG(ID_MainHelpTable, 0xFFFF);
  HelpData.pszHelpWindowTitle = "PM Chaos Help";
  HelpData.pszHelpLibraryName = "pmchaos.hlp";

  theAnchorBlock = WinQueryAnchorBlock(aWindow);
  aFrame = WinQueryWindow(aWindow, QW_PARENT);

  theHelpWindow = WinCreateHelpInstance(theAnchorBlock, &HelpData);
  if (!theHelpWindow || HelpData.ulReturnCode ||
      !WinAssociateHelpInstance(theHelpWindow, aFrame)) {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
		  "Warning: On-line help is unavailable",
		  "PM Chaos", 0,
		  MB_OK | MB_WARNING);
    DestroyHelp(theHelpWindow);
    theHelpWindow = 0;
  }
  
  return
    theHelpWindow;
}
    

MRESULT EXPENTRY ChaosWindowProc (HWND Win, ULONG Msg, MPARAM mp1, MPARAM mp2)
{
  static struct InterThread theControlParams;
  static HWND theHelpWin;	/* Window that handles the help system. */
  static struct GraphRange theRange;    /* Min and max values of graph. */
  static struct GraphInfo G;	/* Sizing information about the axes. */
  static struct Bitmap ShadowBitmap;
  static struct point StartingPoint;
  static POINTL WinSize;        /* Size of window, in device coordinates. */

  static int ButtonDown;        /* Is user holding right mouse button down? */
  static int RectExists;        /* Has user finished drawing a rectangle? */
  static HPOINTER RectPointer;  /* Mouse pointer used for drawing a rect. */
  static POINTL Corner1, Corner2; /* Corner1 is where user first depressed */
                                  /*  button 2, Corner2 is current position. */
  SIZEL dummy = {0L, 0L};	/* Used in calls to GpiSetPS. */
    
  switch(Msg) {
  case WM_CREATE: {
    static unsigned long PostCount;
    
    ShadowBitmap.isValid = 0;   /* We haven't created it yet.  We can't
                                   /*  do so until we receive WM_SIZE. */
    ButtonDown = RectExists = 0;
    RectPointer = WinLoadPointer(HWND_DESKTOP, 0, ID_RectPtr);

    theRange.Min.J = theRange.Min.theta = 0; /* Default range of graph. */
    theRange.Max.J = theRange.Max.theta = TwoPi; 

    theControlParams.K = 0.85;  /* Set default values. */
    theControlParams.theColor = CLR_BLUE;

    theControlParams.isRunning = 0;
    theControlParams.Terminate = 0;
    theControlParams.theWindow = Win;
    theControlParams.theBitmap = &ShadowBitmap;
    theControlParams.G = &G;
    theControlParams.CurrentPoint = &StartingPoint;

    theControlParams.isSlowMotion = 0;
    theControlParams.theDelay     = 500; /* 1/2 second. */
    theControlParams.PosWin = 
      WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP,
		 DisplayPoint, 0,
		 DID_CurPosition, 0); /* This window is created invisible. */

                                /* Create Thread 2. */
    DosCreateEventSem(0, &theControlParams.Thread1ToThread2, 0, 0);
    DosCreateEventSem(0, &theControlParams.Thread2ToThread1, 0, 0);
    DosResetEventSem(theControlParams.Thread1ToThread2, &PostCount);
    DosResetEventSem(theControlParams.Thread2ToThread1, &PostCount);
    _beginthread(MappingThread, 0, 0x10000, &theControlParams); 
				/* Create the help window. */
    theHelpWin = InitHelp(Win);
				/* Disable some menu items. */
    MenuDisable(Win, IDM_Stop); /* Can't stop mapping until we start. */
    MenuDisable(Win, IDM_ZoomIn); /* Can't zoom in without a zoom rectangle. */
    return 0;
  }
  case WM_SIZE:
    if (theControlParams.isRunning) /* Stop mapping, if we're doing it. */
      StopMapping(&theControlParams);
    MenuDisable(Win, IDM_Stop);

    RectExists = 0;             /* Get rid of zooming rectangle. */
    MenuDisable(Win, IDM_ZoomIn);

    WinSize.x = SHORT1FROMMP(mp2); /* Get new size in window coords. */
    WinSize.y = SHORT2FROMMP(mp2); 
                                /* Reset shodow bitmap and draw axes. */
    ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
    return (MRESULT) 1;

  case WM_PAINT: {
    POINTL Points[3];
    RECTL InvalidArea;
    HPS PS;

    PS = WinBeginPaint(Win, 0, &InvalidArea);
    GpiSetPS (PS, &dummy, PU_HIMETRIC);    

    Points[0].x = Points[0].y = Points[2].x = Points[2].y = 0; /* Lower left */
    Points[1] = WinSize;        /* Upper right. */

    DosEnterCritSec();          /* Prevent conflict over the shadow. */
    GpiErase(PS);
    GpiBitBlt(PS, ShadowBitmap.MemPS, 3, Points, ROP_SRCCOPY, BBO_OR);
    DrawAxes(PS, &G);
    if(RectExists) {            /* Has user marked a rectangle? */
      ClipToGraph(PS, &G);      
      DrawRect(PS, &Corner1, &Corner2); /* Draw the rectangle. */
      UndoClipping(PS);
    }
    DosExitCritSec();

    WinEndPaint(PS);
    return (MRESULT) 1;
  }
  case WM_DESTROY:              /* Free resources we've allocated. */
    WinSetCapture(HWND_DESKTOP, 0); /* Ungrab the mouse. */
    WinDestroyPointer(RectPointer);
    DestroyHelp(theHelpWin);
    KillMappingThread(&theControlParams);
    DestroyBitmap(&ShadowBitmap);
    WinDestroyWindow(theControlParams.PosWin); /* Get rid of point display. */
    DosCloseEventSem(theControlParams.Thread1ToThread2);
    DosCloseEventSem(theControlParams.Thread2ToThread1);
    return 0;
  case WM_BUTTON1DOWN: {        /* Start running! */
    POINTL MousePosition;
    HPS PS;
    double J, theta;

    if (ButtonDown) {           /* If button 2 is down already, puke. */
      DosBeep(1045, 100);       /* C', for 1/10 sec. */
      return (MRESULT) 0;
    }

    if (theControlParams.isRunning) { /* If we're already running, stop and */
      StopMapping(&theControlParams); /* restart. */
      MenuDisable(Win, IDM_Stop);
    }

    MousePosition.x = (short) SHORT1FROMMP(mp1);
    MousePosition.y = (short) SHORT2FROMMP(mp1);

    PS = WinGetPS(Win);         /* Get position in GPI coordinates. */
    GpiSetPS (PS, &dummy, PU_HIMETRIC);
    GpiConvert(PS, CVTC_DEVICE, CVTC_PAGE, 1L, &MousePosition);
    if (RectExists) {           /* If there's a marked rectangle, kill it. */
      RectExists = 0;
      MenuDisable(Win, IDM_ZoomIn); /* Can't do this without a rectangle. */
      ClipToGraph(PS, &G);
      DrawRect(PS, &Corner1, &Corner2); /* Erase the rectangle. */
      UndoClipping(PS);
    }
    WinReleasePS(PS);
                                /* Convert to graph coordinates. */
    toXY(MousePosition, &J, &theta, &G);
                                /* Verify that mouse was actually clicked */
                                /*  within the graph. */
    if (J     >= theRange.Min.J     && J     <= theRange.Max.J &&
        theta >= theRange.Min.theta && theta <= theRange.Max.theta) {
      MenuEnable(Win, IDM_Stop);        /* Allow user to stop the process. */
      StartingPoint.J     = J;
      StartingPoint.theta = theta;
      StartMapping(&theControlParams); /* Signal thread 2 to start. */
    }
    else
      DosBeep(1045, 100);       /* C', for 1/10 sec. */

    return WinDefWindowProc(Win, Msg, mp1, mp2);
  }
  case WM_BUTTON2DOWN: {        /* Begin processing for user to draw a zoom */
    HPS PS;			/*   rectangle. */

    ButtonDown = 1;
    WinSetCapture(HWND_DESKTOP, Win); /* Grab the mouse. */
    WinSetPointer(HWND_DESKTOP, RectPointer); /* Use box-drawing pointer. */

    StopMapping(&theControlParams); /* Signal thread 2 to stop. */
    MenuDisable(Win, IDM_Stop);      

    PS = WinGetPS(Win);         /* Get a PS to use for drawing the rect. */
    GpiSetPS (PS, &dummy, PU_HIMETRIC);
    ClipToGraph(PS, &G);

    if (RectExists)             /* Erase the old rectangle, if there is one. */
      DrawRect(PS, &Corner1, &Corner2);

    RectExists = 1;             /* We have one now, anyway. */
    MenuEnable(Win, IDM_ZoomIn);

    Corner1.x = Corner2.x = (short) SHORT1FROMMP(mp1); /* Set the corners. */
    Corner1.y = Corner2.y = (short) SHORT2FROMMP(mp1);
    GpiConvert(PS, CVTC_DEVICE, CVTC_PAGE, 1, &Corner1); /* Convert corners */
    GpiConvert(PS, CVTC_DEVICE, CVTC_PAGE, 1, &Corner2); /*  to win coords */

    DrawRect(PS, &Corner1, &Corner2); /* Draw the new rectangle. */
    UndoClipping(PS);
    WinReleasePS(PS);           /* Get rid of the PS. */
                                /* Raise window to the top. */
    WinSetWindowPos(WinQueryWindow(Win, QW_PARENT),
                    HWND_TOP,
                    0, 0, 0, 0,
                    SWP_ZORDER);
    WinSetFocus(HWND_DESKTOP, WinQueryWindow(Win, QW_PARENT));
    return (MRESULT) 1;
  }
 
  case WM_MOUSEMOVE:
    if (ButtonDown) {           /* Is user drawing a box? */
      HPS PS;
      PS = WinGetPS(Win);       /* Get a presentation space. */
      GpiSetPS (PS, &dummy, PU_HIMETRIC);
      ClipToGraph(PS, &G);

      WinSetPointer(HWND_DESKTOP, RectPointer);
      DrawRect(PS, &Corner1, &Corner2); /* Erase the old rectangle. */

      Corner2.x = (short) SHORT1FROMMP(mp1); /* Get corner 2 of new rect. */
      Corner2.y = (short) SHORT2FROMMP(mp1);
      GpiConvert(PS, CVTC_DEVICE, CVTC_PAGE, 1, &Corner2);

      DrawRect(PS, &Corner1, &Corner2); /* Draw the new rectangle. */
      UndoClipping(PS);
      WinReleasePS(PS);
      return (MRESULT) 1;
    }
    else
      return WinDefWindowProc(Win, Msg, mp1, mp2);

  case WM_BUTTON2UP:
    if (ButtonDown) {           /* Is uer drawing a box? */
      HPS PS;
      PS = WinGetPS(Win);       /* Get a presentation space. */
      GpiSetPS (PS, &dummy, PU_HIMETRIC);
      ClipToGraph(PS, &G);

      DrawRect(PS, &Corner1, &Corner2); /* Erase the old rectangle. */

      Corner2.x = (short) SHORT1FROMMP(mp1); /* Get corner 2 of new rect. */
      Corner2.y = (short) SHORT2FROMMP(mp1);
      GpiConvert(PS, CVTC_DEVICE, CVTC_PAGE, 1, &Corner2);

      DrawRect(PS, &Corner1, &Corner2); /* Draw the new rectangle. */
      UndoClipping(PS);
      WinReleasePS(PS);

      ButtonDown = 0;
      WinSetCapture(HWND_DESKTOP, 0); /* Ungrab the mouse. */
    }
    return WinDefWindowProc(Win, Msg, mp1, mp2);    

  case WM_SETFOCUS:
    if (theControlParams.isRunning) {
      StopMapping(&theControlParams);
      StartMapping(&theControlParams);
    }
    return WinDefWindowProc(Win, Msg, mp1, mp2);    
   
  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) { /* Which command was it? */

    case IDM_Stop:
      StopMapping(&theControlParams); /* Signal thread 2 to stop. */
      MenuDisable(Win, IDM_Stop);      
      return (MRESULT) 1;

    case IDM_ClearScreen:
      if (theControlParams.isRunning) 
        StopMapping(&theControlParams);

      if (ButtonDown) {         /* This is pathological. */
        DosBeep(1045, 100);     /* C', for 1/10 sec. */
        return (MRESULT) 1;
      }
      else
        RectExists = 0;

      MenuDisable(Win, IDM_ZoomIn);
      MenuDisable(Win, IDM_Stop);      
      ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
      return (MRESULT) 1;

    case IDM_Exit:
      WinSetCapture(HWND_DESKTOP, 0); /* Ungrab the mouse. */
      WinPostMsg(Win, WM_QUIT, 0, 0);
      return (MRESULT) 1;

    case IDM_SlowMotion:	/* Toggle the slow motion option. */
      if (theControlParams.isRunning) {
	StopMapping(&theControlParams);
	MenuDisable(Win, IDM_Stop);
      }
      if (theControlParams.isSlowMotion) { /* Toggle variable and uncheck */
	theControlParams.isSlowMotion = 0; /*  the menu item. */
	WinSendMsg(WinWindowFromID(WinQueryWindow(Win, QW_PARENT), FID_MENU),
		   MM_SETITEMATTR,
		   MPFROM2SHORT(IDM_SlowMotion, TRUE),
		   MPFROM2SHORT(MIA_CHECKED, 0));
      }
      else {			/* Again: toggle variable and put a check */
	theControlParams.isSlowMotion = 1; /*  mark on the menu item. */
	WinSendMsg(WinWindowFromID(WinQueryWindow(Win, QW_PARENT), FID_MENU),
		   MM_SETITEMATTR,
		   MPFROM2SHORT(IDM_SlowMotion, TRUE),
		   MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
      }
      return (MRESULT) 1;

    case IDM_DelayTime:		/* Set how slow the slow motion is. */
      if(WinDlgBox(HWND_DESKTOP, Win, DelayDlgProc, NULLHANDLE, DID_DelayTime,
                   &theControlParams.theDelay) == DLG_OK)
	if (theControlParams.isRunning && theControlParams.isSlowMotion) {
	  StopMapping(&theControlParams);
	  MenuDisable(Win, IDM_Stop);
	}
      return (MRESULT) 1;

    case IDM_Colors:            /* Change color of the plot. */
      if(WinDlgBox(HWND_DESKTOP, Win, ColorDlg, NULLHANDLE, DID_Color,
                   &theControlParams.theColor) == DLG_OK)
        if (theControlParams.isRunning) {
          StopMapping(&theControlParams);
          MenuDisable(Win, IDM_Stop);
        }
      return (MRESULT) 1;

    case IDM_SetK:              /* All we have to be careful about here */
                                /*  is making sure that nothing happens if */
                                /*  the user presses CANCEL. */
      if(WinDlgBox(HWND_DESKTOP, Win, KFactorDlg, NULLHANDLE, DID_KFactor,
                   &theControlParams.K) == DLG_OK) {
        if (theControlParams.isRunning)
          StopMapping(&theControlParams);

        MenuDisable(Win, IDM_Stop);     
        RectExists = 0;
        MenuDisable(Win, IDM_ZoomIn);
        ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
      }
      return (MRESULT) 1;

    case IDM_ZoomOut:           /* Set range to maximum possible. */
      if (theControlParams.isRunning)
        StopMapping(&theControlParams);

      if (ButtonDown) {         /* User shouldn't be issuing commands while */
        DosBeep(1045, 100);     /*  holding down a mouse button... */
        return (MRESULT) 1;
      }
      else
        RectExists = 0;         /* Clear marked rectangle. */

      MenuDisable(Win, IDM_ZoomIn);
      MenuDisable(Win, IDM_Stop);

      theRange.Min.J = theRange.Min.theta = 0; 
      theRange.Max.J = theRange.Max.theta = TwoPi;

      ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
      return (MRESULT) 1;

    case IDM_SetRange: {        /* Much the same as above. */
      if (ButtonDown) {         /* User shouldn't be issuing commands while */
        DosBeep(1045, 100);     /*  holding down a mouse button... */
        return (MRESULT) 1;
      }
      
      if (WinDlgBox(HWND_DESKTOP, Win, RangeDialog, 0, DID_Ranges, &theRange)
	  != DLG_OK)
        return (MRESULT) 1;     /* User selected CANCEL. */

      if (theControlParams.isRunning)
        StopMapping(&theControlParams);
      MenuDisable(Win, IDM_Stop);
      MenuDisable(Win, IDM_ZoomIn);
      RectExists = 0;
      ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
      return (MRESULT) 1;
    }
    case IDM_ZoomIn: {          /* Zoom to rectangle the user has chosen. */
      double x, y, x2, y2;

      if (theControlParams.isRunning)
        StopMapping(&theControlParams);
      MenuDisable(Win, IDM_Stop);
      MenuDisable(Win, IDM_ZoomIn);
      RectExists = 0;

      toXY(Corner1, &x, &y, &G);
      toXY(Corner2, &x2, &y2, &G);

      if (x == x2 || y == y2) { /* This would be bad... */
	HPS PS;

        PS = WinGetPS(Win);
        GpiSetPS (PS, &dummy, PU_HIMETRIC);
        ClipToGraph(PS, &G);
        DrawRect(PS, &Corner1, &Corner2); /* Erase the rectangle. */
        WinMessageBox(HWND_DESKTOP, Win,
                     "Error: Upper and lower bounds of the range must differ.",
                     "PM Chaos", 0,
                     MB_ERROR | MB_OK | MB_MOVEABLE);
	UndoClipping(PS);
	WinReleasePS(PS);
        return (MRESULT) 1;
      }

      theRange.Min.J     = InRange(fmin(x, x2), 0, TwoPi);
      theRange.Min.theta = InRange(fmin(y, y2), 0, TwoPi);
      theRange.Max.J     = InRange(fmax(x, x2), 0, TwoPi);
      theRange.Max.theta = InRange(fmax(y, y2), 0, TwoPi);

      ClearScreen(&ShadowBitmap, Win, &WinSize, &theRange, &G);
      return (MRESULT) 1;
    }

    case IDM_HelpAbout:
      WinDlgBox(HWND_DESKTOP, Win, AboutDlg, 0, DID_About, 0);
      return (MRESULT) 1;

    case IDM_HelpHelp:
      if (theHelpWin)
	WinSendMsg(theHelpWin, HM_DISPLAY_HELP, 0, 0);
      return (MRESULT) 1;

    default:
      return WinDefWindowProc(Win, Msg, mp1, mp2);
    }
  case HM_QUERY_KEYS_HELP:	/* Tell help system where to find keys help. */
    return (MRESULT) ID_KeysHelpPanel;

  case HM_ERROR:		/* Oops...  Something has gone wrong. */
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
		  "Warning: the on-line help system has encountered an error.",
		  "PM Chaos", 0,
		  MB_WARNING | MB_OK);
    return (MRESULT) 0;

  default:
    return WinDefWindowProc(Win, Msg, mp1, mp2);
  }
}


int main(void)
{
  HAB anchor_block = WinInitialize(0);
  HMQ Queue = WinCreateMsgQueue (anchor_block, 0);
  unsigned long StyleFlag;
  HWND Frame, Window;
  QMSG message;

  WinRegisterClass (anchor_block,
                    (unsigned char *) "Chaos",
                    ChaosWindowProc,
                    0,
                    0);

  StyleFlag =
    FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST
    | FCF_SIZEBORDER | FCF_MENU | FCF_ICON | FCF_ACCELTABLE;
  Frame = WinCreateStdWindow
    (HWND_DESKTOP,		/* Parent window. */
     WS_VISIBLE,		/* Frame window style. */
     &StyleFlag,		/* Frame creation flage. */
     (unsigned char *) "Chaos",	/* Class name. */
     "PM Chaos",		/* Text that goes in the title bar. */
     0L,			/* Window style. */
     0,				/* This means resources come from the .EXE */
     ID_MainWindow,             /* Resource ID. */
     &Window);

  do {
    while (WinGetMsg (anchor_block, &message, 0, 0, 0))
      WinDispatchMsg (anchor_block, &message);
  } while (WinMessageBox(HWND_DESKTOP, Window,
                        "Are you sure you want to exit PMChaos?", "PM Chaos", 
                        0, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2)
           != MBID_YES);

  WinDestroyWindow (Frame);
  WinDestroyMsgQueue(Queue);
  WinTerminate(anchor_block);
  return 0;
}
