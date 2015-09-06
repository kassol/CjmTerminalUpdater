#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
#include <QAction>
#include <QTextCodec>
#include <QSettings>
#include "databasemanage.h"
#include "updater.h"

void setAutoRun(const QString & filePath) {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);
    settings.setValue("CjmTerminalUpdate", filePath);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    setAutoRun(QApplication::applicationFilePath());

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    QSystemTrayIcon *trayicon = new QSystemTrayIcon();

    QIcon icon(":/update.ico");
    trayicon->setIcon(icon);

    Updater *updater = new Updater();

    QMenu *trayiconMenu = new QMenu();

    QAction *updateAction = new QAction(QObject::tr("显示主窗口"), NULL);
    QObject::connect(updateAction, SIGNAL(triggered()), updater, SLOT(showUp()));
    QAction *quitAction = new QAction(QObject::tr("退出"), NULL);
    QObject::connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayiconMenu->addAction(updateAction);
    trayiconMenu->addAction(quitAction);
    trayicon->setContextMenu(trayiconMenu);

    trayicon->show();
    if (!DatabaseManage::connect())
    {
        return -1;
    }

    updater->checkForUpdate();

    return a.exec();
}
