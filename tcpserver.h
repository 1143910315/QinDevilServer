#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QTcpServer>
#include "tcpsocket.h"
int constexpr Version = -3;
class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    Buffer *getSendBuffer(int signal, uint length);
    void writeBuffer(Buffer **buffer, char *data, int dataLength);
    void sendBuffer(TcpSocket *client, Buffer *buffer);
protected:
    void incomingConnection(qintptr handle) override;
protected slots:
    void clientReceive(int signal, char *data, int count);
    void clientDisconnected();
private:
    BufferList bufferList;
    QVector<TcpSocket *> socketList;
signals:
    void connection(TcpSocket *client);
    void receive(TcpSocket *client, int signal, char *data, int count);
    void disconnected(TcpSocket *client);
};

#endif // TCPSERVER_H
