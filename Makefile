


CC = mpicc #pgcc
FC = mpif90 #pgf90

FRIDA_DIR := frida

INCLUDE = -I. -I./blas -I./utils -I./$(FRIDA_DIR)

BLAS = -Mnvpl
LD_FLAGS = -ldl -lrt -lresolv -lm -pthread -Wl,-z,noexecstack,--gc-sections 

TARGET1 = libprof-dbi.so
TARGET2 = libprof-dl.so

CFLAGS = -O2 -mp -fPIC -w  -g $(INCLUDE) $(CPPFLAGS) 
CFLAGS1 = $(CFLAGS) -DDBI -I./$(FRIDA_DIR)
FFLAGS = -O2 -mp -g -mcmodel=large #$(MEMMODEL)

#----------------------------

#BLAS_SRCS = $(wildcard linear_algebra/$(GPUARCH)/*.c)
UTIL_SRCS = $(wildcard utils/*.c)
COMMON_SRCS = init.c fini.c $(UTIL_SRCS)

SRCS1 = main-dbi.c BLAS/blas-dbi.c PBLAS/pblas-dbi.c LAPACK/lapack-dbi.c ScaLAPACK/scalapack-dbi.c
OBJ1 = $(patsubst %.c,%-dbi.o,$(COMMON_SRCS)) $(SRCS1:.c=.o)

SRCS2 = main-dl.c BLAS/blas-dl.c PBLAS/pblas-dl.c LAPACK/lapack-dl.c ScaLAPACK/scalapack-dl.c
OBJ2 = $(patsubst %.c,%-dl.o,$(COMMON_SRCS))  $(SRCS2:.c=.o)


all: gen-wrapper dbi dl 

gen-wrapper:
	cd BLAS       && ../tools/gen-wrapper.py -dbi prototypes-blas.txt      > blas-dbi.c      
	cd BLAS       && ../tools/gen-wrapper.py -dl  prototypes-blas.txt      > blas-dl.c       
	cd BLAS       && ../tools/gen-wrapper.py -dl  prototypes-blas.txt      > blas-dl.c       
	cd BLAS       && ../tools/gen-index.sh prototypes-blas.txt
	cd PBLAS      && ../tools/gen-wrapper.py -dbi prototypes-pblas.txt     > pblas-dbi.c     
	cd PBLAS      && ../tools/gen-wrapper.py -dl  prototypes-pblas.txt     > pblas-dl.c      
	cd PBLAS      && ../tools/gen-index.sh prototypes-pblas.txt
	cd LAPACK     && ../tools/gen-wrapper.py -dbi prototypes-lapack.txt    > lapack-dbi.c    
	cd LAPACK     && ../tools/gen-wrapper.py -dl  prototypes-lapack.txt    > lapack-dl.c     
	cd LAPACK     && ../tools/gen-index.sh prototypes-lapack.txt
	cd ScaLAPACK  && ../tools/gen-wrapper.py -dbi prototypes-scalapack.txt > scalapack-dbi.c 
	cd ScaLAPACK  && ../tools/gen-wrapper.py -dl  prototypes-scalapack.txt > scalapack-dl.c  
	cd ScaLAPACK  && ../tools/gen-index.sh prototypes-scalapack.txt
	
dbi: $(FRIDA_DIR) $(TARGET1)
$(TARGET1): $(OBJ1) 
	@echo "Building DBI based Libprof"
	$(CC) -o $@ -shared -ffunction-sections -fdata-sections $^ ./$(FRIDA_DIR)/libfrida-gum.a ${LD_FLAGS} ${CFLAGS} $(LIBS)

dl: $(TARGET2)
$(TARGET2): $(OBJ2)
	@echo "Building DL based Libprof"
	$(CC) -o $@ -shared  $^  ${LD_FLAGS} ${CFLAGS} $(LIBS)

%-dbi.o: %.c
	$(CC) $(CFLAGS1) -c $< -o $@

%-dl.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------- setup frida for DBI --------------------------

# Determine the architecture
CPUARCH := $(shell uname -m)
# Set architecture-specific variables
FRIDA_HOME := https://github.com/frida/frida/releases/download/
FRIDA_VERSION := 16.2.1
ifeq ($(CPUARCH),x86_64)
    FRIDA_DEVKIT_URL := $(FRIDA_HOME)$(FRIDA_VERSION)/frida-gum-devkit-$(FRIDA_VERSION)-linux-x86_64.tar.xz
    FRIDA_DEVKIT_FILE := frida-gum-devkit-$(FRIDA_VERSION)-linux-x86_64.tar.xz
else ifeq ($(CPUARCH),aarch64)
    FRIDA_DEVKIT_URL := $(FRIDA_HOME)$(FRIDA_VERSION)/frida-gum-devkit-$(FRIDA_VERSION)-linux-arm64.tar.xz
    FRIDA_DEVKIT_FILE := frida-gum-devkit-$(FRIDA_VERSION)-linux-arm64.tar.xz
else ifeq ($(CPUARCH),arm64)
    FRIDA_DEVKIT_URL := $(FRIDA_HOME)$(FRIDA_VERSION)/frida-gum-devkit-$(FRIDA_VERSION)-linux-arm64.tar.xz
    FRIDA_DEVKIT_FILE := frida-gum-devkit-$(FRIDA_VERSION)-linux-arm64.tar.xz
else
    $(error Unsupported architecture: $(CPUARCH))
endif

$(FRIDA_DIR): $(FRIDA_DIR)/$(FRIDA_DEVKIT_FILE)
	@if [ ! -e "$(FRIDA_DIR)/frida-gum.h" ]; then \
        tar -xvf $(FRIDA_DIR)/$(FRIDA_DEVKIT_FILE) -C $(FRIDA_DIR); \
        fi

$(FRIDA_DIR)/$(FRIDA_DEVKIT_FILE):
	mkdir -p $(FRIDA_DIR)
	curl -s  -L $(FRIDA_DEVKIT_URL) -o $(FRIDA_DIR)/$(FRIDA_DEVKIT_FILE)

# ------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(OBJ1) $(OBJ2) $(FRIDA_DIR)

.PHONY: veryclean
veryclean:
	rm -rf $(TARGET1) $(TARGET2) $(OBJ1) $(OBJ2) $(FRIDA_DIR) BLAS/*.[ch] PBLAS/*.[ch] LAPACK/*.[ch] ScaLAPACK/*.[ch]
