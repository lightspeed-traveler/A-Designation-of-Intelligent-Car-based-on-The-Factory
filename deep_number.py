import cv2
import numpy as np

import torch
from torch import nn
from torchvision import transforms

#将圈出数字的框重置为近似方框
def reshape_rec(x,y,w,h):
    if w<=h:
        h*=1.3
        x-=int((h-w)/2)
        w=h
        y-=int(h*0.1)
    else:
        w*=1.3
        y-=int((w-h)/2)
        h=w
        x-=int(w*0.1)
    return x,y,int(w),int(h)

#在多个轮廓中找到符合数字大小的那个
def find_number(gray,dst):
    contours, hierarchy = cv2.findContours(dst,cv2.RETR_CCOMP,cv2.CHAIN_APPROX_SIMPLE)
    alternative=[]
    for i in range(len(contours)):
        if cv2.contourArea(contours[i])>4000 and cv2.contourArea(contours[i])<13000:
            alternative.append(contours[i])
    if len(alternative)==0:
        return gray[0:0,0:0],0,0,0,0
    else:
        cnt=alternative[0]
        if len(alternative)>1:
           for i in range(len(alternative)-1):
                if abs(cv2.arcLength(alternative[i+1],True)-700)<abs(cv2.arcLength(alternative[i],True)-700):
                    cnt=alternative[i+1] 
        x, y, w, h = cv2.boundingRect(cnt)
        x, y, w, h=reshape_rec(x,y,w,h)
        number=gray[y:y+h,x:x+w]#提取出灰度图中数字所在ROI
        return number,x, y, w, h    
#将数字在二值化图像中移除
def remove_num(dst,x, y, w, h ):
    dst[y:y+h,x:x+w]=255 #二值化图片中去掉数字
    return dst

def recon_number(number):
    number=cv2.resize(number,(224,224))#重置图片大小
    number=255-number#灰度图黑白反转匹配训练模型
    transf = transforms.ToTensor()#把图片转成成torch的tensor数据
    gray_tensor = transf(number)  # tensor数据格式是torch(C,H,W)   
    gray_tensor=torch.reshape(gray_tensor,((1,1,224,224)))
    device = torch.device('cpu')
    model  = torch.load('/home/traveler/Desktop/My_Project/AlexNet.pth',map_location=device)
    model.eval()#不做梯度计算

    #device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    inputs = gray_tensor.to(device)
    with torch.no_grad():
        output = model(inputs)
        _, prediction = torch.max(output, 1)
        prediction = prediction.cpu().numpy()[0]
    return prediction

#彩色图片中框出数字,并写上预测值
def rec_number(img,x, y, w, h, prediction):
    cv2.rectangle(img, (x, y), (x+w, y+h), (148, 0, 211), 2)
    cv2.putText(img,str(prediction),(x+int(w/2),y-10),cv2.FONT_HERSHEY_COMPLEX, 2.0, (100, 200, 200), 5)
    return img
#将TCp命令接受的数据匹配成模型翻译出来的数字
def match_number(socRec):
    number=10
    if socRec=="num0":
        number=0
    elif socRec=="num1":
        number=1
    elif socRec=="num2":
        number=2
    elif socRec=="num3":
        number=3
    elif socRec=="num4":
        number=4
    elif socRec=="num5":
        number=5
    elif socRec=="num6":
        number=6
    elif socRec=="num7":
        number=7
    elif socRec=="num8":
        number=8
    elif socRec=="num9":
        number=9
    return number
    



class model(nn.Module):
    def __init__(self):
        super(model,self).__init__()
        self.net = nn.Sequential(
        self.conv_layer(1, 32),    # 32 to 16
        self.conv_layer(32, 64),   # 16 to 8
        self.conv_layer(64, 128),  # 8 to 4
        self.conv_layer(128, 128), # 4 to 2
        # 将数据展平
        nn.Flatten(),
        nn.Linear(128  * 4 * 4, 2048),
        nn.LeakyReLU(0.2),
        nn.Linear(2048, 4096),
        nn.LeakyReLU(0.2),
        nn.Linear(4096, 10)
        )
    def conv_layer(self, in_channels, out_channels):
        return nn.Sequential(
        # 卷积层只提取细节，不减少图像尺寸
        nn.Conv2d(in_channels, out_channels,kernel_size=3, stride=1,padding=1),
        # 批量归一化，防止过拟合与梯度爆炸、消失
        nn.BatchNorm2d(out_channels),
        # 最大池化，增加模型稳定性，减少数据
        nn.MaxPool2d(kernel_size=2,stride=2),
        # Relu激活函数
        nn.ReLU()
    ) 
    def forward(self, x):
        return self.net(x)
  


class Discriminator():
    def __init__(self):
        self.net = model()
        self.state_dict = torch.load('noise-net1r.pth',map_location='cpu')
        self.net.load_state_dict(self.state_dict)
        self.compose = [
        transforms.ToPILImage(),
        # transforms.RandomRotation(90),
        transforms.Resize((64, 64)),
        transforms.ToTensor(),
        transforms.Normalize(mean=(0), std=(1))
        ]
        self.trans = transforms.Compose(self.compose)
        
    def __call__(self, X):
        self.net.eval()
        X = self.trans(X)
        X = X.view(-1, 1, 64, 64)
        y_hat = self.net(X)
        return y_hat.argmax(axis=1)
    
def recon_number1(number):
    number=255-number#灰度图黑白反转匹配训练模型
    D=Discriminator()
    retval, number = cv2.threshold(number, 0, 255, cv2.THRESH_OTSU)#cv2.THRESH_OTSU
    prediction=D(number).cpu().numpy()[0]
    # print(prediction)
    return prediction