#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include <QDateTime>
#include <QTcpSocket>
#include "bufferlist.h"
#include "aes.h"
class TcpSocket : public QTcpSocket {
    Q_OBJECT
public:
    explicit TcpSocket(BufferList *bufferList, QObject *parent = nullptr);
    bool setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState socketState = ConnectedState, QIODevice::OpenMode openMode = ReadWrite) override;
    AES aes;
    int id;
    int line;
    int powerLevel;
    int index;
    QString userId;
    qint64 lastReceiveTime;
protected slots:
    void receiveData();
private:
    BufferList *const bufferList;
    int dataLength = 0;
    int effectiveDataLength;
    Buffer *buffer;
signals:
    void receive(int signal, char *data, int count);
};

#endif // TCPSOCKET_H
