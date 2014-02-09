#include "networkaccessmanagerfactory.h"
#include "customnetworkaccessmanager.h"

NetworkAccessManagerFactory::NetworkAccessManagerFactory(QString p_userAgent) : QQmlNetworkAccessManagerFactory(), __userAgent(p_userAgent)
{
}

QNetworkAccessManager* NetworkAccessManagerFactory::create(QObject* parent)
{
    CustomNetworkAccessManager* manager = new CustomNetworkAccessManager(__userAgent, parent);
    return manager;
}
