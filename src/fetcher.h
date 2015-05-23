/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FETCHER_H
#define FETCHER_H

#include <QThread>
#include <QString>
#include <QList>
#include <QUrl>
#include <QSslError>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkCookieJar>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonObject>
#else
#include <QVariantMap>
#endif

#include "databasemanager.h"

class FetcherCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    FetcherCookieJar(QObject *parent = 0);
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, const QUrl & url);
};

class Fetcher : public QThread
{
    Q_OBJECT
    Q_ENUMS(BusyType)
    Q_PROPERTY (bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY (BusyType busyType READ readBusyType NOTIFY busyChanged)

public:

    enum BusyType {
        UnknownBusyType = 0,
        Initiating = 1,
        Updating = 2,
        CheckingCredentials = 3,
        GettingAuthUrl = 4,
        InitiatingWaiting = 11,
        UpdatingWaiting = 21,
        CheckingCredentialsWaiting = 31,
        GettingAuthUrlWaiting = 41
    };

    explicit Fetcher(QObject *parent = 0);
    ~Fetcher();

    Q_INVOKABLE bool init();
    Q_INVOKABLE bool update();
    Q_INVOKABLE bool checkCredentials();
    Q_INVOKABLE void cancel();

    Q_INVOKABLE virtual void getConnectUrl(int type) = 0;
    Q_INVOKABLE virtual bool setConnectUrl(const QString &url) = 0;

    BusyType readBusyType();
    bool isBusy();

Q_SIGNALS:
    void quit();
    void busyChanged();
    void progress(int current, int total);
    void networkNotAccessible();
    void uploading();
    void checkingCredentials();
    void newAuthUrl(const QString &url, int type);

    /*
    200 - Fether is busy
    400 - Email/password is not defined
    401 - SignIn failed
    402 - SignIn user/password do no match
    403 - Cookie expired
    500 - Network error
    501 - SignIn resposne is null
    502 - Internal error
    600 - Error while parsing JSON
    601 - Unknown JSON response
     */
    void credentialsValid();
    void errorCheckingCredentials(int code);
    void errorGettingAuthUrl();
    void error(int code);
    void canceled();
    void ready();
    void addDownload(DatabaseManager::CacheItem item);

private Q_SLOTS:
    void sslErrors(const QList<QSslError> &errors);
    void networkAccessibleChanged (QNetworkAccessManager::NetworkAccessibility accessible);
    bool delayedUpdate(bool state);
    void networkError(QNetworkReply::NetworkError);
    void readyRead();

protected:
    QNetworkAccessManager nam;
    QNetworkConfigurationManager ncm;
    QNetworkReply* currentReply;
    QByteArray data;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject jsonObj;
#else
    QVariantMap jsonObj;
#endif
    Fetcher::BusyType busyType;
    QList<DatabaseManager::Action> actionsList;
    bool busy;
    int proggress;
    int proggressTotal;

    void setBusy(bool busy, Fetcher::BusyType type = Fetcher::UnknownBusyType);
    bool parse();
    void prepareUploadActions();
    void taskEnd();

private:
    virtual void signIn() = 0;
    virtual void startFetching() = 0;
    virtual void uploadActions() = 0;
};

#endif // FETCHER_H
