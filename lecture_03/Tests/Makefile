TESTS 	= mxv_div mxv
CCC	= g++
CCFLAGS	= -I../Include ../Src/simple.cc
DEPS	= ../Include/simple.hh ../Src/simple.cc

all:	${TESTS}

mxv_div: 	mxv_div.cc ../Src/mxv.cc ../Include/mxv.hh $(DEPS)
	${CCC} ${CCFLAGS} $< ../Src/mxv.cc -o $@
	./mxv_div

mxv: 	mxv.cc ../Src/mxv.cc ../Include/mxv.hh $(DEPS)
	${CCC} ${CCFLAGS} $< ../Src/mxv.cc -o $@
	./mxv

clean:
	/bin/rm -rf ${TESTS} *.txt

