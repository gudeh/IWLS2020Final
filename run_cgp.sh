#This file compiles the CGP code into a binary file called "cgp" at the same folder the code is located.
#This akward location of folders is due to a paralelization of the execution of exemplars which kept 
#all input AIG files in a root folder. Such execution was not included in the public git version for simplicity.

#fopenmp and O0 options are optional, although they optmize the software execution
g++ -std=c++11 -O0 -fopenmp CGPcode/CPP/*.cpp CGPcode/CPP/*.h -o CGPcode/CPP/cgp
cd CGPcode/CPP/
./cgp