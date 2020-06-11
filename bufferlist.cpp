#include "bufferlist.h"

BufferList::BufferList(QObject *parent) : QObject(parent) {
}

Buffer *BufferList::getBuffer(uint bufferLength) {
    for(iterator i = begin(); i != end(); ++i) {
        if((*i)->length == bufferLength) {
            Buffer *buffer = *i;
            erase(i);
            return buffer;
        }
    }
    return new Buffer(bufferLength, this);
}

void BufferList::setBuffer(Buffer *buffer) {
    prepend(buffer);
}
