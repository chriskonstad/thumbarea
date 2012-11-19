#include "saveddialog.h"
#include "ui_saveddialog.h"

SavedDialog::SavedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavedDialog)
{
    ui->setupUi(this);
    QPalette background = this->palette();
    background.setBrush(QPalette::Background, QBrush(Qt::darkGreen));
    this->setPalette(background);
}

SavedDialog::~SavedDialog()
{
    delete ui;
}
