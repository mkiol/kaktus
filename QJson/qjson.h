#ifndef QJSON_H
#define QJSON_H

#include <QtCore/qglobal.h>

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>

class QIODevice;
class QVariant;

class QJsonPrivate;

/**
* @brief Main class used to convert JSON data to QVariant objects
*/
class QJson : public QObject
{
    Q_OBJECT

public:
  explicit QJson(QObject *parent=0);
  ~QJson();

  /**
  * Read JSON string from the I/O Device and converts it to a QVariant object
  * @param io Input output device
  * @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
  * @returns a QVariant object generated from the JSON string
  */
  QVariant parse(QIODevice* io, bool* ok = 0);

  /**
  * This is a method provided for convenience.
  * @param jsonData data containing the JSON object representation
  * @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
  * @returns a QVariant object generated from the JSON string
  * @sa errorString
  * @sa errorLine
  */
  Q_INVOKABLE QVariant parse(const QByteArray& jsonData, bool* ok = 0);

  /**
  * This is a method provided for convenience.
  * @param url to the server page containing the JSON object representation
  * @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
  * @returns a QVariant object generated from the JSON string
  * @sa errorString
  * @sa errorLine
  */
  Q_INVOKABLE QVariant parse (const QUrl url, bool* ok = 0);

  /**
  * This method returns the error message
  * @returns a QString object containing the error message of the last parse operation
  * @sa errorLine
  */
  QString errorString() const;

  /**
  * This method returns line number where the error occurred
  * @returns the line number where the error occurred
  * @sa errorString
  */
  int errorLine() const;

signals:
  void error(int line, QString message);

private:

  QNetworkAccessManager *m_networkManager;

  Q_DISABLE_COPY(QJson)
  QJsonPrivate* const d;
};


#endif // QJSON_H
