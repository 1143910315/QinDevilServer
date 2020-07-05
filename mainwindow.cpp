#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\\QinDevilCommonStructure\\commonstructure.h"
#include <QSettings>

constexpr auto settingFileName = "appProgram.settings";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("QinDevilDreamTeam");
    QCoreApplication::setOrganizationDomain("qindevil.dreamteam.com");
    QCoreApplication::setApplicationName("QinDevil");
    timer.start();
    gameDataList.append(new GameData(timer.elapsed(), this));
    connect(&server, &TcpServer::connection, this, &MainWindow::connection);
    connect(&server, &TcpServer::receive, this, &MainWindow::receive);
    connect(&server, &TcpServer::disconnected, this, &MainWindow::disconnected);
    server.listen(QHostAddress::Any, 12580);
    //QSettings a;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connection(TcpSocket *client) {
    client->id = ++id;
    client->index = 0;
    //gameDataList[0]->idMapName.insert(client->id, QString::asprintf("用户%d", client->id));
    client->userName = QString::asprintf("用户%d", client->id);
    idMapName.insert(client->id, client->userName);
    gameDataList[0]->appendClient(client);
    //ui->textEdit->append("客户进入");
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
            QSettings setting(settingFileName, QSettings::IniFormat);
            QVariant localValue = setting.value(client->userId + "/name", "");
            QString userName = localValue.toString();
            if(userName.length() > 0) {
                client->userName = userName;
                idMapName[client->id] = userName;
            }
            //ui->textEdit->append("客户特征字：" + client->userId);
            constexpr int structure_allGameData_size = sizeof(structure_allGameData);
            int sendDataSize = structure_allGameData_size + userName.length() * 2;
            char *sendOriginal = new char[sendDataSize];
            structure_allGameData *sendData = (structure_allGameData *)sendOriginal;
            sendData->time = localGameData->time;
            constexpr auto loop = sizeof(sendData->qinLessKey) / sizeof(unsigned short);
            for(uint i = 0; i < loop; i++) {
                sendData->qinLessKey[i] = localGameData->lessKey[i];
            }
            constexpr int qinForUserNameLength = sizeof(sendData->repairKey.qinForUserName) / sizeof(sendData->repairKey.qinForUserName[0].userName);
            for(int i = 0; i < qinForUserNameLength; i++) {
                if(localGameData->qinForId[i] != 0) {
                    QString str = idMapName[localGameData->qinForId[i]];
                    const ushort *localUtf16 = str.utf16();
                    int strLength = str.length() ;
                    for(int j = 0; j <= strLength; j++) {
                        sendData->repairKey.qinForUserName[i].userName[j] = localUtf16[j];
                    }
                } else {
                    sendData->repairKey.qinForUserName[i].userName[0] = 0;
                }
            }
            const QChar *localConstData = userName.constData();
            int j = 0;
            for(int total = userName.length(); j < total; ++j) {
                sendData->name[j] = localConstData[j].unicode();
            }
            sendData->name[j] = 0;
            Buffer *sendBuffer = server.getSendBuffer(0, sendDataSize);
            server.writeBuffer(&sendBuffer, (char *)sendData, sendDataSize);
            server.sendBuffer(client, sendBuffer);
            if(userName.length() > 0) {
                sendLog(localGameData, userName + " 来了");
            }
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
        case 2: {
            bool infoChange = false;
            structure_LessKey *receiveData = (structure_LessKey *)data;
            structure_LessKey sendData;
            int start = receiveData->numberQin * 4;
            int keyStart = receiveData->numberQin * 3;
            for(int i = 0; i < 3; i++) {
                unsigned short charName = localGameData->lessKey[start + i];
                unsigned short newCharName = receiveData->lessKey[0 + i];
                if(charName != newCharName) {
                    int userId = localGameData->qinForId[keyStart + i];
                    if(userId != 0) {
                        localGameData->appendRepairKey(userId, charName - '1');
                        localGameData->qinForId[keyStart + i] = 0;
                        infoChange = true;
                    }
                    localGameData->lessKey[start + i] = newCharName;
                }
            }
            //localGameData->lessKey[start + 3] = receiveData->lessKey[3];
            sendData.numberQin = receiveData->numberQin;
            sendData.lessKey[0] = receiveData->lessKey[0];
            sendData.lessKey[1] = receiveData->lessKey[1];
            sendData.lessKey[2] = receiveData->lessKey[2];
            //sendData.lessKey[3] = receiveData->lessKey[3];
            sendData.lessKey[3] = 0;
            int i = 0;
            TcpSocket *tempClient;
            while((tempClient = localGameData->getClient(i))) {
                if(tempClient != client) {
                    Buffer *sendBuffer = server.getSendBuffer(2, sizeof(sendData));
                    server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
                    server.sendBuffer(tempClient, sendBuffer);
                }
            }
            infoChange |= localGameData->distributionRepairKey();
            if(infoChange) {
                structure_repairKeyForUserName sendData;
                constexpr int qinForUserNameLength = sizeof(sendData.qinForUserName) / sizeof(sendData.qinForUserName[0].userName);
                for(int i = 0; i < qinForUserNameLength; i++) {
                    if(localGameData->qinForId[i] != 0) {
                        QString str = idMapName[localGameData->qinForId[i]];
                        const ushort *localUtf16 = str.utf16();
                        int strLength = str.length() ;
                        for(int j = 0; j <= strLength; j++) {
                            sendData.qinForUserName[i].userName[j] = localUtf16[j];
                        }
                    } else {
                        sendData.qinForUserName[i].userName[0] = 0;
                    }
                }
                int index = 0;
                TcpSocket *tempClient;
                while((tempClient = localGameData->getClient(index))) {
                    Buffer *sendBuffer = server.getSendBuffer(3, sizeof(sendData));
                    server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
                    server.sendBuffer(tempClient, sendBuffer);
                }
            }
            QString log = "";
            log += client->userName;
            if(receiveData->numberQin == 0) {
                log += " 修改一号琴缺弦为：";
            } else if(receiveData->numberQin == 1) {
                log += " 修改二号琴缺弦为：";
            } else if(receiveData->numberQin == 2) {
                log += " 修改三号琴缺弦为：";
            } else if(receiveData->numberQin == 3) {
                log += " 修改四号琴缺弦为：";
            }
            log += QString::fromUtf16(receiveData->lessKey, -1);
            sendLog(localGameData, log);
            break;
        }
        case 3: {
            structure_repairKey *receiveData = (structure_repairKey *)data;
            QString log = "";
            if(receiveData->isChecked) {
                unsigned short j = '1' + receiveData->keyId;
                int i = 0;
                for(; i < 12; i++) {
                    if(localGameData->qinForId[i] == 0) {
                        if(localGameData->lessKey[i + i / 3] == j) {
                            localGameData->qinForId[i] = client->id;
                            log += client->userName;
                            if(receiveData->keyId == 0) {
                                log += " 补 宫 ，";
                            } else if(receiveData->keyId == 1) {
                                log += " 补 商 ，";
                            } else if(receiveData->keyId == 2) {
                                log += " 补 角 ，";
                            } else if(receiveData->keyId == 3) {
                                log += " 补 徵 ，";
                            } else if(receiveData->keyId == 4) {
                                log += " 补 羽 ，";
                            } else {
                                log += " 补 梦 ，";
                            }
                            switch(i / 3) {
                                case 0: {
                                    log += "且被分配到 一号琴";
                                    break;
                                }
                                case 1: {
                                    log += "且被分配到 二号琴";
                                    break;
                                }
                                case 2: {
                                    log += "且被分配到 三号琴";
                                    break;
                                }
                                case 3: {
                                    log += "且被分配到 四号琴";
                                    break;
                                }
                                default: {
                                    log += "且被分配到 鬼琴";
                                }
                            }
                            break;
                        }
                    }
                }
                if(i == 12) {
                    localGameData->appendRepairKey(client->id, receiveData->keyId);
                    log += client->userName;
                    if(receiveData->keyId == 0) {
                        log += " 补 宫 ，但未分配琴";
                    } else if(receiveData->keyId == 1) {
                        log += " 补 商 ，但未分配琴";
                    } else if(receiveData->keyId == 2) {
                        log += " 补 角 ，但未分配琴";
                    } else if(receiveData->keyId == 3) {
                        log += " 补 徵 ，但未分配琴";
                    } else if(receiveData->keyId == 4) {
                        log += " 补 羽 ，但未分配琴";
                    } else {
                        log += " 补 梦 ，但未分配琴";
                    }
                }
            } else {
                QMap<int, QString>::ConstIterator it = idMapName.constBegin();
                while(it != idMapName.constEnd()) {
                    const int localKey = it.key();
                    const QString localValue = it.value();
                    if(localValue == client->userName) {
                        localGameData->removeRepairKey(localKey, receiveData->keyId);
                        unsigned short j = '1' + receiveData->keyId;
                        for(int i = 0; i < 12; i++) {
                            if(localGameData->qinForId[i] == localKey && localGameData->lessKey[i + i / 3] == j) {
                                localGameData->qinForId[i] = 0;
                            }
                        }
                    }
                    ++it;
                }
                localGameData->distributionRepairKey();
                log += client->userName;
                if(receiveData->keyId == 0) {
                    log += " 放弃补 宫";
                } else if(receiveData->keyId == 1) {
                    log += " 放弃补 商";
                } else if(receiveData->keyId == 2) {
                    log += " 放弃补 角";
                } else if(receiveData->keyId == 3) {
                    log += " 放弃补 徵";
                } else if(receiveData->keyId == 4) {
                    log += " 放弃补 羽";
                } else {
                    log += " 放弃补 梦";
                }
            }
            structure_repairKeyForUserName sendData;
            constexpr int qinForUserNameLength = sizeof(sendData.qinForUserName) / sizeof(sendData.qinForUserName[0].userName);
            for(int i = 0; i < qinForUserNameLength; i++) {
                if(localGameData->qinForId[i] != 0) {
                    QString str = idMapName[localGameData->qinForId[i]];
                    const ushort *localUtf16 = str.utf16();
                    int strLength = str.length() ;
                    for(int j = 0; j <= strLength; j++) {
                        sendData.qinForUserName[i].userName[j] = localUtf16[j];
                    }
                } else {
                    sendData.qinForUserName[i].userName[0] = 0;
                }
            }
            int index = 0;
            TcpSocket *tempClient;
            while((tempClient = localGameData->getClient(index))) {
                Buffer *sendBuffer = server.getSendBuffer(3, sizeof(sendData));
                server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
                server.sendBuffer(tempClient, sendBuffer);
            }
            sendLog(localGameData, log);
            break;
        }
        case 4: {
            structure_repairForUserName *receiveData = (structure_repairForUserName *)data;
            idMapName[client->id] = client->userName = QString::fromUtf16(receiveData->userName, -1);
            QSettings setting(settingFileName, QSettings::IniFormat);
            setting.setValue(client->userId + "/name", client->userName);
            break;
        }
        case 5: {
            structure_playKey *receiveData = (structure_playKey *)data;
            int index = 0;
            TcpSocket *tempClient;
            while((tempClient = localGameData->getClient(index))) {
                Buffer *sendBuffer = server.getSendBuffer(4, sizeof(structure_playKey));
                server.writeBuffer(&sendBuffer, (char *)receiveData, sizeof(structure_playKey));
                server.sendBuffer(tempClient, sendBuffer);
            }
            break;
        }
        case 6: {
            structure_clearData sendData;
            sendData.time = localGameData->time = timer.elapsed();
            localGameData->clearRepairKey();
            constexpr auto loop = sizeof(localGameData->lessKey) / sizeof(unsigned short);
            for(uint i = 0; i < loop; i++) {
                localGameData->lessKey[i] = 0;
            }
            for(int i = 0; i < 12; i++) {
                localGameData->qinForId[i] = 0;
            }
            int index = 0;
            TcpSocket *tempClient;
            while((tempClient = localGameData->getClient(index))) {
                Buffer *sendBuffer = server.getSendBuffer(5, sizeof(sendData));
                server.writeBuffer(&sendBuffer, (char *)&sendData, sizeof(sendData));
                server.sendBuffer(tempClient, sendBuffer);
            }
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

void MainWindow::sendLog(GameData *gamedata, QString logMessage) {
    const ushort *localUtf16 = logMessage.utf16();
    const int datalength = logMessage.length() * 2 + 2;
    int index = 0;
    TcpSocket *tempClient;
    //qDebug("%s", qPrintable(logMessage));
    while((tempClient = gamedata->getClient(index))) {
        Buffer *sendBuffer = server.getSendBuffer(6, datalength);
        server.writeBuffer(&sendBuffer, (char *)localUtf16, datalength);
        server.sendBuffer(tempClient, sendBuffer);
    }
}







