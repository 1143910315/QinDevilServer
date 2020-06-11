#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\\QinDevilCommonStructure\\commonstructure.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    gameDataList.append(new GameData(this));
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
    ui->textEdit->append("客户发送了消息");
    switch(signal) {
        case 0:
            structure_initData *d = (structure_initData *)data;
            client->powerLevel = d->powerLevel;
            if(client->line != d->line) {
                moveLine(client, d->line);
            }
            qDebug("%d %d", d->powerLevel, d->line);
            break;
    }
}

void MainWindow::disconnected(TcpSocket *client) {
    ui->textEdit->append("客户离开");
}

void MainWindow::moveLine(TcpSocket *client, int line) {
    GameData *oldGameData = gameDataList[client->index];
    GameData *newGameData;
    for(int i = 0; i < gameDataList.count(); i++) {
        newGameData = gameDataList[i];
        if(newGameData->line == line) {
            oldGameData->removeClient(client);
            client->index = i;
            client->line = line;
            newGameData->appendClient(client);
            return;
        }
    }
    newGameData = new GameData(this);
    newGameData->line = line;
    oldGameData->removeClient(client);
    client->index = gameDataList.count();
    client->line = line;
    gameDataList.append(newGameData);
    newGameData->appendClient(client);
}

