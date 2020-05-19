#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include <QWidget>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QList>
#include <QTime>
#include <QDateTime>
#include <QCursor>
#include <QDebug>
#include <windows.h>

#define ALPHA 0.8
#define CALIBRE_COUNTER 100
#define SENSIVITY 50

namespace Ui {
class win_main;
}

class win_main : public QWidget
{
    Q_OBJECT

public:
    explicit win_main(QWidget *parent = 0);
    ~win_main();

private:
    Ui::win_main *ui;
    QSerialPort* sPort;
    QString inputData;
    QString currentPort;
    bool isConnected;
    bool calibrePhase;
    int calibreCounter;
    int LUx;
    int LUy;
    float gx_bias;
    float gy_bias;
    float gz_bias;

    void detectDevice();
    void handleData();
    void updateData(int index, float x);
    void calibre(int index, float x);
    void moveMouseX(int vx);
    void moveMouseY(int vy);
    void mouseLeftClick(bool clicked);

private slots:
    void receiveData();
};

#endif // WIN_MAIN_H
