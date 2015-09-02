#ifndef DATABASEMANAGE_H
#define DATABASEMANAGE_H

#include <QObject>
#include <QSqlDatabase>
#include "updateitem.h"

class DatabaseManage : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManage(QObject *parent = 0);
    static bool connect();
    static int getVersionWithTitle(QString title);
    static void insertItem(UpdateItem *item);
    static void setStatus(UpdateItem *item, int status);

signals:

public slots:

private:
    static QSqlDatabase db;
    static QString dbName;
};

#endif // DATEBASEMANAGE_H
