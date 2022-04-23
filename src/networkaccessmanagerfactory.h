#ifndef NETWORKACCESSMANAGERFACTORY_H
#define NETWORKACCESSMANAGERFACTORY_H

#include <QNetworkAccessManager>
#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0) // QT5
#include <QQmlNetworkAccessManagerFactory>

class NetworkAccessManagerFactory: public QQmlNetworkAccessManagerFactory
{
#else // QT5
#include <QDeclarativeNetworkAccessManagerFactory>

class NetworkAccessManagerFactory: public QDeclarativeNetworkAccessManagerFactory
{
#endif // QT5

public:
    explicit NetworkAccessManagerFactory(const QString &userAgent);
    virtual QNetworkAccessManager *create(QObject* parent);

private:
    QString userAgent;

};

#endif // NETWORKACCESSMANAGERFACTORY_H
