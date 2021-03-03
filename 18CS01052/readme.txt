Each program is in its seperate folder.
Each of this folder contains following:
  - .cpp file containing the source code
  - in folder containing test cases
  - out follder containing output for the given cases
  - .sh file used to automate the compilation and running of the program
    (use bash <filename>.sh to compile and execute the program on linux based systems)

Check the files in the output folder to get the test case and the output of the algo.

To compile:
  g++ -o LJDecompTest LJDecompTest.cpp
  g++ -o 3NFSyn 3NFSyn.cpp
  g++ -o BCNFSyn BCNFSyn.cpp
To run the program: 
  ./LJDecompTest (Add input interactively/paste the text from .in file)
  ./3NFSyn
  ./BCNFSyn

Note: 

Lossless Join Decomposition Test:
  - For X->Y, when for the rows having same values for X, has bijs in 
  corresponding Y column, I use Ci to replace the bij's in these columns 
  to avoid ambiguity.
  (instead of duplicating bij's in all the rows)
  - makeRHSSame function of this code handles all the corner cases that results
  while making the values of Y in rows same.

BCNF Synthesis:
  - For any relation Q that results from decomposition of R, instead of finding
  the projection of F-closure (F+) on Q, I will find the closure of all the proper 
  subsets X of Q, and check if the functional dependency X -> XClosure-X violates
  BCNF properties in Q. 
  (XClosure is found with respect to R using F, and then the attributes in 
  R-Q are removed to obtain closure of X in Q. In this new closure contain
  anything other than X but does not contain the entire Q-X, then it violates
  the BCNF property.)
  - As soon as we find the first such dependency, we exit the loop and decompose
  Q into (Q-X) and (XClosure). 
  (Note that XClosure = (X union Y), where Y = XClosure-X)
  
  
  - It can be theoritically proven that the set: 
  F' = {X -> Y': X is any subset of Q & Y' = (Closure of X in Q)}
  is same as the projection of the closure of the set of functional dependencies
  of R on Q.
  - Hence, the method described above is equivalent to the algo 15.5.

  - This method performs a little better than finding the projection of F closure
  as finding the projection requires us to process all the dependencies in F closure.
  (This method will compute XClosure until it finds the first X->Y violating BCNF)