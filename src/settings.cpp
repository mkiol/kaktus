/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

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

#include <QDir>
#include <QDebug>
#include <QVariant>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#include <QGuiApplication>
#else
#include <QtGui/QDesktopServices>
#include <QCoreApplication>
#endif

#include "settings.h"
#include "downloadmanager.h"
#include "databasemanager.h"
#include "cacheserver.h"
#include "netvibesfetcher.h"
#include "simplecrypt.h"
#include "../key.h"

Settings* Settings::inst = 0;

Settings::Settings(QObject *parent) : QObject(parent),
    settings(),
    db(NULL),
    cache(NULL),
    dm(NULL),
    fetcher(NULL)
{
    // Reset if not Signed in
    if (!getSignedIn()) {
        reset();
    }

    connect(this, SIGNAL(signedInChanged()), this, SLOT(reset()));
}

Settings* Settings::instance()
{
    if (Settings::inst == NULL) {
        Settings::inst = new Settings();
    }

    return Settings::inst;
}

const QList<QVariant> Settings::viewModeHistory()
{
    return settings.value("viewmodehistory").toList();
}

void Settings::setShowStarredTab(bool value)
{
    if (getShowStarredTab() != value) {
        settings.setValue("showstarredtab", value);
        emit showStarredTabChanged();
    }
}

bool Settings::getShowStarredTab()
{
    return settings.value("showstarredtab", true).toBool();
}

void Settings::setPowerSaveMode(bool value)
{
    if (getPowerSaveMode() != value) {
        settings.setValue("powersavemode", value);
        emit powerSaveModeChanged();
    }
}

bool Settings::getPowerSaveMode()
{
    return settings.value("powersavemode", true).toBool();
}

void Settings::setShowOnlyUnread(bool value)
{
    if (getShowOnlyUnread() != value) {
        settings.setValue("showonlyunread", value);
        emit showOnlyUnreadChanged();
    }
}

bool Settings::getShowOnlyUnread()
{
    return settings.value("showonlyunread", true).toBool();
}

void Settings::setOfflineMode(bool value)
{
#ifdef KAKTUS_LIGHT
    return;
#else
    if (getOfflineMode() != value) {
        settings.setValue("offlinemode", value);
        emit offlineModeChanged();
    }
#endif
}

bool Settings::getOfflineMode()
{
#ifdef KAKTUS_LIGHT
    return false;
#else
    return settings.value("offlinemode", false).toBool();
#endif
}

void Settings::setReaderMode(bool value)
{
    if (getReaderMode() != value) {
        settings.setValue("readermode", value);
        emit readerModeChanged();
    }
}

bool Settings::getReaderMode()
{
    return settings.value("readermode", false).toBool();
}

void Settings::setShowTabIcons(bool value)
{
    if (getShowTabIcons() != value) {
        settings.setValue("showtabicons", value);
        emit showTabIconsChanged();
    }
}

bool Settings::getShowTabIcons()
{
    return settings.value("showtabicons", true).toBool();
}

void Settings::setSignedIn(bool value)
{
    if (getSignedIn() != value) {
        settings.setValue("signedin", value);
        emit signedInChanged();
    }
}

bool Settings::getSignedIn()
{
    return settings.value("signedin", false).toBool();
}

void Settings::setHelpDone(bool value)
{
    if (getHelpDone() != value) {
        settings.setValue("helpdone", value);
        emit helpDoneChanged();
    }
}

bool Settings::getHelpDone()
{
    return settings.value("helpdone", false).toBool();
}

void Settings::setHint1Done(bool value)
{
    settings.setValue("hint1done", value);
}

bool Settings::getHint1Done()
{
    return settings.value("hint1done", false).toBool();
}

void Settings::setAutoDownloadOnUpdate(bool value)
{
    if (getAutoDownloadOnUpdate() != value) {
        settings.setValue("autodownloadonupdate", value);
        emit autoDownloadOnUpdateChanged();
    }
}

bool Settings::getAutoDownloadOnUpdate()
{
    return settings.value("autodownloadonupdate", true).toBool();
}

void Settings::setUsername(const QString &value)
{
    settings.setValue("username", value);
}

QString Settings::getUsername()
{
    return settings.value("username", "").toString();
}

void Settings::setPassword(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedPassword = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("password", encryptedPassword);
}

QString Settings::getPassword()
{
    SimpleCrypt crypto(KEY);
    QString plainPassword = crypto.decryptToString(settings.value("password", "").toString());
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainPassword;
}

void Settings::setCookie(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("cookie", encryptedValue);
}

QString Settings::getCookie()
{
    SimpleCrypt crypto(KEY);
    QString plainValue = crypto.decryptToString(settings.value("cookie", "").toString());
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setTwitterCookie(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("twittercookie", encryptedValue);
}

QString Settings::getTwitterCookie()
{
    SimpleCrypt crypto(KEY);
    QString plainValue = crypto.decryptToString(settings.value("twittercookie", "").toString());
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setAuthUrl(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("authurl", encryptedValue);
}

QString Settings::getAuthUrl()
{
    SimpleCrypt crypto(KEY);
    QString plainValue = crypto.decryptToString(settings.value("authurl", "").toString());
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}


void Settings::setDashboardInUse(const QString &value)
{
    if (getDashboardInUse() != value) {
        if (getDashboardInUse() == "") {
            settings.setValue("dafaultdashboard", value);
        } else {
            settings.setValue("dafaultdashboard", value);
            emit dashboardInUseChanged();
        }
    }
}

QString Settings::getDashboardInUse()
{
    return settings.value("dafaultdashboard", "").toString();
}

void Settings::setLocale(const QString &value)
{
    if (getLocale() != value) {
        settings.setValue("locale", value);
        emit localeChanged();
    }
}

QString Settings::getLocale()
{
    return settings.value("locale", "").toString();
}

void Settings::setLastUpdateDate(int value)
{
    if (getLastUpdateDate() != value) {
        settings.setValue("lastupdatedate", value);
        emit lastUpdateDateChanged();
    }
}

int Settings::getSigninType()
{
    return settings.value("signintype", 0).toInt();
}

void Settings::setSigninType(int value)
{
    if (getSigninType() != value) {
        settings.setValue("signintype", value);
        emit signinTypeChanged();
    }
}

int Settings::getLastUpdateDate()
{
    return settings.value("lastupdatedate", 0).toInt();
}

void Settings::setAllowedOrientations(int value)
{
    if (getAllowedOrientations() != value) {
        settings.setValue("allowedorientations", value);
        emit allowedOrientationsChanged();
    }
}

int Settings::getAllowedOrientations()
{
    return settings.value("allowedorientations", 0).toInt();
}

void Settings::setCachingMode(int value)
{
    if (getCachingMode() != value) {
        settings.setValue("cachingmode", value);
        emit cachingModeChanged();
    }
}

int Settings::getCachingMode()
{
#ifdef KAKTUS_LIGHT
    return 0;
#else
    return settings.value("cachingmode", 0).toInt();
#endif
}

void Settings::setOffsetLimit(int value)
{
    if (getOffsetLimit() != value) {
        settings.setValue("offsetLimit", value);
        emit offsetLimitChanged();
    }
}

int Settings::getOffsetLimit()
{
    return settings.value("offsetLimit", 20).toInt();
}

QString Settings::getSettingsDir()
{

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString value = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).path();
#else
    QString value = QDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation)).path();
#endif

    if (!QDir(value).exists()) {
        if (!QDir::root().mkpath(value)) {
            qWarning() << "Unable to create settings dir!";
            emit error(501);
        }
    }

    return value;
}

void Settings::setDmConnections(int value)
{
    settings.setValue("connections", value);
}

int Settings::getDmConnections()
{
//#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
#if defined(Q_OS_SYMBIAN)
    return settings.value("connections", 1).toInt();
#else
    return settings.value("connections", 10).toInt();
#endif
}

void Settings::setDmTimeOut(int value)
{
    settings.setValue("timeout", value);
}

int Settings::getDmTimeOut()
{
    return settings.value("timeout", 20000).toInt();
}

void Settings::setDmMaxSize(int value)
{
    settings.setValue("maxsize", value);
}

int Settings::getDmMaxSize()
{

    return settings.value("maxsize", 500000).toInt();
}

QString Settings::getDmCacheDir()
{

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString value = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).filePath("cached_files");
#else
    QString value = QDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation)).filePath("cached_files");
#endif

    //qDebug() << "Cache dir is" << value;

    if (!QDir(value).exists()) {
        if (!QDir::root().mkpath(value)) {
            qWarning() << "Unable to create cache dir!";
            emit error(502);
        }
    }

    return value;
}

void Settings::setDmUserAgent(const QString &value)
{
    settings.setValue("useragent", value);
}

QString Settings::getDmUserAgent()
{
    QString value = "Mozilla/5.0 (Linux; Android 4.2.1; Nexus 4 Build/JOP40D) AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.166 Mobile Safari/535.19";
    //QString value = "Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, like Gecko) NokiaBrowser/8.5.0 Mobile Safari/534.13";
    //QString value = "Mozilla/5.0 (Maemo; Linux; U; Jolla; Sailfish; Mobile; rv:26.0) Gecko/26.0 Firefox/26.0 SailfishBrowser/1.0 like Safari/538.1";
    //QString value = "Mozilla/5.0 (Mobile; rv:26.0) Gecko/26.0 Firefox/26.0";
    return settings.value("useragent", value).toString();
}

QString Settings::getOfflineTheme()
{
/*#ifdef BB10
    return settings.value("theme", "white").toString();
#endif*/
    return settings.value("theme", "black").toString();
}

void Settings::setOfflineTheme(const QString &value)
{
    if (getOfflineTheme() != value) {
        settings.setValue("theme", value);
        emit offlineThemeChanged();
    }
}

int Settings::getFontSize()
{
    return settings.value("fontsize", 1).toInt();
}

void Settings::setFontSize(int value)
{
    if (getFontSize() != value) {
        settings.setValue("fontsize", value);
        emit fontSizeChanged();
    }
}

int Settings::getTheme()
{
    // Default is Dark theme
    return settings.value("apptheme", 2).toInt();
}

void Settings::setTheme(int value)
{
    if (getTheme() != value) {
        settings.setValue("apptheme", value);
        emit themeChanged();
    }
}

void Settings::setFeedsAtOnce(int value)
{
    settings.setValue("feedsatonce", value);
}

int Settings::getFeedsAtOnce()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    return settings.value("feedsatonce", 1).toInt();
#else
    return settings.value("feedsatonce", 5).toInt();
#endif
}

void Settings::setFeedsUpdateAtOnce(int value)
{
    settings.setValue("feedsupdateatonce", value);
}

int Settings::getFeedsUpdateAtOnce()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    return settings.value("feedsupdateatonce", 1).toInt();
#else
    return settings.value("feedsupdateatonce", 10).toInt();
#endif
}

void Settings::setViewMode(int value)
{
    if (getViewMode() != value) {
        settings.setValue("viewmode", value);

        //update history
        QList<QVariant> list = settings.value("viewmodehistory").toList();
        if (list.indexOf(value)==-1)
            list.prepend(value);
        if (list.length()>3)
            list.removeLast();
        settings.setValue("viewmodehistory", list);

        emit viewModeChanged();
    }
}

int Settings::getViewMode()
{
    int viewMode = settings.value("viewmode", 0).toInt();
#ifdef KAKTUS_LIGHT
    if (viewMode==5)
        return 0;
#endif
    return viewMode;
}

void Settings::setReinitDB(bool value)
{
    settings.setValue("reinitdb", value);
}

bool Settings::getReinitDB()
{
    return settings.value("reinitdb", false).toBool();
}

void Settings::reset()
{
    if (!getSignedIn()) {
        setLastUpdateDate(0);
        setDashboardInUse("");
        setPassword("");
        setHelpDone(false);
        setCookie("");
        setTwitterCookie("");
        setAuthUrl("");
        setHint1Done(false);
        setCachingMode(0);
    }
}
