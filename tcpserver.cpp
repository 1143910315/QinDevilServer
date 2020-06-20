#include "tcpserver.h"
#include <QRandomGenerator>
TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {
}

Buffer *TcpServer::getSendBuffer(int signal, uint length) {
    uint mod = length & 0xFFFFFFF0;
    if(mod != length) {
        length = mod + 16;
    }
    Buffer *buffer = bufferList.getBuffer(length + 8);
    int *bufferPointer = (int *)buffer->buffer;
    bufferPointer[0] = 4;
    bufferPointer[1] = signal;
    return buffer;
}

void TcpServer::writeBuffer(Buffer **buffer, char *data, int dataLength) {
    Buffer *b = *buffer;
    uint *bufferPointer = (uint *)b->buffer;
    uint bufferActualLength = bufferPointer[0];
    if(bufferActualLength + 4 + dataLength > b->length) {
        uint length = bufferActualLength - 4 + dataLength;
        uint mod = length & 0xFFFFFFF0;
        if(mod != length) {
            length = mod + 16;
        }
        Buffer *newb = bufferList.getBuffer(length + 8);
        char *newbuffer = newb->buffer;
        char *oldbuffer = b->buffer;
        for(uint i = 0; i < bufferActualLength + 4; i++) {
            newbuffer[i] = oldbuffer[i];
        }
        *buffer = newb;
        bufferList.setBuffer(b);
        b = newb;
        bufferPointer = (uint *)b->buffer;
    }
    bufferPointer[0] = bufferActualLength + dataLength;
    char *wr = b->buffer + bufferActualLength + 4;
    for(int i = 0; i < dataLength; i++) {
        wr[i] = data[i];
    }
}

void TcpServer::sendBuffer(TcpSocket *client, Buffer *buffer) {
    char *bufferPointer = buffer->buffer;
    int actualLength = ((int *)bufferPointer)[0];
    uint length = actualLength - 4;
    uint mod = length & 0xFFFFFFF0;
    if(mod != length) {
        length = mod + 16;
    }
    client->aes.code(bufferPointer + 8, length);
    client->write(bufferPointer, length + 8);
}

void TcpServer::incomingConnection(qintptr handle) {
    TcpSocket *client;
    if(socketList.isEmpty()) {
        client = new TcpSocket(&bufferList, this);
        connect(client, &TcpSocket::receive, this, &TcpServer::clientReceive);
        connect(client, &TcpSocket::disconnected, this, &TcpServer::clientDisconnected);
    } else {
        client = socketList.last();
        socketList.removeLast();
    }
    client->setSocketDescriptor(handle);
    uint key[5];
    key[0] = Version;
    key[1] = QRandomGenerator::global()->generate();
    key[2] = QRandomGenerator::global()->generate();
    key[3] = QRandomGenerator::global()->generate();
    key[4] = QRandomGenerator::global()->generate();
    qDebug("%u %u %u %u", key[1], key[2], key[3], key[4]);
    client->aes.setKey((char *)&key[1]);
    client->write((char *)key, sizeof(key));
    emit connection(client);
}

void TcpServer::clientReceive(int signal, char *data, int count) {
    emit receive((TcpSocket *)sender(), signal, data, count);
}

void TcpServer::clientDisconnected() {
    TcpSocket *client = (TcpSocket *)sender();
    emit disconnected(client);
    socketList.append(client);
}
