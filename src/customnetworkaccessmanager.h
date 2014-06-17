#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QIODevice>

class CustomNetworkAccessManager: public QNetworkAccessManager
{

public:
    CustomNetworkAccessManager(QString userAgent, QObject *parent = 0);
    QNetworkReply *createRequest(Operation operation, const QNetworkRequest &reqest, QIODevice *outgoingData);

private:
    QString userAgent;

};

#endif // CUSTOMNETWORKACCESSMANAGER_H
