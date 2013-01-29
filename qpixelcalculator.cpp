/*Copyright (c) <2013> <Chris Konstad (chriskon149@gmail.com)>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
