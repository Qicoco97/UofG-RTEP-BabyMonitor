/**
 * This file is released under the Patented Algorithm with Software Released
 * Agreement. See LICENSE.md for more information.
 * Original repository: https://github.com/tbl3rd/Pyramids
 */

 #ifndef BUTTERWORTH_HPP_INCLUDED
 #define BUTTERWORTH_HPP_INCLUDED
 
 #include <vector>
 
 /**
  * Generate a digital Butterworth filter of order N.
  *
  * @param N     Order of the filter
  * @param Wn    Normalized analog cutoff frequency (relative to sample rate)
  * @param out_a Output vector of denominator coefficients (a)
  * @param out_b Output vector of numerator coefficients (b)
  */
 void butterworth(unsigned int N, double Wn,
                  std::vector<double>& out_a,
                  std::vector<double>& out_b);
 
 #endif // BUTTERWORTH_HPP_INCLUDED
 