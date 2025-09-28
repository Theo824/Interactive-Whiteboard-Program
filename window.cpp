#include "window.h"
#include <QPainter>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <map>
#include <vector>
#include <QPushButton>
//#include <pigpio.h>
#include <QList>
#include <cmath>

using namespace std;

/* ************ Send Window *************** */

sendWindow::sendWindow(QWidget *parent) : QWidget(parent)
{
   // set form size
   setFixedSize(255,255);
   setWindowTitle("Send");
   auto *clearBtn = new QPushButton("Clear", this); //this= &sendWindow
   clearBtn->setGeometry(180, 220, 75, 30);
   connect(clearBtn, &QPushButton::clicked, this, &sendWindow::Clear);
}



void sendWindow::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QPen pen;
    QFont font;
    font.setPixelSize(50);
    pen.setWidth(5);
    painter.setPen(pen);
    painter.setFont(font);
    for(int j=0; j<xydataList.length(); j++)
    {
    painter.drawPoint(xydataList[j][0], xydataList[j][1]);
    }


}
void sendWindow::mouseMoveEvent(QMouseEvent * event)
{
    // get click position
    qDebug() << "Mouse x " << event->x() << " Mouse y " << event->y();

    //stores xyvalues from mouse input into a member xydataList to be drawn on the paintEvent()
    xydata[0]= event->x();
    xydata[1]= event->y();
    if((xydata[0]>= 254) || (xydata[1]>=254) ||(xydata[0]<0) || (xydata[1]<0))
    {
        xydata[0]= 254;
        xydata[1]= 254;
        xydataList.append(xydata);
    }
    else
    {
        xydataList.append(xydata);
    }
    //emit send1(xydata, 0);
    clear=0;
    update();
}

void sendWindow::mousePressEvent(QMouseEvent * event)
{
    // get click position
    qDebug() << "Mouse x " << event->x() << " Mouse y " << event->y();

    xydata[0]= event->x();
    xydata[1]= event->y();
    xydataList.append(xydata);

    clear=0;
    //emit send1(xydata, 0);

    update();
}

//clears the send window
void sendWindow::Clear()
{
    xydataList.clear();
    clear=1;
    update();

}

/* ************ Receive Window *************** */

sinkWindow::sinkWindow(QWidget *parent) : QWidget(parent)
{
    // set form size
    setFixedSize(255,255);
    setWindowTitle("Receive");
}

void sinkWindow::paintEvent(QPaintEvent * event)
{
    QPainter painter1(this);
    QPen pen;
    QFont font;
    font.setPixelSize(50);
    pen.setWidth(5);
    painter1.setPen(pen);
    painter1.setFont(font);
    for(int j=0; j<xydataList2.length(); j++)
    {
        painter1.drawPoint(xydataList2[j][0], xydataList2[j][1]);
    }
}

//clears the receive window
void sinkWindow::Clear()
{
    xydataList2.clear();
    update();
}


