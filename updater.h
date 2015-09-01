#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "updateitem.h"
#include "downloadmanagerHTTP.h"

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);
    ~Updater();
    void checkForUpdate();

signals:

public slots:

private:
    void initWithConfig();
    QString getMacInfo();
    void downloadNext();
    void copyFiles();

private slots:
    void checkAvailableFinished();
    void checkDownloadListFinished();
    void downloadFinished();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    static QString deviceInfoUrl;
    static QString downloadListUrl;
    QList<UpdateItem*> updateList;
    DownloadManagerHTTP *http;
    QString moduleDir;
    QString videoDir;
    QString imageDir;
    QString docDir;
    QString othersDir;
};

#endif // UPDATER_H
