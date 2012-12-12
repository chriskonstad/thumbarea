#include "rangeofmotion.h"

RangeOfMotion::RangeOfMotion(QObject *parent) :
    QObject(parent)
{
    radius = -1;
    centerPoint.setX(-1);
    centerPoint.setY(-1);
}
