CXX = g++

ifeq ($(CXX), g++)
    OPTFLAGS = -O2
    WARNS = -Wall -Wno-switch -Wno-unused-function -Wno-unused-variable -Wno-unused-local-typedefs
else ifeq ($(CXX), icpc)
    OPTFLAGS = -O3 -no-prec-div -no-prec-sqrt -fp-model strict -fimf-precision:low -xAVX
    WARNS = -Wall -Wno-switch -Wno-unused-function -Wno-unused-variable
else ifeq ($(CXX), clang++-3.5)
    OPTFLAGS = -O3
    WARNS = -Wall -Wno-switch -Wno-unused-function -Wno-unused-variable -Wno-unused-local-typedefs
endif

PROJNAME = Words
CPPFLAGS = -std=c++11

SRCS=\
	Words.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all:
	@echo -e \\n=========================================================\\n\\n
	+make $(PROJNAME)

$(PROJNAME): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@

clean:
	@rm -vf -- $(OBJS) $(OBJS:.o=.d)
	@rm -vf -- icpc*.d
	@find . \( -name "*.o" -o -name "*.gch" \) -print0 | xargs -0 rm -vf --
	@rm -vf -- *.o-*
	@rm -vf -- $(PROJNAME)
