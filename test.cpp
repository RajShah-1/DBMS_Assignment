#include <iostream>
#include <vector>

using namespace std;

void printVec(vector<int>& A){
  cout << "A: ";
  for(int v: A){
    cout << v << " ";
  }
  cout << "\n";
}

int main(){
  vector<int> A = {1, 2, 3, 4, 5};
  vector<int>::iterator it1 = A.begin(), it2;
  it1++;
  printVec(A);
  A.erase(it1);
  printVec(A);
  A.insert(it1, -2);
  printVec(A);
}