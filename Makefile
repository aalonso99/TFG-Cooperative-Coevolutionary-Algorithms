all: dep_ext exp1 exp2

dep_ext: 
	g++ -O2 -std=c++11 BMOptimizer.cpp RandomGenerator.cpp dependency_extraction.cpp -o dependency_extraction
	
exp1:
	g++ -O2 -std=c++11 -fopenmp BMOptimizer.cpp RandomGenerator.cpp CCEAvsEA.cpp -o CCEAvsEA
	
exp2:
	g++ -O2 -std=c++11 -fopenmp BMOptimizer.cpp RandomGenerator.cpp CCEAvsEA_2.cpp -o CCEAvsEA_2
