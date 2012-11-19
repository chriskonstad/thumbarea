#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QString patient, QString test, double diagonalCM, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void saveSettings();
    void cancelSettings();

signals:
    void patientInfo(QString patient);
    void testInfo(QString test);
    void diagonalCM(double cm);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
