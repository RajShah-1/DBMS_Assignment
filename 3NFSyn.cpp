#include <algorithm>
#include <iomanip>
#include <iostream>
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
  bool isFDCover(vector<FuncDependency>& F1, vector<FuncDependency>& F2);
  void computeAttrsClosure(vector<int>& attrs, vector<FuncDependency>& F,
                           vector<bool>& attrsCover);
  bool areFDEq(vector<FuncDependency>& F1, vector<FuncDependency>& F2);
  void minCoverF(void);
  bool isSuperKey(vector<int>& attrs);
  vector<int> getPK(void);

 public:
  void readFD(FuncDependency& newFD);
  void readRelation(void);
  void readDecomposition(void);
  void printFD(const FuncDependency& FD);
  void printDecomposition(void);
  void syn3NFDecomposition(void);
};

int main() {
  // Taking R as an input
  Relation R;
  int numAttrs, numFDs;
  R.readRelation();
  R.syn3NFDecomposition();
  cout << "========================3NF Generated "
          "successfully==============================\n";
  return 0;
}

// =================================================================
void Relation::syn3NFDecomposition(void) {
  // Find minCover
  this->minCoverF();
  unordered_map<string, vector<int>> orgAttrs;
  unordered_map<string, unordered_set<int>> rhs;
  for (FuncDependency fd : this->F) {
    string attrStr = this->attrListToStr(fd.lhs);
    if (orgAttrs.find(attrStr) == orgAttrs.end()) {
      orgAttrs.insert(make_pair(attrStr, fd.lhs));
    }
    for (int attr : fd.rhs) {
      rhs[attrStr].insert(attr);
    }
  }

  unordered_set<string> prevDecomps;
  bool hasKey = false;
  for (auto orgAttr : orgAttrs) {
    vector<int> Ri;
    for (int attrID : orgAttr.second) {
      Ri.push_back(attrID);
    }
    for (int attrID : rhs[orgAttr.first]) {
      Ri.push_back(attrID);
    }
    string attrStr = this->attrListToStr(Ri);
    if (prevDecomps.find(attrStr) == prevDecomps.end()) {
      this->Decomposition.push_back(Ri);
    }
    prevDecomps.insert(attrStr);
    if (this->isSuperKey(Ri)) {
      hasKey = true;
    }
  }
  if (!hasKey) {
    vector<int> PK = this->getPK();
    // cout << "PK: ";
    // for (int i = 0; i < PK.size(); ++i) {
    //   cout << attributes[PK[i]] << " ";
    // }
    // cout << "\n";
    this->Decomposition.push_back(PK);
  }
  this->printDecomposition();
}

vector<int> Relation::getPK(void) {
  int numAttrs = this->attributes.size();
  vector<int> PK(numAttrs);

  for (int i = 0; i < numAttrs; ++i) {
    PK[i] = i;
  }
  // Attempt to remove one attribute at a time
  int removedAttr;
  for (vector<int>::iterator it = PK.begin(); it != PK.end();) {
    removedAttr = *it;
    PK.erase(it);
    if (!this->isSuperKey(PK)) {
      it = PK.insert(it, removedAttr);
      it++;
    }
  }
  return PK;
}

string Relation::attrListToStr(vector<int> attrs) {
  string str = "";
  sort(attrs.begin(), attrs.end());
  for (int attr : attrs) {
    str = str + "_" + to_string(attr);
  }
  return str;
}

bool Relation::isSuperKey(vector<int>& attrs) {
  int numAttrs = this->attributes.size();
  vector<bool> AClosure(numAttrs, false);
  this->computeAttrsClosure(attrs, this->F, AClosure);
  for (int i = 0; i < numAttrs; ++i) {
    if (!AClosure[i]) return false;
  }
  return true;
}

// Updates F = minCover(F)
void Relation::minCoverF(void) {
  // Make a deep copy of F and delete it
  vector<FuncDependency> Forg = this->F;
  this->F.clear();

  // make sure there is only one attr on the rhs of each fd
  FuncDependency newFD;
  for (FuncDependency& fd : Forg) {
    newFD.lhs = fd.lhs;
    for (int attrID : fd.rhs) {
      newFD.rhs = {attrID};
      this->F.push_back(newFD);
    }
  }

  // Removing extraneous attributes
  int removedAttr;
  for (FuncDependency& fd : this->F) {
    for (auto it = fd.lhs.begin(); it != fd.lhs.end();) {
      removedAttr = *it;
      fd.lhs.erase(it);
      if (!this->areFDEq(this->F, Forg)) {
        it = fd.lhs.insert(it, removedAttr);
        it++;
      }
    }
  }

  // Remove one fd and check it the closure changes
  FuncDependency remFD;
  for (auto it = this->F.begin(); it != this->F.end();) {
    remFD = *it;
    F.erase(it);
    if (!this->areFDEq(this->F, Forg)) {
      it = this->F.insert(it, remFD);
      it++;
    }
  }

  // Print minCover
  cout << "MinCover:\n";
  for (FuncDependency fd : this->F) {
    this->printFD(fd);
  }
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

// Return true if F1 covers F2
bool Relation::isFDCover(vector<FuncDependency>& F1,
                         vector<FuncDependency>& F2) {
  int numAttrs = this->attributes.size();
  for (FuncDependency& fd1 : F1) {
    vector<bool> XF1Cover(numAttrs, false);
    vector<bool> XF2Cover(numAttrs, false);
    this->computeAttrsClosure(fd1.lhs, F1, XF1Cover);
    this->computeAttrsClosure(fd1.lhs, F2, XF2Cover);

    // XF1Cover must include XF2Cover
    for (int i = 0; i < numAttrs; ++i) {
      if (XF2Cover[i] == true && XF1Cover[i] == false) {
        return false;
      }
      if (XF1Cover[i] == true && XF2Cover[i] == false) {
        return false;
      }
    }
  }
  return true;
}

bool Relation::areFDEq(vector<FuncDependency>& F1, vector<FuncDependency>& F2) {
  return (isFDCover(F1, F2) && isFDCover(F2, F1));
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