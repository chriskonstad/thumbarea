#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QtDebug>

SettingsDialog::SettingsDialog(QString patient, QString test,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->pbCancel->setFocus();

    ui->lePatient->setText(patient);
    ui->leTest->setText(test);

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
    this->done(1);
}

void SettingsDialog::cancelSettings()
{
    this->done(0);  //reject code
}
