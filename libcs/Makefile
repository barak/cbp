OFILES= abspath.o an_bsearch.o ansi_bsearch.o atoh.o atoo.o atot.o \
	boolarg.o bsearch.o chrarg.o ci.o concat.o cs_bsearch.o del.o \
	dfork.o doublearg.o editor.o errmsg.o expand.o fdate.o \
	ffilecopy.o fgetpass.o filecopy.o floatarg.o fold.o \
	fwantread.o fwantwrite.o getbool.o getchr.o getdouble.o \
	getfloat.o gethex.o gethostattr.o getint.o getlong.o getname.o \
	getoct.o getpwwho.o getsearch.o getshort.o getstab.o getstr.o \
	gtime.o hexarg.o intarg.o longarg.o makepath.o movefile.o \
	nxtarg.o octarg.o openp.o parsedate.o path.o pathof.o prstab.o \
	quit.o run.o salloc.o searcharg.o searchp.o setpath.o \
	shortarg.o sindex.o skipto.o srchscore.o stabarg.o stablk.o \
	stabsearch.o stlmatch.o strarg.o vmnlist.o vprintf.o \
	wantread.o wantwrite.o

CPPFLAGS += -Iinclude

all: libcs.a($(OFILES))

clean:
	-rm -f $(OFILES)
	-rm -f parsedate.c
	-rm -f libcs.a

## The old Makefile had this:
# parsedate.c: parsedate.y parsedate.scr
#	${YACC} ${_YFLAGS_} $*.y
#	ed - y.tab.c < parsedate.scr
#	mv y.tab.c $*.c
