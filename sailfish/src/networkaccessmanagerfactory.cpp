#include "networkaccessmanagerfactory.h"
#include "customnetworkaccessmanager.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0) // QT5

NetworkAccessManagerFactory::NetworkAccessManagerFactory(const QString &userAgent) : QQmlNetworkAccessManagerFactory(),
    userAgent(userAgent)
{
}

#else // QT5

NetworkAccessManagerFactory::NetworkAccessManagerFactory(const QString &userAgent) : QDeclarativeNetworkAccessManagerFactory(),
    userAgent(userAgent)
{
}

#endif // QT5

QNetworkAccessManager* NetworkAccessManagerFactory::create(QObject* parent)
{
    CustomNetworkAccessManager* manager = new CustomNetworkAccessManager(userAgent, parent);
    return manager;
}
