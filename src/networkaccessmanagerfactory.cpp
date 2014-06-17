#include "networkaccessmanagerfactory.h"
#include "customnetworkaccessmanager.h"

NetworkAccessManagerFactory::NetworkAccessManagerFactory(QString userAgent) : QDeclarativeNetworkAccessManagerFactory(),
    userAgent(userAgent)
{
}

QNetworkAccessManager* NetworkAccessManagerFactory::create(QObject* parent)
{
    CustomNetworkAccessManager* manager = new CustomNetworkAccessManager(userAgent, parent);
    return manager;
}

