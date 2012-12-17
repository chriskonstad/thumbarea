#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>
#include <qmath.h>
#include <math.h>
#include <QTime>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString versionString;
    versionString.append(QApplication::applicationVersion());
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

    //Seed the random number generator
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    dataFieldFont = scene->font();
    dataFieldFont.setPointSize(18);

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
    QRectF view = ui->graphicsView->sceneRect();
    if(lastCursorPosition != mouse)    //make sure someone is touching the screen
    {
        if(mouse.x() - ui->graphicsView->x() > view.left() && mouse.x() - ui->graphicsView->x() < view.right() && mouse.y() - ui->graphicsView->y() > view.top() && mouse.y() - ui->graphicsView->y() < view.bottom())
        {
            ui->graphicsView->scene()->addEllipse(mouse.x() - ui->graphicsView->x(), mouse.y() - ui->graphicsView->y(), 3, 3, QPen(), QBrush(Qt::red));
            dataListRaw.append(QPoint(mouse.x() - ui->graphicsView->x(), mouse.y() - ui->graphicsView->y()));
        }
        else
        {
            qDebug() << "Touch outside of the scene view!";
        }
        lastCursorPosition = mouse.toPoint();
    }
}

void MainWindow::on_pbReset_clicked()
{
    calcPPCM();

    loadSettings();
    dataListRaw.clear();
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
    dateItem->setFont(dataFieldFont);
    dateItem->setText(date.currentDate().toString("MMM dd yyyy"));
    dateItem->setPos(0,0);
    scene->addItem(dateItem);

    //Add patient info to data field
    QGraphicsSimpleTextItem * patientInfo = new QGraphicsSimpleTextItem;
    patientInfo->setFont(dataFieldFont);
    patientInfo->setText(patientInfoString);
    patientInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 50, 0);
    scene->addItem(patientInfo);

    //Add test info to data field
    QGraphicsSimpleTextItem * testInfo = new QGraphicsSimpleTextItem;
    testInfo->setFont(dataFieldFont);
    testInfo->setText(testInfoString);
    testInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 50 + patientInfo->boundingRect().width() + 50, 0);
    scene->addItem(testInfo);

    //Add trial (index) number
    QString trialString;
    trialString.append("Trial: ");
    trialString.append(QString::number(picIndex).rightJustified(2,'0'));
    QGraphicsSimpleTextItem * trialInfo = new QGraphicsSimpleTextItem;
    trialInfo->setFont(dataFieldFont);
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

void MainWindow::loadSettings()
{
    patientInfoString = settings->value("patientInfoString", "Patient Info").toString();
    testInfoString = settings->value("testInfoString", "Test Info").toString();
    diagonalCMDouble = settings->value("diagonalCM", 1).toDouble();
    qDebug() << "Settings loaded in MainWindow";

    drawDataFieldInformation(); //refresh the display to reflect updates
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

void MainWindow::on_pbAnalyze_clicked()
{
    static bool reAnalyze = false;

    if(reAnalyze)   //delete old analyzation
    {
        clearOldAnalyzation();
    }

    double romDegrees = 0;
    for(int i=0;i<10;i++)
    {
        clearOldAnalyzation();
        romDegrees = romDegrees + calcROM();
    }

    romDegrees = romDegrees/10;


    QGraphicsSimpleTextItem *romDisplay = new QGraphicsSimpleTextItem(0, scene);
    romDisplay->setFont(dataFieldFont);
    romDisplay->setText("Range of motion: " + QString::number(romDegrees, 'f', 1) + " degrees");
    romDisplay->setPos(ui->graphicsView->width()/2 - romDisplay->boundingRect().width()/2, ui->graphicsView->height()/2 - romDisplay->boundingRect().height()/2);

    reAnalyze = true;
}

void MainWindow::clearOldAnalyzation()
{
    QList<QGraphicsItem *> list = scene->items();
    foreach(QGraphicsItem *i, list)
    {
        QGraphicsEllipseItem *e = qgraphicsitem_cast<QGraphicsEllipseItem *>(i);
        if(e)
        {
            if(e->brush().color() == QColor(0,255,0,64))
            {
                scene->removeItem(i);
            }
        }
        QGraphicsLineItem *l = qgraphicsitem_cast<QGraphicsLineItem *>(i);
        if(l)
        {
            if(l->pen().color() == Qt::blue)
            {
                scene->removeItem(i);
            }
        }
        QGraphicsSimpleTextItem *t = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(i);
        if(t)
        {
            if(t->text().contains("Range"))
            {
                scene->removeItem(i);
            }
        }
    }
}

double MainWindow::calcROM()    //calculate the Range of Motion
{
    if(dataListRaw.count())
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
    else
        return -1;
}

QPointF MainWindow::calcCircle()
{
    //Prep work first, including gathering of data points
    QPointF center;
    center.setX(-1);
    center.setY(-1);

    if(dataListRaw.count())
    {
        QPointF roughCenter;
        roughCenter.setX(-1);
        roughCenter.setY(-1);

        QList<QPointF> roughCenterList;  //hold all calculated center points

        qDebug() << "---------- Starting to calculate center points ----------";
        QList<QPoint> dataListTemp = dataListRaw;
        while(dataListTemp.count() > 3)
        {
            int indexA = randomInt(0, dataListTemp.count() - 1);
            QPointF a = dataListTemp.takeAt(indexA);
            int indexB = randomInt(0, dataListTemp.count() - 1);
            QPointF b = dataListTemp.takeAt(indexB);
            int indexC = randomInt(0, dataListTemp.count() - 1);
            QPointF c = dataListTemp.takeAt(indexC);
            if(a.x() != b.x() && b.x() != c.x() && a.x() != c.x() && a.y() != b.y() && b.y() != c.y() && a.y() != c.y())
            {
                QPointF cP = calcCenter(a,b,c);
                if(cP.x() < - 3000 || cP.x() > 3000 || cP.y() < 0 || cP.y() > 3000)
                {
                    qDebug() << "Ignoring centerpoint at " << cP << " for being too far away from a reasonable value!";
                }
                else
                {
                    roughCenterList.append(cP);
                }
            }
            else
                qDebug() << "Raw point triplet was unusable. Discarding combination";
        }

        qDebug() << "---------- Finished calculating center points ----------";

        roughCenter = calcAveragePoint(roughCenterList);  //Calculate the rough average center point

        if(!roughCenterList.isEmpty())
        {
            center = calcAveragePoint(roughCenterList);
        }
        else
        {
            qWarning() << "ERROR: Center point list is empty!";
        }

        //Draw information on screen
        double radius = 0;
        foreach(QPointF p, dataListRaw)
        {
            radius = radius + calcDistance(center, p);
        }
        radius = radius / (double)dataListRaw.count();

        scene->addEllipse(center.x()-radius, center.y()-radius, 2 * radius, 2 * radius, QPen(Qt::black), QBrush(QColor(0,255,0,64)));

        rom.setRadius((double)radius);
        rom.setCenterPoint(center);

        qDebug() << "Radius (px): " << radius;
        qDebug() << "PPCM: " << calc->getPPCM();
    }

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

    Q_ASSERT(yDelta0 != 0);
    Q_ASSERT(yDelta1 != 0);
    Q_ASSERT(xDelta0 != 0);
    Q_ASSERT(xDelta1 != 0);

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

    qDebug() << "---------- Starting to calculate average point ----------";
    QPointF avgPnt = QPointF(0,0);

    foreach(QPointF p, l)
    {
        avgPnt.setX(avgPnt.x() + p.x());
        avgPnt.setY(avgPnt.y() + p.y());
    }

    avgPnt.setX(avgPnt.x() / (double)l.count());
    avgPnt.setY(avgPnt.y() / (double)l.count());
    qDebug() << "Average point: " << avgPnt;
    qDebug() << "---------- Calculated average point ----------";
    return avgPnt;
}

int MainWindow::randomInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void MainWindow::calcPPCM()
{
    calc->calculatePPCM(this->width(), this->height(), diagonalCMDouble);
}
