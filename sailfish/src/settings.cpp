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

void Settings::setFilter(int value)
{
    if (getFilter() != value) {
        settings.setValue("filter", value);
        emit filterChanged();
    }
}

int Settings::getFilter()
{
    return settings.value("filter", 0).toInt();
}

void Settings::setShowOldestFirst(bool value)
{
    if (getShowOldestFirst() != value) {
        settings.setValue("showoldestfirst", value);
        emit showOldestFirstChanged();
    }
}

bool Settings::getShowOldestFirst()
{
    return settings.value("showoldestfirst", false).toBool();
}

void Settings::setIconContextMenu(bool value)
{
    if (getIconContextMenu() != value) {
        settings.setValue("iconcontextmenu", value);
        emit showOldestFirstChanged();
    }
}

bool Settings::getIconContextMenu()
{
    return settings.value("iconcontextmenu", true).toBool();
}

void Settings::setShowBroadcast(bool value)
{
    if (getShowBroadcast() != value) {
        settings.setValue("showbroadcast", value);
        emit showBroadcastChanged();
    }
}

bool Settings::getShowBroadcast()
{
    return settings.value("showbroadcast", true).toBool();
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

void Settings::setAutoOffline(bool value)
{
    if (getAutoOffline() != value) {
        settings.setValue("autooffline", value);
        emit autoOfflineChanged();
    }
}

bool Settings::getAutoOffline()
{
    return settings.value("autooffline", true).toBool();
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

void Settings::setNightMode(bool value)
{
    if (getNightMode() != value) {
        settings.setValue("nightmode", value);
        emit nightModeChanged();
    }
}

bool Settings::getNightMode()
{
    return settings.value("nightmode", false).toBool();
}

void Settings::setSyncRead(bool value)
{
    if (getSyncRead() != value) {
        settings.setValue("syncread", value);
        emit syncReadChanged();
    }
}

bool Settings::getSyncRead()
{
    return settings.value("syncread", false).toBool();
}

void Settings::setClickBehavior(int value)
{
    if (getClickBehavior() != value) {
        settings.setValue("clickbehavior", value);
        emit clickBehaviorChanged();
    }
}

int Settings::getClickBehavior()
{
    return settings.value("clickbehavior", 0).toInt();
}

void Settings::setWebviewNavigation(int value)
{
    if (getWebviewNavigation() != value) {
        settings.setValue("webviewnavigation", value);
        emit webviewNavigationChanged();
    }
}

int Settings::getWebviewNavigation()
{
    // Default is 0 - open in web view
    return settings.value("webviewnavigation", 0).toInt();
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

void Settings::setExpandedMode(bool value)
{
    if (getExpandedMode() != value) {
        settings.setValue("expandedmode", value);
        emit expandedModeChanged();
    }
}

bool Settings::getExpandedMode()
{
    return settings.value("expandedmode", false).toBool();
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

void Settings::setDoublePane(bool value)
{
    if (getDoublePane() != value) {
        settings.setValue("doublepane", value);
        emit doublePaneChanged();
    }
}

bool Settings::getDoublePane()
{
    return settings.value("doublepane", false).toBool();
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

void Settings::setUserId(const QString &value)
{
    settings.setValue("userid", value);
}

QString Settings::getUserId()
{
    return settings.value("userid", "").toString();
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

void Settings::setRefreshCookie(const QString &value)
{
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (!crypto.lastError() == SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    settings.setValue("refreshcookie", encryptedValue);
}

QString Settings::getRefreshCookie()
{
    SimpleCrypt crypto(KEY);
    QString plainValue = crypto.decryptToString(settings.value("refreshcookie", "").toString());
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

void Settings::setProvider(const QString &value)
{
    settings.setValue("provider", value);
}

QString Settings::getProvider()
{
    return settings.value("provider", "").toString();
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
    return settings.value("offsetLimit", 150).toInt();
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
#ifdef BB10
    return settings.value("connections", 10).toInt();
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

QString Settings::getReaderTheme()
{
    QString theme = settings.value("theme", "dark").toString();
    return theme != "light" ? "dark" : "light";
}

void Settings::setReaderTheme(const QString &value)
{
    if (getReaderTheme() != value) {
        settings.setValue("theme", value);
        emit readerThemeChanged();
    }
}

int Settings::getFontSize()
{
    int size = settings.value("fontsize", 10).toInt();
    return size < 5 ? 5 : size > 50 ? 50 : size;
}

void Settings::setFontSize(int value)
{
    // Min value is 5 & max value is 50
    if (value > 50 || value < 5)
        return;

    if (getFontSize() != value) {
        settings.setValue("fontsize", value);
        emit fontSizeChanged();
    }
}

float Settings::getZoom()
{
    float size = settings.value("zoom", 1.0).toFloat();
    //size = static_cast<float>(static_cast<int>(size*100+0.5))/100.0;
    return size < 0.5 ? 0.5 : size > 2.0 ? 2.0 : size;
}

void Settings::setZoom(float value)
{
    // Min value is 0.5 & max value is 2.0
    if (value < 0.5 || value > 2.0)
        return;

    //value = static_cast<float>(static_cast<int>(value*100+0.5))/100.0;

    if (getZoom() != value) {
        settings.setValue("zoom", value);
        emit zoomChanged();
    }
}

int Settings::getRetentionDays()
{
    // Default is 14 days
    return settings.value("retentiondays", 14).toInt();
}

void Settings::setRetentionDays(int value)
{
#ifdef KAKTUS_LIGHT
    if (value < 1 || value >= 30 )
        return;
#endif
    settings.setValue("retentiondays", value);
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
    int type = getSigninType();
    if (getViewMode() != value) {
        if (type < 10) {
            // Netvibes, Forbidden modes: 6, 7
            if (value == 6 || value == 7) {
                qWarning() << "Netvibes forbidden mode!";
                return;
            }
        } else if (type >= 10 && type < 20) {
            // OldReader, Forbidden modes: none
        } else if (type >= 20 && type < 30) {
            // Feedly, Forbidden modes: 6, 7
            if (value == 6 || value == 7) {
                qWarning() << "Old Reader forbidden mode!";
                return;
            }
        }

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
        setRetentionDays(14);
        setSyncRead(false);
        setProvider("");
        setUserId("");
        setShowBroadcast(true);
    }
}
