#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include <QMainWindow>
#include <QTimer>
#include <QCursor>
#include "qpixelcalculator.h"
#include <QDate>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "errordialog.h"
#include "saveddialog.h"
#include <QString>
#include <QSettings>
#include "settingsdialog.h"
#include <QGraphicsEllipseItem>
#include <QPoint>
#include <QPointF>
#include "rangeofmotion.h"
#include <QtAlgorithms>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPixelCalculator *calc;

signals:
    void analysisCompleted();

public slots:
    void updatePos();
    void calcPPCM();
    void loadSettings();
    void resetPicIndex();
    void patientInfo(QString patient);
    void testInfo(QString test);
    void diagonalCM(double cm);
    QPointF calcCircle(QList<QPoint> data);   //do math to draw circle
    QPointF calcCenter(QPointF a, QPointF b, QPointF c);    //calculate the center of a circle with 3 points
    double calcROM();   //Calculate the Range of Motion
    double calcDistance(QPointF a, QPointF b);  //calculate the distance between two points
    double calcDistance(QPoint a, QPoint b);    //calculate the distance between two points
    QPointF calcAveragePoint(QList<QPointF> l); //return the average point from a list of points
    QPoint calcAveragePoint(QList<QPoint> l);
    int randomInt(int low, int high);
    void clearOldAnalysis();
    QList<QPoint> removeOutliers(QList<QPoint> data);

    bool saveFile(QString fileName, QPixmap pixMap);
    void showErrorDialog();
    void showSavedDialog(QString fileName);

    void drawDataFieldInformation();

    void on_pbSaveData_clicked();

    void on_pbSettings_clicked();

    void on_pbAnalyze_clicked();

    void on_pbReset_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QCursor *cursor;

    QGraphicsScene *scene;

    ErrorDialog *errorDialog;

    SavedDialog *savedDialog;

    QSettings *settings;

    SettingsDialog *settingsDialog;

    QString patientInfoString;
    QString testInfoString;
    int picIndex;
    double diagonalCMDouble;

    QList<QPoint> dataListRaw;

    RangeOfMotion rom;

    QFont dataFieldFont;

    QGraphicsPixmapItem *indicator;
};

#endif // MAINWINDOW_H
