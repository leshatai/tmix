#	@(#)Makefile	8.2 (Berkeley) 4/4/94
# $FreeBSD: src/libexec/ftpd/Makefile,v 1.57 2006/06/05 15:50:34 yar Exp $
# $DragonFly: src/libexec/ftpd/Makefile,v 1.3 2004/01/23 14:55:52 joerg Exp $

PROG=	tmix
MAN=	
SRCS=\
        mixer_manager.cxx \
        mixer_window.cxx \
        mixer_panel.cxx \
    	mixer_device.cxx \
        main.cxx

CC=c++
CXXFLAGS=-std=c++14 -Wall
CXXFLAGS+=-I${.CURDIR}
YFLAGS=
WARNS?=	2
WFORMAT=0

DPADD=	${LIBNCURSES}
LDADD=	-L/usr/local/lib -lncurses

#LSDIR=	../../bin/ls
#.PATH:	${.CURDIR}/${LSDIR}

#all: $(SRCS)

#.include <bsd.subdir.mk>
.include <bsd.prog.mk>
