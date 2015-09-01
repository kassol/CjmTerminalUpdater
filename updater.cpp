#include "updater.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

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
    QNetworkReply *reply =  manager->post(request, postData.encodedQuery());
    connect(reply, SIGNAL(finished()), this, SLOT(checkAvailableFinished()));
}

void Updater::checkAvailableFinished()
{
    disconnect(reply, SIGNAL(finished()), this, SLOT(checkAvailableFinished()));
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
}

void Updater::checkDownloadListFinished()
{

}

QString Updater::getMacInfo()
{
    return tr("9DFJ98j");
}
