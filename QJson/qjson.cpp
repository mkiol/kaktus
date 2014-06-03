#include "qjson.h"
#include "qjson_p.h"
#include "json_parser.hh"
#include "json_scanner.h"

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include <QMessageBox>
#include <QEventLoop>

#include <QDebug>

QJsonPrivate::QJsonPrivate() :
    m_scanner(0)
  , m_negate(false)
  , m_error(false)
{   
}

QJsonPrivate::~QJsonPrivate()
{
  delete m_scanner;
}

void QJsonPrivate::setError(QString errorMsg, int errorLine) {
  m_error = true;
  m_errorMsg = errorMsg;
  m_errorLine = errorLine;
}

QJson::QJson(QObject *parent) :
    QObject(parent)
    ,d(new QJsonPrivate)
  , m_networkManager(NULL)
{
    m_networkManager = new QNetworkAccessManager( this );
}

QJson::~QJson()
{
  delete d;
}

QVariant QJson::parse (QIODevice* io, bool* ok)
{
  d->m_errorMsg.clear();
  delete d->m_scanner;
  d->m_scanner = 0;

  if (!io->isOpen()) {
    if (!io->open(QIODevice::ReadOnly)) {
      if (ok != NULL) *ok = false;
      emit error(-1,"Error opening device");
      qCritical ("Error opening device");
      return QVariant();
    }
  }

  if (!io->isReadable()) {
    if (ok != NULL)*ok = false;
    qCritical ("Device is not readable");
    emit error(-1,"Device is not readable");
    io->close();
    return QVariant();
  }

  d->m_scanner = new JSonScanner (io);
  yy::json_parser parser(d);
  parser.parse();

  delete d->m_scanner;
  d->m_scanner = 0;

  if (ok != NULL) *ok = !d->m_error;

  if(d->m_error)
    emit error(d->m_errorLine, d->m_errorMsg);

  io->close();
  return d->m_result;
}

QVariant QJson::parse(const QByteArray& jsonString, bool* ok) {
  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);
  buffer.write(jsonString);
  buffer.seek(0);
  return parse (&buffer, ok);
}

QVariant QJson::parse (const QUrl url, bool* ok)
{
    QVariant result;
    //QByteArray data;
    QNetworkRequest request;
    //request.setUrl(QString(url.toEncoded()));
    request.setUrl(url);
    //request.setRawHeader("User-Agent", "Mozilla/5.0 (Symbian/3; Series60/5.2 NokiaN8-00/013.016; Profile/MIDP-2.1 Configuration/CLDC-1.1 ) AppleWebKit/525 (KHTML, like Gecko) Version/3.0 BrowserNG/7.2.8.10 3gpp-gba");

    QEventLoop loop;
    //QNetworkReply *reply = m_networkManager->post(request,data);
    QNetworkReply *reply = m_networkManager->get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();

    if( reply->error() == QNetworkReply::NoError )  {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        switch (statusCode) {
            case 301:
            case 302:
            case 307:{
                    QUrl newUrl = url;
                    QString newPath = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
                    qDebug() << "redirected: " << reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
                    newUrl.setPath(newPath);
                    return parse(newUrl, ok);
                    break;
                }
            case 200:{
                    QByteArray data = reply->readAll();
                    //qDebug() << data;
                    result = parse(data, ok);
                    break;
                }
        }
    } else  {
        QMessageBox msgBox; msgBox.setText(reply->errorString()); msgBox.exec();
        qDebug() << "QJson::parse error:" << reply->errorString();
    }

    delete reply;

    return result;
}

QString QJson::errorString() const
{
  return d->m_errorMsg;
}

int QJson::errorLine() const
{
  return d->m_errorLine;
}
