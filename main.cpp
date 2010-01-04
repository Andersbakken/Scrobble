#include <QtGui>
#include "game.h"

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = 0)
        : QGraphicsView(parent)
    {
        setBackgroundBrush(QPixmap(":/wood.jpg"));
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
    void showEvent(QShowEvent *e)
    {
        restoreGeometry(QSettings().value("geometry").toByteArray());
        QGraphicsView::showEvent(e);
    }

    void closeEvent(QCloseEvent *e)
    {
        QSettings().setValue("geometry", saveGeometry());
        QGraphicsView::closeEvent(e);
    }

};

#include "main.moc"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setOrganizationName("AndersSoft");
    a.setOrganizationDomain("www.AndersSoft.com");
    a.setApplicationName("Scrobble");
    GraphicsView view;
    Game game;
    view.setScene(&game);
    view.show();
    return a.exec();
}
