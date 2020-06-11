#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    Ui::MainWindow *ui;
    TcpServer server;
    QVector<GameData *> gameDataList;
    int id = 0;
    void moveLine(TcpSocket *client, int line);
};
#endif // MAINWINDOW_H
