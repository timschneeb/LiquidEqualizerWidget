#include "LiquidEqualizerWidget.h"

#include <QApplication>
#include <QStyle>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LiquidEqualizerWidget* w = new LiquidEqualizerWidget();

    //Force standard color palette
    QApplication::setPalette(w->style()->standardPalette());

    //Set bands
    w->setBands(QVector<double>(std::initializer_list<double>({12.0,2.0,-1.0,1.0,2.0,3.0,0.0})));

    //Inherit accent color from palette
    w->setAccentColor(w->palette().highlight().color());
    w->setGridVisible(true);
    w->show();

    //Randomize bands every 3 seconds
    QTimer* timer = new QTimer();
    srand(time(NULL));
    QObject::connect(timer,&QTimer::timeout,[=](){
        QVector<double> nums;
        for (int i = 0; i < 15; ++i)
            nums.push_back((double)rand()/(double)(RAND_MAX/24)-12);

        w->setBands(nums);
        timer->start(3000);
    });
    timer->start(3000);

    return a.exec();
}
