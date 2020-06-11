#ifndef BUFFERLIST_H
#define BUFFERLIST_H

#include <QObject>
#include <QList>
#include "buffer.h"
class BufferList : public QObject, private QList<Buffer *> {
    Q_OBJECT
public:
    explicit BufferList(QObject *parent = nullptr);
    Buffer *getBuffer(uint bufferLength);
    void setBuffer(Buffer *buffer);
};

#endif // BUFFERLIST_H
