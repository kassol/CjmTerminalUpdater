#include "updater.h"
#include "databasemanage.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>

QString Updater::deviceInfoUrl = "http://61.186.130.102:28671/cjmDataServices/data/dataset.do";
QString Updater::downloadListUrl = "http://61.186.130.102:28671/cjmDataServices/data/dataset.do";
QString Updater::downloadTempDir = "Update/";

Updater::Updater(QObject *parent)
    : QObject(parent)
    , reply(NULL)
{
    manager = new QNetworkAccessManager(this);
    mainWindow = new StatusDialog();
    connect(mainWindow, SIGNAL(updateButtonDidTouch()), this, SLOT(updateFiles()));
    //connect(mainWindow, SIGNAL(checkButtonDidTouch()), this, SLOT(checkForUpdate()));
}

Updater::~Updater()
{
    delete manager;
    manager = NULL;
    reply->abort();
    reply = NULL;
}

void Updater::checkForUpdate()
{
    updateList.clear();
    downloadList.clear();
    initWithConfig();
    QUrl checkAvailabelUrl;
    checkAvailabelUrl.setUrl(deviceInfoUrl);
    QUrl postData;
    postData.addQueryItem("ds_id", "get_terminal_by_mac");
    postData.addQueryItem("p1", getMacInfo());
    QNetworkRequest request(checkAvailabelUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    reply =  manager->post(request, postData.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(checkAvailableFinished())); 
}

void Updater::checkAvailableFinished()
{
    disconnect(reply, SIGNAL(finished()), this, SLOT(checkAvailableFinished()));
    QString strReply = QString(reply->readAll());
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+strReply+")");
    if(result.property("status").toInteger() == 1)
    {
        return;
    }
    QUrl checkDownloadListlUrl;
    checkDownloadListlUrl.setUrl(downloadListUrl);
    QUrl postData;
    postData.addQueryItem("ds_id", "get_download_list_by_mac");
    postData.addQueryItem("p1", getMacInfo());
    QNetworkRequest request(checkDownloadListlUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    reply =  manager->post(request, postData.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(checkDownloadListFinished()));
}

void Updater::checkDownloadListFinished()
{
    disconnect(reply, SIGNAL(finished()), this, SLOT(checkDownloadListFinished()));
    QString strReply = QString(reply->readAll());
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+strReply+")");

    downloadTempDir = QDir::toNativeSeparators(downloadTempDir);
    QScriptValue contents = result.property("contents");
    QScriptValueIterator it(contents);
    while (it.hasNext()) {
        it.next();
        UpdateItem *item = new UpdateItem();
        item->id = it.value().property("id").toString();
        item->title = it.value().property("title").toString();
        item->version = it.value().property("ver").toString();
        item->versionCode = it.value().property("verCode").toInteger();
        item->url = it.value().property("url").toString();
        item->type = it.value().property("type").toString();
        item->status = 0;
        QFileInfo fileInfo(item->url);
        QString filename = fileInfo.fileName();
        item->filePath = downloadTempDir+filename;
        downloadList.push_back(item);
    }
    downloadList.pop_back();

    QList<UpdateItem *>::iterator ite= downloadList.begin();
    while (ite != downloadList.end())
    {
        if (DatabaseManage::currentVersionIsExpired((*ite)->title, (*ite)->versionCode))
        {
            DatabaseManage::insertItem(*ite);
            ++ite;
        }
        else
        {
            delete *ite;
            *ite = NULL;
            ite = downloadList.erase(ite);
        }
    }
    mainWindow->refreshList();
    mainWindow->setItemCount(downloadList.count());
    ite = downloadList.begin();
    int index = 0;
    while (ite != downloadList.end())
    {
        (*ite)->index = index;
        mainWindow->addItem(*ite);
        ++index;
        ++ite;
    }
    downloadNext();
}

void Updater::initWithConfig()
{
    QSettings setting("config.ini", QSettings::IniFormat);
    moduleDir = setting.value("Path/modulePath").toString();
    videoDir = setting.value("Path/videoPath").toString();
    imageDir = setting.value("Path/imagePath").toString();
    docDir = setting.value("Path/docPath").toString();
    othersDir = setting.value("Path/othersPath").toString();
}

QString Updater::getMacInfo()
{
    QSettings setting("CjmTerminal.ini", QSettings::IniFormat);
    QString mac = setting.value("MAC").toString();
    return mac;
}

void Updater::downloadNext()
{
    if (http == NULL)
    {
        http = new DownloadManagerHTTP(this);
    }
    if (!downloadList.empty())
    {
        UpdateItem *item = downloadList.front();
        QUrl downloadUrl;
        downloadUrl.setUrl(item->url);
        http->download(downloadUrl.toString(), downloadTempDir);
        connect(http, SIGNAL(downloadComplete()), this, SLOT(downloadFinished()));
    }
    else
    {
        delete http;
        http = NULL;
        copyFiles();
    }
}

void Updater::copyFiles()
{
    QList<UpdateItem *>::iterator ite = updateList.begin();
    QDir dir;
    while (ite != updateList.end())
    {
        QFileInfo fileInfo((*ite)->url);
        QString filename = fileInfo.fileName();
        if ((*ite)->type == "module")
        {
            if (!dir.exists(moduleDir))
            {
                dir.mkdir(moduleDir);
            }
            if(QFile::copy((*ite)->filePath, moduleDir+filename))
            {
                DatabaseManage::setStatus(*ite, 2);
                mainWindow->updateState((*ite)->index, 2);
                delete *ite;
                *ite = NULL;
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "video")
        {
            if (!dir.exists(videoDir))
            {
                dir.mkdir(videoDir);
            }
            if(QFile::copy((*ite)->filePath, videoDir+filename))
            {
                DatabaseManage::setStatus(*ite, 2);
                mainWindow->updateState((*ite)->index, 2);
                delete *ite;
                *ite = NULL;
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "image")
        {
            if (!dir.exists(imageDir))
            {
                dir.mkdir(imageDir);
            }
            if(QFile::copy((*ite)->filePath, imageDir+filename))
            {
                DatabaseManage::setStatus(*ite, 2);
                mainWindow->updateState((*ite)->index, 2);
                delete *ite;
                *ite = NULL;
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "doc")
        {
            if (!dir.exists(docDir))
            {
                dir.mkdir(docDir);
            }
            if(QFile::copy((*ite)->filePath, docDir+filename))
            {
                DatabaseManage::setStatus(*ite, 2);
                mainWindow->updateState((*ite)->index, 2);
                delete *ite;
                *ite = NULL;
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "others")
        {
            if (!dir.exists(othersDir))
            {
                dir.mkdir(othersDir);
            }
            if(QFile::copy((*ite)->filePath, othersDir+filename))
            {
                DatabaseManage::setStatus(*ite, 2);
                mainWindow->updateState((*ite)->index, 2);
                delete *ite;
                *ite = NULL;
                ite = updateList.erase(ite);
                continue;
            }
        }
        ++ite;
    }
}

void Updater::downloadFinished()
{
    disconnect(http, SIGNAL(downloadComplete()), this, SLOT(downloadFinished()));
    UpdateItem *item = downloadList.front();
    DatabaseManage::setStatus(item, 1);
    mainWindow->updateState(item->index, 1);
    downloadList.pop_front();
    updateList.push_back(item);
    downloadNext();
}

void Updater::showUp()
{
    mainWindow->show();
}

void Updater::updateFiles()
{
    QMessageBox msg;
    msg.setIcon (QMessageBox::Question);
    msg.setText ("<b>" +
                 tr ("要进行更新您必须退出智能终端")+"</b>");
    msg.setInformativeText (tr ("您想现在退出智能终端?"));
    msg.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    if (msg.exec() == QMessageBox::Yes)
    {
        QProcess p;
        p.start("taskkill /IM 1.exe /F");
        p.waitForFinished();
        p.start("taskkill /IM 2.exe /F");
        p.waitForFinished();
        copyFiles();
    }
}

void Updater::progress(int percent)
{
    UpdateItem *item = downloadList.front();
}
