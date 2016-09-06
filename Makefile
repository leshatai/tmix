#	@(#)Makefile	8.2 (Berkeley) 4/4/94

PROG=	tmix
MAN=	
SRCS=\
        dragonfly_manager.cxx \
        mixer_window.cxx \
        mixer_panel.cxx \
        mixer_device.cxx \
        main.cxx

CC=c++
CXX=c++
.ifdef CLANG
CC=clang++38
CXX=clang++38
.endif

CXXFLAGS=-std=c++14 -Wall
CXXFLAGS+=-I${.CURDIR}
YFLAGS=
WARNS?=	2
WFORMAT=0

.ifdef DEBUG
CXXFLAGS += -g
.else
CXXFLAGS += -O3
.endif

DPADD=	${LIBNCURSES}
LDADD=	-L/usr/local/lib -lncurses

#LSDIR=	../../bin/ls
#.PATH:	${.CURDIR}/${LSDIR}

#all: $(SRCS)

#.include <bsd.subdir.mk>
.include <bsd.prog.mk>
