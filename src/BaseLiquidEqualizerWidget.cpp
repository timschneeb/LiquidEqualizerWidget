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

    ThePBone <tim.schneeberger(at)outlook.de> (c) 2020

    Design inspired by
    https://github.com/vipersaudio/viper4android_fx
 */

#include "BaseLiquidEqualizerWidget.h"

#include <QDebug>
#include <QLinearGradient>
#include <QPainterPath>
#include <cstring>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BaseLiquidEqualizerWidget::BaseLiquidEqualizerWidget(int bandsNum, int minFreq, int maxFreq, double minDb, double maxDb, int resolution, double horizLineInterval, QWidget *parent)
    : QWidget(parent), mBandsNum(bandsNum), mResolution(resolution), mMinFreq(minFreq), mMaxFreq(maxFreq), mMinDb(minDb), mMaxDb(maxDb), mHorizLineInterval(horizLineInterval)
{
    mLevels = new double[mBandsNum];
    mActualLevels = new double[mBandsNum];
    std::memset(mLevels, 0, mBandsNum);
    std::memset(mActualLevels, 0, mBandsNum);

    mAnimators.reserve(mBandsNum);
    for (int i = 0; i < mBandsNum; i++)
    {
        mAnimators.append(new QVariantAnimation(this));
	}

    dispFreq             = new double[resolution];
    response             = new float[resolution];
    precomputeCurveXAxis = new float[resolution];

    for (int i = 0; i < resolution; i++)
	{
        dispFreq[i] = reverseProjectX(i / (float) (resolution - 1));
	}

    for (int i = 0; i < resolution; i++)
	{
		precomputeCurveXAxis[i] = projectX(dispFreq[i]);
	}

	int i = 0;

    for (int freq = minFreq; freq < maxFreq;)
	{
		precomputeFreqAxis[i] = projectX(freq);

        if (freq < 100)
		{
			freq += 10;
		}
		else if (freq < 1000)
		{
			freq += 100;
		}
		else if (freq < 10000)
		{
			freq += 1000;
		}
		else
		{
			freq += 10000;
		}

		i++;
	}

    connect(this, &BaseLiquidEqualizerWidget::redrawRequired, [this]() {
		repaint();
	});
}

BaseLiquidEqualizerWidget::~BaseLiquidEqualizerWidget()
{
    delete [] dispFreq;
    delete [] response;
    delete [] precomputeCurveXAxis;
    delete [] precomputeFreqAxis;
    delete [] mLevels;
    delete [] mActualLevels;
}

void BaseLiquidEqualizerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	float y = (float) event->y() / (float) height();

	if (y < 0 || y > 1)
	{
		return;
	}

	float dB = reverseProjectY(y);
    setBand(getIndexUnderMouse(event->x()), dB, false);
}

int BaseLiquidEqualizerWidget::getAnimationDuration() const
{
	return mAnimationDuration;
}

void BaseLiquidEqualizerWidget::setAnimationDuration(int animationDuration)
{
	mAnimationDuration = animationDuration;
}

QColor BaseLiquidEqualizerWidget::getAccentColor() const
{
	return mAccentColor;
}

void BaseLiquidEqualizerWidget::setAccentColor(const QColor &accentColor)
{
	mAccentColor = accentColor;
	emit redrawRequired();
}

bool BaseLiquidEqualizerWidget::getAlwaysDrawHandles() const
{
	return mAlwaysDrawHandles;
}

void BaseLiquidEqualizerWidget::setAlwaysDrawHandles(bool alwaysDrawHandles)
{
	mAlwaysDrawHandles = alwaysDrawHandles;
	emit redrawRequired();
}

bool BaseLiquidEqualizerWidget::getGridVisible() const
{
	return mGridVisible;
}

void BaseLiquidEqualizerWidget::setGridVisible(bool gridVisible)
{
	mGridVisible = gridVisible;
	emit redrawRequired();
}

void BaseLiquidEqualizerWidget::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	mHoldDown = true;
}

void BaseLiquidEqualizerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	mHoldDown = false;
	emit bandsUpdated();
	emit mouseReleased();
	repaint();
}

void BaseLiquidEqualizerWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (mHoldDown)
	{
		float y = (float) event->y() / (float) height();

		if (y < 0 || y > 1)
		{
			return;
		}

		float dB = reverseProjectY(y);
        setBand(getIndexUnderMouse(event->x()), dB, false, true);
	}
}

void BaseLiquidEqualizerWidget::keyPressEvent(QKeyEvent *event)
{
	QPoint point      = this->mapFromGlobal(QCursor::pos());
    int    index      = getIndexUnderMouse(point.x());

	QRect  widgetRect = this->geometry();
	widgetRect.moveTopLeft(this->parentWidget()->mapToGlobal(widgetRect.topLeft()));

	if (widgetRect.contains(QCursor::pos()))
	{
		mHoldDown = true;

		if (mKey_LastMousePos != QCursor::pos())
		{
			mKey_CurrentIndex = index;
		}

		if (event->key() == Qt::Key::Key_Up)
		{
			float cur = getBand(mKey_CurrentIndex);
            setBand(mKey_CurrentIndex, cur >= mMaxDb ? mMaxDb : cur + 0.5, false, true);
			emit  bandsUpdated();
		}
		else if (event->key() == Qt::Key::Key_Down)
		{
			float cur = getBand(mKey_CurrentIndex);
            setBand(mKey_CurrentIndex, cur <= mMinDb ? mMinDb : cur - 0.5, false, true);
			emit  bandsUpdated();
		}
		else if (event->key() == Qt::Key::Key_Left && mKey_LastMousePos == QCursor::pos())
		{
			if (mKey_CurrentIndex > 0)
			{
				mKey_CurrentIndex--;
			}

			setBand(mKey_CurrentIndex, getBand(mKey_CurrentIndex), false, true);
		}
		else if (event->key() == Qt::Key::Key_Right && mKey_LastMousePos == QCursor::pos())
		{
            if (mKey_CurrentIndex < mBandsNum)
			{
				mKey_CurrentIndex++;
			}

			setBand(mKey_CurrentIndex, getBand(mKey_CurrentIndex), false, true);
		}
	}

	mKey_LastMousePos = QCursor::pos();
}

void BaseLiquidEqualizerWidget::keyReleaseEvent(QKeyEvent *event)
{
	Q_UNUSED(event)
	QPoint point = this->mapFromGlobal(QCursor::pos());
    int index = getIndexUnderMouse(point.x());

	if (index == mKey_CurrentIndex)
	{
		mHoldDown = false;
		repaint();
	}
}

void BaseLiquidEqualizerWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	mWidth  = this->width() + 1;
	mHeight = this->height() + 1;

	QPainterPath frequencyResponse;
	float        x, y;

    if(mFreqs.empty()) {
        mFreqs = getFrequencyPoints();
    }

    computeCurve(mFreqs.constData(), mLevels, mResolution, dispFreq, response);
    for (int i = 0; i < mResolution; i++)
	{
		/* Magnitude response, dB */
		x = precomputeCurveXAxis[i] * mWidth;
		y = projectY(response[i]) * mHeight;

		/* Set starting point at first point */
		if (i == 0)
		{
			frequencyResponse.moveTo(x, y);
		}
		else
		{
			frequencyResponse.lineTo(x, y);
		}
	}

	QPainterPath    frequencyResponseBackground;
	frequencyResponseBackground.addPath(frequencyResponse);
	frequencyResponseBackground.lineTo(mWidth, mHeight);
	frequencyResponseBackground.lineTo(0.0f,   mHeight);

	QLinearGradient gradient(QPoint(width(), 0), QPoint(width(), height()));
	gradient.setColorAt(0.0, mAccentColor);
	gradient.setColorAt(1.0, QColor(0, 0, 0, 0));

	if (mGridVisible)
	{
		mGridLines.begin(this);
		mGridLines.setPen(QPen(palette().mid(), 0.75, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
		mGridLines.setRenderHint(QPainter::RenderHint::Antialiasing, true);

        float decibel = mMinDb + mHorizLineInterval;

        while (decibel <= mMaxDb - mHorizLineInterval)
		{
			float y = projectY(decibel) * mHeight;
			mGridLines.drawLine(0.0f, y, mWidth - 1, y);
            decibel += mHorizLineInterval;
		}

		mGridLines.end();
	}

	mFrequencyResponseBg.begin(this);
	mFrequencyResponseBg.setBrush(gradient);
	mFrequencyResponseBg.setRenderHint(QPainter::RenderHint::Antialiasing, true);
	mFrequencyResponseBg.drawPath(frequencyResponseBackground);
	mFrequencyResponseBg.end();

	mFrequencyResponseHighlight.begin(this);
	mFrequencyResponseHighlight.setPen(QPen(QBrush(mAccentColor), 3, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
	mFrequencyResponseHighlight.setRenderHint(QPainter::RenderHint::Antialiasing, true);
	mFrequencyResponseHighlight.drawPath(frequencyResponse);
	mFrequencyResponseHighlight.end();

    for (int i = 0; i < mBandsNum; i++)
	{
        double  frequency = mFreqs[i];
		double  x         = projectX(frequency) * mWidth;
		double  y         = projectY(mLevels[i]) * mHeight;

		QString frequencyText = QString::asprintf(frequency < 1000 ? "%.0f" : "%.0fk", frequency < 1000 ? frequency : frequency / 1000);
        QString gainText = QString::asprintf("%.1f", mLevels[i]);

		if ((mManual && i == mSelectedBand && mHoldDown) || mAlwaysDrawHandles)
		{
			mControlBarKnob.begin(this);
			mControlBarKnob.setBrush(mAccentColor);
			mControlBarKnob.setPen(mAccentColor.lighter(50));
			mControlBarKnob.setRenderHint(QPainter::RenderHint::Antialiasing, true);
			mControlBarKnob.drawEllipse(x - 8, y - 9, 18.0f, 18.0f);
			mControlBarKnob.end();
		}

		mControlBarText.begin(this);

		QFont font = mControlBarText.font();
		font.setPointSize(8);

        mControlBarText.setFont(font);
		mControlBarText.drawText(x, 19,              gainText);
		mControlBarText.drawText(x, mHeight - 16.0f, frequencyText);
		mControlBarText.end();

#ifdef UI_DEBUG
        mGridLines.begin(this);
        mGridLines.setPen(QPen(Qt::green, 0.75, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
        mGridLines.setRenderHint(QPainter::RenderHint::Antialiasing, true);
        mGridLines.drawLine(x, 0, x, mHeight - 1);
        mGridLines.end();
#endif
	}

#ifdef UI_DEBUG
    auto cursor = mapFromGlobal(QCursor::pos());
    mGridLines.begin(this);
    mGridLines.setPen(QPen(Qt::red, 0.75, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
    mGridLines.setRenderHint(QPainter::RenderHint::Antialiasing, true);
    mGridLines.drawLine(0, cursor.y(), mWidth - 1, cursor.y());
    mGridLines.end();
#endif
}

void BaseLiquidEqualizerWidget::setBand(int    i,
                                    double value,
                                    bool   animate,
                                    bool   manual)
{
	mSelectedBand    = i;
	mManual          = manual;

	mActualLevels[i] = value;

	if (animate)
	{
        if (mAnimators[i] != nullptr)
		{
            mAnimators[i]->stop();
		}

        mAnimators[i] = new QVariantAnimation(this);
        mAnimators[i]->setDuration(500);
        mAnimators[i]->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCirc));
        mAnimators[i]->setStartValue(mLevels[i]);
        mAnimators[i]->setEndValue(value);
        mAnimators[i]->setDirection(QVariantAnimation::Direction::Forward);

        connect(mAnimators[i], QOverload<const QVariant &>::of(&QVariantAnimation::valueChanged), this, [this, i](const QVariant &v) {
			mLevels[i] = v.toFloat();
			repaint();
		});

        mAnimators[i]->start();

	}
	else
	{
		mLevels[i] = value;
		repaint();
	}
}

double BaseLiquidEqualizerWidget::getBand(int i)
{
	return mActualLevels[i];
}

void BaseLiquidEqualizerWidget::setBands(const QVector<double> &vector,
                                     bool                   animate)
{
    int availableBandCount = (vector.count() < mBandsNum) ? vector.count() - 1 : mBandsNum;

	for (int i = 0; i < availableBandCount; i++)
	{
		setBand(i, vector.at(i), animate);
	}

	repaint();
}

QVector<double> BaseLiquidEqualizerWidget::getBands()
{
	QVector<double> vector;

    for (int i = 0; i < mBandsNum; i++)
	{
		vector.push_back(mActualLevels[i]);
	}

	return vector;
}
