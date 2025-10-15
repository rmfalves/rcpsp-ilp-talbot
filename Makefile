SYSTEM     = x86_sles10_4.1
LIBFORMAT  = static_pic

CPLEXDIR      = /root/ILOG/CPLEX_Studio_AcademicResearch122/cplex
CONCERTDIR    = /root/ILOG/CPLEX_Studio_AcademicResearch122/concert

CCC = g++

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m32 -O -fPIC -fexceptions -DNDEBUG -DIL_STD
COPT  = -m32 -fPIC 

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -m32 -lm -pthread
CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -m32 -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = $(CPLEXDIR)/examples
EXINC         = $(EXDIR)/include
EXDATA        = $(EXDIR)/data
EXSRCC        = $(EXDIR)/src/c
EXSRCCPP      = .
EXSRCJAVA     = $(EXDIR)/src/java

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

CPP_EX = rcpsp-ilp-talbot

clean:
	/bin/rm -f *.o
	make main

main: main.o
	$(CCC) $(CCFLAGS) main.o -o $(CPP_EX) $(CCLNFLAGS)
main.o: $(EXSRCCPP)/main.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRCCPP)/main.cpp -o main.o

