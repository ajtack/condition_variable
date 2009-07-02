CXX_DEMO_OBJECTS =      \
	main.oo          \
	condvar/futex.oo \

C_DEMO_OBJECTS =      \
	main.o          \
	condvar/futex.o \
	
CC = icc
CCOPTS = -c -Qtm_enabled -g
CXX = icpc
CXXOPTS = -c -Qtm_enabled -g
LINK = icc
LINKOPTS = -Qtm_enabled

.PHONY: all
all: demonstration_in_c demonstration_in_c++

demonstration_in_c++: $(CXX_DEMO_OBJECTS) 
	$(CXX) $(LINKOPTS) -o $@ $^

demonstration_in_c: $(C_DEMO_OBJECTS)
	$(LINK) $(LINKOPTS) -o $@ $^

main.oo: main.cxx environment.h label.h condvar/condvar.h \
  condvar/futex.h
	$(CXX) $(CXXOPTS) -o $@ $<

main.o: main.c environment.h label.h condvar/condvar.h \
  condvar/futex.h
	$(CC) $(CCOPTS) -o $@ $<

condvar/futex.oo: condvar/futex.c condvar/lowlevellock.h condvar/tls-simple.h \
  condvar/futex.h
	$(CXX) $(CXXOPTS) -o $@ $<

condvar/futex.o: condvar/futex.c condvar/lowlevellock.h condvar/tls-simple.h \
  condvar/futex.h
	$(CC) $(CCOPTS) -o $@ $<

.PHONY: clean
clean:
	rm -Rf demonstration_in_c demonstration_in_c++ $(C_DEMO_OBJECTS) $(CXX_DEMO_OBJECTS)
