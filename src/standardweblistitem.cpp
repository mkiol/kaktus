/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

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



