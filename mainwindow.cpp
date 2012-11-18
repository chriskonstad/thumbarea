#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cursor = new QCursor();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePos()));

    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    calc = new QPixelCalculator(this);

    timer->start(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePos()
{
    ui->labX->setText("X: " + QString::number(calc->getPointToCM(ui->graphicsView->cursor().pos().x())));
    ui->labY->setText("Y: " + QString::number(calc->getPointToCM(ui->graphicsView->cursor().pos().y())));

    ui->graphicsView->scene()->addEllipse(cursor->pos().x() - ui->graphicsView->x(), cursor->pos().y() - ui->graphicsView->y(), 3, 3, QPen(), QBrush(Qt::red));
}

void MainWindow::calcPPCM()
{
    calc->calculatePPCM(this->width(), this->height(), 25.654);
}

void MainWindow::on_pbCalibrate_clicked()
{
    calcPPCM();

    drawDataFieldInformation();
}

void MainWindow::drawDataFieldInformation()
{
    scene->clear();
    scene->setSceneRect(0,0,ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->resetTransform();

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
    dateItem = new QGraphicsSimpleTextItem;
    dateItem->setText(date.currentDate().toString("MMM dd yyyy"));
    dateItem->setPos(0,0);
    scene->addItem(dateItem);

    //Add patient info to data field
    QGraphicsSimpleTextItem * patientInfo = new QGraphicsSimpleTextItem;
    patientInfo->setText("Patient Info");
    patientInfo->setPos(200, 0);
    patientInfo->setPos(dateItem->boundingRect().x() + dateItem->boundingRect().width() + 100, 0);
    scene->addItem(patientInfo);

    QList<QGraphicsItem *> items = scene->items();
    foreach(QGraphicsItem *i, items)
    {
        i->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }
}

void MainWindow::on_pbSaveData_clicked()
{
    ui->graphicsView->viewport()->update(); //update data field
}
