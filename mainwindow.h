#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

public slots:
    void updatePos();
    void calcPPCM();
    void loadSettings();
    void resetPicIndex();
    void patientInfo(QString patient);
    void testInfo(QString test);
    void diagonalCM(double cm);
    QPointF calcCircle();   //do math to draw circle
    QPointF calcCenter(QPointF a, QPointF b, QPointF c);    //calculate the center of a circle with 3 points
    double calcROM();   //Calculate the Range of Motion
    double calcDistance(QPointF a, QPointF b);  //calculate the distance between two points
    double calcDistance(QPoint a, QPoint b);    //calculate the distance between two points
    QPointF calcAveragePoint(QList<QPointF> l); //return the average point from a list of points
    int randomInt(int low, int high);
private slots:
    void on_pbCalibrate_clicked();

    void drawDataFieldInformation();

    void on_pbSaveData_clicked();

    void on_pbSettings_clicked();

    void on_pbAnalyze_clicked();

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

};

#endif // MAINWINDOW_H
