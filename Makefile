# The Makefile for the C++ implementation of Hd.p

COMPILER = g++
OBJS = utils.o topic.o document.o corpus.o gibbs.o ahdp_main.o table.o author.o
SOURCE = $(OBJS:.o=.cc)

FLAGS = -g -Wall  -I/usr/local/Cellar/gsl/1.16/include -std=c++11

# GSL library
LIBS = -lgsl -lgslcblas -L/usr/local/Cellar/gsl/1.16/lib

default: ahdp

ahdp: $(OBJS) 
	$(COMPILER) $(FLAGS) $(OBJS) -o ahdp  $(LIBS)

%.o: %.cc
	$(COMPILER) -c $(FLAGS) -o $@  $< 

.PHONY: clean
clean: 
	rm -f *.o