#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QList>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QHostAddress>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mIsOpen=false;
    IsConnected=false;
    IsKeyControl=false;
    comMode=false;//自动模式
    Ifvideo=false;//不打开视频
    lastPause=false;
    setWindowTitle("天真吴邪");
    ui->pBSend->setEnabled(false);
    ui->pBRun->setEnabled(false);
    ui->pBStop->setEnabled(false);
    ui->pBKeyBoard->setEnabled(false);
    ui->bPVideo->setEnabled(false);
    ui->lEIP->setText("192.168.43.215");
    ui->lEPort->setText("6666");
    imagewidth=640;
    imageheight=480;
    numFindDark();
    direConDark();


    //显示图片
//    QPixmap pix("E:\\Programs\\QT\\SerialPort\\1.jpg");
//    QPixmap dest=pix.scaled(ui->Image->size(),Qt::KeepAspectRatio);
//    ui->Image->setPixmap(dest);
    //检测设备端口号
    QList<QSerialPortInfo> serialportinfo = QSerialPortInfo::availablePorts();
    int count = serialportinfo.count();
    for (int i=0;i<count;i++)
    {
        ui->cBSerialPort->addItem(serialportinfo.at(i).portName());
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
//网络相关函数

void MainWindow::receive()
{
    if(Ifvideo==true){
        QByteArray bytes=NULL;
        while(tcpsocket->waitForReadyRead(400))
        {
            bytes.append((QByteArray)tcpsocket->readAll());
            package_count++;
        }

        if(package_count>10){
            package_count=0;
            memcpy(imagebuffer, bytes, IMAGESIZE);
            image_receive=new QImage(imagebuffer, imagewidth,imageheight,QImage::Format_RGB888);
             if(!image_receive->isNull()){
                package_count=0;
                QPixmap pix = QPixmap::fromImage(*image_receive);
                 ui->Image->setPixmap(pix.scaled(ui->Image->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
            }
             QString str;
             if(lastSend=="Send1")
                str="Send2";
             else
                 str="Send1";
             //将自己的信息显示在聊天窗口
             tcpsocket->write(str.toUtf8());
             lastSend=str;
        }
        else{
            package_count=0;
            QString str;
            if(lastSend=="Send1")
               str="Send2";
            else
                str="Send1";
            //将自己的信息显示在聊天窗口
            tcpsocket->write(str.toUtf8());
            lastSend=str;
        }
    }
    else{
        QByteArray recvMsg = tcpsocket->readAll();
        if(recvMsg.size()<50){
            ui->pTAccept->appendPlainText("服务器: " + recvMsg);
        }
    }



}
void MainWindow::TcpConnect()
{

    tcpsocket=new QTcpSocket(this);
    QString ip=ui->lEIP->text();
    quint16 port=ui->lEPort->text().toInt();
    //与服务器连接
    tcpsocket->connectToHost(QHostAddress(ip),port);
    // 检测服务器是否回复了数据
//    connect(tcpsocket, &QTcpSocket::readyRead, [=]()
//    {
//        // 接收服务器发送的数据
//        QByteArray recvMsg = tcpsocket->readAll();
//        ui->pTAccept->appendPlainText("服务器: " + recvMsg);
//    });
    connect(tcpsocket, &QTcpSocket::readyRead,this,&MainWindow::receive);
    // 检测是否和服务器是否连接成功了
    connect(tcpsocket, &QTcpSocket::connected, this, [=]()
    {
        ui->pTAccept->appendPlainText("连接成功!");
        ui->bPNet->setText("断开");
        numFindLight();
        IsConnected=true;
        ui->bPVideo->setEnabled(true);
//        package_count=0;
//        QString str="Send";
//        tcpsocket->write(str.toUtf8());
    });

    // 检测服务器是否和客户端断开了连接
    connect(tcpsocket, &QTcpSocket::disconnected, this, [=]()
    {
        ui->pTAccept->appendPlainText("断开连接");
        ui->bPVideo->setText("打开视频");
        ui->bPNet->setText("连接");
        ui->bPVideo->setEnabled(false);
        IsConnected=false;
        Ifvideo=false;
    });

}
void MainWindow::on_bPNet_clicked()
{
    if(false==IsConnected)
    {
        if(nullptr==ui->lEIP || nullptr==ui->lEPort)
            return ;
        TcpConnect();
    }
    else
    {
        QString str="Disconnect";
        tcpsocket->write(str.toUtf8());
        IsConnected=false;
        numFindDark();
        ui->bPNet->setText("连接");
        ui->bPVideo->setEnabled(false);
        if(nullptr==tcpsocket)
            return;
//        tcpsocket->disconnectFromHost();//断开与服务器的连接
//        tcpsocket->close();//关闭通信套接字

    }

}
void MainWindow::on_bPVideo_clicked()
{
    if(Ifvideo==true){
        Ifvideo=false;
        QString str="CloseVideo";
        tcpsocket->write(str.toUtf8());
        ui->bPVideo->setText("打开视频");
        QByteArray recvMsg = tcpsocket->readAll();
    }
    else{
        package_count=0;
        QString str="Send";
        tcpsocket->write(str.toUtf8());
        Ifvideo=true;
        ui->bPVideo->setText("关闭视频");
    }
}






//串口相关函数
bool MainWindow::getSerialPortConfig()
{
    //获取串口配置
    mPortName=ui->cBSerialPort->currentText();
    mBaudRate=ui->cBBaudRate->currentText();
    mParity=ui->cBParity->currentText();
    mDataBits=ui->cBDataBits->currentText();
    mStopBits=ui->cBStopBits->currentText();
    comMode=ui->cBMode->currentText();
    //qDebug()<<mBaudRate;
    port = new QSerialPort(mPortName);
    connect(port,SIGNAL(readyRead()),this,SLOT(on_SerialPortreadyRead()));
    //设置串口配置
    //波特率
    if("9600"==mBaudRate){
        //serialport.setBaudRate(QSerialPort::Baud9600);
        port->setBaudRate(QSerialPort::Baud9600);
    }
    else if("19200"==mBaudRate){
        //serialport.setBaudRate(QSerialPort::Baud19200);
        port->setBaudRate(QSerialPort::Baud19200);
    }
    else
    {
        //serialport.setBaudRate(QSerialPort::Baud115200);
        port->setBaudRate(QSerialPort::Baud115200);
    }
    //校验位
    if("Odd"==mParity){
        //serialport.setParity(QSerialPort::OddParity);
        port->setParity(QSerialPort::OddParity);
    }
    else if("Even"==mParity){
        //serialport.setParity(QSerialPort::EvenParity);
        port->setParity(QSerialPort::EvenParity);
    }
    else
    {
        //serialport.setParity(QSerialPort::NoParity);
        port->setParity(QSerialPort::NoParity);
    }
    //数据位
    if("5"==mDataBits){
        //serialport.setDataBits(QSerialPort::Data5);
        port->setDataBits(QSerialPort::Data5);
    }
    else if("6"==mDataBits){
        //serialport.setDataBits(QSerialPort::Data6);
        port->setDataBits(QSerialPort::Data6);
    }
    else if("7"==mDataBits){
        //serialport.setDataBits(QSerialPort::Data7);
        port->setDataBits(QSerialPort::Data7);
    }
    else
    {
        //serialport.setDataBits(QSerialPort::Data8);
        port->setDataBits(QSerialPort::Data8);
    }
    //停止位
    if("1"==mStopBits){
        //serialport.setStopBits(QSerialPort::OneStop);
        port->setStopBits(QSerialPort::OneStop);
    }
    else
    {
        //serialport.setStopBits(QSerialPort::OneAndHalfStop);
        port->setStopBits(QSerialPort::OneAndHalfStop);
    }
    //qDebug()<<"配置完成";
    if("Auto"==comMode){
        Mode=true;
        ui->pBKeyBoard->setEnabled(false);
    }
    else
    {
        Mode=false;
        ui->pBKeyBoard->setEnabled(true);
    }
    return port->open(QSerialPort::ReadWrite);
}
void MainWindow::on_pBOpenPort_clicked()
{
    if(true==mIsOpen){
        //当前串口已经打开，要执行关闭动作
        //serialport.close();
        port->close();
        ui->pBOpenPort->setText("打开串口");
        mIsOpen=false;

        ui->pBSend->setEnabled(false);

        IsKeyControl=false;
        this->releaseKeyboard();
        ui->pBKeyBoard->setText("开键控");

        ui->pBSend->setEnabled(true);
        ui->cBSerialPort->setEnabled(true);
        ui->cBBaudRate->setEnabled(true);
        ui->cBParity->setEnabled(true);
        ui->cBDataBits->setEnabled(true);
        ui->cBStopBits->setEnabled(true);
        ui->cBMode->setEnabled(true);
        ui->pBRun->setEnabled(false);
        ui->pBStop->setEnabled(false);
        ui->pBKeyBoard->setEnabled(false);
        //numFindDark();
        this->releaseKeyboard();
        direConDark();
    }
    else
    {
        //当前串口没有打开，打开串口
        if(true==getSerialPortConfig()){
            qDebug()<<"串口打开成功";
            mIsOpen=true;

            ui->pBSend->setEnabled(true);

            ui->pBOpenPort->setText("关闭串口");
            ui->cBSerialPort->setEnabled(false);
            ui->cBBaudRate->setEnabled(false);
            ui->cBParity->setEnabled(false);
            ui->cBDataBits->setEnabled(false);
            ui->cBStopBits->setEnabled(false);
            ui->cBMode->setEnabled(false);
            ui->pBRun->setEnabled(true);
            ui->pBStop->setEnabled(true);

            //numFindLight();
            if(false==Mode)
            {
                QString data="Manual";
                QString send="m";
                port->write(send.toUtf8());
                ui->pTAccept->appendPlainText("串口发："+data);
                direConLight();
            }
            else{
                QString data="Auto";
                QString send="a";
                port->write(send.toUtf8());
                ui->pTAccept->appendPlainText("串口发："+data);
                this->grabKeyboard();
            }

        }
        else{
            qDebug()<<"串口打开失败";
        }
    }

}
void MainWindow::on_pBSend_clicked()
{
    if(true==mIsOpen){
        //serialport.write(ui->pTSend->toPlainText().toUtf8());
        port->write(ui->pTSend->toPlainText().toUtf8());
        QString str=ui->pTSend->toPlainText();
        //在显示编辑区域显示
        ui->pTAccept->appendPlainText("串口发："+str);//防止覆盖之前的消息
    }
}
void MainWindow::on_SerialPortreadyRead()
{
    //QByteArray recvData = serialport.readAll();
    QByteArray recvData = port->readAll();

    //ui->pTAccept->appendPlainText(recvData);
    ui->pTAccept->appendPlainText("串口收："+recvData);//防止覆盖之前的消息
}
void MainWindow::on_pBClear_clicked()
{
    ui->pTAccept->clear();
}





//按钮控制亮灭
void MainWindow::numFindLight()//数字那部分的按钮高亮可用
{


    ui->pBNum0->setEnabled(true);
    ui->pBNum1->setEnabled(true);
    ui->pBNum2->setEnabled(true);
    ui->pBNum3->setEnabled(true);
    ui->pBNum4->setEnabled(true);
    ui->pBNum5->setEnabled(true);
    ui->pBNum6->setEnabled(true);
    ui->pBNum7->setEnabled(true);
    ui->pBNum8->setEnabled(true);
    ui->pBNum9->setEnabled(true);

}

void MainWindow::numFindDark()//数字那部分的按钮变回不可用
{

    ui->pBNum0->setEnabled(false);
    ui->pBNum1->setEnabled(false);
    ui->pBNum2->setEnabled(false);
    ui->pBNum3->setEnabled(false);
    ui->pBNum4->setEnabled(false);
    ui->pBNum5->setEnabled(false);
    ui->pBNum6->setEnabled(false);
    ui->pBNum7->setEnabled(false);
    ui->pBNum8->setEnabled(false);
    ui->pBNum9->setEnabled(false);
}

void MainWindow::direConLight()//方向那部分的按钮高亮可用
{
    //ui->pBRun->setEnabled(true);
    //ui->pBStop->setEnabled(true);
    ui->pBLeft->setEnabled(true);
    ui->pBForward->setEnabled(true);
    ui->pBRight->setEnabled(true);
    ui->pBBackward->setEnabled(true);
}

void MainWindow::direConDark()//方向那部分的按钮变灰不可用
{
    //ui->pBRun->setEnabled(false);
    //ui->pBStop->setEnabled(false);
    ui->pBLeft->setEnabled(false);
    ui->pBForward->setEnabled(false);
    ui->pBRight->setEnabled(false);
    ui->pBBackward->setEnabled(false);
}



//方向控制
void MainWindow::on_pBForward_clicked()
{
    QString data="forward";
    QString send="f";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::on_pBBackward_clicked()
{
    QString data="back";
    QString send="b";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::on_pBLeft_clicked()
{
    QString data="left";
    QString send="l";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::on_pBRight_clicked()
{
    QString data="right";
    QString send="r";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::on_pBKeyBoard_clicked()
{
    if(true==IsKeyControl)
    {
        IsKeyControl=false;
        this->releaseKeyboard();
        ui->pBKeyBoard->setText("开键控");

        ui->pBSend->setEnabled(true);
    }
    else
    {
        IsKeyControl=true;
        ui->pBKeyBoard->setText("关键控");
        this->grabKeyboard();
        ui->pBSend->setEnabled(false);
    }
}

void MainWindow::on_pBStop_clicked()
{
    QString data="stop";
    QString send="s";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::on_pBRun_clicked()
{
    QString data="run";
    QString send="g";
    port->write(send.toUtf8());
    ui->pTAccept->appendPlainText("串口发："+data);
}
void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if(true==Mode&&true==mIsOpen){
    if(ev->key() == Qt::Key_Space)
    {
        if(lastPause==false){
            QString data="stop";
            QString send="s";
            port->write(send.toUtf8());
            ui->pTAccept->appendPlainText("串口发："+data);
            lastPause=true;
            return;
        }
        else{
            QString data="run";
            QString send="g";
            port->write(send.toUtf8());
            ui->pTAccept->appendPlainText("串口发："+data);
            lastPause=false;
            return;
            }

        }
    }
    else if(false==Mode&&true==mIsOpen&&true==IsKeyControl)
    {
        if(ev->key() == Qt::Key_Up)
        {
           QString data="forward";
           QString send="f";
           port->write(send.toUtf8());
           ui->pTAccept->appendPlainText("串口发："+data);
           return;
        }
        else if(ev->key() == Qt::Key_Down){
            QString data="back";
            QString send="b";
            port->write(send.toUtf8());
            ui->pTAccept->appendPlainText("串口发："+data);
            return;
        }
        else if(ev->key() == Qt::Key_Left){
            QString data="left";
            QString send="l";
            port->write(send.toUtf8());
            ui->pTAccept->appendPlainText("串口发："+data);
            return;
        }
        else if(ev->key() == Qt::Key_Right){
            QString data="right";
            QString send="r";
            port->write(send.toUtf8());
            ui->pTAccept->appendPlainText("串口发："+data);
            return;
        }
    }

}





//数字发送
void MainWindow::on_pBNum0_clicked()
{
    //获取发送的信息
    QString str="num0";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());

}

void MainWindow::on_pBNum1_clicked()
{
    //获取发送的信息
    QString str="num1";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum2_clicked()
{
    //获取发送的信息
    QString str="num2";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum3_clicked()
{
    //获取发送的信息
    QString str="num3";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum4_clicked()
{
    //获取发送的信息
    QString str="num4";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum5_clicked()
{
    //获取发送的信息
    QString str="num5";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum6_clicked()
{
    //获取发送的信息
    QString str="num6";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum7_clicked()
{
    //获取发送的信息
    QString str="num7";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum8_clicked()
{
    //获取发送的信息
    QString str="num8";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}

void MainWindow::on_pBNum9_clicked()
{
    //获取发送的信息
    QString str="num9";
    //将自己的信息显示在聊天窗口
    ui->pTAccept->appendPlainText("TCP发："+str);
    tcpsocket->write(str.toUtf8());
}




