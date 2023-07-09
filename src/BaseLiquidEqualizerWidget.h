/*
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  ThePBone <tim.schneeberger(at)outlook.de> (c) 2020
 */

#ifndef QBASEFLUENTEQUALIZERWIDGET_H
#define QBASEFLUENTEQUALIZERWIDGET_H

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QVariantAnimation>
#include <QVector>
#include <QWidget>

#include <cmath>

class BaseLiquidEqualizerWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool gridVisible READ getGridVisible WRITE setGridVisible NOTIFY redrawRequired)
	Q_PROPERTY(bool alwaysDrawHandles READ getAlwaysDrawHandles WRITE setAlwaysDrawHandles NOTIFY redrawRequired)
	Q_PROPERTY(QColor accentColor READ getAccentColor WRITE setAccentColor NOTIFY redrawRequired)
	Q_PROPERTY(int animationDuration READ getAnimationDuration WRITE setAnimationDuration NOTIFY redrawRequired)

public:
    ~BaseLiquidEqualizerWidget();
	void            setBand(int    i,
	                        double value,
	                        bool   animate                  = true,
	                        bool   manual                   = false);
	double          getBand(int i);
	void            setBands(const QVector<double> &vector,
	                         bool                   animate = true);
	QVector<double> getBands();

	bool            getGridVisible() const;
	void            setGridVisible(bool gridVisible);
	bool            getAlwaysDrawHandles() const;
	void            setAlwaysDrawHandles(bool alwaysDrawHandles);
	QColor          getAccentColor() const;
	void            setAccentColor(const QColor &accentColor);
	int             getAnimationDuration() const;
	void            setAnimationDuration(int animationDuration);

signals:
	void            mouseReleased();
	void            redrawRequired();
	void            bandsUpdated();
	void            cancelAnimations();

protected:
    BaseLiquidEqualizerWidget(int bandsNum, int minFreq, int maxFreq, double minDb, double maxDb, int resolution, double horizLineInterval, QWidget *parent = nullptr);


    virtual void    computeCurve(const double* freqs, double* gains, int resolution, double* dispFreq, float* response) = 0;
    virtual QVector<double> getFrequencyPoints() = 0;

	void            paintEvent(QPaintEvent *event) override;
	void            mousePressEvent(QMouseEvent *event) override;
	void            mouseMoveEvent(QMouseEvent *event) override;
	void            mouseReleaseEvent(QMouseEvent *event) override;
	void            keyPressEvent(QKeyEvent *event) override;
	void            keyReleaseEvent(QKeyEvent *event) override;
	void            mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QList<QVariantAnimation*> mAnimators;

	QPainter mGridLines;
	QPainter mControlBarText;
	QPainter mFrequencyResponseBg;
	QPainter mFrequencyResponseHighlight;
	QPainter mControlBarKnob;
    double *mLevels       = nullptr;
    double *mActualLevels = nullptr;
	int mHeight           = 0;
	int mWidth            = 0;
	int mSelectedBand     = 0;
	bool mManual          = false;
	bool mHoldDown        = false;

	int mKey_CurrentIndex;
	QPoint mKey_LastMousePos;

	bool mGridVisible       = true;
	bool mAlwaysDrawHandles = false;
	QColor mAccentColor     = QColor(98, 0, 238, 255);
	int mAnimationDuration  = 500;

    int mBandsNum;
    int mResolution;
    int mMinFreq;
    int mMaxFreq;
    double mMinDb;
    double mMaxDb;
    double mHorizLineInterval;
    QVector<double> mFreqs;

	double *dispFreq;
	float *response;
	float *precomputeCurveXAxis;
	float *precomputeFreqAxis = new float[64];

	inline double reverseProjectX(double position)
	{
        double minimumPosition = log(mMinFreq);
        double maximumPosition = log(mMaxFreq);
		return exp(position * (maximumPosition - minimumPosition) + minimumPosition);
	}

	inline float projectX(double frequency)
	{
		double position        = log(frequency);
        double minimumPosition = log(mMinFreq);
        double maximumPosition = log(mMaxFreq);
		return (position - minimumPosition) / (maximumPosition - minimumPosition);
	}

	inline float projectY(float dB)
	{
        float pos = (dB - mMinDb) / (mMaxDb - mMinDb);
		return 1.0f - pos;
	}

	inline float reverseProjectY(float pos)
	{
		float i = abs(pos - 1);
        return -i * mMinDb + i * mMaxDb + mMinDb;
	}

	inline float lin2dB(double rho)
	{
		if (rho != 0.0)
		{
			return log(rho) / log(10.0) * 20;
		}
		else
		{
			return -99.9f;
		}
	}

    inline int getIndexUnderMouse(int pos_x)
	{
		int    idx  = 0;
		double best = 1e8;

        for (int i = 0; i < mBandsNum; i++)
		{
            double freq     = mFreqs[i];
			double cx       = projectX(freq) * mWidth;
			double distance = abs(cx - pos_x);

			if (distance < best)
			{
				idx  = i;
				best = distance;
			}
		}

		return idx;
    }

};

#endif // QBASEFLUENTEQUALIZERWIDGET_H
