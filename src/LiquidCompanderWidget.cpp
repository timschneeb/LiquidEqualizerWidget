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

#include "LiquidCompanderWidget.h"

#include "../3rdparty/JdspImpResToolbox.h"

LiquidCompanderWidget::LiquidCompanderWidget(QWidget *parent) : BaseLiquidEqualizerWidget(7, 21, 24000, -1.2, 1.2, 128, 0.2, parent){}

LiquidCompanderWidget::~LiquidCompanderWidget(){}

void LiquidCompanderWidget::computeCurve(const double *freqs, double *gains, int resolution, double *dispFreq, float *response)
{
    JdspImpResToolbox::ComputeCompResponse(freqs, gains, resolution, dispFreq, response);
}

QVector<double> LiquidCompanderWidget::getFrequencyPoints()
{
    return QVector<double>({ 95.0f, 200.0f, 400.0f, 800.0f, 1600.0f, 3400.0f, 7500.0f });
}


