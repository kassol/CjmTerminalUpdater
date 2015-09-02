#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "updater.h"

namespace Ui {
class StatusDialog;
}

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatusDialog(QWidget *parent = 0);
    ~StatusDialog();

protected:
    void closeEvent(QCloseEvent * event);

private:
    Ui::StatusDialog *ui;
};

#endif // STATUSDIALOG_H
