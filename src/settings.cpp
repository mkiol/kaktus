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

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "settings.h"
#include "downloadmanager.h"
#include "databasemanager.h"
#include "cacheserver.h"
#include "netvibesfetcher.h"
#include "simplecrypt.h"
#include "../key.h"

Settings* Settings::inst = 0;

Settings::Settings(QObject *parent) : QObject(parent), settings()
{
    // Reset Last Update date if not Signed in
    if (!getSignedIn()) {
        setLastUpdateDate(0);
        setDashboardInUse("");
    }
}

Settings* Settings::instance()
{
    if (Settings::inst == NULL) {
        Settings::inst = new Settings();
    }

    return Settings::inst;
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
    return settings.value("showonlyunread", false).toBool();
}

void Settings::setOfflineMode(bool value)
{
    if (getOfflineMode() != value) {
        settings.setValue("offlinemode", value);
        emit offlineModeChanged();
    }
}

bool Settings::getOfflineMode()
{
    return settings.value("offlinemode", false).toBool();
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

void Settings::setAutoDownloadOnUpdate(bool value)
{
    settings.setValue("autodownloadonupdate", value);
}

bool Settings::getAutoDownloadOnUpdate()
{
    return settings.value("autodownloadonupdate", true).toBool();
}

void Settings::setNetvibesUsername(const QString &value)
{
    settings.setValue("username", value);
}

QString Settings::getNetvibesUsername()
{
    return settings.value("username", "").toString();
}

void Settings::setNetvibesPassword(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedPassword = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("password", encryptedPassword);
}

QString Settings::getNetvibesPassword()
{
    SimpleCrypt crypto(KEY);
    QString plainPassword = crypto.decryptToString(settings.value("password", "").toString());
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainPassword;
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
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
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

    return settings.value("maxsize", 2000000).toInt();
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

QString Settings::getCsTheme()
{
    return settings.value("theme", "black").toString();
}

void Settings::setCsTheme(const QString &value)
{
    settings.setValue("theme", value);
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
        emit viewModeChanged();
    }
}

int Settings::getViewMode()
{
    return settings.value("viewmode", 0).toInt();
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
    setNetvibesPassword("");
    setHelpDone(false);
}
