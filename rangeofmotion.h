#ifndef RANGEOFMOTION_H
#define RANGEOFMOTION_H

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
