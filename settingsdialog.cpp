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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QtDebug>

SettingsDialog::SettingsDialog(QString patient, QString test, double diagonalCM, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->pbCancel->setFocus();

    ui->lePatient->setText(patient);
    ui->leTest->setText(test);
    ui->leDiagonalCM->setText(QString::number(diagonalCM));

    connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(cancelSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings()
{
    emit patientInfo(ui->lePatient->text());
    qDebug() << "Emitted PatientInfo";
    emit testInfo(ui->leTest->text());
    qDebug() << "Emitted TestInfo";
    emit diagonalCM(ui->leDiagonalCM->text().toDouble());
    qDebug() << "Emitted diagonalCM";
    this->done(1);
}

void SettingsDialog::cancelSettings()
{
    this->done(0);  //reject code
}
