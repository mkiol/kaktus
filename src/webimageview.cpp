#include "webimageview.h"
#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QtGui/QDesktopServices>
#include <bb/cascades/Image>

using namespace bb::cascades;

QNetworkAccessManager * WebImageView::mNetManager = new QNetworkAccessManager();
QNetworkDiskCache * WebImageView::mNetworkDiskCache = new QNetworkDiskCache();

WebImageView::WebImageView() {
    // Initialize network cache
    mNetworkDiskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));

    // Set cache in manager
    mNetManager->setCache(mNetworkDiskCache);

    // Set defaults
    mLoading = 0;
    mIsLoaded = false;
}

const QUrl& WebImageView::url() const {
    return mUrl;
}

void WebImageView::setUrl(const QUrl& url) {
    // Variables
    mUrl = url;
    mLoading = 0;
    mIsLoaded = false;

    // Reset the image
    resetImage();

    // Create request
    QNetworkRequest request;
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setUrl(url);

    // Create reply
    QNetworkReply * reply = mNetManager->get(request);

    // Connect to signals
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(dowloadProgressed(qint64,qint64)));

    emit urlChanged();
}

double WebImageView::loading() const {
    return mLoading;
}

void WebImageView::imageLoaded() {
    // Get reply
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        if (isARedirectedUrl(reply)) {
            setURLToRedirectedUrl(reply);
            return;
        } else {
            QByteArray imageData = reply->readAll();
            setImage(Image(imageData));
            mIsLoaded = true;
            emit isLoadedChanged();
        }
    }

    // Memory management
    reply->deleteLater();
}

bool WebImageView::isARedirectedUrl(QNetworkReply *reply) {
    QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    return !redirection.isEmpty();
}

bool WebImageView::isLoaded() const {
    return mIsLoaded;
}

void WebImageView::setURLToRedirectedUrl(QNetworkReply *reply) {
    QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    QUrl baseUrl = reply->url();
    QUrl resolvedUrl = baseUrl.resolved(redirectionUrl);

    setUrl(resolvedUrl.toString());
}

void WebImageView::clearCache() {
    mNetworkDiskCache->clear();
}

void WebImageView::dowloadProgressed(qint64 bytes, qint64 total) {
    mLoading = double(bytes) / double(total);

    emit loadingChanged();
}
