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

struct Node {
    Node() { memset(children, 0, sizeof(Node*) * 26); }
    ~Node() { delete []children; }
    bool word;
    Node *children[26];
};

class Game : public QGraphicsScene
{
    Q_OBJECT
public:
    Game(QObject *parent = 0);
    ~Game();
    static QString defaultBag();
    int score(int x, int y, Qt::Orientation orientation, const QString &word) const;
    bool isWord(const QString &word) const;
public slots:
    void onSceneRectChanged(const QRectF &sceneRect);
private:
    void initDictionary(const QString &file);
    void initBoard(const QString &file);

    struct Data {
        Node *dictionary;
        QVector<Tile*> board;
        QVector<QChar> letters;
        QString bag;
        QHash<QChar, int> scoring;
        int rows, columns;
    } d;
};


#endif
