#include "qpixelcalculator.h"
#include "qmath.h"

QPixelCalculator::QPixelCalculator(QObject *parent) :
    QObject(parent)
{
    ppCM = -1;
}

double QPixelCalculator::calculatePPCM(int width, int height, double diagonalScreenSizeCM)
{
    long widthSquared = width * width;
    long heightSquared = height * height;

    double widthHeight = (double)widthSquared + (double)heightSquared;
    double diagonalResolution = sqrt(widthHeight);

    ppCM = diagonalResolution / diagonalScreenSizeCM;
    return ppCM;
}

double QPixelCalculator::getPointToCM(int pixelInput)
{
    if(ppCM > 0)
    {
        return (double)pixelInput / ppCM;
    }
    else
        return -1;
}
