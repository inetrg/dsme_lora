LORA_OMNETPP ?= ../src/$(shell basename $(CURDIR))
ROOT_PATH ?= ..
INET_PATH ?= $(ROOT_PATH)/inet
FLORA_PATH ?= $(ROOT_PATH)/flora
SIMULATION ?= s_30_a_10_spa_1
NUM_CORES ?= 4

OPP_MAKEMAKE_KARGS = -KINET_PROJ=$(INET_PATH) -KFLORA_PROJ=$(FLORA_PATH) -KCFLAGS_EXTRA=-DLORA_SYMBOL_TIME
OPP_MAKEMAKE_DARGS = -DINET_IMPORT
OPP_MAKEMAKE_IARGS = -I. -I$$\(INET_PROJ\)/src -I. -I$$\(FLORA_PROJ\)/src -I$$\(INET_DSME_PROJ\)/src
OPP_MAKEMAKE_LARGS = -L$$\(INET_PROJ\)/src -L$$\(FLORA_PROJ\)/src -L$$\(INET_DSME_PROJ\)/src
OPP_MAKEMAKE_DLARGS = -lINET$$\(D\) -lflora$$\(D\)

OPP_MAKEMAKE_ARGS += -f --deep
OPP_MAKEMAKE_ARGS += $(OPP_MAKEMAKE_KARGS)
OPP_MAKEMAKE_ARGS += $(OPP_MAKEMAKE_DARGS)
OPP_MAKEMAKE_ARGS += $(OPP_MAKEMAKE_IARGS)
OPP_MAKEMAKE_ARGS += $(OPP_MAKEMAKE_LARGS)
OPP_MAKEMAKE_ARGS += $(OPP_MAKEMAKE_DLARGS)

INET_LIB = $(INET_PATH)/src/INET

NED_INCLUDES = ../src:.:$(INET_PATH)/src:$(INET_PATH)/examples:$(INET_PATH)/tutorials:$(INET_PATH)/showcases:$(FLORA_PATH)/src
OMNETPP_LIBS += -l  $(INET_LIB)

ifneq (,$(RUN))
  RUNARGS = -r $(RUN)
endif

CMDENV ?= 0
VERBOSE ?= 0

ifneq (0, $(CMDENV))
  OMNETPP_EXTRA_ARGS += -u Cmdenv
  ifneq (0, $(VERBOSE))
	OMNETPP_EXTRA_ARGS += --cmdenv-express-mode=false --cmdenv-log-prefix="[%l] %m: %|"
  endif
endif

define LORA_OMNETPP_ARGS
-m -n $(NED_INCLUDES) --image-path=$(INET_PATH)/images $(OMNETPP_LIBS) $(OMNETPP_EXTRA_ARGS) -f $(SIMULATION).ini -f common.ini $(RUNARGS)
endef

all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake $(OPP_MAKEMAKE_ARGS)

makefiles-lib:
	cd src && opp_makemake $(OPP_MAKEMAKE_ARGS) -s


checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi

configs:
	cd simulations && $(LORA_OMNETPP) $(LORA_OMNETPP_ARGS) -q runs

run:
	cd simulations && $(LORA_OMNETPP) $(LORA_OMNETPP_ARGS)

runall:
	cd simulations && opp_runall -j$(NUM_CORES) -b1 $(LORA_OMNETPP) $(LORA_OMNETPP_ARGS)

debug:
	cd simulations && $(LORA_OMNETPP)_dbg $(LORA_OMNETPP_ARGS)

gdb:
	cd simulations && gdb --args $(LORA_OMNETPP)_dbg $(LORA_OMNETPP_ARGS)
