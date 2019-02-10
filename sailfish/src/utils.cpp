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

#include <QtGui/QClipboard>
#include <QDebug>
#include <QDateTime>
#include <QtCore/qmath.h>
#include <QCryptographicHash>
#include <QRegExp>

#ifdef SAILFISH
#include <sailfishapp.h>
#endif
#ifdef ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtGui/QGuiApplication>
#endif
#ifdef BB10
#include <bps/navigator.h>
#include <QtGui/QApplication>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/Theme>
#include <bb/cascades/Application>
#include <bb/cascades/ColorTheme>
#include <bb/cascades/VisualStyle>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TextStyle>
#include <bb/cascades/FontSize>
#include <bb/platform/PlatformInfo>
#include <bb/device/DisplayTechnology>
#include <bb/platform/PlatformInfo>
#include <QtCore/QString>
#include <QtCore/QList>
#include <bb/system/Clipboard>
#include <QtGui/QTextDocument>
#else
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QGuiApplication>
#include <QQmlContext>
#include <QStandardPaths>
#else
#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QDeclarativeContext>
#endif
#include <QTextDocument>
#endif

#include "utils.h"
#include "fetcher.h"
#include "oldreaderfetcher.h"
#include "nvfetcher.h"
#include "feedlyfetcher.h"
#include "ttrssfetcher.h"

Utils::Utils(QObject *parent) :
    QObject(parent)//, ncm(new QNetworkConfigurationManager(parent))
{
    dashboardModel = NULL;
    entryModel = NULL;
    tabModel = NULL;
    feedModel = NULL;
#ifdef ANDROID
    screen = QGuiApplication::screens().at(0);
#endif
}

bool Utils::isLight()
{
#ifdef KAKTUS_LIGHT
    return true;
#else
    return false;
#endif
}

#ifdef ANDROID
int Utils::dp(float value)
{
    //float dp = value * (screen->physicalDotsPerInch() / 160) + 0.5;
    //dp = (10 * dp + 5) / 10;
    //qDebug() << "value=" << value << "dp=" << value * (screen->physicalDotsPerInch() / 160) << "dpp=" << dp;
    return value * (screen->physicalDotsPerInch() / 160) + 0.5;
}

int Utils::mm(float value)
{
    return value * (screen->physicalDotsPerInch() / 25.4);
}

int Utils::in(float value)
{
    return value * screen->physicalDotsPerInch();
}

int Utils::pt(float value)
{
    return value * (screen->physicalDotsPerInch() / 72);
}

int Utils::sp(float value)
{
    return dp(value);
}

void Utils::showNotification(const QString &title, const QString &text)
{
    QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jText = QAndroidJniObject::fromString(text);
    QAndroidJniObject::callStaticMethod<void>("net/mkiol/kaktus/KaktusActivity",
                                       "notify",
                                       "(Ljava/lang/String;Ljava/lang/String;)V",
                                       jTitle.object<jstring>(), jText.object<jstring>());
}

void Utils::setStatusBarColor(const QColor &color)
{
    int acolor = (color.alpha() << 24) | (color.red() << 16) | (color.green() << 8) | color.blue();
    QAndroidJniObject::callStaticMethod<void>("net/mkiol/kaktus/KaktusActivity",
                                              "setStatusBarColor",
                                              "(I)V", acolor);
}

#endif

#ifdef BB10
void Utils::launchBrowser(const QString &url)
{
    navigator_invoke(url.toStdString().c_str(),0);
}
#endif

QString Utils::readAsset(const QString &path)
{
#ifdef BB10
    QFile file("app/native/assets/" + path);
#endif
#ifdef ANDROID
    QFile file(":/" + path);
#endif
#ifdef SAILFISH
    QFile file(SailfishApp::pathTo(path).toLocalFile());
#endif
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << path << "for reading: " << file.errorString();
        file.close();
        return "";
    }

    QString data = QString(file.readAll());
    file.close();

    return data;
}

void Utils::copyToClipboard(const QString &text)
{
#ifdef BB10
    bb::system::Clipboard clipboard;
    clipboard.clear();
    clipboard.insert("text/plain", text.toUtf8());
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QClipboard *clipboard = QGuiApplication::clipboard();
#else
    QClipboard *clipboard = QApplication::clipboard();
#endif
    clipboard->setText(text);
#endif
}

void Utils::resetQtWebKit()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList dataDirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    if(dataDirs.size() > 0) {
        QDir dir(QDir(dataDirs.at(0)).filePath(".QtWebKit"));
        qDebug() << dir.path();
        if (dir.exists())
            dir.removeRecursively();
    }
#else
    QString value = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).path();
    value = value + "/.QtWebKit";
    removeDir(value);
#endif
}

void Utils::log(const QString & data)
{
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

QString Utils::formatHtml(const QString & data, bool offline, const QString & style)
{
    QRegExp rxImg("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxTarget("\\s*target\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxSizes("\\s*sizes\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxA("<a[^>]*></a>", Qt::CaseInsensitive);
    QRegExp rxP("<p[^>]*></p>", Qt::CaseInsensitive);

    QTextDocument doc; doc.setHtml(data);
    if (doc.toPlainText().replace(QChar::ObjectReplacementCharacter,QChar(0x0020)).trimmed().isEmpty())
        return "";

    QString content = data;
    content.remove(rxTarget);
    if (offline) {
        content.remove(rxImg); content.remove("</img>", Qt::CaseInsensitive);
        content.remove(rxA);
        content.remove(rxP);
    } else {
        content.remove(rxWidth);
        content.remove(rxHeight);
        content.remove(rxSizes);
    }

    content = "<html><head><meta name=\"viewport\" content=\"width=device-width, maximum-scale=1.0, initial-scale=1.0\">" +
              (style.isEmpty() ? "" : "<style>" + style + "</style>") +
              "</head><body>" + content + "</body></html>";

    return content;
}

/*
 * Copyright (c) 2009 John Schember <john@nachtimwald.com>
 * http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
 */
bool Utils::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        QFileInfoList infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        Q_FOREACH(QFileInfo info, infoList) {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());
            if (!result)
                return result;
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

#ifdef BB10
// Source: http://hecgeek.blogspot.com/2014/10/blackberry-10-multiple-os-versions-from.html
bool Utils::checkOSVersion(int major, int minor, int patch, int build)
{
    bb::platform::PlatformInfo platformInfo;
    const QString osVersion = platformInfo.osVersion();
    const QStringList parts = osVersion.split('.');
    const int partCount = parts.size();

    if(partCount < 4) {
        // Invalid OS version format, assume check failed
        return false;
    }

    // Compare the base OS version using the same method as the macros
    // in bbndk.h, which are duplicated here for maximum compatibility.
    int platformEncoded = (((parts[0].toInt())*1000000)+((parts[1].toInt())*1000)+(parts[2].toInt()));
    int checkEncoded = (((major)*1000000)+((minor)*1000)+(patch));

    if(platformEncoded < checkEncoded) {
        return false;
    }
    else if(platformEncoded > checkEncoded) {
        return true;
    }
    else {
        // The platform and check OS versions are equal, so compare the build version
        int platformBuild = parts[3].toInt();
        return platformBuild >= build;
    }
}

int Utils::du(float value)
{
    int width = display.pixelSize().width();
    int height = display.pixelSize().height();

    /*qDebug() << "display id is " << display.displayId();
    qDebug() << "display name is " << display.displayName();
    qDebug() << "display size is " << display.pixelSize().width() << ", "
             << display.pixelSize().height();*/

    if (width==720 && height==1280) {
        return value*8;
    }

    if (width==720 && height==720) {
        return value*8;
    }

    if (width==768 && height==1280) {
        return value*10;
    }

    if (width==1440 && height==1440) {
        return value*12;
    }

    return 8*value;
}

bb::cascades::Color Utils::background()
{
    bb::cascades::ColorTheme* colorTheme = bb::cascades::Application::instance()->themeSupport()->theme()->colorTheme();
    bool isOled = display.displayTechnology()==bb::device::DisplayTechnology::Oled;
    bool is103 = checkOSVersion(10,3);

    switch (colorTheme->style()) {
        case bb::cascades::VisualStyle::Bright:
            if (is103)
                return bb::cascades::Color::White;
            if (isOled)
                return bb::cascades::Color::fromARGB(0xfff2f2f2);
            return bb::cascades::Color::fromARGB(0xfff8f8f8);
            break;
        case bb::cascades::VisualStyle::Dark:
            if (is103)
                return bb::cascades::Color::Black;
            return bb::cascades::Color::fromARGB(0xff121212);
            break;
    }

    return bb::cascades::Color::Black;
}

bb::cascades::Color Utils::plain()
{
    bb::cascades::ColorTheme* colorTheme = bb::cascades::Application::instance()->themeSupport()->theme()->colorTheme();

    switch (colorTheme->style()) {
        case bb::cascades::VisualStyle::Bright:
            return bb::cascades::Color::fromARGB(0xfff0f0f0);
            break;
        case bb::cascades::VisualStyle::Dark:
            return bb::cascades::Color::fromARGB(0xff323232);
            break;
    }

    return bb::cascades::Color::fromARGB(0xff323232);
}

bb::cascades::Color Utils::plainBase()
{
    bb::cascades::ColorTheme* colorTheme = bb::cascades::Application::instance()->themeSupport()->theme()->colorTheme();

    switch (colorTheme->style()) {
        case bb::cascades::VisualStyle::Bright:
            return bb::cascades::Color::fromARGB(0xffe6e6e6);
            break;
        case bb::cascades::VisualStyle::Dark:
            return bb::cascades::Color::fromARGB(0xff282828);
            break;
    }

    return bb::cascades::Color::fromARGB(0xff282828);
}

bb::cascades::Color Utils::text()
{
    bb::cascades::ColorTheme* colorTheme = bb::cascades::Application::instance()->themeSupport()->theme()->colorTheme();

    switch (colorTheme->style()) {
        case bb::cascades::VisualStyle::Bright:
            return bb::cascades::Color::fromARGB(0xff323232);
            break;
        case bb::cascades::VisualStyle::Dark:
            return bb::cascades::Color::fromARGB(0xfff0f0f0);
            break;
    }

    return bb::cascades::Color::fromARGB(0xfff0f0f0);
}

bb::cascades::Color Utils::secondaryText()
{
    bb::cascades::ColorTheme* colorTheme = bb::cascades::Application::instance()->themeSupport()->theme()->colorTheme();

    switch (colorTheme->style()) {
        case bb::cascades::VisualStyle::Bright:
            return bb::cascades::Color::fromARGB(0xff646464);
            break;
        case bb::cascades::VisualStyle::Dark:
            return bb::cascades::Color::fromARGB(0xff969696);
            break;
    }

    return bb::cascades::Color::fromARGB(0xff969696);
}

bb::cascades::Color Utils::primary()
{
    return bb::cascades::Color::fromARGB(0xff0092cc);
}

bb::cascades::Color Utils::textOnPlain()
{
    return text();
}

/*float Utils::primaryTextFontSize()
{
    bb::cascades::TextStyle style = bb::cascades::SystemDefaults::TextStyles::primaryText();
    qDebug() << "primaryText:" << style.fontSize() << bb::cascades::FontSize::Medium;
    return bb::cascades::SystemDefaults::TextStyles::primaryText().fontSizeValue();
}

QString Utils::primaryTextFontFamily()
{
    return bb::cascades::SystemDefaults::TextStyles::primaryText().fontFamily();
}

float Utils::titleTextFontSize()
{
    return bb::cascades::SystemDefaults::TextStyles::titleText().fontSizeValue();
}

QString Utils::titleTextFontFamily()
{
    return bb::cascades::SystemDefaults::TextStyles::titleText().fontFamily();
}*/
#endif

void Utils::setRootModel()
{
    TabModel *oldTabModel = tabModel;
    FeedModel *oldFeedModel = feedModel;
    EntryModel *oldEntryModel = entryModel;

    //qDebug() << "utils tid:" << QThread::currentThreadId();

    Settings *s = Settings::instance();
    int mode = s->getViewMode();

    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        tabModel = new TabModel(s->db);
        tabModel->init(s->getDashboardInUse());
#ifdef BB10
        s->qml->setContextProperty("tabModel", tabModel);
#else
        s->context->setContextProperty("tabModel", tabModel);
#endif
        if (oldTabModel != NULL) {
            delete oldTabModel;
        }
        if (feedModel != NULL) {
            delete feedModel; feedModel = NULL;
        }
        if (entryModel != NULL) {
            delete entryModel; entryModel = NULL;
        }
        break;
    case 1:
        // View mode: Tabs->Entries
        tabModel = new TabModel(s->db);
        tabModel->init(s->getDashboardInUse());
#ifdef BB10
        s->qml->setContextProperty("tabModel", tabModel);
#else
        s->context->setContextProperty("tabModel", tabModel);
#endif
        if (oldTabModel != NULL) {
            delete oldTabModel;
        }
        if (feedModel != NULL) {
            delete feedModel; feedModel = NULL;
        }
        if (entryModel != NULL) {
            delete entryModel; entryModel = NULL;
        }
        break;
    case 2:
        // View mode: Feeds->Entries
        feedModel = new FeedModel(s->db);
        feedModel->init("root");
#ifdef BB10
        s->qml->setContextProperty("feedModel", feedModel);
#else
        s->context->setContextProperty("feedModel", feedModel);
#endif
        if (tabModel != NULL)
            delete tabModel; tabModel = NULL;
        if (oldFeedModel != NULL) {
            delete oldFeedModel;
        }
        if (entryModel != NULL) {
            delete entryModel; entryModel = NULL;
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
        entryModel = new EntryModel(s->db);
        entryModel->init("root");
#ifdef BB10
        s->qml->setContextProperty("entryModel", entryModel);
#else
        s->context->setContextProperty("entryModel", entryModel);
#endif
        if (tabModel != NULL)
            delete tabModel; tabModel = NULL;
        if (feedModel != NULL) {
            delete feedModel; feedModel = NULL;
        }
        if (oldEntryModel != NULL) {
            delete oldEntryModel;
        }
        break;
    }
}

void Utils::setFeedModel(const QString &tabId)
{
    FeedModel* oldFeedModel = feedModel;
    Settings *s = Settings::instance();

    feedModel = new FeedModel(s->db);
    feedModel->init(tabId);

#ifdef BB10
        s->qml->setContextProperty("feedModel", feedModel);
#else
        s->context->setContextProperty("feedModel", feedModel);
#endif

    if (oldFeedModel != NULL) {
        delete oldFeedModel;
    }
}

void Utils::setEntryModel(const QString &feedId)
{
    EntryModel* oldEntryModel = entryModel;
    Settings *s = Settings::instance();

    entryModel = new EntryModel(s->db);
    entryModel->initInThread(feedId);

#ifdef BB10
        s->qml->setContextProperty("entryModel", entryModel);
#else
        s->context->setContextProperty("entryModel", entryModel);
#endif

    if (oldEntryModel != NULL) {
        delete oldEntryModel;
    }
}

void Utils::setDashboardModel()
{
    DashboardModel* oldDashboardModel = dashboardModel;
    Settings *s = Settings::instance();

    dashboardModel = new DashboardModel(s->db);
    dashboardModel->init();

#ifdef BB10
        s->qml->setContextProperty("dashboardModel", dashboardModel);
#else
       s->context->setContextProperty("dashboardModel", dashboardModel);
#endif

    if (oldDashboardModel != NULL)
        delete oldDashboardModel;
}

void Utils::updateModels()
{
    if (dashboardModel != NULL)
        dashboardModel->init();

    if (tabModel != NULL)
        tabModel->init();

    if (feedModel != NULL)
        feedModel->init();

    if (entryModel != NULL)
        entryModel->init();
}

Utils::~Utils()
{
    if (entryModel != NULL)
        delete entryModel;

    if (feedModel != NULL)
        delete feedModel;

    if (tabModel != NULL)
        delete tabModel;

    if (dashboardModel != NULL)
        delete dashboardModel;
}

QList<QString> Utils::dashboards()
{
    Settings *s = Settings::instance();
    QList<QString> simpleList;
    QList<DatabaseManager::Dashboard> list = s->db->readDashboards();
    QList<DatabaseManager::Dashboard>::iterator i = list.begin();
    while (i != list.end()) {
        simpleList.append((*i).title);
        ++i;
    }
    return simpleList;
}

QString Utils::defaultDashboardName()
{
    Settings *s = Settings::instance();
    DatabaseManager::Dashboard d = s->db->readDashboard(s->getDashboardInUse());
    return d.title;
}

int Utils::countUnread()
{
    Settings *s = Settings::instance();
    return s->db->countEntriesUnreadByDashboard(s->getDashboardInUse());
}

QString Utils::getHumanFriendlySizeString(int size)
{
    if (size==0) {
        return tr("empty");
    }
    if (size<1024) {
        return QString("%1 B").arg(size);
    }
    if (size<1048576) {
        return QString("%1 kB").arg(qFloor(size/1024));
    }
    if (size<1073741824) {
        return QString("%1 MB").arg(qFloor(size/1048576));
    }
    return QString("%1 GB").arg(size/1073741824);
}

QString Utils::getHumanFriendlyTimeString(int date)
{
    QDateTime qdate = QDateTime::fromTime_t(date);
    int secs = qdate.secsTo(QDateTime::currentDateTimeUtc());

    //qDebug() << ">>>>>>>>date" << date << "QDateTime::fromTime_t(date)" << qdate;
    //qDebug() << "QDateTime::currentDateTimeUtc()" << QDateTime::currentDateTimeUtc();
    //qDebug() << "qdate.secsTo(QDateTime::currentDateTimeUtc())" << secs;

    if (secs<=-18000) {
        return tr("unknown date");
    }
    if (secs<=0) {
        return tr("just now");
    }
    if (secs==1) {
        return tr("1 second ago");
    }
    if (secs<5) {
        return tr("%1 seconds ago","less than 5 seconds").arg(secs);
    }
    if (secs<60) {
        return tr("%1 seconds ago","more or equal 5 seconds").arg(secs);
    }
    if (secs<120) {
        return tr("1 minute ago");
    }
    if (secs<300) {
        return tr("%1 minutes ago","less than 5 minutes").arg(qFloor(secs/60));
    }
    if (secs<3600) {
        return tr("%1 minutes ago","more or equal 5 minutes").arg(qFloor(secs/60));
    }
    if (secs<7200) {
        return tr("1 hour ago");
    }
    if (secs<18000) {
        return tr("%1 hours ago","less than 5 hours").arg(qFloor(secs/3600));
    }
    if (secs<86400) {
        return tr("%1 hours ago","more or equal 5 hours").arg(qFloor(secs/3600));
    }

    int days = qdate.daysTo(QDateTime::currentDateTimeUtc());

    if (days==1) {
        return tr("day ago");
    }
    if (days<5) {
        return tr("%1 days ago","less than 5 days").arg(days);
    }
    if (days<8) {
        return tr("%1 days ago","more or equal 5 days").arg(days);
    }
    /*if (days<8) {
        return tr("1 week ago");
    }
    if (days<29) {
        return tr("%1 weeks ago").arg(qFloor(days/7));
    }

    int months = Utils::monthsTo(qdate.date(),QDate::currentDate());

    if (months==1) {
        return tr("1 month ago");
    }

    if (months<5) {
        return tr("%1 months ago","less than 5 months").arg(months);
    }
    if (days<13) {
        return tr("%1 months ago","more or equal 5 months").arg(months);
    }*/

    return qdate.toString("dddd, d MMMM yy");
}

QString Utils::hash(const QString &url)
{
    return QString(QCryptographicHash::hash(url.toLatin1(), QCryptographicHash::Md5).toHex());
}

// Source: https://github.com/radekp/qtmoko
int Utils::monthsTo(const QDate &from, const QDate &to)
{
    int result = 12 * (to.year() - from.year());
    result += (to.month() - from.month());

    return result;
}

int Utils::yearsTo(const QDate &from, const QDate &to)
{
    return to.year() - from.year();
}

bool Utils::isSameWeek(const QDate &date1, const QDate &date2)
{
    int y1, y2;
    int w1 = date1.weekNumber(&y1);
    int w2 = date2.weekNumber(&y2);
    //qDebug() << date1 << date2 << y1 << y2 << w1 << w2;
    if (w1==w2 && y1==y2 && w1!=0 && w2!=0)
        return true;
    return false;
}

void Utils::resetFetcher(int type)
{
    Settings *s = Settings::instance();

    if (s->fetcher != NULL) {
        s->fetcher->disconnect();
        delete s->fetcher;
        s->fetcher = NULL;
    }

    if (type == 1) {
        // Netvibes fetcher
        s->fetcher = new NvFetcher();
    }

    if (type == 2) {
        // Old Reader fetcher
        s->fetcher = new OldReaderFetcher();
    }

    if (type == 3) {
        // Feedly fetcher
        s->fetcher = new FeedlyFetcher();
    }

    if (type == 4) {
        // Tiny Tiny Rss fetcher
        s->fetcher = new TTRssFetcher();
    }

    if (s->fetcher != NULL)
#ifdef BB10
        s->qml->setContextProperty("fetcher", s->fetcher);
#else
        s->context->setContextProperty("fetcher", s->fetcher);
#endif

}
