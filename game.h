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
    Q_OBJECT
public:
    Tile(TileType tileType);
    TileType tileType() const { return d.tileType; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * = 0);
    bool isUsed() const { return d.used; }
private:
    struct Data {
        bool used;
        TileType tileType;
    } d;
};

class Letter : public QGraphicsWidget
{
    Q_OBJECT
public:
    Letter(const QChar &ch);
    QChar letter() const { return d.letter; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * = 0);
private:
    struct Data {
        QChar letter;
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
    QChar letter(int x, int y) const;
    int scoreWord(int x, int y, Qt::Orientation orientation, const QVector<QChar> &letters) const;
public slots:
    void onSceneRectChanged(const QRectF &sceneRect);
private:
    void initDictionary(const QString &file);
    void initBoard(const QString &file);

    struct Data {
        Node *dictionary;
        QVector<Tile*> board;
        QVector<Letter*> letters;
        QString bag;
        QHash<QChar, int> scoring;
        int rows, columns;
    } d;
};


#endif
