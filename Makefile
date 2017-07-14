#--------------------- Basic Settings -----------------------------------------#
PROGRAM_NAME  := desbench
BINARY_BASE   := bin
BUILD_BASE    := bld
SOURCE_BASE   := src
MAIN_FILE     := src/main.cc

#--------------------- External Libraries -------------------------------------#
HEADER_DIRS   := \
	../libprim/inc \
	../libdes/inc \
	../librnd/inc
STATIC_LIBS   := \
	../libprim/bld/libprim.a \
	../libdes/bld/libdes.a \
	../librnd/bld/librnd.a

#--------------------- Cpp Lint -----------------------------------------------#
LINT          := $(HOME)/.makeccpp/cpplint/cpplint.py
LINT_FLAGS    :=

#--------------------- Unit Tests ---------------------------------------------#
TEST_SUFFIX   := _TEST
GTEST_BASE    := $(HOME)/.makeccpp/gtest

#--------------------- Compilation and Linking --------------------------------#
CXX           := g++
SRC_EXTS      := .cc
HDR_EXTS      := .h .tcc
CXX_FLAGS     := -std=c++11 -Wall -Wextra -pedantic -Wfatal-errors
CXX_FLAGS     += -march=native -g -O3 -flto
CXX_FLAGS     += -pthread
#CXX_FLAGS     += -DNDEBUGLOG
LINK_FLAGS    := -lz -lpthread -lcrypto -lssl -Wl,--no-as-needed

#--------------------- Auto Makefile ------------------------------------------#
include $(HOME)/.makeccpp/auto_bin.mk
