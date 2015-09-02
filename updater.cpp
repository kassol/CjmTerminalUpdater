#include "updater.h"
#include "databasemanage.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QDir>
#include <QSettings>

QString Updater::deviceInfoUrl = "http://61.186.130.102:28670/cjmDataServices/data/dataset.do";
QString Updater::downloadListUrl = "http://61.186.130.102:28670/cjmDataServices/data/dataset.do";
QString Updater::downloadTempDir = "Update/";

Updater::Updater(QObject *parent)
    : QObject(parent)
    , reply(NULL)
{
    manager = new QNetworkAccessManager(this);
    mainWindow = new StatusDialog();
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
    connect(reply, SIGNAL(finished()), this, SLOT(checkAvailableFinished()));
    reply =  manager->post(request, postData.encodedQuery());
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
    connect(reply, SIGNAL(finished()), this, SLOT(checkDownloadListFinished()));
    reply =  manager->post(request, postData.encodedQuery());
}

void Updater::checkDownloadListFinished()
{
    disconnect(reply, SIGNAL(finished()), this, SLOT(checkDownloadListFinished()));
    QString strReply = QString(reply->readAll());
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+strReply+")");

    QScriptValueIterator it(result);
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
        downloadList.push_back(item);
    }

    QList<UpdateItem *>::iterator ite= downloadList.begin();
    while (ite != downloadList.end())
    {
        if (DatabaseManage::getVersionWithTitle((*ite)->title) == -1)
        {
            DatabaseManage::insertItem(*ite);
            ++ite;
        }
        else
        {
            if ((*ite)->versionCode == DatabaseManage::getVersionWithTitle((*ite)->title))
            {
                delete *ite;
                *ite = NULL;
                ite = downloadList.erase(ite);
            }
            else
            {
                ++ite;
            }
        }
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
    QSettings setting("mac.ini", QSettings::IniFormat);
    QString mac = setting.value("MAC/mac").toString();
    return mac;
}

void Updater::downloadNext()
{
    if (http == NULL)
    {
        http = new DownloadManagerHTTP(this);
    }
    if (!updateList.empty())
    {
        connect(http, SIGNAL(downloadComplete()), this, SLOT(downloadFinished()));
        UpdateItem *item = downloadList.front();
        QUrl downloadUrl;
        downloadUrl.setUrl(item->url);
        http->download(downloadUrl, downloadTempDir);
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
    downloadTempDir = QDir::toNativeSeparators(downloadTempDir);
    QList<UpdateItem *>::iterator ite = updateList.begin();
    while (ite != updateList.end())
    {
        QFileInfo fileInfo((*ite)->url);
        QString filename = fileInfo.fileName();
        if ((*ite)->type == "module")
        {
            if(QFile::copy(downloadTempDir+filename, moduleDir+filename))
            {
                delete *ite;
                *ite = NULL;
                DatabaseManage::setStatus(*ite, 2);
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "video")
        {
            if(QFile::copy(downloadTempDir+filename, videoDir+filename))
            {
                delete *ite;
                *ite = NULL;
                DatabaseManage::setStatus(*ite, 2);
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "image")
        {
            if(QFile::copy(downloadTempDir+filename, imageDir+filename))
            {
                delete *ite;
                *ite = NULL;
                DatabaseManage::setStatus(*ite, 2);
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "doc")
        {
            if(QFile::copy(downloadTempDir+filename, docDir+filename))
            {
                delete *ite;
                *ite = NULL;
                DatabaseManage::setStatus(*ite, 2);
                ite = updateList.erase(ite);
                continue;
            }
        }
        else if ((*ite)->type == "others")
        {
            if(QFile::copy(downloadTempDir+filename, othersDir+filename))
            {
                delete *ite;
                *ite = NULL;
                DatabaseManage::setStatus(*ite, 2);
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
    downloadList.pop_front();
    updateList.push_back(item);
    downloadNext();
}

void Updater::showUp()
{
    mainWindow->show();
}
