#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), client_socket(new QTcpSocket(this))
{
   connectToHost() ;
}

MainWindow::~MainWindow()
{

}

bool MainWindow::connectToHost(QHostAddress host, quint16 port)
{
    for(quint16 tested_port = port; tested_port != 65535; ++tested_port)
    {
        client_socket->connectToHost(host, tested_port);
        if(client_socket->waitForConnected(10000)) return true;
    }
    return false;
}
