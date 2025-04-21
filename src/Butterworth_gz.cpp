/**
 * This file is released under the Patented Algorithm with Software released
 * Agreement. See LICENSE.md for more information. Original repo:
 * https://github.com/tbl3rd/Pyramids
 */

 #include "Butterworth.hpp"

 #include <algorithm>
 #include <complex>
 #include <stdexcept>
 
 // Define sorting logic: real part first, then imag part.
 static bool sortComplex(const std::complex<double>& x, const std::complex<double>& y) {
     if (std::real(x) != std::real(y)) return std::real(x) < std::real(y);
     return std::imag(x) < std::imag(y);
 }
 
 static bool hasPosImag(const std::complex<double>& z) {
     return std::imag(z) > 0;
 }
 
 static bool hasNegImag(const std::complex<double>& z) {
     return std::imag(z) < 0;
 }
 
 // Compute polynomial coefficients from roots (sorted for numerical stability).
 static std::vector<std::complex<double>>
 polynomialCoefficients(std::vector<std::complex<double>> roots) {
     std::vector<std::complex<double>> coeffs(roots.size() + 1, 0.0);
     coeffs[0] = 1.0;
 
     std::sort(roots.begin(), roots.end(), sortComplex);
 
     for (size_t k = 0; k < roots.size(); ++k) {
         std::complex<double> w = -roots[k];
         for (size_t j = k + 1; j > 0; --j) {
             coeffs[j] = coeffs[j] * w + coeffs[j - 1];
         }
         coeffs[0] *= w;
     }
 
     // Try to convert result to real coefficients if conjugate symmetry holds
     std::vector<std::complex<double>> pos_roots = roots;
     pos_roots.erase(std::remove_if(pos_roots.begin(), pos_roots.end(), hasNegImag), pos_roots.end());
 
     std::vector<std::complex<double>> neg_roots = roots;
     neg_roots.erase(std::remove_if(neg_roots.begin(), neg_roots.end(), hasPosImag), neg_roots.end());
 
     bool same = (pos_roots.size() == neg_roots.size()) &&
                 std::equal(pos_roots.begin(), pos_roots.end(), neg_roots.begin());
 
     if (same) {
         for (auto& c : coeffs) {
             c = std::real(c);
         }
     }
     return coeffs;
 }
 
 // Compute real transfer function coefficients from zeros, poles, and gain.
 static void zerosPolesToTransferCoefficients(
     std::vector<std::complex<double>> zeros,
     std::vector<std::complex<double>> poles,
     double gain,
     std::vector<std::complex<double>>& a,
     std::vector<std::complex<double>>& b
 ) {
     a = polynomialCoefficients(std::move(poles));
     b = polynomialCoefficients(std::move(zeros));
     for (auto& bk : b) bk *= gain;
 }
 
 // Normalize coefficients so that a[0] = 1. Remove leading zeros.
 static void normalize(std::vector<std::complex<double>>& b, std::vector<std::complex<double>>& a) {
     size_t k = 0;
     while (k < a.size() && a[k] == 0.0) ++k;
     if (k == a.size()) throw std::range_error("Polynomial is zero");
     a.erase(a.begin(), a.begin() + k);
 
     std::complex<double> lead = a.front();
     for (auto& ak : a) ak /= lead;
     for (auto& bk : b) bk /= lead;
 }
 
 // Return (-1)^k
 static inline int minusOneToK(unsigned k) {
     return (k % 2 == 0) ? 1 : -1;
 }
 
 // Bilinear transform: map analog filter coefficients to digital domain.
 static void bilinearTransform(std::vector<std::complex<double>>& b,
                               std::vector<std::complex<double>>& a,
                               double fs) {
     const size_t D = a.size() - 1;
     const size_t N = b.size() - 1;
     const size_t M = std::max(N, D);
 
     std::vector<std::complex<double>> bprime(M + 1, 0.0);
     std::vector<std::complex<double>> aprime(M + 1, 0.0);
 
     for (size_t j = 0; j <= M; ++j) {
         std::complex<double> sumB = 0.0, sumA = 0.0;
 
         for (size_t i = 0; i <= N; ++i) {
             if (i > N) continue;
             for (size_t k = 0; k <= i; ++k) {
                 int iCk = (k == 0) ? 1 : (iCk * (i - k + 1) / k);
                 for (size_t l = 0; l <= M - i; ++l) {
                     int mCk = (l == 0) ? 1 : (mCk * (M - i - l + 1) / l);
                     if (k + l == j) {
                         sumB += double(iCk * mCk * minusOneToK(k)) * b[N - i] * pow(2.0 * fs, i);
                     }
                 }
             }
         }
         bprime[j] = std::real(sumB);
 
         for (size_t i = 0; i <= D; ++i) {
             if (i > D) continue;
             for (size_t k = 0; k <= i; ++k) {
                 int iCk = (k == 0) ? 1 : (iCk * (i - k + 1) / k);
                 for (size_t l = 0; l <= M - i; ++l) {
                     int mCk = (l == 0) ? 1 : (mCk * (M - i - l + 1) / l);
                     if (k + l == j) {
                         sumA += double(iCk * mCk * minusOneToK(k)) * a[D - i] * pow(2.0 * fs, i);
                     }
                 }
             }
         }
         aprime[j] = std::real(sumA);
     }
 
     normalize(bprime, aprime);
     b = bprime;
     a = aprime;
 }
 
 // Transform analog coefficients to lowpass with given cutoff.
 static void toLowpass(std::vector<std::complex<double>>& b,
                       std::vector<std::complex<double>>& a,
                       double w0) {
     size_t d = a.size(), n = b.size();
     size_t M = std::max(d, n);
     std::vector<double> pwo(M);
 
     for (int i = M - 1; i >= 0; --i)
         pwo[M - 1 - i] = pow(w0, i);
 
     for (size_t i = 0; i < b.size(); ++i) {
         b[i] *= pwo[M - n + i] / pwo[M - 1];
     }
 
     for (size_t i = 0; i < a.size(); ++i) {
         a[i] *= pwo[M - d + i] / pwo[M - 1];
     }
 
     normalize(b, a);
 }
 
 // Generate Butterworth prototype poles/zeros and unity gain.
 static void prototypeAnalogButterworth(unsigned N,
                                        std::vector<std::complex<double>>& zeros,
                                        std::vector<std::complex<double>>& poles,
                                        double& gain) {
     const std::complex<double> j(0.0, 1.0);
     poles.reserve(N);
     for (unsigned k = 1; k <= N; ++k) {
         double theta = M_PI * (2.0 * k - 1) / (2.0 * N);
         poles.emplace_back(j * std::exp(j * theta));
     }
     gain = 1.0;
     zeros.clear(); // Butterworth has no finite zeros
 }
 
 // Public API: Compute Butterworth filter coefficients for normalized cutoff Wn
 void butterworth(unsigned int N, double Wn,
                  std::vector<double>& out_a,
                  std::vector<double>& out_b) {
     const double fs = 2.0; // Normalize sample rate
     const double w0 = 2.0 * fs * tan(M_PI * Wn / fs);
 
     std::vector<std::complex<double>> zeros, poles;
     double gain;
     prototypeAnalogButterworth(N, zeros, poles, gain);
 
     std::vector<std::complex<double>> a, b;
     zerosPolesToTransferCoefficients(zeros, poles, gain, a, b);
     toLowpass(b, a, w0);
     bilinearTransform(b, a, fs);
 
     out_a.clear(); out_b.clear();
     for (const auto& coeff : a) out_a.push_back(std::real(coeff));
     for (const auto& coeff : b) out_b.push_back(std::real(coeff));
 }
 