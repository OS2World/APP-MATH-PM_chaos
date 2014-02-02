# OPTS = -q -gm -O -gf -g4 -Oi			# Full optimization
OPTS = -q -gm -Ti -gd -Wall+ -Wppt- -Wuni-	# For debugging.

all: pmchaos.exe pmchaos.hlp

pmchaos.hlp: pmchaos.ipf
       ipfc pmchaos.ipf

pmchaos.exe: pmchaos.obj graph.obj pmchaos.res makefile
       icc -B"/pm:pm" -B"/stack:5000000" $(OPTS)  pmchaos.obj graph.obj
       rc pmchaos.res pmchaos.exe

pmchaos.res: pmchaos.rc pmchaos_res.h \
	     color.dlg kfactor.dlg ranges.dlg about.dlg curpos.dlg delay.dlg \
	     color.h kfactor.h ranges.h about.h curpos.h delay.h
       rc -r pmchaos.rc

pmchaos.obj: pmchaos.c pmchaos_res.h \
	     color.h ranges.h kfactor.h about.h curpos.h delay.h \
	     graph.h inline.h \
	     makefile
       icc -c $(OPTS) pmchaos.c

graph.obj: graph.c graph.h inline.h makefile
       icc -c $(OPTS) graph.c