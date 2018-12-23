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
    cout << "a: " << a << endl;

}
