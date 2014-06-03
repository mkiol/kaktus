#ifndef PARSERRUNNABLE_H
#define PARSERRUNNABLE_H

#include <QtCore/QObject>
#include <QtCore/QRunnable>

class QVariant;

  /**
  * @brief Convenience class for converting JSON data to QVariant objects using a dedicated thread
  */
  class ParserRunnable  : public QObject, public QRunnable
  {
    Q_OBJECT
    public:
      /**
      * This signal is emitted when the conversion process has been completed
      * @param data contains the JSON data that has to be converted
      * @param parent parent of the object
      **/
      explicit ParserRunnable(QObject* parent = 0);
      ~ParserRunnable();

      void setData( const QByteArray& data );

      void run();

    Q_SIGNALS:
      /**
      * This signal is emitted when the parsing process has been completed
      * @param json contains the result of the parsing
      * @param ok if a parsing error occurs ok is set to false, otherwise it's set to true.
      * @param error_msg contains a string explaining the failure reason
      **/
      void parsingFinished(const QVariant& json, bool ok, const QString& error_msg);

    private:
      Q_DISABLE_COPY(ParserRunnable)
      class Private;
      Private* const d;
  };
//}

#endif // PARSERRUNNABLE_H
