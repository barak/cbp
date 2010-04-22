#
# If you change this Makefile, update relevant parts of ../libc/Makefile
#
######################################################################
# HISTORY
# $Log:	Makefile,v $
# Revision 1.11  92/08/26  10:24:18  moore
# 	Rename ansi_bsearch.o to an_bsearch.o so that ar and ranlib wouldn't
# 	have to truncate the name.
# 	[92/08/26  10:22:42  moore]
# 
# Revision 1.10  92/05/19  14:58:35  neek
# 	Added ansi_bsearch().
# 	[92/05/19  14:57:03  neek]
# 
# Revision 1.9  92/05/18  16:38:15  neek
# 	changed name of bsearch.o -> cs_bsearch.o.
# 	[92/05/18            neek]
# 
# Revision 1.8  90/12/11  18:06:05  mja
# 	Use ${MAKETOP}.
# 
# Revision 1.7  88/12/13  13:50:25  gm0w
# 	Added concat, makepath and vprintf modules.
# 	[88/12/13            gm0w]
# 
# Revision 1.6  88/11/08  20:43:53  gm0w
# 	Created for new and improved SSP format.
# 	[88/10/23            gm0w]
# 
######################################################################

IDIR			= /usr/cs/lib/

LIBRARIES		= libcs.a
OFILES			= abspath.o an_bsearch.o atoh.o atoo.o atot.o boolarg.o\
			  cs_bsearch.o chrarg.o ci.o concat.o del.o\
			  dfork.o doublearg.o editor.o errmsg.o\
			  expand.o fdate.o ffilecopy.o fgetpass.o\
			  filecopy.o floatarg.o fold.o fwantread.o\
			  fwantwrite.o getbool.o getchr.o getdouble.o\
			  getfloat.o gethex.o gethostattr.o getint.o\
			  getlong.o getname.o getoct.o getpwwho.o\
			  getsearch.o getshort.o getstab.o getstr.o\
			  gtime.o hexarg.o intarg.o longarg.o\
			  makepath.o movefile.o nxtarg.o octarg.o\
			  openp.o parsedate.o path.o pathof.o prstab.o\
			  quit.o run.o salloc.o searcharg.o searchp.o\
			  setpath.o shortarg.o sindex.o skipto.o\
			  srchscore.o stabarg.o stablk.o stabsearch.o\
			  stlmatch.o strarg.o vmnlist.o vprintf.o\
			  wantread.o wantwrite.o

IMODE			= 644
GARBAGE			= parsedate.c

include ${MAKETOP}Makefile-common

#
# If you change this rule, update ../libc/Makefile
#
parsedate.c: parsedate.y parsedate.scr
	${YACC} ${_YFLAGS_} $*.y
	ed - y.tab.c < parsedate.scr
	mv y.tab.c $*.c
