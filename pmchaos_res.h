/*  PMChaos.   (C) Copyright Matthew Austern, 1993.

    Permission granted to use, distribute, and modify, provided that this
    notice remains intact.  If you distribute a modified version, you must
    identify your modifications as such.
*/

#define ID_MainWindow 1
#define ID_RectPtr    2

#define IDM_Control     10
#define IDM_Stop        12
#define IDM_ClearScreen 14
#define IDM_Exit        15

#define IDM_Options     20
#define IDM_Colors      21
#define IDM_SetK        22
#define IDM_SlowMotion  23
#define IDM_DelayTime   24

#define IDM_Range       30
#define IDM_SetRange    31
#define IDM_ZoomOut     32
#define IDM_ZoomIn      33

#define IDM_Help        40
#define IDM_HelpHelp    41
#define IDM_GeneralHelp 42
#define IDM_KeysHelp    43
#define IDM_HelpIndex   44
#define IDM_HelpAbout   45

#define ID_MainHelpTable  1000
#define ID_MenuHelpTable  1001
#define ID_KHelpTable     1002
#define ID_ColorHelpTable 1003
#define ID_RangeHelpTable 1004
#define ID_DelayHelpTable 1005
#define ID_KeysHelpPanel  400

#define FEF_Enter (WM_USER + 0)
#define WM_UpdatePoint (WM_USER + 1)
#define DLG_OK 1
