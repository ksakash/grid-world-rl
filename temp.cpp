#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

void function(vector<vector<int>> mat) {
    mat[1][0] = 1;
}

int main() {
    vector<vector<int>> mat(2, vector<int>(2));

    cout << "mat[0][0]: " << mat[0][0] << endl;
    cout << "mat[1][0]: " << mat[1][0] << endl;

    return 0;
}