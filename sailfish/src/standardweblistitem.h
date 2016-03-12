// Based on: https://github.com/RileyGB/BlackBerry10-Samples

#ifndef STANDARDWEBLISTITEM_H_
#define STANDARDWEBLISTITEM_H_

#include <bb/cascades/StandardListItem>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>

using namespace bb::cascades;

class StandardWebListItem: public bb::cascades::StandardListItem
{
    Q_OBJECT
    Q_PROPERTY (QUrl url READ getUrl WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY (float loading READ getLoading NOTIFY loadingChanged)

public:
    StandardWebListItem();
    virtual ~StandardWebListItem();
    const QUrl& getUrl() const;
    double getLoading() const;

public Q_SLOTS:
    void setUrl(const QUrl& url);
    void clearCache();

private Q_SLOTS:
    void imageLoaded();
    void dowloadProgressed(qint64,qint64);

signals:
    void urlChanged();
    void loadingChanged();

private:
    static QNetworkAccessManager * nm;
    static QNetworkDiskCache * nc;
    QUrl url;
    float loading;

    bool isARedirectedUrl(QNetworkReply *reply);
    void setURLToRedirectedUrl(QNetworkReply *reply);
};

#endif /* STANDARDWEBLISTITEM_H_ */
