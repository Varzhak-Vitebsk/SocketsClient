#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtNetwork>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool connectToHost(QHostAddress host = QHostAddress::LocalHost, quint16 port = 49152);
    QTcpSocket *client_socket;
};

#endif // MAINWINDOW_H
