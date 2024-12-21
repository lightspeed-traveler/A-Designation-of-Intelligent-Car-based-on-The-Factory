 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QUdpSocket>
#include<QTcpServer>//监听套接字
#include<QTcpSocket>//通信套接字

#include <QLabel>
#include <QMessageBox>
#include <QBuffer>
#include <QImageReader>


#define IMAGESIZE 921600
#define FRAMELENGTH  1024
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    bool getSerialPortConfig(void);
    void numFindLight(void);
    void numFindDark(void);
    void direConLight(void);
    void direConDark(void);

    void TcpConnect(void);
private slots:

    void receive(void);

    void on_pBOpenPort_clicked();

    void on_pBSend_clicked();

    void on_SerialPortreadyRead();

    void on_pBForward_clicked();

    void on_pBBackward_clicked();

    void on_pBLeft_clicked();

    void on_pBRight_clicked();

    void on_pBStop_clicked();

    void on_pBNum0_clicked();

    void on_pBRun_clicked();

    void on_pBNum1_clicked();

    void on_pBNum2_clicked();

    void on_pBNum3_clicked();

    void on_pBNum4_clicked();

    void on_pBNum5_clicked();

    void on_pBNum6_clicked();

    void on_pBNum7_clicked();

    void on_pBNum8_clicked();

    void on_pBNum9_clicked();

    void on_bPNet_clicked();

    void on_pBClear_clicked();

    void on_pBKeyBoard_clicked();

    void on_bPVideo_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort serialport;
    QSerialPort *port;
    //声明套接字

    QTcpSocket *tcpsocket;


    uchar imagebuffer[IMAGESIZE];
    int imagewidth, imageheight;
    QImage *image_receive;  //注意这里是指针形式
    int package_count;
    QString lastSend;


    bool mIsOpen;
    bool IsConnected;
    bool IsKeyControl;
    QString comMode;
    bool Mode;//true为自动，false为手动
    bool Ifvideo;
    bool lastPause;



    QString mPortName;
    QString mBaudRate;
    QString mParity;
    QString mDataBits;
    QString mStopBits;
protected:
    virtual void keyPressEvent(QKeyEvent *ev);
    //virtual void keyReleaseEvent(QKeyEvent *ev);
};
#endif // MAINWINDOW_H
