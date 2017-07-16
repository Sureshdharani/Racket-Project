CXX = g++ -lmraa -lm -O3 -Wall -std=c++11
# CXX = g++ -g -O3 -Wall -std=c++11
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
# TEST_BINARIES = $(basename $(wildcard *Test.cpp))
HEADER = $(wildcard *.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
CPPLINT_PATH = ./cpplint.py

.PRECIOUS: %.o

all: checkstyle compile

compile: $(MAIN_BINARIES) # $(TEST_BINARIES)

test: $(TEST_BINARIES)
	 for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	# RUN CHECKSTYLE 
	# filter header_guards and include check, doesn't work well for
	# projects roots that are not a svn or git root
	python $(CPPLINT_PATH) *.h *.cpp

clean:
	# RUN CLEAN
	rm -f *.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)
	rm -f *.class
	rm -f *Test.TMP.*
	rm -f core
	rm -f *.out

%Main: %Main.o $(OBJECTS)
	$(CXX) -o $@ $^

%Test: %Test.o $(OBJECTS)
	$(CXX) -o $@ $^ -lgtest -lgtest_main -lpthread

%.o: %.cpp $(HEADER)
	$(CXX) -c $<


