#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include "tcpsocket.h"
class GameData : public QObject {
    Q_OBJECT
public:
    explicit GameData(QObject *parent = nullptr);
    void appendClient(TcpSocket *client);
    int line;
private:
    QVector<TcpSocket *> clientList;
signals:

};

#endif // GAMEDATA_H
