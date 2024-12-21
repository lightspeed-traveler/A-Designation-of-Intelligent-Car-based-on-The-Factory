# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import cv2
import follow_line as fl
import uart 
import time
import socket
import numpy as np
import threading
import inspect
import ctypes
import deep_number as du



IMAGESIZE = 921600
IMAGEWIDTH = 640
IMAFEHEIGHT = 480
FRAMELENGTH = 1024
Isconnected=False

# 多线程接收数据
# socket接收为阻塞接收方式，阻断程序运行
# 用多线程的方式来避免发生阻塞
class Receive_Thread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.return_value = "no"
        self.value = "no"
    def run(self):
        while True:
            # 每次接收一个字节的数据
            self.value = conn.recv(30).decode('utf-8')

    # 返回接收到的数据
    def get_value(self):
        if self.return_value!=self.value:
            self.return_value=self.value
            return self.return_value
        else:
            return "no"
            
#关闭线程
def _async_raise(tid, exctype):

    """raises the exception, performs cleanup if needed"""

    tid = ctypes.c_long(tid)

    if not inspect.isclass(exctype):

        exctype = type(exctype)

    res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(exctype))

    if res == 0:

        raise ValueError("invalid thread id")

    elif res != 1:

        # """if it returns a number greater than one, you're in trouble,

        # and you should call it again with exc=NULL to revert the effect"""

        ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)

        raise SystemError("PyThreadState_SetAsyncExc failed")

def stop_thread(thread):

    _async_raise(thread.ident, SystemExit)


     #拍照保存图片
def save_image(image):
    shoot_num=1#拍照数量
    shoot_time=1#拍照时间间隔，单位秒
    for i in range(shoot_num):
        time.sleep(shoot_time)
        cv2.imwrite(str(i+1)+".jpg", image)

# if __name__ == '__main__':
#     pocess_time=5 #程序运行时间，单位为秒
#     cir_times=0 #程序循环次数
#     start=time.time()
#     end=start
#     while True:#int(end-start)<pocess_time:
#         #cir_times+=1
#         #end=time.time()
#         #uart.uart_test()
#         # 得到处理后二值化图像
#         image,gray,dst = fl.get_picture()
#         #拍照保存图片
#         # save_image(image)
#         # break
#         #寻找中线
#         midline=fl.find_midline(dst)
#         # image=fl.draw_line(image,midline)
#         #计算偏差
#         bias=fl.cal_bias(midline)
#         #cv2.imwrite(str(7)+".jpg", image)
#         uart.send_data(str(bias))#
#         #data=uart.accept_data()
#         # binary = fl.test_image()
#         # ave_line = fl.calc_aver(binary)
#         #print(ave_line)
#         # print(bias)
#         # cv2.imshow("Image", image)
#         # cv2.imshow("Dst", dst)
#         # cv2.waitKey(0)
#         # cv2.destroyAllWindows()
#     # print(cir_times)
#     fl.close_camera()
        

# # Press the green button in the gutter to run the script.
# if __name__ == '__main__':
#     # 创建服务器
#     server = socket.socket()
#     # 获取到本机IP
#     # PCname = socket.gethostname()
#     #IP = socket.gethostbyname(PCname)
#     IP="192.168.43.215"
#     print(IP)
#     # 设置IP和端口号
#     server.bind((IP, 6666))
#     server.listen(1)

#     print('connecting...')
#     while True:
#         if False==Isconnected:
#             conn, addr = server.accept()
#             print("连接地址: %s" % str(addr))
#             Isconnected=True    
#         # 创建数据接收线程
#         rec_thread = Receive_Thread()
#         rec_thread.start()
#         while True:
#             image,dst = fl.get_picture()
#             midline=fl.find_midline(dst)
#             bias=fl.cal_bias(midline)
#             uart.send_data(str(bias))
#             #print(bias)
#             # 数据类型为uint8
#             framed = cv2.resize(image, (IMAGEWIDTH, IMAFEHEIGHT))
#             framed = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
#             has_sent = 0
#             rec_data = rec_thread.get_value()
#             if rec_data=="Send"or rec_data=="Send1" or rec_data=="Send2":
#                 # 发送图片，每次发送1024字节
#                 while has_sent < IMAGESIZE:
#                     data_to_send = framed[has_sent: has_sent+FRAMELENGTH]
#                     conn.send(data_to_send)
#                     has_sent += FRAMELENGTH
#                 #cv2.waitKey(30)
#                 # 打印接收到的控制指令
#             else:
#                 if rec_data != "no":
#                     if rec_data=="Disconnect":
#                         Isconnected=False
#                         print("断开连接")
#                         stop_thread(rec_thread)
#                         conn.close()
#                         break
#                     else:
#                         print(rec_data)     




prediction=11
Findingnum=10
# 复杂模型小车运动中识别数字，检测到目标数字停下
if __name__ == '__main__':
    # 创建服务器
    server = socket.socket()
    # 获取到本机IP
    # PCname = socket.gethostname()
    # IP = socket.gethostbyname(PCname)
    IP="192.168.43.215"
    print(IP)
    # 设置IP和端口号
    server.bind((IP, 6666))
    server.listen(1)

    print('connecting...')
    while True:
        if False==Isconnected:
            conn, addr = server.accept()
            print("连接地址: %s" % str(addr))
            Isconnected=True    
        # 创建数据接收线程
        rec_thread = Receive_Thread()
        rec_thread.start()
        while True:
            image,gray,dst = fl.get_picture()#获得原、灰度、二值化图片
            num_ROI,x, y, w, h=du.find_number(gray,dst)#找到图片中的数字和位置
            if num_ROI.size==0 or np.max(num_ROI) is None or h==0 or w==0:
                exist_number=False
            else:
                exist_number=True
                dst=du.remove_num(dst,x, y, w, h)#移除二值化图片中的数字以方便寻线
                prediction=du.recon_number(num_ROI)
                image=du.rec_number(image,x, y, w, h, prediction)
            midline=fl.find_midline(dst)
            image=fl.draw_line(image,midline)
            bias=fl.cal_bias(midline)
            uart.send_data(str(bias))
            #print(bias)
            # 数据类型为uint8
            framed = cv2.resize(image, (IMAGEWIDTH, IMAFEHEIGHT))
            framed = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            has_sent = 0
            rec_data = rec_thread.get_value()
            if rec_data=="Send"or rec_data=="Send1" or rec_data=="Send2":
                # 发送图片，每次发送1024字节
                while has_sent < IMAGESIZE:
                    data_to_send = framed[has_sent: has_sent+FRAMELENGTH]
                    conn.send(data_to_send)
                    has_sent += FRAMELENGTH
                #cv2.waitKey(30)
                # 打印接收到的控制指令
            else:
                if rec_data != "no":
                    if rec_data=="Disconnect":
                        Isconnected=False
                        print("断开连接")
                        stop_thread(rec_thread)
                        conn.close()
                        break
                    else:
                        Findingnum=du.match_number(rec_data)
                        print(rec_data)   
                        if(Findingnum==prediction):
                            uart.send_data(str(666))  