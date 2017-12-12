ifeq ($(BUILDMODE), 0)
	BUILDPARAM=-Wall -Wextra -Og -ggdb -g3 -DPBERRALL='1' \
	  -DBUILDMODE=$(BUILDMODE)
	LINKOPTIONS=-lm -rdynamic
else 
  ifeq ($(BUILDMODE), 1)
	  BUILDPARAM=-Wall -Wextra -Werror -Wfatal-errors -O3 \
		  -DPBERRSAFEMALLOC='1' -DBUILDMODE=$(BUILDMODE)
	  LINKOPTIONS=-lm -rdynamic
  else 
    ifeq ($(BUILDMODE), 2)
	    BUILDPARAM=-Wall -Wextra -Werror -Wfatal-errors -O3 \
		    -DBUILDMODE=$(BUILDMODE)
	    LINKOPTIONS=-lm
		endif
	endif
endif

# compiler
COMPILER=gcc

