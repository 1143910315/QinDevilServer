#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include "tcpserver.h"
#include "gamedata.h"
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected slots:
    void connection(TcpSocket *client);
    void receive(TcpSocket *client, int signal, char *data, int count);
    void disconnected(TcpSocket *client);
private:
    QDateTime strat;
    QElapsedTimer timer;
    Ui::MainWindow *ui;
    TcpServer server;
    QVector<GameData *> gameDataList;
    int id = 0;
    QMap<int, QString> idMapName;
    GameData *moveLine(TcpSocket *client, int line);
    void sendLog(GameData *gamedata, QString logMessage);
};
#endif // MAINWINDOW_H
