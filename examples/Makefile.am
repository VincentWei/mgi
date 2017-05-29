INCLUDES=-I../include

COMM_PROGRAM=phonetest switchtest sktest hwtest  skb_demo

if MGRM_PROCESSES
PROC_PROGRAM=mginit
else
PROC_PROGRAM=
endif

noinst_PROGRAMS=$(COMM_PROGRAM) $(PROC_PROGRAM)

switchtest_SOURCES=switchtest.c
sktest_SOURCES=sktest.c
hwtest_SOURCES=hwtest.c
phonetest_SOURCES=mgphone_edit.c mgphonetest.c pen.c mgphonetest.h
skb_demo_SOURCES=skb_demo.c

switchtest_LDADD=../src/libmgi.la @APP_LIBS@ -ljpeg -lpng
sktest_LDADD=../src/libmgi.la @APP_LIBS@ -ljpeg -lpng
hwtest_LDADD=../src/libmgi.la @APP_LIBS@ -ljpeg -lpng
phonetest_LDADD=../src/libmgi.la @APP_LIBS@ -ljpeg -lpng
skb_demo_LDADD=../src/libmgi.la @APP_LIBS@ -ljpeg -lpng

if MGRM_PROCESSES
mginit_SOURCES=mginit.c mgphonetest.c
mginit_LDADD=../src/libmgi.la @APP_LIBS@
endif
