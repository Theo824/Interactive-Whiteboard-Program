#include "window.h"
#include <stdio.h>
#include <QApplication>
#include <iostream>
#include <QByteArray>
#include <bitset>
#include <vector>
#include <thread>
#include <wiringPi.h>
#include <QDebug>
#include <atomic>
#include <chrono>
#include <cmath>

using namespace std;

/*
(1) Receive-window waits for ready bit before it does anything.
(2) Send-window sends data bit(encdata0) and ready bit (readsig) to receive-window.
(3) Send-window waits for finish bit (finishsig) before it does anything further after (2).
(4) Receive-window receives ready bit, then process the data bit in response, and sends finish bit to send-window.
(5) Send-window receives finish bit and starts again at (1) with a new data.
Process loops at 1->5.
*/

void serialise(atomic<bool>& program_is_running, unsigned int interval,  sendWindow *window);
void deserialise(atomic<bool>& program_is_running, unsigned int interval,  sinkWindow *window);

int main(int argc, char *argv[])
{
    wiringPiSetup();
    //send
    pinMode(0, OUTPUT);      //send readysig

    pinMode(2, OUTPUT);    //send data

    pinMode(3, INPUT);      //receive finishsig

    //receive
    pinMode(21, INPUT);     //receive readysig

    pinMode(22, INPUT);     //receive data

    pinMode(23, OUTPUT);   //send finishsig

    QApplication app(argc, argv);
    sendWindow sendwindow;
    sinkWindow sinkwindow;

     //START Codes from https://cplusplus.com/forum/general/190480/
    atomic<bool> running { true } ;
    const unsigned int update_interval= 10;
    //END

     //connects the slots and signals
    QObject::connect(&sinkwindow, &sinkWindow::clearsend, &sinkwindow, &sinkWindow::Clear);

    //create the threads
    thread serialiseThread(serialise, ref(running), update_interval, &sendwindow);
    thread deserialiseThread(deserialise, ref(running), update_interval, &sinkwindow);

    sendwindow.show();
    sinkwindow.show();

    return app.exec();

    //terminate the threads;
    serialiseThread.join();
    deserialiseThread.join();

    serialiseThread.~thread();
    deserialiseThread.~thread();


}
//serialise xy values into encoded data to be sent to gpio output
void serialise(atomic<bool>& program_is_running, unsigned int interval, sendWindow *window)
{
    while(program_is_running)
    {
        //serialise the message data
        //START Codes inspired from https://www.geeksforgeeks.org/program-decimal-binary-conversion and tlv1n23's P2
        int quotient, remainder;
        QList<int> xbits={0,0,0,0,0,0,0,0}, ybits= {0,0,0,0,0,0,0,0};
        QList<int> messdata= window->xydata;

        int xbit;
        quotient = messdata[0];
        int j=0;
        while(quotient!=0) {
            remainder = quotient % 2;
            xbit= remainder;
            xbits[j]= xbit;
            quotient /= 2;
            j++;
        }

        int ybit;
        quotient = messdata[1];
        j=0;
        while(quotient!=0) {
            remainder = quotient % 2;
            ybit= remainder;
            ybits[j]= ybit;
            quotient /= 2;
            j++;
        }
        // last array element is the MSB

        QList<int> encodeddata={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for(int i=0; i<8; i++)
        {
            encodeddata[i]= xbits[i];
        }
        for(int i=8; i<16; i++)
        {
            encodeddata[i]= ybits[i-8];
        }
        //END

        encodeddata[16]= window->clear;
        // bits= [0,0,0,0,1, 1, 0, 0, 1, 1, 0 ,1, 0, 0, 1, 1, 0] means 00001100 and 11010011 => 00110000 and 11001011 => (x,y)= (48, 203)
        // last bit is clear signal: 1 means clear receive window, 0 means do nothing

        //send data to gpio ouput
        int i=0;
        int l=1;

        digitalWrite(0, 0); //turn off ready sig
        qDebug()<<"encstart";
        while(l==1)
        {
            digitalWrite(2, encodeddata[i]); //send data

            //qDebug()<<i<<", "<<bits[i];
            i++;

            digitalWrite(0, 1); //turn on readysig    
            //qDebug()<<"send: turn on readysig";

            int b=0;
            while(b==0)
            {
                b= digitalRead(3);
                qDebug()<<"send: wait for finishsig to be 1";
            }

            digitalWrite(0,0); //turn off readysig

            int a=1;
            while(a)
            {
                a= digitalRead(3);
                qDebug()<<"send: wait for finishsig to reset to 0";
            }

            if(i==17){l=0;}
        }
        //delay by 10 ms to prevent crashes
        //START Codes from https://cplusplus.com/forum/general/190480/
        const auto wait_duration =chrono::milliseconds(interval);
        this_thread::sleep_for(wait_duration);
        //END
        qDebug()<<"encfinish";
    }
}
//deserialise the encoded binary data from gpio input into xy values
void deserialise(atomic<bool>& program_is_running, unsigned int interval, sinkWindow *window)
{
    while(program_is_running)
    {
        //qDebug()<<"encdata in deserialise"<<dataStruct.encdata0;
        QList<int> encodeddata= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //encoded data from gpio input

        //receive data from gpio input
        int i=0;
        int l=1;

        digitalWrite(23, 0); //turn off finish sig
        qDebug()<<"decstart";
        while(l==1)
        {
           int a=0;
           while(a==0)
           {
                a= digitalRead(21);
                qDebug()<<"sink: wait for readysig";
           }


           encodeddata[i]= digitalRead(22); //receive data

           digitalWrite(23, 1); //turn on finishsig
           //qDebug()<<"sink: send finishsig";
           //qDebug()<<"de_enc"<<i<<","<<encodeddata[i];

           int b=1;
           while(b)
           {
                b= digitalRead(21);
                qDebug()<<"sink: wait for readysig to reset to 0";
           }

           digitalWrite(23, 0); //turn off finishsig

           i++;
           if(i==17){l=0;}
        }
        qDebug()<<"decfinish";

        //deserialise the encoded data
        //START Codes inspired from https://prepinsta.com/cpp-program/cpp-program-for-binary-to-decimal-conversion/ and tlv1n23's P2
        QList<int> xbits={0,0,0,0,0,0,0,0}, ybits={0,0,0,0,0,0,0,0};

        int xvalue=0;
        for(int i=0; i<8; i++)
        {
            xbits[i]= encodeddata[i];
            xvalue+= xbits[i]*pow(2, i);
        }
        for(int i=8; i<16; i++)
        {
            ybits[i-8]= encodeddata[i];
        }

        int yvalue=0;
        for(int i=0; i<8; i++)
        {
            yvalue+= ybits[i]*pow(2, i);
        }
        //END

        //stores xy value into xydata of receive window
        if(((xvalue==0) && (yvalue==0))==0)
        {
            window->xydataList2.append({(xvalue/2), (yvalue/2)}); //change to 1 not 2 if it is the second pair (second program to run its send window)
        } else{}                                                  //otherwise keep the 2 to fix the scaling issue

        if(encodeddata[16]==1){emit window->clearsend();} else{}    //if last bit==1 (clear signal) then erase receive window

        window->update();
        qDebug()<< "xvalue= "<<xvalue<<", "<<"yvalue= "<<yvalue;

        //delay by 10 ms to prevent crashes
        //START Codes from https://cplusplus.com/forum/general/190480/
        const auto wait_duration = chrono::milliseconds(interval);
        this_thread::sleep_for(wait_duration);
        //END
    }

}
