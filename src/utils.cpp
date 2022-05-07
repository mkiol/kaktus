/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "utils.h"

#include <QtCore/qmath.h>
#include <sailfishapp.h>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QGuiApplication>
#include <QQmlContext>
#include <QRegExp>
#include <QStandardPaths>
#include <QTextDocument>
#include <QtGui/QClipboard>

#include "databasemanager.h"
#include "fetcher.h"
#include "nvfetcher.h"
#include "oldreaderfetcher.h"
#include "ttrssfetcher.h"

Utils::Utils(QObject *parent) : QObject{parent} {}

QString Utils::readAsset(const QString &path) const {
    QFile file{SailfishApp::pathTo(path).toLocalFile()};

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << path
                   << "for reading: " << file.errorString();
        return {};
    }

    return QString::fromUtf8(file.readAll());
}

void Utils::copyToClipboard(const QString &text) const {
    QGuiApplication::clipboard()->setText(text);
}

void Utils::resetWebViewStatic() {
    auto cache =
        QDir{QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
             QStringLiteral("/.mozilla")};
    cache.removeRecursively();
}

void Utils::resetWebView() const { resetWebViewStatic(); }

void Utils::log(const QString &data) {
    int l = data.length();
    int n = 120;
    QString d;
    for (int i = 0; i < l; ++i) {
        d += data.at(i);
        if (i % n == 0) {
            qDebug() << i << d;
            d.clear();
        }
    }
}

QString Utils::formatHtml(QString data, bool offline,
                          const QString &style) const {
    static const QRegExp rxImg{QStringLiteral("<img[^>]*>"),
                               Qt::CaseInsensitive};
    static const QRegExp rxWidth{
        QStringLiteral("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')"),
        Qt::CaseInsensitive};
    static const QRegExp rxTarget{
        QStringLiteral("\\s*target\\s*=\\s*(\"[^\"]*\"|'[^']*')"),
        Qt::CaseInsensitive};
    static const QRegExp rxHeight{
        QStringLiteral("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')"),
        Qt::CaseInsensitive};
    static const QRegExp rxSizes{
        QStringLiteral("\\s*sizes\\s*=\\s*(\"[^\"]*\"|'[^']*')"),
        Qt::CaseInsensitive};
    static const QRegExp rxA{QStringLiteral("<a[^>]*></a>"),
                             Qt::CaseInsensitive};
    static const QRegExp rxP{QStringLiteral("<p[^>]*></p>"),
                             Qt::CaseInsensitive};

    data.remove(rxTarget);
    if (offline) {
        data.remove(rxImg);
        data.remove(QStringLiteral("</img>"), Qt::CaseInsensitive);
        data.remove(rxA);
        data.remove(rxP);
    } else {
        data.remove(rxWidth);
        data.remove(rxHeight);
        data.remove(rxSizes);
    }

    data =
        QStringLiteral(
            "<html><head><meta name=\"viewport\" content=\"width=device-width, "
            "maximum-scale=1.0, initial-scale=1.0\">") +
        (style.isEmpty() ? "" : "<style>" + style + "</style>") +
        QStringLiteral("</head><body>") + data +
        QStringLiteral("</body></html>");

    return data;
}

void Utils::setRootModel() {
    switch (Settings::instance()->getViewMode()) {
        case Settings::ViewMode::TabsFeedsEntries:
        case Settings::ViewMode::TabsEntries:
            setTabModel(Settings::instance()->getDashboardInUse());
            feedModel.reset();
            entryModel.reset();
            break;
        case Settings::ViewMode::FeedsEntries:
            setFeedModel(QStringLiteral("root"));
            tabModel.reset();
            entryModel.reset();
            break;
        case Settings::ViewMode::AllEntries:
        case Settings::ViewMode::SavedEntries:
        case Settings::ViewMode::SlowEntries:
        case Settings::ViewMode::LikedEntries:
        case Settings::ViewMode::BroadcastedEntries:
            setEntryModel(QStringLiteral("root"));
            tabModel.reset();
            feedModel.reset();
            break;
    }
}

void Utils::setTabModel(const QString &dashboardId) {
    auto model = std::make_unique<TabModel>();
    std::swap(model, tabModel);
    tabModel->init(dashboardId);
    Settings::instance()->setContextProperty(QStringLiteral("tabModel"),
                                             tabModel.get());
}

void Utils::setFeedModel(const QString &tabId) {
    auto model = std::make_unique<FeedModel>();
    std::swap(model, feedModel);
    feedModel->init(tabId);
    Settings::instance()->setContextProperty(QStringLiteral("feedModel"),
                                             feedModel.get());
}

void Utils::setEntryModel(const QString &feedId) {
    auto model = std::make_unique<EntryModel>();
    std::swap(model, entryModel);
    entryModel->initInThread(feedId);
    Settings::instance()->setContextProperty(QStringLiteral("entryModel"),
                                             entryModel.get());
}

void Utils::setDashboardModel() {
    auto model = std::make_unique<DashboardModel>();
    std::swap(model, dashboardModel);
    dashboardModel->init();
    Settings::instance()->setContextProperty(QStringLiteral("dashboardModel"),
                                             dashboardModel.get());
}

void Utils::updateModels() {
    if (dashboardModel) dashboardModel->init();
    if (tabModel) tabModel->init();
    if (feedModel) feedModel->init();
    if (entryModel) entryModel->init();
}

QList<QString> Utils::dashboards() const {
    QList<QString> simpleList;
    auto list = DatabaseManager::instance()->readDashboards();
    auto i = list.begin();
    while (i != list.end()) {
        simpleList.append((*i).title);
        ++i;
    }
    return simpleList;
}

QString Utils::defaultDashboardName() const {
    return DatabaseManager::instance()
        ->readDashboard(Settings::instance()->getDashboardInUse())
        .title;
}

int Utils::countUnread() const {
    return DatabaseManager::instance()->countEntriesUnreadByDashboard(
        Settings::instance()->getDashboardInUse());
}

QString Utils::getHumanFriendlySizeString(int size) const {
    if (size == 0) {
        return tr("empty");
    }
    if (size < 1024) {
        return QString("%1 B").arg(size);
    }
    if (size < 1048576) {
        return QString("%1 kB").arg(qFloor(size / 1024));
    }
    if (size < 1073741824) {
        return QString("%1 MB").arg(qFloor(size / 1048576));
    }
    return QString("%1 GB").arg(size / 1073741824);
}

QString Utils::getHumanFriendlyTimeString(int date) const {
    QDateTime qdate = QDateTime::fromTime_t(date);
    int secs = qdate.secsTo(QDateTime::currentDateTimeUtc());

    if (secs <= -18000) {
        return tr("unknown date");
    }
    if (secs <= 0) {
        return tr("just now");
    }
    if (secs < 60) {
        return tr("%n second(s) ago", "", secs);
    }
    if (secs < 3600) {
        return tr("%n minute(s) ago", "", qFloor(secs / 60));
    }
    if (secs < 86400) {
        return tr("%n hour(s) ago", "", qFloor(secs / 3600));
    }

    int days = qdate.daysTo(QDateTime::currentDateTimeUtc());

    if (days < 8) {
        return tr("%n day(s) ago", "", days);
    }

    return qdate.toString(QStringLiteral("dddd, d MMMM yy"));
}

QString Utils::hash(const QString &url) {
    return QString(
        QCryptographicHash::hash(url.toLatin1(), QCryptographicHash::Md5)
            .toHex());
}

// Source: https://github.com/radekp/qtmoko
int Utils::monthsTo(const QDate &from, const QDate &to) {
    int result = 12 * (to.year() - from.year());
    result += (to.month() - from.month());
    return result;
}

int Utils::yearsTo(const QDate &from, const QDate &to) {
    return to.year() - from.year();
}

bool Utils::isSameWeek(const QDate &date1, const QDate &date2) {
    int y1, y2;
    int w1 = date1.weekNumber(&y1);
    int w2 = date2.weekNumber(&y2);
    return w1 == w2 && y1 == y2 && w1 != 0 && w2 != 0;
}

void Utils::resetFetcher(int type) const {
    Settings *s = Settings::instance();

    if (s->fetcher != nullptr) {
        s->fetcher->disconnect();
        delete s->fetcher;
        s->fetcher = nullptr;
    }

    if (type == 1) {
        // Netvibes fetcher
        s->fetcher = new NvFetcher();
    }

    if (type == 2) {
        // Old Reader fetcher
        s->fetcher = new OldReaderFetcher();
    }

    if (type == 4) {
        // Tiny Tiny Rss fetcher
        s->fetcher = new TTRssFetcher();
    }

    if (s->fetcher != nullptr)
        s->setContextProperty(QStringLiteral("fetcher"), s->fetcher);
}

void Utils::addExtension(const QString &contentType, QString *path) {
    auto orig_ext = QFileInfo{*path}.suffix();

    QString new_ext;
    if (contentType == "image/jpeg") {
        new_ext = "jpg";
    } else if (contentType == "image/png") {
        new_ext = "png";
    } else if (contentType == "image/gif") {
        new_ext = "gif";
    } else if (contentType == "image/svg+xml") {
        new_ext = "svg";
    } else {
        new_ext = orig_ext;
    }

    if (new_ext != orig_ext) {
        path->append("." + new_ext);
    }
}
