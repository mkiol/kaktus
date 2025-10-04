/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "settings.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QVariant>

#include "cacheserver.h"
#include "databasemanager.h"
#include "downloadmanager.h"
#include "info.h"
#include "key.h"
#include "simplecrypt.h"

Settings::Settings()
    : QSettings{settingsFilepath(), QSettings::NativeFormat}

{
    qDebug() << "app:" << Kaktus::ORG << Kaktus::APP_ID;
    qDebug() << "config location:"
             << QStandardPaths::writableLocation(
                    QStandardPaths::ConfigLocation);
    qDebug() << "data location:"
             << QStandardPaths::writableLocation(
                    QStandardPaths::AppDataLocation);
    qDebug() << "cache location:"
             << QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    qDebug() << "settings file:" << fileName();

    fixWebView();

    if (!getSignedIn()) reset();
}

QString Settings::settingsFilepath() {
    QDir confDir{
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)};
    confDir.mkpath(QCoreApplication::organizationName() + QDir::separator() +
                   QCoreApplication::applicationName());
    return confDir.absolutePath() + QDir::separator() +
           QCoreApplication::organizationName() + QDir::separator() +
           QCoreApplication::applicationName() + QDir::separator() +
           settingsFilename;
}

QString Settings::pocketConsumerKey() const { return pocket_consumer_key; }

void Settings::setViewModeHistory(QList<ViewMode> history) {
    QVariantList l;
    for (auto m : history) {
        l.push_back(static_cast<int>(m));
    }
    setValue("viewmodehistory", l);
}

QList<Settings::ViewMode> Settings::viewModeHistory() const {
    QList<ViewMode> history;
    auto h = value("viewmodehistory").toList();
    for (auto m : h) {
        history.push_back(static_cast<ViewMode>(m.toInt()));
    }
    return history;
}

void Settings::setShowStarredTab(bool value) {
    if (getShowStarredTab() != value) {
        setValue("showstarredtab", value);
        emit showStarredTabChanged();
    }
}

bool Settings::getShowStarredTab() const {
    return value("showstarredtab", true).toBool();
}

void Settings::setFilter(int value) {
    if (getFilter() != value) {
        setValue("filter", value);
        emit filterChanged();
    }
}

int Settings::getFilter() const { return value("filter", 0).toInt(); }

void Settings::setShowOldestFirst(bool value) {
    if (getShowOldestFirst() != value) {
        setValue("showoldestfirst", value);
        emit showOldestFirstChanged();
    }
}

bool Settings::getShowOldestFirst() const {
    return value("showoldestfirst", false).toBool();
}

void Settings::setShowBroadcast(bool value) {
    if (getShowBroadcast() != value) {
        setValue("showbroadcast", value);
        emit showBroadcastChanged();
    }
}

bool Settings::getShowBroadcast() const {
    return value("showbroadcast", false).toBool();
}

void Settings::setOfflineMode(bool value) {
    if (getOfflineMode() != value) {
        setValue("offlinemode", value);
        emit offlineModeChanged();
    }
}

bool Settings::getOfflineMode() const {
    return value("offlinemode", false).toBool();
}

void Settings::setAutoOffline(bool value) {
    if (getAutoOffline() != value) {
        setValue("autooffline", value);
        emit autoOfflineChanged();
    }
}

bool Settings::getAutoOffline() const {
    return value("autooffline", true).toBool();
}

void Settings::setReaderMode(bool value) {
    if (getReaderMode() != value) {
        setValue("readermode", value);
        emit readerModeChanged();
    }
}

bool Settings::getReaderMode() const {
    return value("readermode", false).toBool();
}

void Settings::setPocketEnabled(bool value) {
    if (getPocketEnabled() != value) {
        setValue("pocketenabled", value);
        emit pocketEnabledChanged();
    }
}

bool Settings::getPocketEnabled() const {
    return value("pocketenabled", false).toBool();
}

void Settings::setPocketQuickAdd(bool value) {
    if (getPocketQuickAdd() != value) {
        setValue("pocketquickadd", value);
        emit pocketQuickAddChanged();
    }
}

bool Settings::getPocketQuickAdd() const {
    return value("pocketquickadd", false).toBool();
}

void Settings::setPocketFavorite(bool value) {
    if (getPocketFavorite() != value) {
        setValue("pocketfavorite", value);
        emit pocketFavoriteChanged();
    }
}

bool Settings::getPocketFavorite() const {
    return value("pocketfavorite", false).toBool();
}

void Settings::setPocketToken(const QString &value) {
    if (getPocketToken() != value) {
        SimpleCrypt crypto(KEY);
        QString encryptedToken = crypto.encryptToString(value);
        if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
            emit error(532);
            return;
        }
        setValue("pockettoken", encryptedToken);
        emit pocketTokenChanged();
    }
}

QString Settings::getPocketToken() {
    SimpleCrypt crypto(KEY);
    QString plainToken =
        crypto.decryptToString(value("pockettoken", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(531);
        return "";
    }
    return plainToken;
}

void Settings::setPocketTags(const QString &value) {
    if (getPocketTags() != value) {
        setValue("pockettags", value);
        emit pocketTagsChanged();
    }
}

QString Settings::getPocketTags() const {
    return value("pockettags", "").toString();
}

void Settings::setPocketTagsHistory(const QString &value) {
    if (getPocketTagsHistory() != value) {
        setValue("pockettagshistory", value);
        emit pocketTagsHistoryChanged();
    }
}

QString Settings::getPocketTagsHistory() const {
    return value("pockettagshistory", "").toString();
}

void Settings::setNightMode(bool value) {
    if (getNightMode() != value) {
        setValue("nightmode", value);
        emit nightModeChanged();
    }
}

bool Settings::getNightMode() const {
    return value("nightmode", false).toBool();
}

void Settings::setSyncRead(bool value) {
    if (getSyncRead() != value) {
        setValue("syncread", value);
        emit syncReadChanged();
    }
}

bool Settings::getSyncRead() const { return value("syncread", false).toBool(); }

void Settings::setClickBehavior(int value) {
    if (getClickBehavior() != value) {
        setValue("clickbehavior", value);
        emit clickBehaviorChanged();
    }
}

int Settings::getClickBehavior() const {
    return value("clickbehavior", 0).toInt();
}

void Settings::setWebviewNavigation(int value) {
    if (getWebviewNavigation() != value) {
        setValue("webviewnavigation", value);
        emit webviewNavigationChanged();
    }
}

int Settings::getWebviewNavigation() const {
    // Default is 2 - open in web view
    return value("webviewnavigation", 2).toInt();
}

void Settings::setShowTabIcons(bool value) {
    if (getShowTabIcons() != value) {
        setValue("showtabicons", value);
        emit showTabIconsChanged();
    }
}

bool Settings::getShowTabIcons() const {
    return value("showtabicons", true).toBool();
}

void Settings::setSignedIn(bool value) {
    if (getSignedIn() != value) {
        setValue("signedin", value);
        emit signedInChanged();
        reset();
    }
}

bool Settings::getSignedIn() const { return value("signedin", false).toBool(); }

void Settings::setExpandedMode(bool value) {
    if (getExpandedMode() != value) {
        setValue("expandedmode", value);
        emit expandedModeChanged();
    }
}

bool Settings::getExpandedMode() const {
    return value("expandedmode", false).toBool();
}

void Settings::setHelpDone(bool value) {
    if (getHelpDone() != value) {
        setValue("helpdone", value);
        emit helpDoneChanged();
    }
}

bool Settings::getHelpDone() const { return value("helpdone", false).toBool(); }

void Settings::setDoublePane(bool value) {
    if (getDoublePane() != value) {
        setValue("doublepane", value);
        emit doublePaneChanged();
    }
}

bool Settings::getDoublePane() const {
    return value("doublepane", true).toBool();
}

void Settings::setHint1Done(bool value) { setValue("hint1done", value); }

bool Settings::getHint1Done() const {
    return value("hint1done", false).toBool();
}

void Settings::setAutoDownloadOnUpdate(bool value) {
    if (getAutoDownloadOnUpdate() != value) {
        setValue("autodownloadonupdate", value);
        emit autoDownloadOnUpdateChanged();
    }
}

bool Settings::getAutoDownloadOnUpdate() const {
    return value("autodownloadonupdate", true).toBool();
}

void Settings::setUrl(const QString &value) { setValue("url", value); }

QString Settings::getUrl() const { return value("url", "").toString(); }

void Settings::setUsername(const QString &value) {
    setValue("username", value);
}

QString Settings::getUsername() const {
    return value("username", "").toString();
}

void Settings::setPassword(const QString &value) {
    SimpleCrypt crypto(KEY);
    QString encryptedPassword = crypto.encryptToString(value);
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    setValue("password", encryptedPassword);
}

QString Settings::getPassword() {
    SimpleCrypt crypto(KEY);
    QString plainPassword =
        crypto.decryptToString(value("password", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainPassword;
}

void Settings::setUserId(const QString &value) { setValue("userid", value); }

QString Settings::getUserId() const { return value("userid", "").toString(); }

void Settings::setCookie(const QString &value) {
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    setValue("cookie", encryptedValue);
}

QString Settings::getCookie() {
    SimpleCrypt crypto(KEY);
    QString plainValue = crypto.decryptToString(value("cookie", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setRefreshCookie(const QString &value) {
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    setValue("refreshcookie", encryptedValue);
}

QString Settings::getRefreshCookie() {
    SimpleCrypt crypto(KEY);
    QString plainValue =
        crypto.decryptToString(value("refreshcookie", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setTwitterCookie(const QString &value) {
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    setValue("twittercookie", encryptedValue);
}

QString Settings::getTwitterCookie() {
    SimpleCrypt crypto(KEY);
    QString plainValue =
        crypto.decryptToString(value("twittercookie", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setAuthUrl(const QString &value) {
    SimpleCrypt crypto(KEY);
    QString encryptedValue = crypto.encryptToString(value);
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(512);
    }
    setValue("authurl", encryptedValue);
}

QString Settings::getAuthUrl() {
    SimpleCrypt crypto(KEY);
    QString plainValue =
        crypto.decryptToString(value("authurl", "").toString());
    if (crypto.lastError() != SimpleCrypt::ErrorNoError) {
        emit error(511);
        return "";
    }
    return plainValue;
}

void Settings::setDashboardInUse(const QString &value) {
    if (getDashboardInUse() != value) {
        if (getDashboardInUse() == "") {
            setValue("dafaultdashboard", value);
        } else {
            setValue("dafaultdashboard", value);
            emit dashboardInUseChanged();
        }
    }
}

QString Settings::getDashboardInUse() const {
    return value("dafaultdashboard", "").toString();
}

void Settings::setProvider(const QString &value) {
    setValue("provider", value);
}

QString Settings::getProvider() const {
    return value("provider", "").toString();
}

void Settings::setLocale(const QString &value) {
    if (getLocale() != value) {
        setValue("locale", value);
        emit localeChanged();
    }
}

QString Settings::getLocale() const {
    QString locale = value("locale", "").toString();
    if (locale == "" || locale == "cs" || locale == "de" || locale == "es" ||
        locale == "en" || locale == "it" || locale == "nl" || locale == "pl" ||
        locale == "ru")
        return locale;
    return "";
}

void Settings::setLastUpdateDate(int value) {
    if (getLastUpdateDate() != value) {
        setValue("lastupdatedate", value);
        emit lastUpdateDateChanged();
    }
}

int Settings::getSigninType() const { return value("signintype", 0).toInt(); }

void Settings::setSigninType(int value) {
    if (getSigninType() != value) {
        setValue("signintype", value);
        emit signinTypeChanged();
    }
}

int Settings::getLastUpdateDate() const {
    return value("lastupdatedate", 0).toInt();
}

void Settings::setAllowedOrientations(int value) {
    if (getAllowedOrientations() != value) {
        setValue("allowedorientations", value);
        emit allowedOrientationsChanged();
    }
}

int Settings::getAllowedOrientations() const {
    return value("allowedorientations", 0).toInt();
}

void Settings::setCachingMode(int value) {
    if (getCachingMode() != value) {
        setValue("cachingmode", value);
        emit cachingModeChanged();
    }
}

int Settings::getCachingMode() const { return value("cachingmode", 0).toInt(); }

void Settings::setOffsetLimit(int value) {
    if (getOffsetLimit() != value) {
        setValue("offsetLimit", value);
        emit offsetLimitChanged();
    }
}

int Settings::getOffsetLimit() const {
    return value("offsetLimit", 150).toInt();
}

QString Settings::getSettingsDir() {
    auto dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (!QDir{dir}.exists()) {
        if (!QDir::root().mkpath(dir)) {
            qWarning() << "unable to create settings dir";
            emit error(501);
        }
    }

    return dir;
}

void Settings::setDmConnections(int value) { setValue("connections", value); }

int Settings::getDmConnections() const {
    return value("connections", 10).toInt();
}

void Settings::setDmTimeOut(int value) { setValue("timeout", value); }

int Settings::getDmTimeOut() const { return value("timeout", 20000).toInt(); }

void Settings::setDmMaxSize(int value) { setValue("maxsize", value); }

int Settings::getDmMaxSize() const { return value("maxsize", 500000).toInt(); }

QString Settings::getDmCacheDir() {
    QString dir =
        QDir{QStandardPaths::writableLocation(QStandardPaths::CacheLocation)}
            .filePath("cached_files");

    if (!QDir{dir}.exists()) {
        if (!QDir::root().mkpath(dir)) {
            qWarning() << "Unable to create cache dir";
            emit error(502);
        }
    }

    return dir;
}

QString Settings::getDmUserAgent() const {
    QString agent =
        "Mozilla/5.0 (Linux; Android 4.2.1; Nexus 4 Build/JOP40D) "
        "AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.166 Mobile "
        "Safari/535.19";
    return value("useragent", agent).toString();
}

int Settings::getFontSize() const {
    int size = value("fontsize", 10).toInt();
    return size < 5 ? 5 : size > 50 ? 50 : size;
}

void Settings::setFontSize(int value) {
    // Min value is 5 & max value is 50
    if (value > 50 || value < 5) return;

    if (getFontSize() != value) {
        setValue("fontsize", value);
        emit fontSizeChanged();
    }
}

float Settings::getZoom() const {
    return std::min(std::max(minZoom, value("zoom", 1.0).toFloat()), maxZoom);
}

void Settings::setZoom(float value) {
    value = std::min(std::max(minZoom, value), maxZoom);
    if (getZoom() != value) {
        setValue("zoom", value);
        emit zoomChanged();
    }
}

QString Settings::zoomViewport() const {
    return QString::number(getZoom(), 'f', 1);
}

QString Settings::zoomFontSize() const {
    return QString::number(100 + ((getZoom() - 1.0) * 10), 'f', 0) + "%";
}

int Settings::getRetentionDays() const {
    return value("retentiondays", 14).toInt();
}

void Settings::setRetentionDays(int value) { setValue("retentiondays", value); }

int Settings::getTheme() const { return value("apptheme", 2).toInt(); }

void Settings::setTheme(int value) {
    if (getTheme() != value) {
        setValue("apptheme", value);
        emit themeChanged();
    }
}

void Settings::setFeedsAtOnce(int value) { setValue("feedsatonce", value); }

int Settings::getFeedsAtOnce() const { return value("feedsatonce", 5).toInt(); }

void Settings::setFeedsUpdateAtOnce(int value) {
    setValue("feedsupdateatonce", value);
}

int Settings::getFeedsUpdateAtOnce() const {
    return value("feedsupdateatonce", 10).toInt();
}

/*
View modes:
0 - Tabs->Feeds->Entries
1 - Tabs->Entries
2 - Feeds->Entries
3 - All entries
4 - Saved entries
5 - Slow entries
6 - Liked entries (Old Reader)
7 - Broadcasted entries (Old Reader)
*/
void Settings::setViewMode(ViewMode mode) {
    int type = getSigninType();
    if (getViewMode() != mode) {
        if (type < 10) {
            // Netvibes, Forbidden modes: 6, 7
            if (mode == ViewMode::LikedEntries ||
                mode == ViewMode::BroadcastedEntries) {
                qWarning() << "netvibes forbidden mode";
                return;
            }
        } else if (type < 20) {
            // OldReader, Forbidden modes: none
        } else if (type >= 30 && type < 40) {
            // TT-RSS, Forbidden modes: 5, 6, 7
            if (mode == ViewMode::SlowEntries ||
                mode == ViewMode::LikedEntries ||
                mode == ViewMode::BroadcastedEntries) {
                qWarning() << "ttrss forbidden mode";
                return;
            }
        }

        setValue("viewmode", static_cast<int>(mode));

        // update history
        auto history = viewModeHistory();
        if (history.indexOf(mode) == -1) history.prepend(mode);
        if (history.length() > 3) history.removeLast();
        setViewModeHistory(history);
        emit viewModeChanged();
    }
}

Settings::ViewMode Settings::getViewMode() const {
    return static_cast<ViewMode>(value("viewmode", 0).toInt());
}

void Settings::setReinitDB(bool value) { setValue("reinitdb", value); }

bool Settings::getReinitDB() const { return value("reinitdb", false).toBool(); }

void Settings::reset() {
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
        setIgnoreSslErrors(false);
    }
}

void Settings::setIgnoreSslErrors(bool value) {
    if (getIgnoreSslErrors() != value) {
        setValue("ignoresslerrors", value);
        emit ignoreSslErrorsChanged();
    }
}

bool Settings::getIgnoreSslErrors() const {
    return value("ignoresslerrors", false).toBool();
}

void Settings::setImagesDir(const QString &value) {
    if (getImagesDir() != value) {
        setValue("imagesdir", value);
        emit imagesDirChanged();
    }
}

QString Settings::getImagesDir() const {
    auto dir = value("imagesdir", "").toString();
    QFileInfo d{dir};
    if (d.exists() && d.isDir()) return dir;
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}

QUrl Settings::appIcon() const {
    return QUrl::fromLocalFile(
        QString(QStringLiteral("/usr/share/icons/hicolor/172x172/apps/%1.png"))
            .arg(Kaktus::APP_BINARY_ID));
}

void Settings::fixWebView() {
    QFile::remove(
        QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
        "/__PREFS_WRITTEN__");
}
