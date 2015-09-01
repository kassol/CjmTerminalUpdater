#ifndef DATABASEMANAGE_H
#define DATABASEMANAGE_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseManage : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManage(QObject *parent = 0);
    static bool connect();
    static int getVersionWithTitle(QString title);

signals:

public slots:

private:
    static QSqlDatabase db;
    static QString dbName;
};

#endif // DATEBASEMANAGE_H
