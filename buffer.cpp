#include "buffer.h"

Buffer::Buffer(uint length, QObject *parent) : QObject(parent), length(length) {
    buffer = new char[length];
}

Buffer::~Buffer() {
    delete buffer;
}
