// Based on: https://github.com/RileyGB/BlackBerry10-Samples

#ifndef WEBIMAGEVIEW_H_
#define WEBIMAGEVIEW_H_

#include <bb/cascades/ImageView>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>
#include <QRect>
#include <bb/ImageData>
#include <QtGui/QImage>

using namespace bb::cascades;

class WebImageView: public bb::cascades::ImageView {
	Q_OBJECT
	Q_PROPERTY (QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY (float loading READ loading NOTIFY loadingChanged)
	Q_PROPERTY (bool isLoaded READ isLoaded NOTIFY isLoadedChanged)
	Q_PROPERTY (int width READ getWidth NOTIFY widthChanged)
	Q_PROPERTY (bool doSizeCheck READ getDoSizeCheck WRITE setDoSizeCheck NOTIFY doSizeCheckChanged)
	//Q_PROPERTY (int height READ getHeight NOTIFY heightChanged)
	//Q_PROPERTY (int size READ getSize NOTIFY sizeChanged)

public:
	WebImageView();
	const QUrl& url() const;
	double loading() const;
	bool isLoaded() const;
	int getWidth() const;
	//int getHeight() const;
	//int getSize() const;
	bool getDoSizeCheck();

public Q_SLOTS:
	void setUrl(const QUrl& url);
    void clearCache();
    void setDoSizeCheck(bool value);

private Q_SLOTS:
	void imageLoaded();
	void dowloadProgressed(qint64,qint64);
	void metaDataChanged();

signals:
	void urlChanged();
	void loadingChanged();
	void isLoadedChanged();
	void widthChanged();
	void doSizeCheckChanged();
	//void heightChanged();
	//void sizeChanged();


private:
	static QNetworkAccessManager * mNetManager;
	static QNetworkDiskCache * mNetworkDiskCache;

    static const int maxSourceSize = 500000; // Memory protection fix -> not loading big images
    static const int minSourceSize = 2000; // Tiny images

	QUrl mUrl;
	float mLoading;
	bool mIsLoaded;
	bool doSizeCheck;

	bool isARedirectedUrl(QNetworkReply *reply);
	void setURLToRedirectedUrl(QNetworkReply *reply);

    const static QString availableColors[5];
    const static QString spriteMap[5][8];

    bb::ImageData fromQImage(const QImage &qImage);
    int getOffsetByColor(const QString &color);
    QRect getPosition(const QString &icon, const QString &color);

    int sourceWidth;
    int sourceHeight;
    int sourceSize;
};

#endif /* WEBIMAGEVIEW_H_ */
