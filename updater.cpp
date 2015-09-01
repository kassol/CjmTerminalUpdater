#include "updater.h"
#include "databasemanage.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>
#include <QDir>

QString Updater::deviceInfoUrl = "http://61.186.130.102:28670/cjmDataServices/data/dataset.do";
QString Updater::downloadListUrl = "http://61.186.130.102:28670/cjmDataServices/data/dataset.do";

Updater::Updater(QObject *parent)
    : QObject(parent)
    , reply(NULL)
{
    manager = new QNetworkAccessManager(this);
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

    QScriptValueIterator it(result);
    while (it.hasNext()) {
        it.next();
        UpdateItem item;
        item.id = it.value().property("id").toString();
        item.title = it.value().property("title").toString();
        item.version = it.value().property("ver").toString();
        item.versionCode = it.value().property("verCode").toInteger();
        item.url = it.value().property("url").toString();
        item.type = it.value().property("type").toString();
        item.status = 0;
        updateList.push_back(&item);
    }

    QList<UpdateItem *>::iterator ite= updateList.begin();
    while (ite != updateList.end())
    {
        if ((*ite)->versionCode == DatabaseManage::getVersionWithTitle((*ite)->title))
        {
            ite = updateList.erase(ite);
        }
        else
        {
            ++ite;
        }
    }
    downloadNext();
}

QString Updater::getMacInfo()
{
    return tr("9DFJ98j");
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
        UpdateItem item = *(updateList.front());
        QUrl downloadUrl;
        downloadUrl.setUrl(item.url);
        if (item.type == "module")
        {
            http->download(downloadUrl, moduleDir);
        }
        else if (item.type == "video")
        {
            http->download(downloadUrl, videoDir);
        }
        else if (item.type == "image")
        {
            http->download(downloadUrl, imageDir);
        }
        else if (item.type == "doc")
        {
            http->download(downloadUrl, docDir);
        }
        else if (item.type == "others")
        {
            http->download(downloadUrl, othersDir);
        }
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

}

void Updater::downloadFinished()
{
    disconnect(http, SIGNAL(downloadComplete()), this, SLOT(downloadFinished()));
    updateList.pop_front();
    downloadNext();
}
