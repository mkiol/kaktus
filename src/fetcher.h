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
#include <QJsonArray>
#else
#include <QVariantMap>
#endif

#include "databasemanager.h"

class FetcherCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    FetcherCookieJar(QObject *parent = 0);
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> & cookieList,
                                   const QUrl & url);
};

class Fetcher : public QThread
{
    Q_OBJECT
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
    Q_ENUM(BusyType)

    explicit Fetcher(QObject *parent = 0);
    ~Fetcher();

    Q_INVOKABLE bool init();
    Q_INVOKABLE bool update();
    Q_INVOKABLE bool checkCredentials();
    Q_INVOKABLE void cancel();

    Q_INVOKABLE virtual void getConnectUrl(int type) = 0;
    Q_INVOKABLE virtual bool setConnectUrl(const QString &url) = 0;
    Q_INVOKABLE void saveImage(const QString &url);

    BusyType readBusyType();
    bool isBusy();

signals:
    void quit();
    void busyChanged();
    void progress(double current, double total);
    void networkNotAccessible();
    void uploading();
    void uploadProgress(double current, double total);
    void checkingCredentials();
    void newAuthUrl(const QString &url, int type);
    void imageSaved(const QString &filename);

    /*
    200 - Fether is busy
    400 - Email/password is not defined
    401 - SignIn failed
    402 - SignIn user/password do no match
    403 - Cookie expired
    404 - TT-RSS API disabled
    500 - Generic network error
    501 - SignIn resposne is null
    502 - Internal error
    503 - User ID is empty
    504 - No network
    505 - Refresh token error
    506 - DB backup error
    600 - Error while parsing JSON
    601 - Unknown JSON response
    700 - Generic SSL error
    800 - Image download error
    801 - Image already exists
     */
    void credentialsValid();
    void errorCheckingCredentials(int code);
    void errorGettingAuthUrl();
    void error(int code);
    void canceled();
    void ready();
    void addDownload(DatabaseManager::CacheItem item);

public slots:
    void sslErrors(const QList<QSslError> &errors);

protected:
    QNetworkAccessManager nam;
    QNetworkConfigurationManager ncm;
    QNetworkReply* currentReply;
    QByteArray data;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject jsonObj;
    QJsonArray jsonArr;
#else
    QVariantMap jsonObj;
    QVariantList jsonArr;
#endif
    Fetcher::BusyType busyType;
    QList<DatabaseManager::Action> actionsList;
    bool busy;
    double proggress;
    double proggressTotal;
    double uploadProggressTotal;

    void setBusy(bool busy, Fetcher::BusyType type = Fetcher::UnknownBusyType);
    bool parse();
    void prepareUploadActions();
    void taskEnd();

private slots:
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);
    bool delayedUpdate(bool state);
    void networkError(QNetworkReply::NetworkError);
    void readyRead();

private:
    virtual void signIn() = 0;
    virtual void startFetching() = 0;
    virtual void uploadActions() = 0;

    void mergeActionsIntoList(DatabaseManager::ActionsTypes typeSet,
                              DatabaseManager::ActionsTypes typeUnset,
                              DatabaseManager::ActionsTypes typeSetList,
                              DatabaseManager::ActionsTypes typeUnsetList);
    void copyImage(const QString &path, const QString &contentType);
};

#endif // FETCHER_H
