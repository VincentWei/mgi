INCLUDES=-I../include
CC=gcc
CFLAGS=-Wall -fPIC -O2 
LDFLAGS= ../src/libmgime.a
AR=ar
OBJECT=mgpti.o \
       mgpti_dict.o \
	   mgpti_dict_ops.o \
	   mgpti_helper.o \
	   mgpti_user_dict.o \
	   ime_pinyin.o

.SILENT: 

#keyboard.o:keyboard.c
#	${CC} -c $^ 
#hzime.o:hzime.c
#	${CC} -c $^

libmgmethod.a:${OBJECT}
	echo                                           
	echo =========================================
	echo "Generating libmgmethod(-lmgmethod) library..."
	echo =========================================
	${AR} -rsv $@ ${LDFLAGS} $^ 

.c.o:
	echo -n Compiling $< ... 
	#${CC} ${CFLAGS} ${LDFLAGS}  ${INCLUDES} -c $<
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	echo \ ok

clean:
	echo                                           
	echo "Cleaning the compiled files of libmgmethod..."
	echo                                           
	rm -f *.o ${OBJECT} *.a


