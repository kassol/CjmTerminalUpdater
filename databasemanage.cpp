#include "databasemanage.h"
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QSqlQuery>

QSqlDatabase DatabaseManage::db = QSqlDatabase::addDatabase("QSQLITE");
QString DatabaseManage::dbName = tr("CjmTerminalUpdater.db");

DatabaseManage::DatabaseManage(QObject *parent)
    : QObject(parent)
{
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
        QSqlQuery *query = new QSqlQuery(db);
        bool isSuccess = query->exec("create table log (id varchar(64) primary key, "
                   "title varchar(100) not null, ver varchar(50) not null default '1.0', "
                   "verCode int not null, url varchar(1000) not null, "
                   "type varchar(20) not null, tsp timestamp, "
                   "status int not null, filePath varchar(1000) not null)");
        if (!isSuccess)
        {
            return false;
        }
    }
    return true;
}

int DatabaseManage::getVersionWithTitle(QString title)
{
    return -1;
}

void DatabaseManage::insertItem(UpdateItem *item)
{

}

void DatabaseManage::setStatus(UpdateItem *item, int status)
{

}

