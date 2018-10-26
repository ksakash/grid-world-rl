// reading a text file
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

void stringToInt (string s) {
    std::vector<int> v;
    for (int i = 0; i < s.length(); i++) {
        string n = "";
        while (s[i] != ' ' && i != s.length()-1) {
            n.push_back(s[i]);
            i++;
        }
        if (s.length() == 1) {
            n.push_back(s[i]);
        }
        // cout << n << endl;
        double t = std::stof(n);
        cout << t << " ";
    }
    cout << endl;
}

int main () {
  string line;
  ifstream myfile ("/home/ironman/grid-world-rl/results.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
        // cout << line << '\n';
        // cout << "what's happening" << endl;
        stringToInt(line);
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}