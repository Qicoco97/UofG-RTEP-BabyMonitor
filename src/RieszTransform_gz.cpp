/**
 * This file is released under the Patented Algorithm with Software Released
 * Agreement. See LICENSE.md for more information, and view the original repo
 * at https://github.com/tbl3rd/Pyramids
 */

 #include <cstdlib>
 #include <functional>
 #include <algorithm>
 #include <memory>
 
 #include "RieszTransform.hpp"
 #include "Butterworth.hpp"
 #include "ComplexMat.hpp"
 
 // Temporal filter using Butterworth low/high pass
 class RieszTemporalFilter {
     RieszTemporalFilter &operator=(const RieszTemporalFilter &) = delete;
     RieszTemporalFilter(const RieszTemporalFilter &) = delete;
 
 public:
     double itsFrequency;
     std::vector<double> itsA;
     std::vector<double> itsB;
 
     void computeCoefficients(double halfFps) {
         const double Wn = itsFrequency / halfFps;
         butterworth(1, Wn, itsA, itsB);
     }
 
     void passEach(cv::Mat &result, const cv::Mat &phase, const cv::Mat &prior) const {
         result = itsB[0] * phase + itsB[1] * prior - itsA[1] * result;
         result /= itsA[0];
     }
 
     void pass(CompExpMat &result, const CompExpMat &phase, const CompExpMat &prior) const {
         passEach(cos(result), cos(phase), cos(prior));
         passEach(sin(result), sin(phase), sin(prior));
     }
 
     explicit RieszTemporalFilter(double f) : itsFrequency(f), itsA(), itsB() {}
 };
 
 // Represents one level in a Riesz Laplacian Pyramid
 class RieszPyramidLevel {
     RieszPyramidLevel &operator=(const RieszPyramidLevel &) = delete;
 
     cv::Mat itsLp;
     ComplexMat itsR;
     CompExpMat itsPhase, itsRealPass, itsImagPass;
 
 public:
     void initialize();
     void build(const cv::Mat &octave);
     void assign(const RieszPyramidLevel &current);
     void filter(const RieszTemporalFilter &hiCut, const RieszTemporalFilter &loCut, RieszPyramidLevel &prior);
     void unwrapOrientPhase(const RieszPyramidLevel &prior);
     void amplify(double alpha, double threshold);
     const cv::Mat &get_result() const { return itsLp; }
 };
 
 // Encapsulates a full Riesz Pyramid
 class RieszPyramid {
     RieszPyramid &operator=(const RieszPyramid &) = delete;
     RieszPyramid(const RieszPyramid &) = delete;
 
 public:
     typedef std::vector<RieszPyramidLevel>::size_type size_type;
     std::vector<RieszPyramidLevel> itsLevel;
 
     RieszPyramid() = default;
     bool initialized() const { return !itsLevel.empty(); }
     explicit operator bool() const { return initialized(); }
 
     void initialize(const cv::Mat &frame);
     void build(const cv::Mat &frame);
     void unwrapOrientPhase(const RieszPyramid &prior);
     void amplify(double alpha, double threshold);
     cv::Mat collapse() const;
 
     static int countLevels(const cv::Size &size);
 };
 
 // Manages temporal bandpass using RieszTemporalFilters
 class RieszTemporalBandpass {
     RieszTemporalBandpass &operator=(const RieszTemporalBandpass &) = delete;
 
 public:
     double itsFps;
     RieszTemporalFilter itsLoCut;
     RieszTemporalFilter itsHiCut;
 
     RieszTemporalBandpass() : itsFps(0.0), itsLoCut(0.0), itsHiCut(0.0) {}
     RieszTemporalBandpass(const RieszTemporalBandpass &that);
 
     void computeFilter();
     void lowCutoff(double frequency);
     void highCutoff(double frequency);
     void shift(const RieszPyramidLevel &current, RieszPyramidLevel &prior);
     void filterLevel(RieszPyramidLevel &current, RieszPyramidLevel &prior);
     void filterPyramids(RieszPyramid &current, RieszPyramid &prior);
 };
 
 // Holds the state of Riesz transform processing
 struct RieszTransformState {
     RieszTemporalBandpass itsBand;
     RieszPyramid itsCurrent;
     RieszPyramid itsPrior;
 
     RieszTransformState() = default;
     RieszTransformState(const RieszTransformState &other);
 };
 
 // Public RieszTransform class implementation
 RieszTransform::RieszTransform() : state(new RieszTransformState()), itsAlpha(0.0), itsThreshold(0.0) {}
 RieszTransform::RieszTransform(const RieszTransform &other) : state(new RieszTransformState(*other.state)), itsAlpha(other.itsAlpha), itsThreshold(other.itsThreshold) {
     fps(other.state->itsBand.itsFps);
 }
 RieszTransform::~RieszTransform() = default;
 
 void RieszTransform::fps(double value) {
     state->itsBand.itsFps = value;
     state->itsBand.computeFilter();
 }
 void RieszTransform::lowCutoff(double frequency) {
     state->itsBand.lowCutoff(frequency);
 }
 void RieszTransform::highCutoff(double frequency) {
     state->itsBand.highCutoff(frequency);
 }
 
 void RieszTransform::initialize(const cv::Mat &frame) {
     static const double scaleFactor = 1.0 / 255.0;
     frame.convertTo(itsFrame, CV_32F, scaleFactor);
     state->itsCurrent.initialize(itsFrame);
     state->itsPrior.initialize(itsFrame);
 }
 
 cv::Mat RieszTransform::transform(const cv::Mat &frame) {
     static const double PI_PERCENT = M_PI / 100.0;
     static const double scaleFactor = 1.0 / 255.0;
 
     frame.convertTo(itsFrame, CV_32F, scaleFactor);
     cv::Mat result;
 
     if (state->itsCurrent) {
         state->itsCurrent.build(itsFrame);
         state->itsCurrent.unwrapOrientPhase(state->itsPrior);
         state->itsBand.filterPyramids(state->itsCurrent, state->itsPrior);
         state->itsCurrent.amplify(itsAlpha, itsThreshold * PI_PERCENT);
         itsFrame = state->itsCurrent.collapse();
         itsFrame.convertTo(result, CV_8UC1, 255);
     } else {
         state->itsCurrent.initialize(itsFrame);
         state->itsPrior.initialize(itsFrame);
         frame.copyTo(result);
     }
     return result;
 }
 