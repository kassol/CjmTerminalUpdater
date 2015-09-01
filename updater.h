#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "updateitem.h"

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
    QString getMacInfo();

private slots:
    void checkAvailableFinished();
    void checkDownloadListFinished();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    static QString deviceInfoUrl;
    static QString downloadListUrl;
    QList<UpdateItem*> updateList;
};

#endif // UPDATER_H
