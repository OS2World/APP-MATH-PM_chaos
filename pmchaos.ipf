.*  Help file for PMChaos.     (C) Copyright Matthew Austern, 1993.
.*  
.*  Permission granted to use, distribute, and modify, provided that this
.*  notice remains intact.  If you distribute a modified version, you must
.*  identify your modifications as such.
.*
.*
:userdoc.
.***************************************************
.* This is the introductory panel.
.***************************************************
:h1 res=1. PM Chaos: Extended Help
.*
:p.
:i1 id=calc.calculation
:hp4.PM Chaos:ehp4. is a simulation of a simple chaotic system: the
:hp4.standard mapping:ehp4..  This is a simple function that takes a
point (J,theta) into another point (J',theta').  If you keep applying
this function again and again, you get a sequence of points.  For certain 
initial conditions this sequence is a smooth trajectory, and for other 
initial conditions it is chaotic.
:p.
To start a calculation, click the left mouse button anywhere within the
box of the graph.  PM Chaos will begin calculating the sequence of 
points starting with the initial point you have chosen.  The calculation
continues until you stop it, either by clicking the left mouse button
somewhere else (this will stop the first calculation and start a new
one), by selecting :hp2.Stop!:ehp2. from the :hp2.Control:ehp2. menu,
or by clearing the screen.
:p.:hp4.Related information: :ehp4.
:ul.
:li.:link reftype=hd res=100.  The standard mapping :elink.
:li.:link reftype=hd res=200.  Using the program :elink.
:eul.
.***************************************************
.* Here is the section that describes the physics.
.***************************************************
:h1 res=100.  Physics of the standard mapping
:i1. phase space
:i1. trajectory
:i1. Hamiltonian
:i1 id=Map.mapping
:i2 refid=Map.area-preserving
:p.
The standard mapping is a simplified set of equations that has many of the
same qualitative topological properties as do actual physical systems
in classical mechanics.
:p.
The evolution of a classical mechanical system is given by a set of
differential equations determined by the Hamiltonian.  
It is numerically easier, however, to work with a difference equation,
that is, one that doesn't evolve continuously, but where, given
a point, there is a function that just gives you the next point in
the sequence.
:p.
If you don't know much classical mechanics, you may want to skip the
next three paragraphs.  You don't need to understand them in order to
use the program.
:p.
Consider a time-independent Hamiltonian with two degrees of freedom,
i.e., two canonical coordinates and two canonical momenta.  The state
of the system is represented by a point in the four-dimensional
:hp4.phase space:ehp4., and the time evolution of the system from some
initial state is a trajectory in phase space.  We can choose some
arbitrary two-dimensional surface in this phase space, a 
:hp4.surface of section:ehp4., and study the successive intersections of a
trajectory with that surface.  This gives us a set of difference
equations on a two-dimensional reduced phase space.
:p.
A time-independent Hamiltonian leads to a conserved energy; this means
that a trajectory must lie in some three-dimensional subspace of the
original four-dimensional phase space.  It also means that the 
mapping given by the surface of section (a canonical transformation)
is area-preserving, that is, that any region of the two-dimensional
reduced phase space gets mapped to a region of equal area.  If the
Hamiltonian is :hp4.integrable:ehp4.
(i.e., if there exists a constant of the motion other than the total
energy), then motion is actually in a two-dimensional subspace of
phase space.  In that case, the successive points in the surface of
section lie on closed curves in the reduced phase space.
:p.
This is most easily seen in angle-action variables.  In that case, the
mapping equation for an integrable system is 
:font facename=Courier.
:lines align=left.
          J'     = J
          theta' = theta + f J.
:elines.
:font facename=default.
The standard mapping is very similar to this: it is a near-integrable
area-preserving mapping in angle-action coordinates.
.*
.* End of this panel.  Now some links...
.*
:ul.
:li.:link reftype=hd res=101.  Equations of the standard mapping :elink.
:li.:link reftype=hd res=104.  Where to learn more :elink.
:eul.
.***************************************************
.* Here is the panel with the actual equations.
.***************************************************
:h1 res=101. The standard mapping
:i1. J
:i1. theta
:i1 id=K.K
:i2 refid=K.defining equation of
:i2 refid=Map.standard
:i2 refid=calc.equations of
:p.
The standard mapping is given by the equations
:font facename=Courier.
:lines align=left.
          J'     = J + K sin(theta)
          theta' = theta + J', mod(2 pi).
:elines.
:font facename=default.
:p.
It is defined for J and theta in the range (0, 2 pi).
:ul.
:li.:link reftype=hd res=102.  Significance of K :elink.
:eul.
.***************************************************
.* This panel defines what K means.
.***************************************************
:h1 res=102. The K factor
:i2 refid=K.significance of
:i1. stochasticity
:i1. fixed points
:i1. separatrices
:p.
K is the stochasticity parameter.  For K=0, the standard mapping is
integrable, and the action variable, J, is a constant of the motion.
For K>0 there are some regions of stochasticity, but they are isolated
when K is small, and J is nearly constant for many trajectories.  For
any K<1 there are separatrices dividing the screen into at least two
regions.  That is: if the motion begins near J=0, it will never end up
near J=2 pi.
:p.
Motion becomes globally stochastic at
:link refid=transition reftype=fn.K=1:elink..
At this point, the last separatrix disappears,
and there is nothing separating the J<pi and J>pi regions.
:fn id=transition.
Actually, the transition isn't exactly at K=1.  The approximate value is
K=0.9716.  See 
A.J. Lichtenberg and M.A. Lieberman, 
:hp1. Regular and Stochastic Motion, :ehp1.
for more details.
:efn.
.*
.*
:p.
For any value of K there are still some :hp4.islets of stability:ehp4.,
but they become more and more isolated, and motion becomes more and 
more chaotic, as K increases.  Note that there are always at least two
fixed points: (J,theta) = (0,pi), and (J,theta) = (2pi,pi).  For
K<4, these fixed points are stable.
:p.
This program limits K to the range 0<K<3, because motion with larger
values of K is too disordered to be very interesting.
.*
.* End of the K panel.  Now some links.
.*
:p.
:hp4.Related information: :ehp4.
:ul.
:li.:link reftype=hd res=220.  Setting K :elink.
:li.:link reftype=hd res=101.  Equations of the standard mapping :elink.
:li.:link reftype=hd res=100.  Discussion of the physics :elink.
:eul.
.***************************************************
.* Here is the panel with the bibliography.
.***************************************************
:h1 res=104. References
:i1. references
:i1. Lichtenberg, A. J.
:i1. Lieberman, M. A.
:i1. Goldstein, H.
:p.
The standard mapping is Equation 4.1.3 of 
:lm margin=8.
:rm margin=7.
:font facename='Tms Rmn'.
:p.
A. J. Lichtenberg and M. A. Lieberman, 
:hp1. Regular and Stochastic Motion, :ehp1.
New York: Springer-Verlag, 1983.
:lm margin=1.
:rm margin=1.
:font facename=default.
:p.
Section 4.1b describes the standard mapping in detail; chapter 3 discusses
mappings in general.  This book is somewhat advanced, and assumes that you
already know about Hamiltonian dynamics, angle-action variables, and
so on.  If you don't, the standard reference is 
:lm margin=8.
:rm margin=7.
:font facename='Tms Rmn'.
:p.
H. Goldstein,
:hp1. Classical Mechanics (second edition), :ehp1.
Reading: Addison-Wesley, 1980.
:lm margin=1.
:rm margin=1.
:font facename=default.
:p.
.*************************************************************
.*************************************************************
.**      And here is the actual program documentation.      **
.*************************************************************
.*************************************************************
:h1 res=200. Using PM Chaos
:i2 refid=calc.starting a
:i1. changing window size
:p.
At the most basic level, using PM Chaos is very simple.  Just click
the left mouse button anywhere within the graph on the main screen.
PM Chaos will use the point you have selected as the initial conditions
for a calculation.  It will then begin calculating a sequence of points
and displaying them on the screen; each point is determined from the last
by a simple :link reftype=hd res=101.equation:elink..
If you click the left mouse button in the graph while a calculation is
in progress, it will stop the old calculation and start a new one.
:p.
The :hp2.Control:ehp2. menu contains
a command to stop a calculation that is in progress, a
command to clear the screen (if there is a calculation in progress,
clearing the screen will stop it), and a command to quit the program.
You can also quit the program by selecting :hp2.Close:ehp2.
from the system menu.
:p.
Normally, the points get displayed very quickly.  In 
:link reftype=hd res=240. slow motion mode:elink., however,
the points are display slowly enough so that you can see the
progress of the calculation explicitly.
:p.
You can resize PM Chaos's window in any of the normal ways that 
you can resize any 
Presentation Manager:link refid=TM reftype=fn.**:elink. window:
either by using the sizing border, or by using the minimize
and maximize buttons at the upper right-hand corner of the window,
or by selecting :hp2.Size:ehp2. from the system menu.
.*
:fn id=TM.
Presentation Manager is a trademark of the IBM Corporation.
:efn.
.*
:p.
That's really all you need to know to use PM Chaos.  There are a number
of additional options that you can select, though.
:ul.
:li.:link reftype=hd res=210.  Choosing the color :elink.
:li.:link reftype=hd res=220.  Setting K :elink.
:li.:link reftype=hd res=230.  Changing the range of the graph :elink.
:eul.
.*
.*
.***************************************************
.* Here's the panel on using the color dialog.
.***************************************************
:h1 res=210.  Choosing the color
:i1. color
:p.
You can change the color that PM Chaos will use to display results.  You
can mix different colors on the same plot; that is, you can display the
results of a calculation in one color, and the results of the next 
calculation in a different color.  This is a convenient way to distinguish
trajectories that lie close to each other.
:p.
You choose the color by using the color dialog, which you get to by selecting
:hp2.Colors:ehp2. from the :hp2.Options:ehp2. menu.
:p.
The color dialog displays sixteen different colors, each of which has
a button next to it.  Click on the color that you would like to use
for the next calculation.
:p.
When you click on the :hp2.OK:ehp2. button, the color for the next
calculation will be changed.  If there is already a calculation in
progress, it will stop.  If you click on the :hp2.Cancel:ehp2.
button, then the current color will not be changed, and the
calculation in progress (if any) won't be stopped.
.*
.*
.***************************************************
.* Here's the help panel for the K factor dialog.
.***************************************************
:h1 res=220. Setting K
:i2 refid=K.setting
:p.
The :link reftype=hd res=101.equation:elink. for the standard mapping 
depends on the :link reftype=hd res=102.K:elink. factor,
which deterines the degree of stochasticity.  Essentially, the
larger K is, the more chaotic is the behavior of the system.  
If K=0, the system is completely regular.  If K is much larger than 1,
the system is so chaotic that it is no longer very interesting.
:p.
You can change the value of K by using the K factor dialog, which you
get to by selecting :hp2.Set K:ehp2. from the :hp2.Options:ehp2. menu.
:p.
This dialog box uses a slider.  You can change the value of K by dragging
the slider arm; you can also use the buttons to the left of the slider,
or you can use the arrow keys on the keyboard.  The value you have selected
is displayed just above the slider control.
:p.
When you click on the :hp2.OK:ehp2. button, the value of K will be
changed, and the screen will be cleared.  If you click on the
:hp2.Cancel:ehp2. button, then the current value of K will remain
unchanged, and the calculation in progress (if any) won't be stopped.
.*
.*
.***************************************************
.* Here's the general panel for setting the range.
.***************************************************
:h1 res=230. Setting the range
:i1 id=range.range
:i1 id=zoom.zooming
:p.
You can change the range of the graph, so that you can look more closely
at some particular part of it.  There are three ways to change the range.
No matter which method you use, however, changing the range will stop 
any calculation that may be in progress, and will clear the screen.
:ul.
:li.:link reftype=hd res=231. Zooming out :elink.
:li.:link reftype=hd res=232. Zooming in :elink.
:li.:link reftype=hd res=233. Using the range dialog :elink.
:eul.
.*
.***************************************************
.* Using the zoom out option.
.***************************************************
:h1 res=231. Zooming out
:i2 refid=range.zooming out
:i2 refid=zoom.out
:p.
If you select :hp2.Zoom Out:ehp2. from the :hp2.Range:ehp2.
menu, then the graph will be set to the maximum possible range: both
axes will go from 0 to 2 pi.
:p.
Doing this will clear the screen; if there is a calculation in progress, 
it will be stopped.
.*
.***************************************************
.* Using the zoom out option.
.***************************************************
:h1 res=232. Zooming in
:i2 refid=range.zooming in
:i1. zoom rectangle
:i2 refid=zoom.in
:p.
To zoom in to a particular region of the graph, you must first define a
:hp2.zoom rectangle:ehp2..  You do this with the right mouse button.
:p.
Move the mouse pointer to somewhere within the graph, and then move
the mouse while holding down the right mouse button.  You will see a
rectangle on the screen: one corner of it will be at the point where you
first began to hold down the right mouse button, and the other will be
at the current posigion of the mouse.  The rectangle will continue to
move around as long as you're holding down the right mouse button.
:p.
When you release the right mouse button, the zoom rectangle will
remain in position.  If you select :hp2.Zoom In:ehp2.  from the
:hp2.Range:ehp2. menu, then the range of the graph will be set to
be the region outlined by the zoom rectangle.  Doing this will clear
the screen; if there is a calculation in progress, it will be stopped.
:p.
The zoom rectangle will be cleared if you start a calculation, if you
clear the screen, or if you press the right mouse button to define a 
different zoom rectangle.
.*
.***************************************************
.*
:h1 res=233. Using the range dialog
:i2 refid=range.dialog box
:p.
The simplest way to set the range is usually with the :link reftype=hd
res=231.Zoom Out:elink.  or the :link reftype=hd res=232.Zoom In:elink.
options.  If you want to type the ranges in explicitly,
though, the range dialog allows you to do that.  You get to the range
dialog by selecting :hp2.Set Ranges:ehp2.  from the :hp2.Range:ehp2.
menu.
:p.
The range dialog has four entry fields: the minimum and maximum values
of the range for each of the two coordinate axes.  You can type in a
number in each of those entry fields.
:p.
For each axis, the minimum value must be at least 0 and the maximum must
be no greater than 2pi.  Also, of course, the maximum must be
greater than the minimum...
:p.
When you click on the :hp2.OK:ehp2. button, the range will be
changed to the value you have typed in, and the screen will be
cleared.  If you click on the :hp2.Cancel:ehp2.  button, then the
range will remain unchanged, and the calculation in progress (if any)
won't be stopped.
.*
.* The next two panels deal with slow motion mode.
.*
:h1 res=240. Slow motion mode
:i1 id=Slow.Slow motion mode
:p.
In slow motion mode, a calculation is done slowly enough so that you
can see the points calculated one at a time.  This lets you see, 
explicitly, how the mapping function takes one point into another.
:p.
When a calculation is in progress in slow motion mode, the current point
is displayed prominently in the graph window, and its coordinates are
displayed in a separate window.
:p.
You can :link reftype=hd res=241.set the speed:elink. of 
slow motion mode by selecting the 
:hp2.Delay Time:ehp2. item from the Options menu.
:p.
You can turn slow motion mode on or off by selecting the 
:hp2.Slow Motion:ehp2. item from the Options menu.  If a calculation
is in progress when you turn slow motion mode on or off, it will
be stopped.
.*
.*
:h1 res=241. Setting the slow motion speed
:i2 refid=Slow.Setting speed
:p.
In slow motion mode, PM Chaos pauses for a specified length of time
after displaying each new point.  Selecting the 
:hp2.Delay Time:ehp2. item from the Options menu will display
a dialog box that allows you to set that length of time.
This delay time has no effect unless you are in slow motion mode.
:p.
The delay time is specified in units of 1/10 seconds.  So, for example,
if you want a delay of 1 second, you type in a delay time of 10.
The delay time must be a non-negative integer.  You may specify a delay
time of 0, but doing that isn't very useful.
:p.
When you click on the :hp2.OK:ehp2. button, the delay time will be
changed.  If there is a calculation in progress, and if it is using
slow motion mode, it will be stopped.  If you click on the 
:hp2.Cancel:ehp2. button, then the delay time will remain unchanged.
.*
.*************************************************************
.*************************************************************
.** That's pretty much it.  What remains is a bit of fluff: **
.**  key assignments, brief descriptions of menu items, and **
.**  so on.                                                 **
.*************************************************************
.*************************************************************
:h1 res=300. The Control menu
:p.
The :hp2.Control:ehp2. menu contains the following options:
:ul.
:li.:link reftype=hd res=301. Stop!        :elink.
:li.:link reftype=hd res=302. Clear Screen :elink.
:li.:link reftype=hd res=303. Exit         :elink.
:eul.
.***********
:h1 res=301. Stop!
:i2 refid=calc.stopping a
:p.
This menu option stops the calculation that is in progress.  If there isn't
any calculation in progress, this option is disabled.
:p.
Another way to stop a calculation in progress, without using the menu,
is to click the mouse somewhere :hp1.outside:ehp1. the borders of the graph.
.***********
:h1 res=302. Clear Screen
:p.
This option clears the screen.  If a calculation is in progress, it is
stopped.
.***********
:h1 res=303. Exit
:p.
Use this option to quit PM Chaos.  When you select it, the program will
display a dialog box asking if you really want to quit; click on the
:hp2.Yes:ehp2. button if you do.
:p.
You can also quit PM Chaos by selecting :hp2.Close:ehp2. from the system menu.
.*
.***************************************************
.*
:h1 res=310. The Options menu
:p.
The :hp2.Options:ehp2. menu contains the following choices:
:ul.
:li.:link reftype=hd res=311. Slow Motion:elink.
:li.:link reftype=hd res=241. Delay Time:elink.
:li.:link reftype=hd res=210. Colors:elink.
:li.:link reftype=hd res=220. Set K :elink.
:eul.
.*
.*
.*
:h1 res=311. Slow Motion
:p.
:i2 refid=Slow.toggling
Use this option to turn :link reftype=hd res=240.slow motion mode:elink. on
or off.
:p.
If there is a check mark next to this menu item, that means that 
slow motion mode has been selected.  If there is no check mark, then
slow motion mode has not been selected.
:p.
If a calculation is in progress, turning slow motion mode on or off will
end it.
.*
.***************************************************
.*
:h1 res=320. The Range menu
:p. 
The :hp2.Range:ehp2. menu contains the following choices:
:ul.
:li.:link reftype=hd res=232. Zoom In    :elink.
:li.:link reftype=hd res=231. Zoom Out   :elink.
:li.:link reftype=hd res=233. Set Ranges :elink.
:eul.
:p.:hp4.Related information: :ehp4.:p.
:link reftype=hd res=230. General comments about the range. :elink.
.*
.*************************************************************
.**                Finally, key assignments.                **
.*************************************************************
.*
:h1 res=400. Key assignments
:p.
For on-line help, and for tasks like switching between windows,
PM Chaos uses the same keys as any other Presentation 
Manager:link refid=TM reftype=fn.**:elink. program.  Additionally,
PM Chaos defines some accelerator keys of its own for commonly used 
tasks.
:fn id=TM.
Presentation Manager is a trademark of the IBM Corporation.
:efn.
.*
:ul.
:li.:link reftype=hd res=401. PM Chaos keys          :elink.
:li.:link reftype=hd res=402. Help keys              :elink.
:li.:link reftype=hd res=403. System and window keys :elink.
:eul.
.*
.***************************************************
.*
:h1 res=401. PM Chaos key assignments
.*:lines align=center.
.*:hp4. PM Chaos keys: :ehp4.
.*:elines.
.*
:p.
:dl tsize=8 break=none compact.
:dthd.:hp7.Key    :ehp7.
:ddhd.:hp7.Action :ehp7.
:dt. Esc
:dd. Stop!
:dt. Alt+C
:dd. Clear Screen
:dt. Alt+X
:dd. Exit
:dt. C
:dd. Colors
:dt. K
:dd. Set K
:dt. <
:dd. Zoom In
:dt. >
:dd. Zoom Out
:dt. R
:dd. Set Ranges
:edl.
.*
.***************************************************
.*
:h1 res=402. On-line help key assignments
:p.
.*:lines align=center.
.*:hp4. Help keys :ehp4.
.*:elines.
.*
:dl tsize=12 break=none compact.
:dthd.:hp7.Key    :ehp7.
:ddhd.:hp7.Action :ehp7.
:dt. F1
:dd. Get help
:dt. F2
:dd. Extended help (from within help)
:dt. F9
:dd. Keys help (from within help)
:dt. F11
:dd. Help index (from within help)
:dt. Shift+F10
:dd. Using help (from within help)

:edl.
.*
.***************************************************
.*
:h1 res=403. System and window key assignments
.*:lines align=center.
.*:hp4. System and window keys :ehp4.
.*:elines.
:p.
:dl tsize=5 break=all.
.*:dthd.:hp4.Key    :ehp4.
.*:ddhd.:hp4.Action :ehp4.
:dt. Alt+F9
:dd. Minimize the window
:dt. Alt+F10
:dd. Maximize the window
:dt. Alt+Esc
:dd. Switch to the next program
:dt. Ctrl+Esc
:dd. Switch to the Task List
:dt. Shift+Esc or Alt+Spacebar
:dd. Go to or from the system menu
:dt. F10 or Alt
:dd. Go to or from the action bar
:dt. Underlined letter
:dd. Move among the choices on the action bar and pull-down menus
:dt. Arrow keys
:dd. Move among the choices on the action bar and pull-down menus
:edl.
.*
.*
.*
:index.
:euserdoc.
.*
.* The document per se has now ended.
.* This last little bit of gunk is for the benefit of Emacs. 
.*
.* Local Variables:
.* mode: text
.* comment-column: 0
.* comment-start: ".*"
.* comment-start-skip: "^\\.\\*"
.* comment-end: ""
.* End:

