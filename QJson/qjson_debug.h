#ifndef QJSON_DEBUG_H
#define QJSON_DEBUG_H

#include <QtCore/QDebug>

// define qjsonDebug()
#ifdef QJSON_VERBOSE_DEBUG_OUTPUT
  inline QDebug qjsonDebug() { return QDebug(QtDebugMsg); }
#else
  inline QNoDebug qjsonDebug() { return QNoDebug(); }
#endif

#endif
