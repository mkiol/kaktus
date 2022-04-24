/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QString>
#include <QThread>
#include <QVariant>

#include "feedmodel.h"
#include "listmodel.h"

class EntryModel;

class EntryModelIniter : public QThread {
    Q_OBJECT

   public:
    EntryModelIniter(QObject *parent = nullptr);
    void init(EntryModel *model);
    void init(EntryModel *model, const QString &feedId);

   private:
    QString m_feedId;
    EntryModel *m_model = nullptr;
    void run();
};

class EntryItem : public ListItem {
    Q_OBJECT

   public:
    enum Roles {
        UidRole = Qt::UserRole + 1,
        TitleRole = Qt::DisplayRole,
        AuthorRole,
        ContentRole,
        ContentAllRole,
        ContentRawRole,
        LinkRole,
        ImageRole,
        FeedIdRole,
        FeedIconRole,
        FeedTitleRole,
        AnnotationsRole,
        CachedRole,
        BroadcastRole,
        LikedRole,
        FreshRole,
        ReadRole,
        ReadLaterRole,
        DateRole
    };

   public:
    EntryItem(QObject *parent = nullptr) : ListItem(parent) {}
    explicit EntryItem(const QString &uid, const QString &title,
                       const QString &author, const QString &content,
                       const QString &contentall, const QString &contentraw,
                       const QString &link, const QString &image,
                       const QString &feedId, const QString &feedIcon,
                       const QString &feedTitle, const QString &annotations,
                       bool cached, bool broadcast, bool liked, bool fresh,
                       int read, int readlater, int date,
                       QObject *parent = nullptr);
    QVariant data(int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    inline QString id() const override { return m_uid; }
    inline QString uid() const { return m_uid; }
    inline QString title() const { return m_title; }
    inline QString author() const { return m_author; }
    inline QString content() const { return m_content; }
    inline QString contentall() const { return m_contentall; }
    inline QString contentraw() const { return m_contentraw; }
    inline QString link() const { return m_link; }
    inline QString image() const { return m_image; }
    inline QString feedId() const { return m_feedId; }
    inline QString feedIcon() const { return m_feedIcon; }
    inline QString feedTitle() const { return m_feedTitle; }
    inline QString annotations() const { return m_annotations; }
    inline bool cached() const { return m_cached; }
    inline bool broadcast() const { return m_broadcast; }
    inline bool liked() const { return m_liked; }
    inline bool fresh() const { return m_fresh; }
    inline int read() const { return m_read; }
    inline int readlater() const { return m_readlater; }
    inline int date() const { return m_date; }

    void setReadlater(int value);
    void setRead(int value);
    void setLiked(int value);
    void setBroadcast(bool value, const QString &annotations);
    void setCached(int value);

   private:
    QString m_uid;
    QString m_title;
    QString m_author;
    QString m_content;
    QString m_contentall;
    QString m_contentraw;
    QString m_link;
    QString m_image;
    QString m_feedId;
    QString m_feedIcon;
    QString m_feedTitle;
    QString m_annotations;
    bool m_cached = false;
    bool m_broadcast = false;
    bool m_liked = false;
    bool m_fresh = false;
    int m_read = false;
    int m_readlater = false;
    int m_date = 0;
};

class EntryModel : public ListModel {
    Q_OBJECT

   public:
    bool reInit;  // if true init existing model on setEntryModel

    explicit EntryModel(QObject *parent = nullptr);
    void init(const QString &feedId);
    void initInThread(const QString &feedId);

    Q_INVOKABLE void setData(int row, const QString &fieldName,
                             const QVariant &newValue,
                             const QVariant &newValue2);

    Q_INVOKABLE void setAllAsUnread();
    Q_INVOKABLE void setAllAsRead();

    Q_INVOKABLE void setAboveAsRead(int index);

    Q_INVOKABLE int countRead();
    Q_INVOKABLE int countUnread();

    Q_INVOKABLE int createItems(int offset, int limit);
    Q_INVOKABLE int count() const;
    // Q_INVOKABLE int fixIndex(const QString &id);

   public slots:
    void init();
    void initInThread();
    void initFinished();

   signals:
    void ready();

   private:
    static const int idsOnActionLimit = 100;

    QString m_feedId;
    EntryModelIniter m_initer;

    static int getDateRowId(int date);
};

#endif  // ENTRYMODEL_H
