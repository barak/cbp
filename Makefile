## Try to do things right on various machines. ##

all: scbp cbp

# -DUSE_DOUBLES for double precision.
# -f for single precision (for incompatibility, -fd on the convex)
#
#VAX_IFLAGS	= # -I/usr/cs/include
#VAX_LIBCS	= # /usr/cs/lib/libcs.a
#VAX_OFLAGS	= -O -s # -wa
#VAX_CC		= cc # tcc
#
#SUN_IFLAGS	= 
#SUN_LIBCS	= 
#SUN_OFLAGS	= -O -s -f
#SUN_CC		= cc
#
#IBMRT_IFLAGS	= 
#IBMRT_LIBCS	= 
#IBMRT_OFLAGS	= -O -s -f
#IBMRT_CC	= cc
#
#mmax_IFLAGS     =
#mmax_LIBCS      =
#mmax_OFLAGS     = -O -q optimize=time -q fpa -q novolatile
#mmax_CC         = cc
#
#PMAX_IFLAGS     =
#PMAX_LIBCS      =
#PMAX_OFLAGS     = -O3
#PMAX_CC         = cc
#
#PMAX_EXTRALIB	= /usr/lib/libc.a
#
#CONVEX_IFLAGS	= -Ilibcs/include
#CONVEX_LIBCS	= libcs/libcs.a
#CONVEX_OFLAGS	= -O2 -uo -va -fd
#CONVEX_CC	= vc
#
#SUN4_IFLAGS	= -I../../include
#SUN4_LIBCS	= ../../lib/libcs.a
#SUN4_OFLAGS	= -O
#SUN4_CC		= gcc
#
#NeXT_IFLAGS	= -I../libcs/include -I../../include
#NeXT_LIBCS	= ../../lib/libcs.a
#NeXT_OFLAGS	= -O
#NeXT_CC		= cc
#
#MACHINE = NeXT
#
#EXTRALIB = ${$(MACHINE)_EXTRALIB}
#
#IFLAGS = ${$(MACHINE)_IFLAGS}
#OFLAGS = ${$(MACHINE)_OFLAGS}
#CC = ${$(MACHINE)_CC}
#LIBCS = ${$(MACHINE)_LIBCS}

##  !CMUCS  ##
#$(LIBCS):
#	cd libcs; make libcs.a

IFLAGS	= -I../libcs/include -I../../include
LIBCS	= ../../lib/libcs.a
OFLAGS	= -g
CC	= gcc

##  Common  ##

CFLAGS = $(OFLAGS) $(IFLAGS)
LDFLAGS = -lm

cbp: cbp.c cbp.h $(LIBCS)
	$(CC)               $(CFLAGS) cbp.c $(LIBCS) -o cbp $(LDFLAGS) $(EXTRALIB)
scbp: cbp.c cbp.h $(LIBCS)
	$(CC) -DUSE_SINGLES $(CFLAGS) cbp.c $(LIBCS) -o scbp $(LDFLAGS) $(EXTRALIB)
