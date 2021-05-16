INC=src
FLAGS=-I$(INC) -lm -O3
OMPFLAG=-fopenmp
CXX=g++

test: src/sorting/tests.cpp
	$(CXX) src/sorting/tests.cpp -o bin/test_sorting $(OMPFLAG) $(FLAGS)

clean:
	rm bin/*
