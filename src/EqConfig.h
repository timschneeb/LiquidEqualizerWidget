#ifndef EQCONFIG_H
#define EQCONFIG_H

#define RESOLUTION    128
#define BANDS_NUM     15
#define MIN_DB        -12
#define MAX_DB        12
#define MIN_FREQ      21.0
#define MAX_FREQ      24000.0
#define SAMPLING_RATE (MAX_FREQ * 2)

#ifdef __cplusplus
#include <QVector>
const static QVector<double> mFreq({ 25.0f, 40.0f, 63.0f, 100.0f, 160.0f, 250.0f, 400.0f, 630.0f, 1000.0f, 1600.0f, 2500.0f, 4000.0f, 6300.0f, 10000.0f, 16000.0f });
#endif

#endif // EQCONFIG_H
