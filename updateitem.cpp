#include "updateitem.h"

UpdateItem::UpdateItem(QObject *parent) : QObject(parent)
{

}

UpdateItem::UpdateItem(const UpdateItem &item)
{
    id = item.id;
    title = item.title;
    version = item.version;
    versionCode = item.versionCode;
    url = item.url;
    type = item.type;
    status = item.status;
    filePath = item.filePath;
}
