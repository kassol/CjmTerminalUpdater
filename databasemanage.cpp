#include "databasemanage.h"
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QVariant>
#include <QDebug>
#include <QSqlError>


QSqlDatabase DatabaseManage::db = QSqlDatabase::addDatabase("QSQLITE");
QString DatabaseManage::dbName = tr("CjmTerminalUpdater.db");
QSqlQuery *DatabaseManage::query = NULL;

DatabaseManage::DatabaseManage(QObject *parent)
    : QObject(parent)
{
}

DatabaseManage::~DatabaseManage()
{
    if (query != NULL)
    {
        delete query;
        query = NULL;
    }
}

bool DatabaseManage::connect()
{
    bool isNewCreated = false;
    db.setDatabaseName(dbName);
    if (!QFile::exists(dbName))
    {
        isNewCreated = true;
    }
    if (!db.open())
    {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
                    qApp->tr("Unable to establish a database connection.\n"
                             "This example needs SQLite support. Please read "
                             "the Qt SQL driver documentation for information how "
                             "to build it.\n\n"
                             "Click Cancel to exit."), QMessageBox::Cancel);
                return false;
    }
    if (isNewCreated)
    {
        query = new QSqlQuery(db);
        bool isSuccess = query->exec("create table log (id varchar(64) primary key, "
                   "title varchar(100) not null, ver varchar(50) not null default '1.0', "
                   "verCode int not null, url varchar(1000) not null, "
                   "type varchar(20) not null, tsp TimeStamp NOT NULL DEFAULT CURRENT_TIMESTAMP, "
                   "status int not null, filePath varchar(1000) not null)");
        if (!isSuccess)
        {
            qDebug() <<query->lastError();
            return false;
        }
    }
    return true;
}

bool DatabaseManage::currentVersionIsExpired(QString title, int version)
{
    if (query == NULL)
    {
        query = new QSqlQuery(db);
    }
    QString sql = QString("select verCode from log where title = '%1' order by verCode desc").arg(title);
    if (!query->exec(sql))
    {
        qDebug() <<query->lastError();
    }
    query->next();
    if (query->value(0).toInt() == version)
    {
        return false;
    }
    return true;
}

void DatabaseManage::insertItem(UpdateItem *item)
{
    if (query == NULL)
    {
        query = new QSqlQuery(db);
    }
    QString sql = QString("insert into log (id, title, ver, verCode, url, type, status, filePath) values('%1', '%2', '%3', %4, '%5', '%6', %7, '%8')")
            .arg(item->id).arg(item->title).arg(item->version).arg(item->versionCode)
            .arg(item->url).arg(item->type).arg(item->status).arg(item->filePath);
    if(!query->exec(sql))
    {
        qDebug() <<query->lastError();
    }
}

void DatabaseManage::setStatus(UpdateItem *item, int status)
{
    if (query == NULL)
    {
        query = new QSqlQuery(db);
    }
    if (!query->exec(QString("update log set status = %1 where title = '%2' and verCode = %3").arg(status).arg(item->title).arg(item->versionCode)))
    {
        qDebug() <<query->lastError();
    }
}

