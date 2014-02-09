#ifndef CUSTOMNETWORKACCESSMANAGER_H
#define CUSTOMNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>

class CustomNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit CustomNetworkAccessManager(QString p_userAgent = "", QObject *parent = 0);

    QNetworkReply *get(const QNetworkRequest & req);

signals:

public slots:

protected:
    QNetworkReply *createRequest( Operation op, const QNetworkRequest &req, QIODevice * outgoingData=0 );

private:
    QString __userAgent;
};

#endif // CUSTOMNETWORKACCESSMANAGER_H
