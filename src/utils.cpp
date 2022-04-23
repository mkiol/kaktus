/*
  Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>

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

Utils::Utils(QObject *parent) : QObject(parent) {
    dashboardModel = nullptr;
    entryModel = nullptr;
    tabModel = nullptr;
    feedModel = nullptr;
}

QString Utils::readAsset(const QString &path) {
    QFile file(SailfishApp::pathTo(path).toLocalFile());

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << path
                   << "for reading: " << file.errorString();
        file.close();
        return "";
    }

    QString data = QString(file.readAll());
    file.close();

    return data;
}

void Utils::copyToClipboard(const QString &text) {
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void Utils::resetQtWebKit() {
    QStringList dataDirs =
        QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    if (dataDirs.size() > 0) {
        QDir dir(QDir(dataDirs.at(0)).filePath(".QtWebKit"));
        qDebug() << dir.path();
        if (dir.exists()) dir.removeRecursively();
    }
}

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

QString Utils::formatHtml(const QString &data, bool offline,
                          const QString &style) {
    QRegExp rxImg("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxTarget("\\s*target\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                     Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                     Qt::CaseInsensitive);
    QRegExp rxSizes("\\s*sizes\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxA("<a[^>]*></a>", Qt::CaseInsensitive);
    QRegExp rxP("<p[^>]*></p>", Qt::CaseInsensitive);

    QString content = data;

    content.remove(rxTarget);
    if (offline) {
        content.remove(rxImg);
        content.remove("</img>", Qt::CaseInsensitive);
        content.remove(rxA);
        content.remove(rxP);
    } else {
        content.remove(rxWidth);
        content.remove(rxHeight);
        content.remove(rxSizes);
    }

    content =
        "<html><head><meta name=\"viewport\" content=\"width=device-width, "
        "maximum-scale=1.0, initial-scale=1.0\">" +
        (style.isEmpty() ? "" : "<style>" + style + "</style>") +
        "</head><body>" + content + "</body></html>";

    return content;
}

/*
 * Copyright (c) 2009 John Schember <john@nachtimwald.com>
 * http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
 */
bool Utils::removeDir(const QString &dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        QFileInfoList infoList =
            dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                  QDir::Hidden | QDir::AllDirs | QDir::Files,
                              QDir::DirsFirst);
        foreach (const QFileInfo &info, infoList) {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());
            if (!result) return result;
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void Utils::setRootModel() {
    TabModel *oldTabModel = tabModel;
    FeedModel *oldFeedModel = feedModel;
    EntryModel *oldEntryModel = entryModel;

    // qDebug() << "utils tid:" << QThread::currentThreadId();

    Settings *s = Settings::instance();
    int mode = s->getViewMode();

    switch (mode) {
        case 0:
            // View mode: Tabs->Feeds->Entries
            tabModel = new TabModel();
            tabModel->init(s->getDashboardInUse());
            s->context->setContextProperty("tabModel", tabModel);
            if (oldTabModel != nullptr) {
                delete oldTabModel;
            }
            if (feedModel != nullptr) {
                delete feedModel;
                feedModel = nullptr;
            }
            if (entryModel != nullptr) {
                delete entryModel;
                entryModel = nullptr;
            }
            break;
        case 1:
            // View mode: Tabs->Entries
            tabModel = new TabModel();
            tabModel->init(s->getDashboardInUse());
            s->context->setContextProperty("tabModel", tabModel);
            if (oldTabModel != nullptr) {
                delete oldTabModel;
            }
            if (feedModel != nullptr) {
                delete feedModel;
                feedModel = nullptr;
            }
            if (entryModel != nullptr) {
                delete entryModel;
                entryModel = nullptr;
            }
            break;
        case 2:
            // View mode: Feeds->Entries
            feedModel = new FeedModel();
            feedModel->init("root");
            s->context->setContextProperty("feedModel", feedModel);
            if (tabModel != nullptr) {
                delete tabModel;
                tabModel = nullptr;
            }
            if (oldFeedModel != nullptr) {
                delete oldFeedModel;
            }
            if (entryModel != nullptr) {
                delete entryModel;
                entryModel = nullptr;
            }
            break;
        case 3:
            // View mode: Entries
        case 4:
            // View mode: Saved
        case 5:
            // View mode: Slow
        case 6:
            // View mode: Liked
        case 7:
            // View mode: Broadcast
            entryModel = new EntryModel();
            entryModel->init("root");
            s->context->setContextProperty("entryModel", entryModel);
            if (tabModel != nullptr) {
                delete tabModel;
                tabModel = nullptr;
            }
            if (feedModel != nullptr) {
                delete feedModel;
                feedModel = nullptr;
            }
            if (oldEntryModel != nullptr) {
                delete oldEntryModel;
            }
            break;
    }
}

void Utils::setFeedModel(const QString &tabId) {
    FeedModel *oldFeedModel = feedModel;
    Settings *s = Settings::instance();

    feedModel = new FeedModel();
    feedModel->init(tabId);

    s->context->setContextProperty("feedModel", feedModel);
    if (oldFeedModel != nullptr) {
        delete oldFeedModel;
    }
}

void Utils::setEntryModel(const QString &feedId) {
    EntryModel *oldEntryModel = entryModel;
    Settings *s = Settings::instance();

    entryModel = new EntryModel();
    entryModel->initInThread(feedId);

    s->context->setContextProperty("entryModel", entryModel);

    if (oldEntryModel != nullptr) {
        delete oldEntryModel;
    }
}

void Utils::setDashboardModel() {
    DashboardModel *oldDashboardModel = dashboardModel;
    Settings *s = Settings::instance();

    dashboardModel = new DashboardModel();
    dashboardModel->init();

    s->context->setContextProperty("dashboardModel", dashboardModel);

    if (oldDashboardModel != nullptr) delete oldDashboardModel;
}

void Utils::updateModels() {
    if (dashboardModel != nullptr) dashboardModel->init();

    if (tabModel != nullptr) tabModel->init();

    if (feedModel != nullptr) feedModel->init();

    if (entryModel != nullptr) entryModel->init();
}

Utils::~Utils() {
    if (entryModel != nullptr) delete entryModel;

    if (feedModel != nullptr) delete feedModel;

    if (tabModel != nullptr) delete tabModel;

    if (dashboardModel != nullptr) delete dashboardModel;
}

QList<QString> Utils::dashboards() {
    auto db = DatabaseManager::instance();

    QList<QString> simpleList;
    QList<DatabaseManager::Dashboard> list = db->readDashboards();
    QList<DatabaseManager::Dashboard>::iterator i = list.begin();
    while (i != list.end()) {
        simpleList.append((*i).title);
        ++i;
    }
    return simpleList;
}

QString Utils::defaultDashboardName() {
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    DatabaseManager::Dashboard d = db->readDashboard(s->getDashboardInUse());
    return d.title;
}

int Utils::countUnread() {
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    return db->countEntriesUnreadByDashboard(s->getDashboardInUse());
}

QString Utils::getHumanFriendlySizeString(int size) {
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

QString Utils::getHumanFriendlyTimeString(int date) {
    QDateTime qdate = QDateTime::fromTime_t(date);
    int secs = qdate.secsTo(QDateTime::currentDateTimeUtc());

    // qDebug() << ">>>>>>>>date" << date << "QDateTime::fromTime_t(date)" <<
    // qdate; qDebug() << "QDateTime::currentDateTimeUtc()" <<
    // QDateTime::currentDateTimeUtc(); qDebug() <<
    // "qdate.secsTo(QDateTime::currentDateTimeUtc())" << secs;

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

    return qdate.toString("dddd, d MMMM yy");
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
    // qDebug() << date1 << date2 << y1 << y2 << w1 << w2;
    if (w1 == w2 && y1 == y2 && w1 != 0 && w2 != 0) return true;
    return false;
}

void Utils::resetFetcher(int type) {
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
        s->context->setContextProperty("fetcher", s->fetcher);
}

QString Utils::nameFromPath(const QString &path) {
    return QFileInfo(path).fileName();
}

void Utils::addExtension(const QString &contentType, QString &path) {
    auto orig_ext = QFileInfo(path).suffix();

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
        path.append("." + new_ext);
    }
}
