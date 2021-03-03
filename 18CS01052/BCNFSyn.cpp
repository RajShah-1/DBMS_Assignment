#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

typedef struct {
  // lhs -> rhs (lhs determines rhs)
  vector<int> lhs;
  vector<int> rhs;
} FuncDependency;

class Relation {
  vector<string> attributes;
  vector<int> pk;  // Primary key
  // vector<vector<int>> cks; // Candidate keys

  // maps attribute names to an integer
  unordered_map<string, int> attrIndex;
  vector<vector<int>> Decomposition;
  vector<FuncDependency> F;
  string attrListToStr(vector<int> attrs);
  void computeAttrsClosure(vector<int>& attrs, vector<FuncDependency>& F,
                           vector<bool>& attrsClosure);

 public:
  void readFD(FuncDependency& newFD);
  void readRelation(void);
  void readDecomposition(void);
  void printFD(const FuncDependency& FD);
  void printDecomposition(void);
  bool checkBCNF(unordered_set<int>& attrs, unordered_set<int>& X,
                 vector<bool>& XClosure);
  void BCNFDecompose(void);
};

int main() {
  // Taking R as an input
  Relation R;
  R.readRelation();
  R.BCNFDecompose();
  R.printDecomposition();
  cout << "========================BCNF Generated "
          "successfully==============================\n";
  return 0;
}

void Relation::BCNFDecompose(void) {
  // Iterate
  queue<vector<int>> decompQueue;
  int iter_num = 0;
  vector<int> decomp;
  for (int i = 0; i < this->attributes.size(); ++i) {
    decomp.push_back(i);
  }
  decompQueue.push(decomp);
  while (!decompQueue.empty()) {
    decomp = decompQueue.front();
    iter_num++;
    decompQueue.pop();
    unordered_set<int> decompSet(decomp.begin(), decomp.end());
    // find a problematic FD
    // check all subsets of the attributes
    int decompSize = decomp.size();

    bool violateBCNF = false;
    for (long long i = 0; i <= (long long)pow(2, decompSize); ++i) {
      unordered_set<int> X;
      for (long long j = 0; j < decompSize; ++j) {
        if ((i & (1 << j)) != 0) {
          X.insert(decomp[j]);
        }
      }
      vector<bool> XClosure(this->attributes.size());
      if (!this->checkBCNF(decompSet, X, XClosure)) {
        vector<int> XUYVec, YVec, QMinYVec;
        for (int id = 0; id < XClosure.size(); ++id) {
          if (decompSet.find(id) == decompSet.end()) continue;
          if (XClosure[id] && X.find(id) == X.end()) {
            XUYVec.push_back(id);
          } else if (X.find(id) != X.end()) {
            XUYVec.push_back(id);
            QMinYVec.push_back(id);
          } else {
            QMinYVec.push_back(id);
          }
        }
        if (XUYVec.size() > 0) decompQueue.push(XUYVec);
        if (QMinYVec.size() > 0) decompQueue.push(QMinYVec);
        violateBCNF = true;
        break;
      }
    }
    if (!violateBCNF) {
      this->Decomposition.push_back(decomp);
    }
  }
}

bool Relation::checkBCNF(unordered_set<int>& attrs, unordered_set<int>& X,
                         vector<bool>& XClosure) {
  int numAttrs = this->attributes.size();
  vector<int> XVec;
  for (int Xattr : X) {
    XVec.push_back(Xattr);
  }
  this->computeAttrsClosure(XVec, this->F, XClosure);
  bool isTrivial = true;
  for (int attrID : attrs) {
    if (X.find(attrID) == X.end() && XClosure[attrID]) {
      isTrivial = false;
      break;
    }
  }

  bool isSuperKey = true;
  for (int attrID : attrs) {
    if (!XClosure[attrID]) {
      isSuperKey = false;
      break;
    }
  }
  return isTrivial || isSuperKey;
}

void Relation::computeAttrsClosure(vector<int>& X, vector<FuncDependency>& F,
                                   vector<bool>& XCover) {
  unordered_set<int> X_plus;
  for (int attr : X) {
    X_plus.insert(attr);
  }
  bool noChange;
  bool isSubset;
  do {
    noChange = true;
    for (FuncDependency& fd : F) {
      // check if fd.lhs is a subset of X_plus
      isSubset = true;
      for (int lhsAttr : fd.lhs) {
        if (X_plus.find(lhsAttr) == X_plus.end()) {
          isSubset = false;
          break;
        }
      }
      if (!isSubset) {
        continue;
      }
      // if lhs is a subset of X_plus
      // X_plus = X_plus union rhs of fd
      for (int rhsAttr : fd.rhs) {
        if (X_plus.find(rhsAttr) == X_plus.end()) {
          noChange = false;
          X_plus.insert(rhsAttr);
        }
      }
    }
  } while (!noChange);
  for (int X_plus_elem : X_plus) {
    XCover[X_plus_elem] = true;
  }
}

void Relation::readDecomposition(void) {
  int numR;
  cout << "Enter number of relations in the decompositions: ";
  cin >> numR;
  this->Decomposition.resize(numR);
  cout << "Enter the relations:\n";
  cout << "Format: { <attr1> <attr2> ... <attrk> }\n";
  string attrName, delim;
  bool isValid;
  for (int i = 0; i < numR; ++i) {
    do {
      isValid = false;
      cin >> delim;
      if (delim != "{") {
        cout << delim << " Invalid Format\n";
        continue;
      }
      cin >> attrName;
      while (attrName != "}") {
        if (this->attrIndex.find(attrName) == attrIndex.end()) {
          cout << "Attribute " << attrName
               << " does not exist in the relation\n";
          continue;
        }
        this->Decomposition[i].push_back(this->attrIndex[attrName]);
        cin >> attrName;
      }
      isValid = true;
    } while (!isValid);
  }
}

void Relation::printDecomposition(void) {
  cout << "Decomposition: \n";
  for (int i = 0; i < this->Decomposition.size(); ++i) {
    cout << "R" << i << ": ( ";
    for (int attrID : this->Decomposition[i]) {
      cout << this->attributes[attrID] << " ";
    }
    cout << " )\n";
  }
}

void Relation::readFD(FuncDependency& newFD) {
  bool isValid;
  string delim, attrName;

  do {
    isValid = false;
    cin >> delim;
    if (delim != "{") {
      cout << delim << " Invalid Format\n";
      continue;
    }
    cin >> attrName;
    while (attrName != "}") {
      if (this->attrIndex.find(attrName) == attrIndex.end()) {
        cout << "Attribute " << attrName << " does not exist in the relation\n";
        continue;
      }
      newFD.lhs.push_back(this->attrIndex[attrName]);
      cin >> attrName;
    }

    cin >> delim;
    if (delim != "->") {
      cout << delim << " Invalid Format\n";
      continue;
    }

    cin >> delim;
    if (delim != "{") {
      cout << delim << " Invalid Format\n";
      continue;
    }
    cin >> attrName;
    while (attrName != "}") {
      if (this->attrIndex.find(attrName) == attrIndex.end()) {
        cout << "Attribute " << attrName << " does not exist in the relation\n";
        continue;
      }
      newFD.rhs.push_back(this->attrIndex[attrName]);
      cin >> attrName;
    }
    isValid = true;
  } while (!isValid);

  sort(newFD.lhs.begin(), newFD.lhs.end());
  sort(newFD.rhs.begin(), newFD.rhs.end());
}

void Relation::readRelation(void) {
  int numAttrs, numFDs;

  cout << "Enter number of attributes in the relation: ";
  cin >> numAttrs;
  cout << "Enter number of Functional Dependencies in the relation: ";
  cin >> numFDs;
  this->attributes.resize(numAttrs);
  this->F.resize(numFDs);

  string attrName;
  cout << "Enter " << numAttrs << " Attributes:\n";
  for (int i = 0; i < numAttrs; ++i) {
    cin >> attrName;
    this->attributes[i] = attrName;
    this->attrIndex[attrName] = i;
  }

  cout << "Enter FDs:\n";
  cout << "Format { <attr1>, <attr2> } -> { <attr3>, <attr4> }\n";
  for (int i = 0; i < numFDs; ++i) {
    readFD(this->F[i]);
  }
  for (FuncDependency& fd : this->F) {
    printFD(fd);
  }
}

void Relation::printFD(const FuncDependency& FD) {
  cout << "{ ";
  for (int attrID : FD.lhs) {
    cout << attributes[attrID] << " ";
  }
  cout << "}";

  cout << " -> { ";
  for (int attrID : FD.rhs) {
    cout << attributes[attrID] << " ";
  }
  cout << "}\n";
}