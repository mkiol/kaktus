#ifndef SERIALIZERRUNNABLE_H
#define SERIALIZERRUNNABLE_H

#include <QtCore/qglobal.h>

#include <QtCore/QObject>
#include <QtCore/QRunnable>

class QByteArray;
class QString;
class QVariant;

namespace QJson {
  /**
  * @brief Convenience class for converting JSON data to QVariant objects using a dedicated thread
  */
  class SerializerRunnable  : public QObject, public QRunnable
  {
    Q_OBJECT
    public:
      /**
      * This signal is emitted when the conversion process has been completed
      * @param data contains the JSON data that has to be converted
      * @param parent parent of the object
      **/
      explicit SerializerRunnable(QObject* parent = 0);
      ~SerializerRunnable();

      /**
       * Sets the json object to serialize.
       *
       * @param json QVariant containing the json representation to be serialized
       */
      void setJsonObject( const QVariant& json );

      /* reimp */ void run();

    Q_SIGNALS:
      /**
      * This signal is emitted when the serialization process has been completed
      * @param serialized contains the result of the serialization
      * @param ok if a serialization error occurs ok is set to false, otherwise it's set to true.
      * @param error_msg contains a string explaining the failure reason
      **/
      void parsingFinished(const QByteArray& serialized, bool ok, const QString& error_msg);

    private:
      Q_DISABLE_COPY(SerializerRunnable)
      class Private;
      Private* const d;
  };
}

#endif // SERIALIZERRUNNABLE_H
