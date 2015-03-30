// Based on: https://github.com/RileyGB/BlackBerry10-Samples
#ifndef PROXY_H_
#define PROXY_H_

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QUrl>
#include <QRegExp>
#include <QStringList>

class Proxy: public QObject {
	Q_OBJECT
    Q_PROPERTY (QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY (float loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY (bool ready READ getReady NOTIFY readyChanged)
public:
	Proxy();

    Q_INVOKABLE QString getData();
    Q_INVOKABLE QString getTempFile();

	const QUrl& url() const;
	double loading() const;
    bool getReady() const;

public Q_SLOTS:
	void setUrl(const QUrl& url);
	void clearCache();

private Q_SLOTS:
	void loaded();
	void dowloadProgressed(qint64,qint64);
	void metaDataChanged();
    void sslErrors(const QList<QSslError>& errors);
    void error(QNetworkReply::NetworkError code);

signals:
	void urlChanged();
	void loadingChanged();
    void readyChanged();
	
private:
	static const int maxSourceSize = 500000; // Memory protection fix -> not loading big files
	static QNetworkAccessManager * mNetManager;
	static QNetworkDiskCache * mNetworkDiskCache;

	QUrl mUrl;
    QUrl baseUrl;
    QString data;

	float mLoading;
    bool mReady;
	bool isARedirectedUrl(QNetworkReply *reply);
	void setURLToRedirectedUrl(QNetworkReply *reply);
    void filter(QString &content);
    void appendRelativeUrls(const QString &content, const QRegExp &rx, QStringList &caps);
    void resolveRelativeUrls(QString &content, const QRegExp &rx);
};

#endif /* PROXY_H_ */
