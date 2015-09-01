#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
#include <QAction>
#include <QTextCodec>
#include "databasemanage.h"
#include "updater.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    QSystemTrayIcon *trayicon = new QSystemTrayIcon();

    QIcon icon(":/update.ico");
    trayicon->setIcon(icon);

    QMenu *trayiconMenu = new QMenu();

    QAction *updateAction = new QAction(QObject::tr("��ʾ������"), NULL);
    QAction *quitAction = new QAction(QObject::tr("�˳�"), NULL);
    QObject::connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayiconMenu->addAction(updateAction);
    trayiconMenu->addAction(quitAction);
    trayicon->setContextMenu(trayiconMenu);

    trayicon->show();
    if (!DatabaseManage::connect())
    {
    }

    Updater updater;
    updater.checkForUpdate();

    return a.exec();
}
