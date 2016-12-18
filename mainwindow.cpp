#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), client_socket(new QTcpSocket(this))
    , panel_button_size(QSize(100, 50))
    , buffers(new QHash<QTcpSocket *, QByteArray *>())
    , sizes(new QHash<QTcpSocket *, qint32 *>())
{
    createActions();
    createInstrumentWidgets();
    tuneWindow();
}

MainWindow::~MainWindow()
{
    client_socket->close();
}

void MainWindow::changeMousePos(const QPoint &point)
{
    mouse_pos->setText("X: " + QString().setNum(point.x())
                       + " Y: " +  QString().setNum(point.y()));
}

void MainWindow::instrumentPanelMenuAction()
{
    show_instrument_panel->isChecked()
            ? instrument_dock_widget->show()
            : instrument_dock_widget->hide();
}

void MainWindow::instrumentPanelDockWidgetAction()
{
    instrument_dock_widget->isVisible()
            ? show_instrument_panel->setChecked(true)
            : show_instrument_panel->setChecked(false);
}

void MainWindow::instrumentPanelDockWidgetAreaChanged(Qt::DockWidgetArea area)
{
    QBoxLayout *layout = instrument_dock_widget->findChild < QBoxLayout* >();
    if(layout)
    {
        if((area & Qt::LeftDockWidgetArea) || (area & Qt::RightDockWidgetArea))
        {
            layout->setDirection(QBoxLayout::TopToBottom);
            layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
        }
        if((area & Qt::TopDockWidgetArea) || (area & Qt::BottomDockWidgetArea))
        {
            layout->setDirection(QBoxLayout::LeftToRight);
            layout->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
        }
    }
}

void MainWindow::connectButtonClicked()
{
    if(!(client_socket->state() & QAbstractSocket::ConnectedState) || !(client_socket->state() & QAbstractSocket::ConnectingState))
    {
        //qDebug() << "Connecting";
        connectToHost();
    } else
    {
        //qDebug() << "Connected";
    }
}

void MainWindow::sendLineToHost(QLine line)
{
    if(client_socket->state() & QAbstractSocket::ConnectedState)
    {
        QByteArray data;
        QDataStream data_stream(&data, QIODevice::ReadWrite);
        data_stream << static_cast<qint32>(line.x1());
        data_stream << static_cast<qint32>(line.y1());
        data_stream << static_cast<qint32>(line.x2());
        data_stream << static_cast<qint32>(line.y2());        
        client_socket->write(intToArray(data.size()));
        client_socket->write(data);
    }
}

void MainWindow::readHost()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers->value(socket);
    qint32 *s = sizes->value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());        
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size))
        {            
            if (size == 0 && buffer->size() >= 4)
            {
                size = arrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size)
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;                
                QLine line = QLine(arrayToInt(data.mid(0, 4))
                                     , arrayToInt(data.mid(4, 4))
                                     , arrayToInt(data.mid(8, 4))
                                     , arrayToInt(data.mid(12, 4)));
                view->drawLine(line);
            }
        }
    }
}

bool MainWindow::connectToHost(QHostAddress host, quint16 port)
{
    for(quint16 tested_port = port; tested_port != 65535; ++tested_port)
    {
        client_socket->connectToHost(host, tested_port);
        if(client_socket->waitForConnected(1000))
        {
            QByteArray *buffer = new QByteArray();
            qint32 *s = new qint32(0);
            buffers->insert(client_socket, buffer);
            sizes->insert(client_socket, s);
            connect(client_socket, SIGNAL(readyRead()), this, SLOT(readHost()));
            return true;
        }
    }
    return false;
}

void MainWindow::tuneWindow()
{
    //qApp->setStyle("Windows");
    setMinimumSize(800, 600);

    QFrame *frame = new QFrame;
    view = new GraphicsView(frame);
    view->setMouseTracking(true);

    if(view->scene()) delete(view->scene());
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 640, 480);
    view->setScene(scene);
    view->update();

    mouse_pos = new QLabel();
    mouse_pos->setToolTip("Mouse coordinates on canvas");
    statusBar()->addPermanentWidget(mouse_pos);
    statusBar()->setLayoutDirection(Qt::RightToLeft);
    view->show();

    setCentralWidget(frame);
    connect(view, SIGNAL(getMousePos(QPoint)), this, SLOT(changeMousePos(QPoint)));
    connect(view, SIGNAL(getLine(QLine)), this, SLOT(sendLineToHost(QLine)));
}

void MainWindow::createActions()
{
    QMenu *panelsMenu = menuBar()->addMenu(tr("&Panels"));
    // Instrument menu actions
    show_instrument_panel = new QAction(tr("&Instrumet panel"), this);
    show_instrument_panel->setCheckable(true);
    show_instrument_panel->setChecked(true);
    show_instrument_panel->setStatusTip(tr("Ahow instrument panel"));
    connect(show_instrument_panel, &QAction::changed, this, &MainWindow::instrumentPanelMenuAction);
    panelsMenu->addAction(show_instrument_panel);
}

void MainWindow::createInstrumentWidgets()
{
    // DockWidget with instrument buttons
    instrument_dock_widget = new QDockWidget("Instruments");
    connect(instrument_dock_widget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(instrumentPanelDockWidgetAction()));
    connect(instrument_dock_widget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(instrumentPanelDockWidgetAreaChanged(Qt::DockWidgetArea)));
    QWidget *instrument_panel = new QWidget();
    instrument_panel->setMinimumWidth(150);

    QBoxLayout *instrument_widget_layout = new QBoxLayout(QBoxLayout::TopToBottom, instrument_panel);
    instrument_widget_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    instrument_panel->setLayout(instrument_widget_layout);

    QPushButton *connect_button = new QPushButton("Connect");
    connect_button->setMinimumSize(panel_button_size);
    connect_button->setMaximumSize(panel_button_size);
    connect(connect_button, SIGNAL(clicked(bool)), this, SLOT(connectButtonClicked()));
    instrument_widget_layout->addWidget(connect_button);

    instrument_dock_widget->setWidget(instrument_panel);
    addDockWidget(Qt::LeftDockWidgetArea, instrument_dock_widget);
}

qint32 MainWindow::arrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray MainWindow::intToArray(qint32 source)
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
