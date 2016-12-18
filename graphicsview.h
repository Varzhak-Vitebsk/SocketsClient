#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QtWidgets>

enum class DrawForm { DOT, LINE, RECT, RUBBER };

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = Q_NULLPTR);
    void setDrawForm(DrawForm draw_form);
    void setPenSize(int size);
    void setPenColor(QColor color);
    void drawLine(const QLine &line);

signals:
    void getMousePos(const QPoint&);
    void getLine(const QLine&);

public slots:    

protected:    
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QPointF latest_mouse_pos, initial_mouse_pos;
    DrawForm current_draw_form;
    QGraphicsItem *current_item;
    QPen *pen;

};

#endif // GRAPHICSVIEW_H
