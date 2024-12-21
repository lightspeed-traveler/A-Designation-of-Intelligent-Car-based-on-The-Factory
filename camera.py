import cv2
import numpy as np

cap = cv2.VideoCapture(0)
cap.set(3, 160)
cap.set(4, 120)
while True:
    ret, frame = cap.read()
    high_b = np.uint8([10, 10, 10])
    low_b = np.uint8([0, 0, 0])
    mask = cv2.inRange(frame, low_b, high_b)  # 设阈值，高于high_b或低于low_b设为0，去除背景部分，中间部分设为255白色
    fb, lr, turn = 0, 0, 0
    kp = 0.3
    img, contours, hierarchy = cv2.findContours(mask, 1, cv2.CHAIN_APPROX_NONE)
    if len(contours) > 0:
        c = max(contours, key=cv2.contourArea)  # 是一个轮廓
        M = cv2.moments(c)  # 求图像的矩
        if M["m00"] != 0:
            cx = int(M['m10'] / M['m00'])  # 轮廓contour的质心的横坐标
            cy = int(M['m01'] / M['m00'])  # 轮廓contour的质心的纵坐标
            print("CX : " + str(cx) + "  CY : " + str(cy))
            fb = int((110-cy)*kp)
            turn = int((cx - 160 / 2) * kp)
            lr = -turn
        cv2.circle(frame, (cx, cy), 5, (255, 255, 255), -1)

    else:
        print("I don't see the line")
        fb = turn = 0

    #m.move(lr, fb, turn)
    cv2.drawContours(frame, c, -1, (0, 255, 0), 1)
    cv2.imshow("Mask", mask)
    cv2.imshow("Frame", frame)
    if cv2.waitKey(1) & 0xff == ord('q'):  # 1 is the time in ms
        m.MX_motorUnlockAll()
        break
cap.release()
cv2.destroyAllWindows()