#include <algorithm>
#include <iomanip>
#include <iostream>
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

 public:
  void readFD(FuncDependency& newFD);
  void readRelation(void);
  void readDecomposition(void);
  void printFD(const FuncDependency& FD);
  void printDecomposition(void);
  bool isFDCover(vector<FuncDependency>& F1, vector<FuncDependency>& F2);
  void computeAttrsCover(vector<int>& attrs, vector<FuncDependency>& F,
                         vector<bool>& attrsCover);
  bool areFDEq(vector<FuncDependency>& F1, vector<FuncDependency>& F2);

  void test(void);
};

void Relation::test(void) {
  vector<bool> ACover(attributes.size(), false);
  vector<int> attrs = {1, 3};
  this->computeAttrsCover(attrs, this->F, ACover);
  cout << "cover: ";
  for (int i = 0; i < ACover.size(); ++i) {
    if (ACover[i]) {
      cout << this->attributes[i] << " ";
    }
  }
  cout << "\n";
}

int main() {
  // Taking R as an input
  Relation R;
  int numAttrs, numFDs;
  R.readRelation();
  R.test();
  return 0;
}

// =================================================================
void Relation::computeAttrsCover(vector<int>& X, vector<FuncDependency>& F,
                                 vector<bool>& XCover) {
  unordered_set<int> X_plus;
  for (int attr : X) {
    X_plus.insert(attr);
  }
  bool noChange;
  bool isSubset;
  do {
    noChange = true;
    for (FuncDependency fd : F) {
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

// Return true if F1 covers F2
bool Relation::isFDCover(vector<FuncDependency>& F1,
                         vector<FuncDependency>& F2) {
  int numAttrs = this->attributes.size();
  for (FuncDependency fd1 : F1) {
    vector<bool> XF1Cover(numAttrs, false);
    vector<bool> XF2Cover(numAttrs, false);
    this->computeAttrsCover(fd1.lhs, F1, XF1Cover);
    this->computeAttrsCover(fd1.lhs, F2, XF2Cover);
    // XF1Cover must include XF2Cover
    for (int i = 0; i < numAttrs; ++i) {
      if (XF2Cover[i] == true && XF1Cover[i] == false) {
        return false;
      }
    }
  }
  return true;
}

bool Relation::areFDEq(vector<FuncDependency>& F1, vector<FuncDependency>& F2) {
  return isFDCover(F1, F2) && isFDCover(F2, F1);
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
  for (vector<int>& Ri : this->Decomposition) {
    cout << "Relation: { ";
    for (int attrID : Ri) {
      cout << this->attributes[attrID] << " ";
    }
    cout << " }\n";
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