#ifndef RANGEOFMOTION_H
#define RANGEOFMOTION_H

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

#include <QObject>
#include <QPointF>

class RangeOfMotion : public QObject
{
    Q_OBJECT
public:
    explicit RangeOfMotion(QObject *parent = 0);

signals:

public slots:
    void setCenterPoint(QPointF c) { centerPoint = c; }
    QPointF getCenterPoint() { return centerPoint; }

    void setRadius(double r) { radius = r; }
    double getRadius() { return radius; }

private:
    QPointF centerPoint;
    double radius;

};

#endif // RANGEOFMOTION_H
