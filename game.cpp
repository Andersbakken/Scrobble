#include "game.h"

Game::Game(QObject *parent)
    : QGraphicsScene(parent)
{
    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(onSceneRectChanged(QRectF)));
    setBackgroundBrush(Qt::white);
    d.rows = -1;
    d.columns = -1;

    static const char *scoring[] = {
        "AEILNORSTU", // 1
        "DG", // 2
        "BCMP", // 3
        "FHVWY", // 4
        "K", // 5
        "", // 6
        "", // 7
        "JX", // 8
        "", // 9
        "QZ", // 10
        0
    };
    for (int i=0; scoring[i]; ++i) {
        const QByteArray letters = QByteArray::fromRawData(scoring[i], strlen(scoring[i]));
        for (int i=0; i<letters.size(); ++i) {
            d.scoring[letters.at(i)] = (i + 1);
        }
    }
    initBoard(":/board.txt");
}

Game::~Game()
{
}

QString Game::defaultBag()
{
    struct {
        const char letter;
        const int count;
    } static const letters[] = {
        { 'A', 9 },
        { 'B', 2 },
        { 'C', 2 },
        { 'D', 4 },
        { 'E', 12 },
        { 'F', 2 },
        { 'G', 3 },
        { 'H', 2 },
        { 'I', 9 },
        { 'J', 1 },
        { 'K', 1 },
        { 'L', 4 },
        { 'M', 2 },
        { 'N', 6 },
        { 'O', 8 },
        { 'P', 2 },
        { 'Q', 1 },
        { 'R', 6 },
        { 'S', 4 },
        { 'T', 6 },
        { 'U', 4 },
        { 'V', 2 },
        { 'W', 2 },
        { 'X', 1 },
        { 'Y', 2 },
        { 'Z', 1 },
        { ' ', 2 },
        { '\0', -1 }
    };
    QString bag;
    for (int i=0; letters[i].count > 0; ++i) {
        for (int j=0; j<letters[i].count; ++j) {
            bag.insert(rand() % bag.size(), letters[i].letter);
        }
    }
    return bag;
}

void Game::initBoard(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    Q_ASSERT(f.isReadable());
    d.columns = -1;
    d.rows = 0;
    char ch;
    d.board.reserve(f.size());
    int col = 0;
    while (f.getChar(&ch)) {
        TileType type;
        switch (ch) {
        case 'T': type = TripleWord; break;
        case 'D': type = DoubleWord; break;
        case 't': type = TripleLetter; break;
        case 'd': type = DoubleLetter; break;
        case 'X': type = Center; break;
        case ' ': type = Normal; break;
        case '\n':
            if (d.columns == -1) {
                d.columns = col;
            } else {
                Q_ASSERT(col == d.columns);
            }
            ++d.rows;
            col = 0;
            continue;
        default:
            qDebug() << ch;
            Q_ASSERT(0);
            break;
        }
        Tile *tile = new Tile(type);
        addItem(tile);
        d.board.append(tile);
        ++col;
    }
    d.board.resize(d.columns * d.rows);
}

void Game::onSceneRectChanged(const QRectF &sceneRect)
{
    if (d.columns > 0 && d.rows > 0) {
        QRectF r(0, 0, qMin(sceneRect.width(), sceneRect.height()),
                 qMin(sceneRect.width(), sceneRect.height()));
        r.moveCenter(sceneRect.center());
        const QSizeF size(r.width() / d.columns, r.height() / d.rows);
        for (int i=0; i<d.board.size(); ++i) {
            d.board.at(i)->setGeometry(QRectF(r.x() + ((i % d.columns) * size.width()),
                                              r.y() + ((i / d.columns) * size.height()),
                                              size.width(), size.height()));
        }
    }

}


Tile::Tile(TileType tileType)
{
    d.tileType = tileType;
}
void Tile::paint(QPainter *realPainter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    static QPixmap cache[NumTypes];
    enum { Margin = 4 };
    const QRect r = option->rect.adjusted(Margin, Margin, -Margin, -Margin);
    if (r.width() < 1 || r.height() < 1)
        return;

    if (cache[d.tileType].size() != r.size()) {
        QPixmap &pixmap = cache[d.tileType];
        pixmap = QPixmap(r.size());
        static const QColor brushes[] = {
            QColor("brown"), QColor("lightblue"), QColor(Qt::blue),
            QColor("orange"), QColor(Qt::red), QColor("orange")
        };
        pixmap.fill(brushes[d.tileType]);
        QPainter painter(&pixmap);
        static const QString text[] = {
            QString(), QLatin1String("Double\nLetter\nScore"),
            QLatin1String("Triple\nLetter\nScore"), QLatin1String("Double\nWord\nScore"),
            QLatin1String("Triple\nWord\nScore"), QString()
        };
        if (!text[d.tileType].isEmpty()) {
            painter.setPen(Qt::black);
            int pixelSize = option->rect.height() / 5;
            QFont font;
            while (pixelSize > 8) {
                font.setPixelSize(pixelSize);
                if (QFontMetrics(font).boundingRect(text[d.tileType]).size().expandedTo(r.size()) == r.size())
                    break;
                --pixelSize;
            }
            qDebug() << pixelSize;
            painter.setFont(font);
            painter.drawText(pixmap.rect(), Qt::AlignCenter, text[d.tileType]);
        }
        if (d.tileType == Center) {
            painter.setPen(QPen(Qt::black, 3));
            painter.drawLine(0, 0, pixmap.width(), pixmap.height());
            painter.drawLine(pixmap.width(), 0, 0, pixmap.height());
        }
    }
    Q_ASSERT(cache[d.tileType].size() == r.size());
    realPainter->drawPixmap(r, cache[d.tileType]);
}