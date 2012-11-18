#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCursor>
#include "qpixelcalculator.h"
#include <QDate>
#include <QGraphicsItem>
#include <QGraphicsScene>

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

private slots:
    void on_pbCalibrate_clicked();

    void drawDataFieldInformation();

    void on_pbSaveData_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QCursor *cursor;

    QGraphicsScene *scene;

    QGraphicsSimpleTextItem * dateItem;

};

#endif // MAINWINDOW_H
