#include "statusdialog.h"
#include "ui_statusdialog.h"
#include <QProgressBar>

StatusDialog::StatusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusDialog)
{
    ui->setupUi(this);
    ui->downloadList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->downloadList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->downloadList->verticalHeader()->setVisible(false);
    refreshList();
    connect(ui->updateButton, SIGNAL(clicked(bool)), this, SLOT(updateFiles(bool)));
    connect(ui->checkButton, SIGNAL(clicked(bool)), this, SLOT(checkForUpdate(bool)));
}

StatusDialog::~StatusDialog()
{
    delete ui;
}

void StatusDialog::closeEvent(QCloseEvent * event)
{
    event->ignore();
    hide();
}

void StatusDialog::checkForUpdate(bool)
{
    emit checkButtonDidTouch();
}

void StatusDialog::updateFiles(bool)
{
    emit updateButtonDidTouch();
}

void StatusDialog::refreshList()
{
    ui->downloadList->clear();
    ui->downloadList->setColumnCount(4);
    QStringList header;
    header.push_back(tr("文件名"));
    header.push_back(tr("版本"));
    header.push_back(tr("进度"));
    header.push_back(tr("状态"));
    ui->downloadList->setHorizontalHeaderLabels(header);
}

void StatusDialog::setItemCount(int count)
{
    ui->downloadList->setRowCount(count);
}

void StatusDialog::addItem(UpdateItem *item)
{
    ui->downloadList->setItem(item->index, 0, new QTableWidgetItem(item->title));
    ui->downloadList->setItem(item->index, 1, new QTableWidgetItem(QString("%1").arg(item->versionCode)));
    QProgressBar *progressBar = new QProgressBar();
    progressBar->setMaximum(100);
    progressBar->setMinimum(0);
    ui->downloadList->setCellWidget(item->index, 2, progressBar);
    QString status;
    switch (item->status)
    {
    case 0:
        status = tr("未下载");
        break;
    case 1:
        status = tr("已下载");
        break;
    case 2:
        status = tr("已更新");
        break;
    }
    ui->downloadList->setItem(item->index, 3, new QTableWidgetItem(status));
}

void StatusDialog::updateProgress(int row, int percent)
{
    QProgressBar *progressBar = (QProgressBar *)ui->downloadList->cellWidget(row, 2);
    progressBar->setValue(percent);
}

void StatusDialog::updateState(int row, int status)
{
    QString statusStr;
    switch (status)
    {
    case 0:
        statusStr = tr("未下载");
        break;
    case 1:
        statusStr = tr("已下载");
        break;
    case 2:
        statusStr = tr("已更新");
        break;
    }
    QTableWidgetItem *item = ui->downloadList->item(row, 3);
    item->setText(statusStr);
}
