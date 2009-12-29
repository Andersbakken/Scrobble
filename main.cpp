#include <QtGui>
#include "game.h"

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = 0)
        : QGraphicsView(parent)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void resizeEvent(QResizeEvent *e)
    {
        if (scene()) {
            scene()->setSceneRect(viewport()->rect());
        }
        QGraphicsView::resizeEvent(e);
    }
};

#include "main.moc"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    GraphicsView view;
    Game game;
    view.setScene(&game);
    view.show();
    return a.exec();
}
