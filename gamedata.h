#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include "tcpsocket.h"
class GameData : public QObject {
    Q_OBJECT
public:
    explicit GameData(long long time,QObject *parent = nullptr);
    void appendClient(TcpSocket *client);
    void removeClient(TcpSocket *client);
    int line;
    long long time;
private:
    QVector<TcpSocket *> clientList;
signals:

};

#endif // GAMEDATA_H
