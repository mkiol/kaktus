#include "serializerrunnable.h"
#include "parserrunnable.h"
#include "serializer.h"

#include <QtCore/QDebug>
#include <QtCore/QVariant>

using namespace QJson;

class SerializerRunnable::Private
{
public:
  QVariant json;
};

SerializerRunnable::SerializerRunnable(QObject* parent)
    : QObject(parent),
      QRunnable(),
      d(new Private)
{
  qRegisterMetaType<QVariant>("QVariant");
}

SerializerRunnable::~SerializerRunnable()
{
  delete d;
}

void SerializerRunnable::setJsonObject( const QVariant& json )
{
  d->json = json;
}

void SerializerRunnable::run()
{
  Serializer serializer;
  emit parsingFinished( Serializer().serialize( d->json ), true, QString() );
}
