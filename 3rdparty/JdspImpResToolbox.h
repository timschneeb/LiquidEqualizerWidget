#ifndef JDSPIMPRESTOOLBOX_H
#define JDSPIMPRESTOOLBOX_H

namespace JdspImpResToolbox {
    int ComputeEqResponse(const double* jfreq, double* jgain, int interpolationMode, int queryPts, double* dispFreq, float* response);
}

#endif // JDSPIMPRESTOOLBOX_H
