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

#include <sailfishapp.h>

#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QStandardPaths>
#include <QTranslator>

#include "cacheserver.h"
#include "databasemanager.h"
#include "downloadmanager.h"
#include "iconprovider.h"
#include "info.h"
#include "networkaccessmanagerfactory.h"
#include "nviconprovider.h"
#include "settings.h"
#include "utils.h"

static void registerTypes() {
    qRegisterMetaType<DatabaseManager::CacheItem>("CacheItem");
}

static void installTranslator() {
    auto *translator = new QTranslator{qApp};
    auto transDir =
        SailfishApp::pathTo(QStringLiteral("translations")).toLocalFile();
    if (!translator->load(QLocale{}, QStringLiteral("kaktus"),
                          QStringLiteral("_"), transDir,
                          QStringLiteral(".qm"))) {
        qDebug() << "cannot load translation:" << QLocale::system().name()
                 << transDir;
        if (!translator->load(QStringLiteral("kaktus_en"), transDir)) {
            qDebug() << "cannot load default translation";
            delete translator;
            return;
        }
    }

    QGuiApplication::installTranslator(translator);
}

Q_DECL_EXPORT int main(int argc, char **argv) {
    SailfishApp::application(argc, argv);

    QGuiApplication::setApplicationName(Kaktus::APP_ID);
    QGuiApplication::setOrganizationName(Kaktus::ORG);
    QGuiApplication::setApplicationDisplayName(Kaktus::APP_NAME);
    QGuiApplication::setApplicationVersion(Kaktus::APP_VERSION);

    registerTypes();

    auto *view = SailfishApp::createView();
    auto *context = view->rootContext();

    context->setContextProperty(QStringLiteral("APP_NAME"), Kaktus::APP_NAME);
    context->setContextProperty(QStringLiteral("APP_ID"), Kaktus::APP_ID);
    context->setContextProperty(QStringLiteral("APP_VERSION"),
                                Kaktus::APP_VERSION);
    context->setContextProperty(QStringLiteral("AUTHOR"), Kaktus::AUTHOR);
    context->setContextProperty(QStringLiteral("COPYRIGHT_YEAR"),
                                Kaktus::COPYRIGHT_YEAR);
    context->setContextProperty(QStringLiteral("AUTHOR1"), Kaktus::AUTHOR1);
    context->setContextProperty(QStringLiteral("COPYRIGHT_YEAR1"),
                                Kaktus::COPYRIGHT_YEAR1);
    context->setContextProperty(QStringLiteral("SUPPORT_EMAIL"),
                                Kaktus::SUPPORT_EMAIL);
    context->setContextProperty(QStringLiteral("PAGE"), Kaktus::PAGE);
    context->setContextProperty(QStringLiteral("LICENSE"), Kaktus::LICENSE);
    context->setContextProperty(QStringLiteral("LICENSE_URL"),
                                Kaktus::LICENSE_URL);
    installTranslator();

    auto *settings = Settings::instance();

    view->engine()->addImageProvider(QStringLiteral("icons"),
                                     new IconProvider{});
    view->engine()->addImageProvider(QStringLiteral("nvicons"),
                                     new NvIconProvider{});

    settings->context = context;

    NetworkAccessManagerFactory namfactory{settings->getDmUserAgent()};
    view->engine()->setNetworkAccessManagerFactory(&namfactory);

    Utils utils;

    context->setContextProperty(QStringLiteral("db"),
                                DatabaseManager::instance());
    context->setContextProperty(QStringLiteral("utils"), &utils);
    context->setContextProperty(QStringLiteral("dm"),
                                DownloadManager::instance());
    context->setContextProperty(QStringLiteral("cache"),
                                CacheServer::instance());
    context->setContextProperty(QStringLiteral("cserver"),
                                CacheServer::instance());
    context->setContextProperty(QStringLiteral("settings"), settings);

    view->setSource(SailfishApp::pathTo(QStringLiteral("qml/main.qml")));
    view->show();

    return QGuiApplication::exec();
}
