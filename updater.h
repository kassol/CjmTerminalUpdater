#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "updateitem.h"
#include "downloadmanagerHTTP.h"
#include "statusdialog.h"

class StatusDialog;

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);
    ~Updater();

signals:

public slots:
    void checkForUpdate();
    void showUp();
    void progress(int);

private:
    void initWithConfig();
    QString getMacInfo();
    void downloadNext();
    void copyFiles();

private slots:
    void checkAvailableFinished();
    void checkDownloadListFinished();
    void downloadFinished();
    void updateFiles();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    static QString deviceInfoUrl;
    static QString downloadListUrl;
    static QString downloadTempDir;
    QList<UpdateItem *> downloadList;
    QList<UpdateItem *> updateList;
    DownloadManagerHTTP *http;
    QString moduleDir;
    QString videoDir;
    QString imageDir;
    QString docDir;
    QString othersDir;
    StatusDialog *mainWindow;
};

#endif // UPDATER_H
