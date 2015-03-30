// Based on: https://github.com/RileyGB/BlackBerry10-Samples


#include <QNetworkReply>
#include <QNetworkDiskCache>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#else
#include <QtGui/QDesktopServices>
#endif

#include "proxy.h"
#include "settings.h"
#include "utils.h"
#include "cacheserver.h"

QNetworkAccessManager * Proxy::mNetManager = new QNetworkAccessManager();
QNetworkDiskCache * Proxy::mNetworkDiskCache = new QNetworkDiskCache();

Proxy::Proxy() {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    mNetworkDiskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
#else
    mNetworkDiskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#endif
    //mNetManager->setCache(mNetworkDiskCache);

    // Set defaults
    mLoading = 0;
    mReady = false;
}

const QUrl& Proxy::url() const
{
    return mUrl;
}

void Proxy::setUrl(const QUrl& url)
{
    mUrl = url;
    mLoading = 0;
    mReady = false;

    QNetworkRequest request;
    //request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setUrl(url);

    Settings *s = Settings::instance();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    request.setHeader(QNetworkRequest::UserAgentHeader, s->getDmUserAgent());
#else
    request.setRawHeader("User-Agent", s->getDmUserAgent().toLatin1());
#endif
    request.setRawHeader("Accept", "*/*");

    QNetworkReply * reply = mNetManager->get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(loaded()));
    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(dowloadProgressed(qint64,qint64)));
    QObject::connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
#ifndef QT_NO_SSL
    QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    qDebug() << url;

    emit urlChanged();
}

QString Proxy::getData()
{
    filter(data);
    //qDebug() << data;
    return data;
}

void Proxy::appendRelativeUrls(const QString &content, const QRegExp &rx, QStringList &caps)
{
    int i = 1, pos = 0;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2); cap = cap.mid(1,cap.length()-2);
        QUrl capUrl(cap);
        qDebug() << "filter, cap=" << cap;
        if (capUrl.isRelative()) {
            caps.append(baseUrl.resolved(capUrl).toString());
        }
        pos += rx.matchedLength();
        ++i;
    }
}

void Proxy::resolveRelativeUrls(QString &content, const QRegExp &rx)
{
    int i = 1, pos = 0;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2); cap = cap.mid(1,cap.length()-2);
        QUrl capUrl(cap);
        qDebug() << "filter, cap1=" << cap;
        if (capUrl.isRelative()) {
            qDebug() << "filter, cap2=" << baseUrl.resolved(capUrl).toString();
            content.replace(cap, baseUrl.resolved(capUrl).toString());
        }
        pos += rx.matchedLength();
        ++i;
    }
}

void Proxy::filter(QString &content)
{
    /*
    QRegExp rxMetaViewport("<meta\\s[^>]*name\\s*=(\"viewport\"|'viewport')[^>]*>", Qt::CaseInsensitive);
    QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxImgAll("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxBody("(<body[^>]*>)", Qt::CaseInsensitive);
    QRegExp rxImgUrl(image, Qt::CaseInsensitive);
    QRegExp rxLinkAll("<link[^>]*>", Qt::CaseInsensitive);
    QRegExp rxFormAll("<form[^>]*>((?!<\\/form>).)*<\\/form>", Qt::CaseInsensitive);
    QRegExp rxInputAll("<input[^>]*>", Qt::CaseInsensitive);
    QRegExp rxMetaAll("<meta[^>]*>", Qt::CaseInsensitive);
    QRegExp rxScript("(<script\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>", Qt::CaseInsensitive);
    QRegExp rxStyleAll("<style[^>]*>((?!<\\/style>).)*<\\/style>", Qt::CaseInsensitive);
    QRegExp rxStyle("\\s*style\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxClass("\\s*class\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeadEnd("</head>", Qt::CaseInsensitive);

    content = content.replace(rxImgAll,"");
    content = content.replace(rxLinkAll,"");
    content = content.replace(rxScript,"\\1");
    content = content.replace(rxScriptAll,"");
    content = content.replace(rxFormAll,"");
    content = content.replace(rxInputAll,"");
    content = content.replace(rxMetaAll,"");
    content = content.replace(rxMetaViewport,"");
    content = content.replace(rxStyle,"");
    content = content.replace(rxClass,"");
    content = content.replace(rxWidth,"");
    content = content.replace(rxHeight,"");
    content = content.replace(rxStyleAll,"");
    content = content.replace(rxFrame,"\\1");
    content = content.replace(rxA,"\\1");
    content = content.replace(rxUrl,"http://0.0.0.0");
    */

    //content = content.replace(rxCss,"\\1");

    // Change CSS link

    //QString url = baseUrl.toString();

    //QRegExp rxCss("(<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxScript("(<script\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);

    resolveRelativeUrls(content, rxCss);
    resolveRelativeUrls(content, rxImg);
    resolveRelativeUrls(content, rxScript);

    //qDebug() << content;
}

/*QUrl Proxy::getBaseUrl() const
{
    //qDebug() << "baseUrl 1:" << baseUrl;
    //qDebug() << "baseUrl 2:" << QString("%1://%2").arg(baseUrl.scheme()).arg(baseUrl.authority());

    //return QUrl(QString("%1://%2").arg(baseUrl.scheme()).arg(baseUrl.authority()));
    return baseUrl;
}*/

void Proxy::metaDataChanged()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    // Memory protection fix -> not loading big files
    /*if (reply->header(QNetworkRequest::ContentLengthHeader).isValid()) {
        int length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
        if (length > maxSourceSize) {
            //qDebug() << "metaDataChanged, length=" << length;
            reply->close();
            return;
        }
    }*/
}

double Proxy::loading() const
{
    return mLoading;
}

void Proxy::error(QNetworkReply::NetworkError code)
{
    qWarning() << "Proxy error: " << code;
}

void Proxy::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        qWarning() << "SSL error: " << error.errorString();
#else
    Q_UNUSED(sslErrors);
#endif
}

void Proxy::loaded() {
    QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error()) {
    qDebug() << "error" << reply->error();
    qDebug() << "error" << reply->errorString();
    }

    qDebug() << "url" << reply->url();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()) {
        qDebug() << "status code:" << statusCode.toInt();
        qDebug() << "response string:" << reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
    }

    if (reply->error() == QNetworkReply::NoError) {
        if (isARedirectedUrl(reply)) {
            setURLToRedirectedUrl(reply);
            return;
        }
        data = QString(reply->readAll());
        baseUrl = reply->url();
        mReady = true;
    } else {
        mReady = false;
    }

    emit readyChanged();
    reply->deleteLater();
}

bool Proxy::isARedirectedUrl(QNetworkReply *reply) {
    QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    return !redirection.isEmpty();
}

bool Proxy::getReady() const {
    return mReady;
}

void Proxy::setURLToRedirectedUrl(QNetworkReply *reply) {
    QUrl redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    QUrl baseUrl = reply->url();
    QUrl resolvedUrl = baseUrl.resolved(redirectionUrl);

    setUrl(resolvedUrl.toString());
}

void Proxy::clearCache() {
    mNetworkDiskCache->clear();
}

void Proxy::dowloadProgressed(qint64 bytes, qint64 total) {
    mLoading = double(bytes) / double(total);

    emit loadingChanged();
}

QString Proxy::getTempFile()
{
    Settings *s = Settings::instance();
    QString filename = Utils::hash(baseUrl.toString());
    QString filepath = s->getDmCacheDir() + "/" + filename;
    QFile file(filepath);

    if (file.exists()) {
        qWarning() << "File exists, deleting file" << filename;
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open" << filename << "for writing: " << file.errorString();
        return "";
    }

    filter(data);

    file.write(data.toUtf8());
    file.close();

    return s->cache->getUrlbyUrl(baseUrl.toString());
}
