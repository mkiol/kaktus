#ifndef NETWORKACCESSMANAGERFACTORY_H
#define NETWORKACCESSMANAGERFACTORY_H

#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkAccessManager>

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    explicit NetworkAccessManagerFactory(QString p_userAgent = "");

    virtual QNetworkAccessManager* create(QObject* parent);

private:
    QString __userAgent;
};
#endif // NETWORKACCESSMANAGERFACTORY_H
