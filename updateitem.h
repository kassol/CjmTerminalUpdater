#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include <QObject>

class UpdateItem : public QObject
{
    Q_OBJECT
public:
    explicit UpdateItem(QObject *parent = 0);
    UpdateItem(const UpdateItem &item);

public:
    QString id;
    QString title;
    QString version;
    int versionCode;
    QString url;
    QString type;
    int status;
    QString filePath;
    int index;

signals:

public slots:
};

#endif // UPDATEITEM_H
