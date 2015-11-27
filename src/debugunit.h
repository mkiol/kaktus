#ifndef DEBUGUNIT_H
#define DEBUGUNIT_H

#include <QObject>

class DebugUnit : public QObject
{
    Q_OBJECT
public:
    explicit DebugUnit(QObject *parent = 0);

signals:

public slots:
};

#endif // DEBUGUNIT_H
