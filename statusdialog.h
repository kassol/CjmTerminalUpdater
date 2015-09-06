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
    void refreshList();
    void setItemCount(int count);
    void addItem(UpdateItem* item);
    void updateState(int row, int status);

public slots:
    void updateFiles(bool);
    void checkForUpdate(bool);
    void updateProgress(int row, int percent);

signals:
    void updateButtonDidTouch();
    void checkButtonDidTouch();

protected:
    void closeEvent(QCloseEvent * event);

private:
    Ui::StatusDialog *ui;
};

#endif // STATUSDIALOG_H
