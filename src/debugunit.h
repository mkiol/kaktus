#ifndef DEBUGUNIT_H
#define DEBUGUNIT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class DebugUnit : public QObject
{
    Q_OBJECT
public:
    explicit DebugUnit(QObject *parent = 0);

private slots:
    void networkAccessibleChanged (QNetworkAccessManager::NetworkAccessibility accessible);
    void error(QNetworkReply::NetworkError);
    void readyRead();
    void finished();

public slots:
    void test();

private:
    QNetworkAccessManager nam;
};

#endif // DEBUGUNIT_H
