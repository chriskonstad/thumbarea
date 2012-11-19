#ifndef SAVEDDIALOG_H
#define SAVEDDIALOG_H

#include <QDialog>

namespace Ui {
    class SavedDialog;
}

class SavedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SavedDialog(QWidget *parent = 0);
    ~SavedDialog();

private:
    Ui::SavedDialog *ui;
};

#endif // SAVEDDIALOG_H
