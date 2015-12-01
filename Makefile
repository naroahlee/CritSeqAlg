INCLUDE_DIR := ./inc
SRC_PATH := ./src
CFLAG = -I${INCLUDE_DIR}
CC = /usr/bin/gcc
LDLIBS = -lm

vpath %.c ${SRC_PATH}


.PHONY: all clean install
ALL = main main2 main3

all: ${ALL}

clean:
	rm -rf ${ALL} *.o *.dat

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

main: main.o utility.o alg_bruteforce.o alg_headtail.o alg_dinsert.o
	${CC} ${CFLAG} $^ -o $@

main2: main2.o utility.o alg_bruteforce.o alg_dinsert.o
	${CC} ${CFLAG} ${LDLIBS} $^ -o $@

main3: main3.o utility.o alg_bruteforce.o alg_dinsert.o
	${CC} ${CFLAG} $^ -o $@

