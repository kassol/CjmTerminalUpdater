#ifndef DATABASEMANAGE_H
#define DATABASEMANAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "updateitem.h"

class DatabaseManage : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManage(QObject *parent = 0);
    ~DatabaseManage();
    static bool connect();
    static bool currentVersionIsExpired(QString title, int version);
    static void insertItem(UpdateItem *item);
    static void setStatus(UpdateItem *item, int status);

signals:

public slots:

private:
    static QSqlDatabase db;
    static QString dbName;
    static QSqlQuery *query;
};

#endif // DATEBASEMANAGE_H
