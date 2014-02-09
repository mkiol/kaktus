#include "customnetworkaccessmanager.h"
#include <QNetworkReply>
#include <QDebug>

CustomNetworkAccessManager::CustomNetworkAccessManager(QString p_userAgent, QObject *parent) :
    QNetworkAccessManager(parent), __userAgent(p_userAgent)
{
}

QNetworkReply *CustomNetworkAccessManager::createRequest( Operation op,
                                                          const QNetworkRequest &req,
                                                          QIODevice * outgoingData )
{
    QNetworkRequest new_req(req);
    qDebug() << __userAgent.toUtf8();
    new_req.setRawHeader("User-Agent", __userAgent.toUtf8());

    QNetworkReply *reply = QNetworkAccessManager::createRequest( op, new_req, outgoingData );
    return reply;
}

QNetworkReply *CustomNetworkAccessManager::get(const QNetworkRequest & req)
{
    qDebug() << __userAgent.toUtf8();
    return QNetworkAccessManager::get(req);
}
