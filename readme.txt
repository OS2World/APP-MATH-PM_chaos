			     PM Chaos 1.1
		      Matthew Austern, 12/12/93
		     (matt@physics.berkeley.edu)

PM Chaos is a program that simulates a simple chaotic system, the
"standard map".  This system is described by a very simple equation,
but it exhibits quite complicated behavior.


SYSTEM REQUIREMENTS: OS/2 2.x is required.  A mouse is required.  A color
  display, and a math coprocessor, are recommended.


INSTALLATION: just copy pmchaos.exe to some directory on your path, and
  copy pmchaos.hlp to some directory that's on your help path.  If you like,
  you can also create a program object for PM Chaos so that you can put
  it somewhere on your desktop.


DEINSTALLATION: just delete pmchaos.exe and pmchaos.hlp.  PM Chaos doesn't 
  create any hidden files, and it doesn't make changes to any of your
  system files.


HOW TO USE: start PM Chaos, either from the WPS or from the command line.
  You will see a box with labelled axes on it.  The way the program works
  is that you start a calculation using some point as the initial condition.
  The program has an equation that maps a point into a different point;
  it keeps applying that map over and over, displaying every point it
  comes up with.  To start a calculation at some point, just click the
  left mouse button anywhere on the graph.

  There are also options for changing the color, setting the range, etc.
  These are all explained in the online help system.


SOURCE CODE: I have included the complete source code for PM Chaos in
  the file source.zip.  If you have IBM's compiler, you shouldn't have
  to do anything more than just typing "make".  If you have a different
  compiler, you shouldn't have all that much trouble anyway: this
  program is written in C, and I don't use any vendor-specific language
  extensions.  You will, however, need the OS/2 2.x toolkit, or the
  equivalent.  In particular, you will need the resource compiler (rc),
  the help file compiler (ipfc), and the declarations for Presentation
  Manager.  If you want to make any changes to the code, you will probably
  also want the bitmap editor and the dialog editor.

CHANGES FROM VERSION 1.0:
  Fixed a bug in the color dialog box that affected systems with only
  16 colors.  Added slow-motion mode.  Added axis labels.

LEGAL STUFF: This program is not public domain.  However, you are
  welcome to use it, copy it, and distribute it, either exactly as is or
  in a modified form.  If you distribute a modified copy, I ask that you
  (1) Leave my notices of authorship and copyright intact; and (2) Put
  in your own notice saying that you have made modifications, and
  telling what those modifications are.

  OS/2, Presentation Manager, and Workplace Shell are trademarks of IBM.

