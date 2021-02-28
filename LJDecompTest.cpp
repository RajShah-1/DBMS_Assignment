#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
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
  vector<FuncDependency> F;
  vector<vector<int>> Decomposition;

  void initLJCheck(vector<vector<string>>& S);
  void LJAddFD(vector<vector<string>>& S);
  void printS(vector<vector<string>>& S);

 public:
  void readFD(FuncDependency& newFD);
  void readRelation(void);
  void readDecomposition(void);
  void printFD(const FuncDependency& FD);
  void printDecomposition(void);
  bool isLJDecomposition(void);
};

int main() {
  // Taking R as an input
  Relation R;
  int numAttrs, numFDs;
  R.readRelation();
  R.readDecomposition();
  R.printDecomposition();
  bool isLJDecomp = R.isLJDecomposition();
  if (isLJDecomp) {
    cout << "The given decomposition is a lossless join decomposition\n";
  } else {
    cout << "The given decomposition is not a lossless join decomposition\n";
  }
  return 0;
}

bool Relation::isLJDecomposition(void) {
  vector<vector<string>> S(this->Decomposition.size(),
                           vector<string>(this->attributes.size(), "b"));
  this->initLJCheck(S);
  this->LJAddFD(S);
  printS(S);
  bool isLJDecomposition;
  for (vector<string> Si : S) {
    isLJDecomposition = true;
    for (string Sij : Si) {
      if (Sij != "a") {
        isLJDecomposition = false;
        break;
      }
    }
    if (isLJDecomposition) return true;
  }
  return false;
}

void Relation::initLJCheck(vector<vector<string>>& S) {
  for (int i = 0; i < S.size(); ++i) {
    for (int attrID : this->Decomposition[i]) {
      S[i][attrID] = 'a';
    }
  }
}

void Relation::LJAddFD(vector<vector<string>>& S) {
  // iterate over FDs
  bool noChange;
  int numAttrs = this->attributes.size();
  // Repeat until there is no change
  do {
    noChange = true;
    for (FuncDependency& FD : this->F) {
      // FD: lhs -> rhs
      // get all the rows having same values for lhs
      vector<int> sameX(numAttrs, true);
      for (int lhsAttr : FD.lhs) {
        for (int i = 0; i < S.size(); ++i) {
          if (S[i][lhsAttr] != "a") {
            sameX[i] = false;
          }
        }
      }
      bool isA;
      for (int rhsAttr : FD.rhs) {
        isA = false;
        for (int i = 0; i < S.size(); ++i) {
          if (!sameX[i]) continue;
          if (S[i][rhsAttr] == "a") {
            isA = true;
            break;
          }
        }
        if (!isA) continue;
        for (int i = 0; i < S.size(); ++i) {
          if (!sameX[i]) continue;
          if (S[i][rhsAttr] != "a") {
            noChange = true;
            S[i][rhsAttr] = "a";
          }
        }
      }
    }
  } while (!noChange);
}

void Relation::printS(vector<vector<string>>& S) {
  cout << "Priniting S:\n";
  for (int i = 0; i < S.size(); ++i) {
    for (int j = 0; j < S[i].size(); ++j) {
      cout << setw(2) << S[i][j] << " ";
    }
    cout << "\n";
  }
  cout << "=========================\n";
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