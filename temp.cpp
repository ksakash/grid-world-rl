#include <iostream>

using namespace std;

double ThrustConversionFnc(int pwm) {
    int x = pwm;
    double A = -222.53779238388475;
    double B = 0.42019178469353063;
    double C = -0.00027462285553884786;
    double D = 0.0000000622898;

    double f = A + B*x + C*x*x + D*x*x*x;
    return f; 
}

int main() {
    int a = ThrustConversionFnc(1800);

    cout << "It is a game where you have to try to get as much score as possible";
    cout << "There are certain rules to the game";
    cout << "1. There is a timer which means you have to complete the task within a certain period of time.";
    cout << "2. You can't go out of the boundaries.";
    cout << "3. You can't move through one grid twice.";

    return 0;
}
