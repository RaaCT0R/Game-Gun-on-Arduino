#include "win_main.h"
#include "ui_win_main.h"

win_main::win_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::win_main)
{
    ui->setupUi(this);

    sPort = new QSerialPort();
    sPort->setBaudRate(38400);

    isConnected = false;
    calibrePhase = true;
    calibreCounter = 0;

    gx_bias = 0;
    gy_bias = 0;
    gz_bias = 0;

    LUx = -1;
    LUy = -1;

    detectDevice();
}

win_main::~win_main()
{
    delete ui;
}

void win_main::receiveData()
{
    inputData.append(sPort->readAll());
    handleData();
}

void win_main::detectDevice()
{
    if (isConnected)
    {
        ui->textEdit->append("\t|--ERROR You are already connected to " + currentPort);
        return;
    }

    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        ui->textEdit->append("Try on port: " + port.portName());
        sPort->setPortName(port.portName());

        if (!sPort->open(QIODevice::ReadWrite))
        {
            ui->textEdit->append(sPort->errorString());
            return;
        }
        else
        {
            connect(sPort, SIGNAL(readyRead()), this, SLOT(receiveData()));
            ui->textEdit->append("\t|--SUCCESS Successfully connected.");
            isConnected = true;
            currentPort = port.portName();
            return;
        }
    }
    ui->textEdit->append("\t|--ERROR No device detected!");
}

void win_main::handleData()
{
    QStringList data = inputData.split('\n');
    inputData.clear();
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].size() == 0)
            continue;
        if (!data[i].contains('\r'))
            inputData.append(data[i]);
        else
        {
            data[i].replace("\r", "");
            QStringList params = data[i].split('\t');
            float x = 0;

            if (params.size() != 8)
            {
                qDebug() << "\t|--ERROR Ambiguous on split, size is " << params.size();
                break;
            }

            //ax
            x = QVariant(params[1]).toFloat();
            x /= 16384;
            if (calibrePhase)
                calibre(1, x);
            else
                updateData(1, x);

            //ay
            x = QVariant(params[2]).toFloat();
            x /= 16384;
            if (calibrePhase)
                calibre(2, x);
            else
                updateData(2, x);

            //az
            x = QVariant(params[3]).toFloat();
            x /= 16384;
            if (calibrePhase)
                calibre(3, x);
            else
                updateData(3, x);

            //gx
            x = QVariant(params[4]).toFloat();
            x /= 131;
            if (calibrePhase)
                calibre(4, x);
            else
                updateData(4, x);

            //gy
            x = QVariant(params[5]).toFloat();
            x /= 131;
            if (calibrePhase)
                calibre(5, x);
            else
                updateData(5, x);

            //gz
            x = QVariant(params[6]).toFloat();
            x /= -131;
            if (calibrePhase)
                calibre(6, x);
            else
                updateData(6, x);

            //button state
            bool bState = QVariant(params[7]).toBool();
            mouseLeftClick(bState);
        }
    }
}

void win_main::updateData(int index, float x)
{
    float data = 0;
    switch (index)
    {
    case 1:
        data = ui->lineEdit_ax->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * x;
        ui->lineEdit_ax->setText(QString::number(data, 'f', 2));
        break;
    case 2:
        data = ui->lineEdit_ay->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * x;
        ui->lineEdit_ay->setText(QString::number(data, 'f', 2));
        break;
    case 3:
        data = ui->lineEdit_az->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * x;
        ui->lineEdit_az->setText(QString::number(data, 'f', 2));
        break;
    case 4:
        data = ui->lineEdit_gx->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * (x - gx_bias);
        ui->lineEdit_gx->setText(QString::number(data, 'f', 2));
        break;
    case 5:
        data = ui->lineEdit_gy->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * (x - gy_bias);
        ui->lineEdit_gy->setText(QString::number(data, 'f', 2));
        moveMouseY((int) data);
        break;
    case 6:
        data = ui->lineEdit_gz->text().toFloat();
        data = ALPHA * data + (1 - ALPHA) * (x - gz_bias);
        ui->lineEdit_gz->setText(QString::number(data, 'f', 2));
        moveMouseX((int) data);
        break;
    default:
        break;
    }
}

void win_main::calibre(int index, float x)
{
    switch (index)
    {
    case 4:
        gx_bias = gx_bias + x;
        calibreCounter++;
        break;
    case 5:
        gy_bias = gy_bias + x;
        break;
    case 6:
        gz_bias = gz_bias + x;
        break;
    default:
        break;
    }
    if (calibreCounter == CALIBRE_COUNTER)
    {
        gx_bias /= calibreCounter;
        gy_bias /= calibreCounter;
        gz_bias /= calibreCounter;
        calibrePhase = false;

        ui->textEdit->append("\t|--SUCCESS Calibration finished");
        ui->textEdit->append("\t\t| gx = " + QString::number(gx_bias, 'f', 2));
        ui->textEdit->append("\t\t| gy = " + QString::number(gy_bias, 'f', 2));
        ui->textEdit->append("\t\t| gz = " + QString::number(gz_bias, 'f', 2));
    }
}

void win_main::moveMouseX(int vx)
{
    if (LUx == -1)
        LUx = QTime::currentTime().msecsSinceStartOfDay();

    else
    {
        int diff = QTime::currentTime().msecsSinceStartOfDay() - LUx;
        LUx += diff;
        QPoint pos = QCursor::pos();
        pos.setX(pos.x() + SENSIVITY * (vx * diff) / 1000);
        QCursor::setPos(pos);
    }
}

void win_main::moveMouseY(int vy)
{
    if (LUy == -1)
        LUy = QTime::currentTime().msecsSinceStartOfDay();

    else
    {
        int diff = QTime::currentTime().msecsSinceStartOfDay() - LUy;
        LUy += diff;
        QPoint pos = QCursor::pos();
        pos.setY(pos.y() + 50 * (vy * diff) / 1000);
        QCursor::setPos(pos);
    }
}

void win_main::mouseLeftClick(bool clicked)
{
    INPUT input[2] = {0};
    if (clicked)
    {
        input[0].type = INPUT_MOUSE;
        input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    }
    else
    {
        input[0].type = INPUT_MOUSE;
        input[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }
    SendInput(1, input, sizeof(INPUT));
}
