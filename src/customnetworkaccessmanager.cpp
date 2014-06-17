#include "customnetworkaccessmanager.h"
#include <QNetworkReply>

CustomNetworkAccessManager::CustomNetworkAccessManager(QString userAgent, QObject *parent) :
    QNetworkAccessManager(parent), userAgent(userAgent)
{
}

QNetworkReply *CustomNetworkAccessManager::createRequest(Operation operation, const QNetworkRequest &reqest, QIODevice *outgoingData)
{
    QNetworkRequest newRequest(reqest);
    newRequest.setRawHeader("User-Agent", userAgent.toAscii());

    QNetworkReply *reply = QNetworkAccessManager::createRequest(operation, newRequest, outgoingData);
    return reply;
}
