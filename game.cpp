#include "game.h"

Game::Game(QObject *parent)
    : QGraphicsScene(parent)
{
    d.dictionary = 0;
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
    initDictionary(":/dict");
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
    d.letters.resize(d.columns * d.rows);
    Letter *l;
    d.letters[14] = (l = new Letter('A'));
    addItem(l);
    d.letters[15] = (l = new Letter('a'));
    addItem(l);
    d.letters[16] = (l = new Letter('r'));
    addItem(l);
    d.letters[17] = (l = new Letter('d'));
    addItem(l);
    d.letters[18] = (l = new Letter('v'));
    addItem(l);
    d.letters[19] = (l = new Letter('a'));
    addItem(l);
    d.letters[20] = (l = new Letter('r'));
    addItem(l);
    d.letters[21] = (l = new Letter('k'));
    addItem(l);
}

void Game::onSceneRectChanged(const QRectF &sceneRect)
{
    if (d.columns > 0 && d.rows > 0) {
        QRectF r(0, 0, qMin(sceneRect.width(), sceneRect.height()),
                 qMin(sceneRect.width(), sceneRect.height()));
        r.moveCenter(sceneRect.center());
        const QSizeF size(r.width() / d.columns, r.height() / d.rows);
        for (int i=0; i<d.board.size(); ++i) {
            const QRectF rect(r.x() + ((i % d.columns) * size.width()),
                              r.y() + ((i / d.columns) * size.height()),
                              size.width(), size.height());
            d.board.at(i)->setGeometry(rect);
            if (d.letters.at(i)) {
                d.letters.at(i)->setGeometry(rect);
            }
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
    enum { Margin = 2 };
    const QRect r = option->rect.adjusted(Margin, Margin, -Margin, -Margin);
    if (r.width() < 1 || r.height() < 1)
        return;

//     if (d.tileType == Normal) {
//         static QPixmap bg("wood.png");
//         Q_ASSERT(!bg.isNull());
//         realPainter->setBrushOrigin(-pos());
//         realPainter->fillRect(r, bg);
//         return;
//     }

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

int Game::score(int x, int y, Qt::Orientation orientation, const QString &word) const
{

}

int count = 1;
static void findOrCreate(Node *node, const QString &word, int index)
{
    Q_ASSERT(node);
    while (index < word.size() && !word.at(index).isLetter()) // '
        ++index;
    if (index == word.size()) {
        node->word = true;
    } else {
        const int idx = word.at(index).toLower().toLatin1() - 'a';
        if (!node->children[idx]) {
            node->children[idx] = new Node;
            ++count;
        }
        findOrCreate(node->children[idx], word, index + 1);
    }
}

static inline void dump(Node *node, int indent = 0)
{
    for (int i=0; i<26; ++i) {
        if (node->children[i]) {
            printf("%s%c\n", QByteArray().fill(' ', indent).constData(), 'a' + i);
            dump(node->children[i], indent + 2);
        }
    }
}

void Game::initDictionary(const QString &file)
{
    delete d.dictionary;
    d.dictionary = new Node;
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    Q_ASSERT(f.isReadable());
    QTextStream ts(&f);
    QString word;
    while (!ts.atEnd()) {
        ts >> word;
        findOrCreate(d.dictionary, word, 0);
    }
//    dump(d.dictionary);
}

bool Game::isWord(const QString &word) const
{
    Node *node = d.dictionary;
    int idx = 0;
    while (node && idx < word.size()) {
        const char ch = word.at(idx++).toLower().toLatin1();
        node = node->children[ch - 'a'];
    }
    return node && node->word;
}

Letter::Letter(const QChar &ch)
{
    setZValue(10);
    d.letter = ch;
}

void Letter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * )
{
    static QPixmap bg("wood.png");
    Q_ASSERT(!bg.isNull());
    painter->setBrushOrigin(-d.letter.toLatin1(), -d.letter.toLatin1());
    painter->fillRect(option->rect, bg);
    QFont font;
    font.setPixelSize(option->rect.height() * .9);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(option->rect, Qt::AlignCenter, d.letter);
}
