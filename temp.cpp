#include <iostream>
#include <fstream>

using namespace std;

int main() {
    ofstream myfile;
    myfile.open("/home/ironman/grid-world-rl/results.txt");
    myfile << "it is better to sit in a room then to roam outside and find girls" << endl;
    myfile.close();
    return 0;
}