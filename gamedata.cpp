#include "gamedata.h"

GameData::GameData(QObject *parent) : QObject(parent) {
}

void GameData::appendClient(TcpSocket *client) {
    clientList.append(client);
}

void GameData::removeClient(TcpSocket *client) {
    clientList.removeOne(client);
}
