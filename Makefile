DEMO_OBJECTS =      \
	main.o          \
	condvar/futex.o \
	
CC = icc
CCOPTS = -c -Qtm_enabled -g
LINK = icc
LINKOPTS = -Qtm_enabled

demonstration: $(DEMO_OBJECTS)
	$(LINK) $(LINKOPTS) -o $@ $^

main.o: main.c condition_variable_environment.h label.h condvar/condvar.h \
  condvar/futex.h
	$(CC) $(CCOPTS) -o $@ $<

condvar/futex.o: condvar/futex.c condvar/lowlevellock.h condvar/tls-simple.h \
  condvar/futex.h
	$(CC) $(CCOPTS) -o $@ $<

.PHONY: clean
clean:
	rm -Rf demonstration $(DEMO_OBJECTS)
