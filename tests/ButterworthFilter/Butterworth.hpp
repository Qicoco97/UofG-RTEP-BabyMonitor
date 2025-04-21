#ifndef BUTTERWORTH_HPP
#define BUTTERWORTH_HPP

#include <vector>

void butterworth(unsigned int N, double Wn,
                 std::vector<double> &out_a,
                 std::vector<double> &out_b);

#endif
