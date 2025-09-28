
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMouseEvent>
#include <vector>
#include <map>

#include <QList>
using namespace std;

class sendWindow : public QWidget
{
    Q_OBJECT

public:
    sendWindow(QWidget *parent = 0);
    QList<int> xydata= {0, 0};  //stores temporary xy values
    QList<QList<int>> xydataList; //stores an array of xy values to be drawn on paintEvent()
    int clear=0;

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);

public slots:
    void Clear();

signals:
    void send1(QList<int> XYDATA, int CLEAR);
    void send2(QList<int> XYDATA); //acts as transmit to gpio

};

class sinkWindow : public QWidget
{
    Q_OBJECT

public:
    sinkWindow(QWidget *parent = 0);
    QList<int> xydata2= {0, 0};
    QList<QList<int>> xydataList2;

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void Clear();

signals:
    void clearsend();

};


