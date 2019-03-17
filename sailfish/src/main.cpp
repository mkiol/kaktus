/*
  Copyright (C) 2014-2019 Michal Kosciesza <michal@mkiol.net>

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

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QtDebug>
#include <QTranslator>
#include <QScopedPointer>
#ifdef SAILFISH
#include <sailfishapp.h>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#endif
#ifdef ANDROID
#include <QQmlApplicationEngine>
#include <QtWebView>
#include <QColor>
#endif

#include "info.h"
#include "iconprovider.h"
#include "nviconprovider.h"
#include "databasemanager.h"
#include "downloadmanager.h"
#include "cacheserver.h"
#include "utils.h"
#include "settings.h"
#include "networkaccessmanagerfactory.h"

int main(int argc, char *argv[])
{
#ifdef SAILFISH
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    QScopedPointer<QQmlEngine> engine(view->engine());
    QQmlContext *context = view->rootContext();
    QString translationsDirPath = SailfishApp::pathTo("translations").toLocalFile();
#endif
#ifdef ANDROID
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QScopedPointer<QGuiApplication> app(new QGuiApplication(argc, argv));
    QtWebView::initialize();
    QScopedPointer<QQmlApplicationEngine> engine(new QQmlApplicationEngine());
    QQmlContext *context = engine->rootContext();
    QString translationsDirPath = ""; //TODO
#endif

    Utils utils;
#ifdef ANDROID
    utils.setStatusBarColor(QColor("#00796b"));
    app->setApplicationName(APP_NAME);
#endif

    app->setApplicationDisplayName(Kaktus::APP_NAME);
    app->setApplicationVersion(Kaktus::APP_VERSION);

    context->setContextProperty("APP_NAME", Kaktus::APP_NAME);
    context->setContextProperty("APP_VERSION", Kaktus::APP_VERSION);
    context->setContextProperty("AUTHOR", Kaktus::AUTHOR);
    context->setContextProperty("COPYRIGHT_YEAR", Kaktus::COPYRIGHT_YEAR);
    context->setContextProperty("AUTHOR1", Kaktus::AUTHOR1);
    context->setContextProperty("COPYRIGHT_YEAR1", Kaktus::COPYRIGHT_YEAR1);
    context->setContextProperty("SUPPORT_EMAIL", Kaktus::SUPPORT_EMAIL);
    context->setContextProperty("PAGE", Kaktus::PAGE);
    context->setContextProperty("LICENSE", Kaktus::LICENSE);
    context->setContextProperty("LICENSE_URL", Kaktus::LICENSE_URL);

    engine->addImageProvider(QLatin1String("icons"), new IconProvider);
    engine->addImageProvider(QLatin1String("nvicons"), new NvIconProvider);

    qRegisterMetaType<DatabaseManager::CacheItem>("CacheItem");

#ifdef SAILFISH
    //-- temp fix --
    // config file
    QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)).path();
    if (QFile::exists(path + "/harbour-kaktus/Kaktus.conf")) {
        qWarning() << "Old config file exists -> doing migration";
        QFile::remove(path + "/harbour-kaktus/harbour-kaktus.conf");
        if (QFile::copy(path + "/harbour-kaktus/Kaktus.conf",
                    path + "/harbour-kaktus/harbour-kaktus.conf")) {
            QFile::remove(path + "/harbour-kaktus/Kaktus.conf");
        }
    }
    // cache file
    path = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)).path();
    QDir newDir(path + "/harbour-kaktus/Kaktus/");
    if (newDir.exists()) {
        qWarning() << "Old cache dir exists -> doing migration";
        QDir oldDir(path + "/harbour-kaktus/harbour-kaktus/");
        if (oldDir.exists()) {
            oldDir.removeRecursively();
        }
        qDebug() << newDir.rename(path + "/harbour-kaktus/Kaktus/",
                                  path + "/harbour-kaktus/harbour-kaktus/");
    }
    //--
#endif

    Settings *settings = Settings::instance();

    QTranslator translator;
    QString locale = settings->getLocale() == "" ? QLocale::system().name() : settings->getLocale();
    if(!translator.load(locale, "kaktus", "_", translationsDirPath, ".qm")) {
        qDebug() << "Couldn't load translation for locale " + locale + " from " + translationsDirPath;
    }
    app->installTranslator(&translator);

    settings->context = context;

    QObject::connect(engine.data(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    NetworkAccessManagerFactory NAMfactory(settings->getDmUserAgent());
    engine->setNetworkAccessManagerFactory(&NAMfactory);

    context->setContextProperty("db", DatabaseManager::instance());
    context->setContextProperty("utils", &utils);
    context->setContextProperty("dm", DownloadManager::instance());
    context->setContextProperty("cache", CacheServer::instance());
    context->setContextProperty("cserver", CacheServer::instance());
    context->setContextProperty("settings", settings);

#ifdef SAILFISH
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->show();
#endif
#ifdef ANDROID
    engine->load(QUrl(QLatin1String("qrc:/qml/main.qml")));
#endif

    return app->exec();
}
