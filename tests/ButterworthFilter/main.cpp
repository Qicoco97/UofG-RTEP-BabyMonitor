#include <iostream>
#include <vector>
#include "Butterworth.hpp"

void printCoefficients(const std::string &name, const std::vector<double> &coeffs) {
    std::cout << name << ": ";
    for (const auto &c : coeffs) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

int main() {
    unsigned int order = 4;
    double Wn = 0.4;

    std::vector<double> a, b;
    butterworth(order, Wn, a, b);

    printCoefficients("b (numerator)", b);
    printCoefficients("a (denominator)", a);

    return 0;
}
