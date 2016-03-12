#include <QNetworkRequest>
#include <QUrl>

#include "debugunit.h"

DebugUnit::DebugUnit(QObject *parent) : QObject(parent)
{
    connect(&nam, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));

}

void DebugUnit::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    qDebug() << "DebugUnit::networkAccessibleChanged: " << accessible;
}

void DebugUnit::finished()
{
    //qDebug() << "DebugUnit::finished";

    QNetworkReply * reply = static_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "DebugUnit::finished: error!";
    } else {
        qDebug() << "DebugUnit::finished: no error!";
    }

    qDebug() << reply->readAll();
}

void DebugUnit::readyRead()
{
    qDebug() << "DebugUnit::readyRead";
}

void DebugUnit::error(QNetworkReply::NetworkError error)
{
    qDebug() << "DebugUnit::error";

    QNetworkReply * reply = static_cast<QNetworkReply*>(sender());
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray phrase = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();

    qDebug() << "Network error!" << "Url:" << reply->url().toString() << "Error code:" << error
               << "HTTP code:" << code << phrase << "Content:" << reply->readAll();
}

void DebugUnit::test()
{
    QUrl url("http://localhost:9999/test");
    QNetworkRequest request(url);

    nam.setNetworkAccessible(QNetworkAccessManager::Accessible);
    QNetworkReply * reply = nam.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

