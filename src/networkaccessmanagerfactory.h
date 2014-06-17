#ifndef NETWORKACCESSMANAGERFACTORY_H
#define NETWORKACCESSMANAGERFACTORY_H

#include <QDeclarativeNetworkAccessManagerFactory>

class NetworkAccessManagerFactory: public QDeclarativeNetworkAccessManagerFactory
{

public:
    NetworkAccessManagerFactory(QString userAgent);
    QNetworkAccessManager *create(QObject* parent);

private:
    QString userAgent;

};

#endif // NETWORKACCESSMANAGERFACTORY_H
