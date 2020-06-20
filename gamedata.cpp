#include "gamedata.h"

GameData::GameData(long long time, QObject *parent) : QObject(parent), time(time) {
}

void GameData::appendClient(TcpSocket *client) {
    clientList.append(client);
}

void GameData::removeClient(TcpSocket *client) {
    clientList.removeOne(client);
}

TcpSocket *GameData::getClient(int &index) {
    if(index >= 0 && index < clientList.count()) {
        return clientList[index++];
    }
    return nullptr;
}

void GameData::appendRepairKey(int id, int keyId) {
    repairKeyListUseless.append(new repairKey{id, keyId});
}

bool GameData::distributionRepairKey() {
    bool result = false;
    for(int i = 0; i < 12; i++) {
        if(qinForId[i] == 0) {
            for(int j = 0, total = repairKeyListUseless.count(); j < total; ++j) {
                repairKey *localRepairKey = repairKeyListUseless[j];
                unsigned short c = '1' + localRepairKey->keyId;
                if(lessKey[i + i / 3] == c) {
                    qinForId[i] = localRepairKey->id;
                    repairKeyListUseless.remove(j);
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}

void GameData::removeRepairKey(int id, int keyId) {
    for(int i = 0, total = repairKeyListUseless.count(); i < total; ++i) {
        repairKey *localRepairKey = repairKeyListUseless[i];
        if(localRepairKey->id == id && localRepairKey->keyId == keyId) {
            repairKeyListUseless.remove(i);
            i--;
            total--;
        }
    }
}

void GameData::clearRepairKey() {
    repairKeyListUseless.clear();
}
