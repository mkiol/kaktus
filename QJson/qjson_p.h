#ifndef QJSON_PARSER_P_H
#define QJSON_PARSER_P_H

#include "qjson.h"

#include <QtCore/QString>
#include <QtCore/QVariant>

class JSonScanner;

namespace yy {
  class json_parser;
}

class QJsonPrivate
{
public:
    QJsonPrivate();
    ~QJsonPrivate();

    void setError(QString errorMsg, int line);

    JSonScanner* m_scanner;
    bool m_negate;
    bool m_error;
    int m_errorLine;
    QString m_errorMsg;
    QVariant m_result;
};


#endif // QJSON_PARSER_H
