#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include <QMap>
#include "tcpsocket.h"

struct repairKey {
    int id;
    int keyId;
};

class GameData : public QObject {
    Q_OBJECT
public:
    explicit GameData(long long time, QObject *parent = nullptr);
    void appendClient(TcpSocket *client);
    void removeClient(TcpSocket *client);
    TcpSocket *getClient(int &index);
    void appendRepairKey(int id, int keyId);
    bool distributionRepairKey();
    void removeRepairKey(int id, int keyId);
    void clearRepairKey();
    int line;
    long long time;
    unsigned short lessKey[16] = {0};
    int qinForId[12] = {0};
    //QMap<int, QString> idMapName;
private:
    QVector<repairKey *> repairKeyListUseless;
    QVector<TcpSocket *> clientList;
signals:

};

#endif // GAMEDATA_H
