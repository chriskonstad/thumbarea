#include "errordialog.h"
#include "ui_errordialog.h"

ErrorDialog::ErrorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);
    QPalette background = this->palette();
    background.setBrush(QPalette::Background, QBrush(Qt::darkRed));
    this->setPalette(background);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}
