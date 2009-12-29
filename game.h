#ifndef GAME_H
#define GAME_H

#include <QtGui>

enum TileType {
    Normal,
    DoubleLetter,
    TripleLetter,
    DoubleWord,
    TripleWord,
    Center,
    NumTypes
};

class Tile : public QGraphicsWidget
{
public:
    Tile(TileType tileType);
    TileType tileType() const { return d.tileType; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * = 0);
private:
    struct Data {
        TileType tileType;
    } d;
};

class Game : public QGraphicsScene
{
    Q_OBJECT
public:
    Game(QObject *parent = 0);
    ~Game();
    static QString defaultBag();
public slots:
    void onSceneRectChanged(const QRectF &sceneRect);
private:
    void initBoard(const QString &file);

    struct Data {
        QVector<Tile*> board;
        QVector<QChar> letters;
        QString bag;
        QHash<QChar, int> scoring;
        int rows, columns;
    } d;
};


#endif
