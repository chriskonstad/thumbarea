#ifndef QPIXELCALCULATOR_H
#define QPIXELCALCULATOR_H

#include <QObject>

class QPixelCalculator : public QObject
{
    Q_OBJECT
public:
    explicit QPixelCalculator(QObject *parent = 0);
    
signals:
    
public slots:
    double calculatePPCM(int width, int height, double diagonalScreenSizeCM);  //find out how many pixels per centimeter
    
    double getPPCM() { return ppCM; }   //return an already calculated pixels-per-centimeter

    double getPointToCM(int pixelInput);    //convert a pixel reading to a centimeter reading

private:
    double ppCM;
};

#endif // QPIXELCALCULATOR_H
