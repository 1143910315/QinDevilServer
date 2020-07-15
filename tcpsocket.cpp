#include "tcpsocket.h"

TcpSocket::TcpSocket(BufferList *bufferList, QObject *parent) : QTcpSocket(parent), bufferList(bufferList) {
    connect(this, &TcpSocket::readyRead, this, &TcpSocket::receiveData);
}

bool TcpSocket::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState, QIODevice::OpenMode openMode) {
    line = 0;
    return QTcpSocket::setSocketDescriptor(socketDescriptor, socketState, openMode);
}

void TcpSocket::receiveData() {
    qint64 localBytesAvailable = bytesAvailable();
    while(localBytesAvailable > 0) {
        if(dataLength == 0) {
            if(localBytesAvailable >= sizeof(dataLength)) {
                localBytesAvailable -= read((char *)&effectiveDataLength, sizeof(effectiveDataLength));
                effectiveDataLength = effectiveDataLength - 4;
                int mod = effectiveDataLength & 0xFFFFFFF0;
                if(mod != effectiveDataLength) {
                    dataLength = mod + 16 + 4;
                } else {
                    dataLength = effectiveDataLength + 4;
                }
                buffer = bufferList->getBuffer(dataLength);
            } else {
                break;
            }
        } else {
            char *localData = buffer->buffer;
            localData += buffer->length - dataLength;
            qint64 localRead = read(localData, dataLength);
            localBytesAvailable -= localRead;
            dataLength -= localRead;
            if(dataLength == 0) {
                aes.decode(buffer->buffer + 4, buffer->length - 4);
                emit receive(*((int *)buffer->buffer), buffer->buffer + 4, effectiveDataLength);
                bufferList->setBuffer(buffer);
                buffer = nullptr;
            }
        }
    }
}

void TcpSocket::onDisconnected() {
    dataLength = 0;
    if(buffer != nullptr) {
        bufferList->setBuffer(buffer);
        buffer = nullptr;
    }
    emit connectBreak();
}
