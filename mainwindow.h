#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtNetwork>
#include "graphicsview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeMousePos(const QPoint &point);
    void instrumentPanelMenuAction();
    void instrumentPanelDockWidgetAction();
    void instrumentPanelDockWidgetAreaChanged(Qt::DockWidgetArea area);
    void connectButtonClicked();
    void sendLineToHost(QLine line);
    void readHost();

private:
    bool connectToHost(QHostAddress host = QHostAddress::LocalHost, quint16 port = 49152);
    void tuneWindow();
    void createActions();
    void createInstrumentWidgets();
    qint32 arrayToInt(QByteArray source);
    QByteArray intToArray(qint32 source);

    QTcpSocket *client_socket;
    QLabel *mouse_pos;
    GraphicsView *view;
    const QSize panel_button_size;
    QDockWidget *instrument_dock_widget;
    QAction *show_instrument_panel;
    QHash<QTcpSocket*, QByteArray*> *buffers;
    QHash<QTcpSocket*, qint32*> *sizes;
};

#endif // MAINWINDOW_H
