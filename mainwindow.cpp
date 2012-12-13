#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>
#include <qmath.h>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString versionString;
    versionString.append(APP_VERSION);
    qDebug() << versionString;
    ui->labVersion->setText("Version: " + versionString);
    cursor = new QCursor();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePos()));

    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    calc = new QPixelCalculator(this);

    patientInfoString = "PatientInfo";
    testInfoString = "TestInfo";
    settings = new QSettings(this);
    picIndex = 1;
    diagonalCMDouble = 1;

    timer->start(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePos()
{
    static QPoint lastCursorPosition;
    QPointF mouse = cursor->pos();
    if(lastCursorPosition != mouse)    //make sure someone is touching the screen
    {
        ui->graphicsView->scene()->addEllipse(mouse.x() - ui->graphicsView->x(), mouse.y() - ui->graphicsView->y(), 3, 3, QPen(), QBrush(Qt::red));
        lastCursorPosition = mouse.toPoint();
        dataListRaw.append(QPoint(mouse.x() - ui->graphicsView->x(), mouse.y() - ui->graphicsView->y()));
    }
}

void MainWindow::calcPPCM()
{
    calc->calculatePPCM(this->width(), this->height(), diagonalCMDouble);
}

void MainWindow::on_pbCalibrate_clicked()
{
    calcPPCM();

    loadSettings();
    dataListRaw.clear();

    if(ui->pbCalibrate->text() != "Reset")
    {
        ui->pbCalibrate->setText("Reset");
    }
}

void MainWindow::drawDataFieldInformation()
{
    scene->clear();
    scene->setSceneRect(0,0,ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->resetTransform();

    calcPPCM();

    for(int i=0; i<ui->graphicsView->width();i = i + (int)calc->getPPCM())
    {
        scene->addLine(i, 0, i, ui->graphicsView->height(), QPen(QBrush(Qt::gray), 1)); //vertical lines
    }
    for(int i=0; i<ui->graphicsView->height();i = i+ (int)calc->getPPCM())
    {
        scene->addLine(0, i, ui->graphicsView->width(), i, QPen(QBrush(Qt::gray), 1));  //horizontal lines
    }

    //Add date to data field
    QDate date;
    QGraphicsSimpleTextItem * dateItem = new QGraphicsSimpleTextItem;
    dateItem->setText(date.currentDate().toString("MMM dd yyyy"));
    dateItem->setPos(0,0);
    scene->addItem(dateItem);

    //Add patient info to data field
    QGraphicsSimpleTextItem * patientInfo = new QGraphicsSimpleTextItem;
    patientInfo->setText(patientInfoString);
    patientInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 50, 0);
    scene->addItem(patientInfo);

    //Add test info to data field
    QGraphicsSimpleTextItem * testInfo = new QGraphicsSimpleTextItem;
    testInfo->setText(testInfoString);
    testInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 50 + patientInfo->boundingRect().width() + 50, 0);
    scene->addItem(testInfo);

    //Add trial (index) number
    QString trialString;
    trialString.append("Trial: ");
    trialString.append(QString::number(picIndex).rightJustified(2,'0'));
    QGraphicsSimpleTextItem * trialInfo = new QGraphicsSimpleTextItem;
    trialInfo->setText(trialString);
    trialInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 50 + patientInfo->boundingRect().width() + 50 + testInfo->boundingRect().width() + 50, 0);
    scene->addItem(trialInfo);

    QList<QGraphicsItem *> items = scene->items();
    foreach(QGraphicsItem *i, items)
    {
        i->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }
    qDebug() << "Redrew data field static components";
}

void MainWindow::on_pbSaveData_clicked()
{
    timer->stop();  //pause the data-gathering
    ui->graphicsView->viewport()->update(); //update data field

    QDir photoDir("/mnt/sdcard/thumbdata");
    if(!photoDir.exists())
    {
        if(photoDir.mkdir("/mnt/sdcard/thumbdata") == false)    //if couldn't make dir
        {
            errorDialog = new ErrorDialog(this);
            errorDialog->exec();
            qDebug() << "Error: Could not find or create the folder to save the data in.";
        }
    }

    QString fileName = photoDir.absolutePath() + "/" + patientInfoString + "-" + testInfoString + QString::number(picIndex).rightJustified(2,'0') + ".png";
    QPixmap pixMap = QPixmap::grabWidget(ui->graphicsView);
    if(pixMap.save(fileName))
    {
        savedDialog = new SavedDialog(this);
        savedDialog->exec();
        qDebug() << "Info: Picture saved as " + fileName;
    }
    else
    {
        errorDialog = new ErrorDialog(this);
        errorDialog->exec();
        qDebug() << "Error: Couldn't save the pixmap";
    }

    picIndex++;

    timer->start(); //resume the data-gathering
}

void MainWindow::loadSettings()
{
    patientInfoString = settings->value("patientInfoString", "Patient Info").toString();
    testInfoString = settings->value("testInfoString", "Test Info").toString();
    diagonalCMDouble = settings->value("diagonalCM", 1).toDouble();
    qDebug() << "Settings loaded in MainWindow";

    drawDataFieldInformation(); //refresh the display to reflect updates
}

void MainWindow::on_pbSettings_clicked()
{
    loadSettings(); //fix CTD bug?
    settingsDialog = new SettingsDialog(patientInfoString, testInfoString, diagonalCMDouble, this);
    connect(settingsDialog, SIGNAL(patientInfo(QString)), this, SLOT(patientInfo(QString)));
    connect(settingsDialog, SIGNAL(testInfo(QString)), this, SLOT(testInfo(QString)));
    connect(settingsDialog, SIGNAL(diagonalCM(double)), this, SLOT(diagonalCM(double)));
    connect(settingsDialog, SIGNAL(accepted()), this, SLOT(resetPicIndex()));
    connect(settingsDialog, SIGNAL(accepted()), timer, SLOT(start()));
    connect(settingsDialog, SIGNAL(rejected()), timer, SLOT(start()));
    timer->stop();  //pause the data-gathering
    settingsDialog->exec();
}

void MainWindow::resetPicIndex()
{
    picIndex = 1;   //reset counter
    loadSettings(); //load the settings
    qDebug() << "Reset counter and called loadSettings()";
}

void MainWindow::patientInfo(QString patient)
{
    patientInfoString = patient;
    settings->setValue("patientInfoString", patientInfoString);
    settings->sync();
    qDebug() << "Patient info copied from Settings dialog";
}

void MainWindow::testInfo(QString test)
{
    testInfoString = test;
    settings->setValue("testInfoString", testInfoString);
    settings->sync();
    qDebug() << "Test info copied from Settings dialog";
}

void MainWindow::diagonalCM(double cm)
{
    diagonalCMDouble = cm;
    settings->setValue("diagonalCM", diagonalCMDouble);
    settings->sync();
    qDebug() << "diagonalCMDouble copied from Settings dialog";
}

QPointF MainWindow::calcCircle()
{
    //Prep work first, including gathering of data points
    QPointF center;
    center.setX(-1);
    center.setY(-1);

    int sectionBreak = dataListRaw.count()/10;  //break the datalist into 10 sections

    QList<QPointF> centerList;

    //find average center
    int numCenters = 0;

    for(int i= 0;i<20;i++)
    {
        int indexA = qrand() % (dataListRaw.count());
        int indexB = qrand() % (dataListRaw.count());
        int indexC = qrand() % (dataListRaw.count());
        QPointF a = dataListRaw.at(indexA);
        QPointF b = dataListRaw.at(indexB);
        QPointF c = dataListRaw.at(indexC);
        if(a.x() != b.x() && b.x() != c.x() && a.x() != c.x() && a.y() != b.y() && b.y() != c.y() && a.y() != c.y())
        {
            numCenters++;
            centerList.append(calcCenter(a,b,c));
        }
    }

    double xTotal = 0;
    double yTotal = 0;

    foreach(QPointF p, centerList)
    {
        xTotal += p.x();
        yTotal += p.y();
    }

    center.setX((int)xTotal/numCenters);
    center.setY((int)yTotal/numCenters);

    //Draw information on screen
    QPointF testPoint = dataListRaw.at(sectionBreak*3);
    int radius = sqrt( ((center.x() - testPoint.x()) * (center.x() - testPoint.x())) + ((center.y() - testPoint.y()) * (center.y() - testPoint.y())) );
    scene->addEllipse(center.x()-radius, center.y()-radius, 2 * radius, 2 * radius, QPen(Qt::black), QBrush(QColor(0,255,0,64)));

    rom.setRadius((double)radius);
    rom.setCenterPoint(center);

    qDebug() << "Radius (px): " << radius;
    qDebug() << "PPCM: " << calc->getPPCM();

    return center;
}

QPointF MainWindow::calcCenter(QPointF a, QPointF b, QPointF c)
{
    QPointF centerTemp;

    qDebug() << a << "; " << b << "; " << c;
    double yDelta0 = b.y() - a.y();
    double xDelta0 = b.x() - a.x();
    double yDelta1 = c.y() - b.y();
    double xDelta1 = c.x() - b.x();

    double slope0 = yDelta0/xDelta0;
    double slope1 = yDelta1/xDelta1;
    double xD = 0;
    double yD = 0;

    xD = ( (slope0 * slope1 * (a.y() - c.y())) + (slope1 * (a.x() + b.x())) - (slope0 * (b.x() + c.x())) ) / (2.0 *(slope1 - slope0));
    yD = -1.0 * (xD - ((a.x() + b.x()) / 2 )) / slope0 + ((a.y() +b.y()) / 2);

    centerTemp.setX((int)xD);
    centerTemp.setY((int)yD);

    qDebug() << "Calculated center here: " << centerTemp;
    return centerTemp;
}

double MainWindow::calcROM()    //calculate the Range of Motion
{
    double romDegrees = -1;

    calcCircle();

    QPointF furthestPoint = QPointF(-1, -1);
    QPoint lastPoint = QPoint(0,0);

    foreach(QPoint p, dataListRaw)  //find the furthest point down
    {
        if(p.y() >= lastPoint.y())
        {
            furthestPoint.setX(p.x());
            furthestPoint.setY(p.y());
            lastPoint = p;
        }
        else
        {

        }
    }

    scene->addLine(rom.getCenterPoint().x(), rom.getCenterPoint().y(), furthestPoint.x(), furthestPoint.y(), QPen(Qt::blue));
    qDebug() << "Furthest Point: " << furthestPoint;

    QPointF verticalPoint = rom.getCenterPoint();

    verticalPoint.setY(verticalPoint.y() - rom.getRadius());
    scene->addLine(rom.getCenterPoint().x(), rom.getCenterPoint().y(), verticalPoint.x(), verticalPoint.y(), QPen(Qt::blue));
    qDebug() << "Vertical Point: " << verticalPoint;

    double deltaX = verticalPoint.x() - furthestPoint.x();
    double deltaY = verticalPoint.y() - furthestPoint.y();

    double chordLength = sqrt( (deltaX * deltaX) + (deltaY * deltaY) );

    double thetaR = qAcos( ( (chordLength * chordLength) - (2 * (rom.getRadius() * rom.getRadius())) ) / ( -2 * (rom.getRadius() * rom.getRadius())) );

    double thetaD = 180.0 * thetaR / M_PI;

    romDegrees = thetaD;

    qDebug() << "Range of Motion degrees: " << romDegrees;

    return romDegrees;
}

void MainWindow::on_pbAnalyze_clicked()
{
    calcROM();
}

double MainWindow::calcDistance(QPointF a, QPointF b)
{
    double deltaX = a.x() - b.x();
    double deltaY = a.y() - b.y();

    double d = sqrt( (deltaX * deltaX) + (deltaY * deltaY) );

    return d;
}

double MainWindow::calcDistance(QPoint a, QPoint b)
{
    double deltaX = (double)a.x() - (double)b.x();
    double deltaY = (double)a.y() - (double)b.y();

    double d = sqrt( (deltaX * deltaX) + (deltaY * deltaY) );

    return d;
}

QPointF MainWindow::calcAveragePoint(QList<QPointF> l)
{
    QPointF avgPnt = QPointF(0,0);

    foreach(QPointF p, l)
    {
        avgPnt.setX(avgPnt.x() + p.x());
        avgPnt.setY(avgPnt.y() + p.y());
    }

    avgPnt.setX(avgPnt.x() / (double)l.count());
    avgPnt.setY(avgPnt.y() / (double)l.count());

    return avgPnt;
}
