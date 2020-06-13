#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\\QinDevilCommonStructure\\commonstructure.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    timer.start();
    gameDataList.append(new GameData(timer.elapsed(), this));
    connect(&server, &TcpServer::connection, this, &MainWindow::connection);
    connect(&server, &TcpServer::receive, this, &MainWindow::receive);
    connect(&server, &TcpServer::disconnected, this, &MainWindow::disconnected);
    server.listen(QHostAddress::Any, 12580);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connection(TcpSocket *client) {
    client->id = ++id;
    client->index = 0;
    gameDataList[0]->appendClient(client);
    ui->textEdit->append("客户进入");
}

void MainWindow::receive(TcpSocket *client, int signal, char *data, int count) {
    (void)count;
    //QElapsedTimer thhtt;
    //ui->textEdit->append("客户发送了消息");
    GameData *localGameData = gameDataList[client->index];
    client->lastReceiveTime = timer.elapsed();
    switch(signal) {
        case 0: {
            structure_initData *receiveData = (structure_initData *)data;
            client->powerLevel = receiveData->powerLevel;
            if(client->line != receiveData->line) {
                localGameData = moveLine(client, receiveData->line);
            }
            QByteArray userIdByte(receiveData->userId, sizeof(receiveData->userId));
            client->userId = userIdByte.toHex();
            structure_allGameData sendData;
            sendData.time = localGameData->time;
            Buffer *sendBuffer = server.getSendBuffer(0, sizeof(sendData));
            server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
            server.sendBuffer(client, sendBuffer);
            //qDebug("%d %d", d->powerLevel, d->line);
            //qDebug("");
            break;
        }
        case 1: {
            structure_pingData *receiveData = (structure_pingData *)data;
            structure_pingData sendData;
            sendData.time = receiveData->time;
            Buffer *sendBuffer = server.getSendBuffer(1, sizeof(sendData));
            server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
            server.sendBuffer(client, sendBuffer);
            break;
        }
    }
}

void MainWindow::disconnected(TcpSocket *client) {
    GameData *localGameData = gameDataList[client->index];
    localGameData->removeClient(client);
    //ui->textEdit->append("客户离开");
}

GameData *MainWindow::moveLine(TcpSocket *client, int line) {
    GameData *oldGameData = gameDataList[client->index];
    GameData *newGameData;
    for(int i = 0; i < gameDataList.count(); i++) {
        newGameData = gameDataList[i];
        if(newGameData->line == line) {
            oldGameData->removeClient(client);
            client->index = i;
            client->line = line;
            newGameData->appendClient(client);
            return newGameData;
        }
    }
    newGameData = new GameData(timer.elapsed(), this);
    newGameData->line = line;
    oldGameData->removeClient(client);
    client->index = gameDataList.count();
    client->line = line;
    gameDataList.append(newGameData);
    newGameData->appendClient(client);
    return newGameData;
}







