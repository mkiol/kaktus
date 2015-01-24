// Based on: https://github.com/RileyGB/BlackBerry10-Samples

#include "standardweblistitem.h"

#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QtGui/QDesktopServices>
#include <bb/cascades/Image>

using namespace bb::cascades;

QNetworkAccessManager * StandardWebListItem::nm = new QNetworkAccessManager();
QNetworkDiskCache * StandardWebListItem::nc = new QNetworkDiskCache();

StandardWebListItem::StandardWebListItem()
{
    nc->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    nm->setCache(nc);
    loading = 0;
}

StandardWebListItem::~StandardWebListItem(){}

const QUrl& StandardWebListItem::getUrl() const
{
    return url;
}

void StandardWebListItem::setUrl(const QUrl& url)
{
    this->url = url;
    loading = 0;

    resetImage();

    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setUrl(url);

    QNetworkReply * reply = nm->get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(dowloadProgressed(qint64,qint64)));

    emit urlChanged();
}

double StandardWebListItem::getLoading() const
{
    return loading;
}

void StandardWebListItem::imageLoaded()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        if (isARedirectedUrl(reply)) {
            setURLToRedirectedUrl(reply);
            return;
        } else {
            QByteArray imageData = reply->readAll();
            setImage(Image(imageData));
        }
    }

    reply->deleteLater();
}

bool StandardWebListItem::isARedirectedUrl(QNetworkReply *reply)
{
    QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    return !redirection.isEmpty();
}

void StandardWebListItem::setURLToRedirectedUrl(QNetworkReply *reply)
{
    QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    QUrl baseUrl = reply->url();
    QUrl resolvedUrl = baseUrl.resolved(redirectionUrl);

    setUrl(resolvedUrl.toString());
}

void StandardWebListItem::clearCache()
{
    nc->clear();
}

void StandardWebListItem::dowloadProgressed(qint64 bytes, qint64 total) {
    loading = double(bytes) / double(total);
    emit loadingChanged();
}



