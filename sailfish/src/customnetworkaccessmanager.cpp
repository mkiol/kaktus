#include "customnetworkaccessmanager.h"
#include <QNetworkReply>
#include <QDebug>

CustomNetworkAccessManager::CustomNetworkAccessManager(const QString &userAgent, QObject *parent) :
    QNetworkAccessManager(parent), userAgent(userAgent)
{
    this->setNetworkAccessible(QNetworkAccessManager::Accessible);
}

QNetworkReply *CustomNetworkAccessManager::createRequest(Operation operation, const QNetworkRequest &reqest, QIODevice *outgoingData)
{
    //qDebug() << "CustomNetworkAccessManager::createRequest, url:" << reqest.url().toString();

    QNetworkRequest newRequest(reqest);
    newRequest.setRawHeader("User-Agent", userAgent.toLatin1());

    QNetworkReply *reply = QNetworkAccessManager::createRequest(operation, newRequest, outgoingData);
    return reply;
}
