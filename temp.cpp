#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

void function(vector<vector<int>> mat) {
    mat[1][0] = 1;
}

int main() {
    vector<vector<double>> mat1;
    mat1.resize(2, vector<double>(2));

    cout << "mat1[0][0]: " << mat1[0][0] << endl;

    return 0;
}