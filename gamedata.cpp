#include "gamedata.h"

GameData::GameData(long long time, QObject *parent) : QObject(parent),time(time) {
}

void GameData::appendClient(TcpSocket *client) {
    clientList.append(client);
}

void GameData::removeClient(TcpSocket *client) {
    clientList.removeOne(client);
}
