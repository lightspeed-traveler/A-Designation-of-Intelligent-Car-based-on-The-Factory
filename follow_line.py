import cv2
import numpy as np
cap = cv2.VideoCapture(0)#, cv2.CAP_DSHOW
cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.25)
#assert cap.isOpened(),'Cannot capture source'
# 获取捕获的分辨率
# size =(int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)),
#       int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))

# 巡线行数
simple = [460,440,420,400,380,360]


def video_test():
    ret, frame = cap.read()
    try:
        cv2.imshow('1',frame)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    except Exception as e:
        print("open exception: %s: %s" %(e.errno, e.strerror))
    


# 计算n行中心线的平均值
def calc_aver(dst):
    n = simple.__len__()
    num=0
    direction = 0
    while n > 0:
        n = n-1
        # 单看第某行的像素值
        color = dst[simple[n]]
        # 找到黑色的像素点个数
        black_count = np.sum(color == 0)
        # 找到黑色的像素点索引
        black_index = np.where(color == 0)

        # # black_count=255的报错
        # if black_count == 255:
        #     black_count = 254
        if black_count != 0:   
            # 找到黑色像素的中心点位置
            center = (black_index[0][black_count - 1] + black_index[0][0]) / 2
            num +=1
            direction = direction + center
    if num == 0:
        return 0
    return direction/num


def show_image():
    ret, frame = cap.read()
    # img = cv2.imread('4.png', cv2.IMREAD_UNCHANGED)
    # 转化为灰度图
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 大津法二值化
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)
    # 膨胀，白区域变大
    # dst = cv2.dilate(dst, None, iterations=2)
    # # 腐蚀，白区域变小
    # dst = cv2.erode(dst, None, iterations=6)
    # 获取图像高度、宽度
    height, width = dst.shape[0:2]
    # print(width, height)
    cv2.imshow('image', dst)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def get_picture():
    ret, frame = cap.read()
    # 转化为灰度图
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 大津法二值化
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)
    # 膨胀，白区域变大
    dst = cv2.dilate(dst, None, iterations=1)
    # # 腐蚀，白区域变小
    dst = cv2.erode(dst, None, iterations=8)
    return frame,gray,dst
    # # 计算出center与标准中心点的偏移量
    # direction = center - 320
    #
    # print(direction)
    # # 释放清理
    # cap.release()
    # cv2.destroyAllWindows()
def get_picture1():
    ret, frame = cap.read()
    # 转化为灰度图
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 大津法二值化
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)
    # 膨胀，白区域变大
    dst = cv2.dilate(dst, None, iterations=1)
    # # 腐蚀，白区域变小
    dst = cv2.erode(dst, None, iterations=8)
    return dst
def get_picture2():
    ret, frame = cap.read()
    # 转化为灰度图
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 大津法二值化
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)
    # 膨胀，白区域变大
    dst = cv2.dilate(dst, None, iterations=1)
    # # 腐蚀，白区域变小
    dst = cv2.erode(dst, None, iterations=8)
    return gray,dst

def test_image():
    img = cv2.imread("2.jpg")
    img = cv2.resize(img ,(640,480),interpolation=cv2.INTER_AREA)
    #cv2.imshow("Image",img)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    #cv2.imshow("Gray",gray)
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)
    # cv2.imshow("Binary",dst)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
    return dst


def show_image1(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    #print(gray.shape)
    retval, dst = cv2.threshold(gray, 0, 255, cv2.THRESH_OTSU)#cv2.THRESH_OTSU
    #print(dst.shape)
    # 膨胀，白区域变大
    dst = cv2.dilate(dst, None, iterations=6)
    # 腐蚀，白区域变小
    dst = cv2.erode(dst, None, iterations=8)
    return img,gray,dst



 
#寻找黑色中线
def find_midline(dst):
    midline=[]
    height, width = dst.shape[0:2]
    for i in range(height-1,-1,-1):
        color = dst[i]
        black_count = np.sum(color == 0)
        black_index = np.where(color == 0)
        if black_count != 0: 
            center = int((black_index[0][black_count - 1] + black_index[0][0]) / 2)
            midline.append((center,i))
            #print(midline[0])
        else:
            if i<470:
                # midline.append((int(midline[478-i][0]+(midline[478-i][0]-midline[475-i][0])/3),i))
                midline.append(((2*midline[478-i][0]-midline[477-i][0]),i))
                #midline.append((int((midline[i+1][0]+midline[i+2][0]+midline[i+3][0]+midline[i+4][0]+midline[i+5][0])/5),i))
            else:
                midline.append((320,i))
    return midline


#在原图上画出寻找到的黑线
def draw_line(img,midline):
    for point in midline:
        cv2.circle(img, point, 3, (0, 255, 0), 2)
    return img

#根据当前车身位置计算出一个合适数值反馈给单片机
def cal_bias(midline):
    bot_bias=0
    top_bias=0
    break_line=479
    for i in range(len(midline)):
        if (midline[i][0]>0 and midline[i][0]<640):
            if i<280:
                bot_bias+=midline[i+1][0]-midline[i][0]
            else:
                top_bias+=midline[i][0]-midline[i-1][0]
        else:
            break_line=i
            break
    if bot_bias>0:
        bias=int(bot_bias*0.3+top_bias*0.2+(479-break_line)*0.3+0.4*(midline[0][0]-320))
    else:
        bias=int(bot_bias*0.3+top_bias*0.2-(479-break_line)*0.3+0.4*(midline[0][0]-320))
    return bias
def close_camera(void):
     cap.release()
                