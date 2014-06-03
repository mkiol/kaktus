#include "qobjecthelper.h"
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>

class QObjectHelper::QObjectHelperPrivate {
};

QObjectHelper::QObjectHelper()
  : d (new QObjectHelperPrivate)
{
}

QObjectHelper::~QObjectHelper()
{
  delete d;
}

QVariantMap QObjectHelper::qobject2qvariant( const QObject* object,
                              const QStringList& ignoredProperties)
{
  QVariantMap result;
  const QMetaObject *metaobject = object->metaObject();
  int count = metaobject->propertyCount();
  for (int i=0; i<count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
      continue;

    QVariant value = object->property(name);
    result[QLatin1String(name)] = value;
 }
  return result;
}

void QObjectHelper::qvariant2qobject(const QVariantMap& variant, QObject* object)
{
  QStringList properies;
  const QMetaObject *metaobject = object->metaObject();
  int count = metaobject->propertyCount();
  for (int i=0; i<count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    if (metaproperty.isWritable()) {
      properies << QLatin1String( metaproperty.name());
    }
  }

  QVariantMap::const_iterator iter;
  for (iter = variant.constBegin(); iter != variant.end(); iter++) {
    if (properies.contains(iter.key())) {
      object->setProperty(iter.key().toAscii(), iter.value());
    }
  }
}
