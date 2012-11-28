#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>

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
    ui->graphicsView->scene()->addEllipse(cursor->pos().x() - ui->graphicsView->x(), cursor->pos().y() - ui->graphicsView->y(), 3, 3, QPen(), QBrush(Qt::red));
}

void MainWindow::calcPPCM()
{
    calc->calculatePPCM(this->width(), this->height(), diagonalCMDouble);
}

void MainWindow::on_pbCalibrate_clicked()
{
    calcPPCM();

    loadSettings();

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
