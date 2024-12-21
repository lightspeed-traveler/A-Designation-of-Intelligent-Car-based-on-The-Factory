# -*- coding:utf-8 -*-
import serial
import time
#UART2： GPIO0  = TXD2 -> ttyAMA1     GPIO1  = RXD2 -> ttyAMA1
ser = serial.Serial("/dev/ttyAMA1", 115200,timeout=0.01)  #使用树莓派的GPIO口连接串行口


def send_data(bias):
    if ser.isOpen()==False:
        ser.open()  # 打开端口
    ser.write(bias.encode("utf-8"))
    #ser.write(1("gbk"))
    # ser.flushOutput() #终止当前写操作，并丢弃发送缓存中的数据。
    #count = ser.inWaiting() 
    # if count != 0:
    #     recv = ser.read(count)  # 位置5
    #     # ser.write("Recv some data is : ".encode("utf-8"))  # 位置6
    #     # ser.write(recv)  # 位置7
    #     ser.flushInput()
    #time.sleep(0.1)  # 位置8
    #ser.close()  # 关闭端口


def accept_data():
    if ser.isOpen()==False:
        ser.open()  # 打开端口
    data = ser.read(2) #是读2个字符
    return data.decode("utf-8",'ignore')
 


def uart_test():
    data=accept_data()
    if data is not None:
        print(data)
        #send_data(data)
    else:
        print("no data!")


def close_uart():
    ser.close()#关闭端口