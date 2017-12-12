# Build mode
# 0: development (max safety, no optimisation)
# 1: release (min safety, optimisation)
# 2: fast and furious (no safety, optimisation)
BUILDMODE=0

include ./Makefile.inc

BUILDOPTIONS=$(BUILDPARAM) $(INCPATH)

#rules
all : main

main: main.o pberr.o Makefile 
	$(COMPILER) main.o pberr.o $(LINKOPTIONS) -o main

main.o : main.c pberr.h Makefile
	$(COMPILER) $(BUILDOPTIONS) -c main.c

pberr.o : pberr.c pberr.h Makefile
	$(COMPILER) $(BUILDOPTIONS) -c pberr.c

clean : 
	rm -rf *.o main

valgrind :
	valgrind -v --track-origins=yes --leak-check=full --gen-suppressions=yes --show-leak-kinds=all ./main
	
unitTest :
	main > unitTest.txt; diff unitTest.txt unitTestRef.txt
