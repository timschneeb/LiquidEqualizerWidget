#ifndef JDSPIMPRESTOOLBOX_H
#define JDSPIMPRESTOOLBOX_H

namespace JdspImpResToolbox {
    int ComputeEqResponse(const double* jfreq, double* jgain, int interpolationMode, int queryPts, double* dispFreq, float* response);
    int ComputeCompResponse(const double* jfreq, const double* jgain, int queryPts, const double* dispFreq, float* response);
}

#endif // JDSPIMPRESTOOLBOX_H
