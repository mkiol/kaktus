#ifndef _JSON_SCANNER
#define _JSON_SCANNER

#include <fstream>
#include <string>

#include <QtCore/QIODevice>
#include <QtCore/QVariant>

#define YYSTYPE QVariant

#include "qjson_p.h"

namespace yy {
  class location;
  int yylex(YYSTYPE *yylval, yy::location *yylloc, QJsonPrivate* driver);
}

class JSonScanner
{
    public:
        explicit JSonScanner(QIODevice* io);
        int yylex(YYSTYPE* yylval, yy::location *yylloc);
        
    protected:
        bool m_quotmarkClosed;
        unsigned int m_quotmarkCount;
        QIODevice* m_io;
};

#endif

