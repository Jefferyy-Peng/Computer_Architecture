TESTS 	= memcpy
CCC	= g++
CCFLAGS	= -I../Include ../Src/simple.cc
DEPS	= ../Include/simple.hh ../Src/simple.cc

all:	${TESTS}

%: 	%.cc ../Src/%.cc $(DEPS)
	${CCC} ${CCFLAGS} $< ../Src/$< -o $@

clean:
	/bin/rm -rf ${TESTS}
