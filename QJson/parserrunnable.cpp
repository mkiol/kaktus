#include "parserrunnable.h"
#include "qjson.h"

#include <QtCore/QDebug>
#include <QtCore/QVariant>

class ParserRunnable::Private
{
  public:
    QByteArray m_data;
};

ParserRunnable::ParserRunnable(QObject* parent)
    : QObject(parent),
      QRunnable(),
      d(new Private)
{
  qRegisterMetaType<QVariant>("QVariant");
}

ParserRunnable::~ParserRunnable()
{
  delete d;
}

void ParserRunnable::setData( const QByteArray& data ) {
  d->m_data = data;
}

void ParserRunnable::run()
{
  qDebug() << Q_FUNC_INFO;

  bool ok;
  QJson parser;
  QVariant result = parser.parse (d->m_data, &ok);
  if (ok) {
    qDebug() << "successfully converted json item to QVariant object";
    emit parsingFinished(result, true, QString());
  } else {
    const QString errorText = tr("An error occured while parsing json: %1").arg(parser.errorString());
    qCritical() << errorText;
    emit parsingFinished(QVariant(), false, errorText);
  }
}
