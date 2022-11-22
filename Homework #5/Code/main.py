import os
from PIL import Image
import numpy as np
from PyQt6 import uic
from PyQt6.QtGui import *
from PyQt6.QtWidgets import *

#UI파일 연결
form_class = uic.loadUiType("frame.ui")[0]

def load_image(path) :
    with Image.open(path) as img:
        img.load()
        img.mode = "RGBA" # 4 bytes per pixel
        data = np.asarray(img, dtype=np.uint8) # read the data into a numpy array
    return data

def save_image(img, path):
    with Image.fromarray(img) as im:
        im.save(path)
        print("Image saved to {}".format(path))

def convert2qtPixmap(img):
    h, w, num_channels = img.shape
    assert num_channels == 4
    bytesPerLine = num_channels * w
    return QPixmap(QImage(img.data, w, h, bytesPerLine, QImage.Format.Format_RGBA8888))

def upscaleImage(img, scale):
    assert scale >= 1
    h, w, num_channels = img.shape
    assert num_channels == 4
    new_h, new_w = int(scale * h), int(scale * w)
    
    # Vectorized Code
    new_img = np.arange(new_h * new_w * num_channels, dtype=np.uint)
    fimg = img.flatten()
    i = (new_img // (new_w * num_channels)).astype(np.uint)
    j = (new_img % (new_w * num_channels) // num_channels).astype(np.uint)
    c = (new_img % num_channels).astype(np.uint)
    y = (i // scale).astype(np.uint)
    x = (j // scale).astype(np.uint)
    ny = np.clip(y + 1, 0, h - 1).astype(np.uint)
    nx = np.clip(x + 1, 0, w - 1).astype(np.uint)
    ltop = fimg[y * w * num_channels + x * num_channels + c]
    rtop = fimg[y * w * num_channels + nx * num_channels + c]
    lbot = fimg[ny * w * num_channels + x * num_channels + c]
    rbot = fimg[ny * w * num_channels + nx * num_channels + c]
    vertical_ratio = i / scale - y
    horizontal_ratio = j / scale - x
    l = ltop * (1 - vertical_ratio) + lbot * vertical_ratio
    r = rtop * (1 - vertical_ratio) + rbot * vertical_ratio
    new_img = l * (1 - horizontal_ratio) + r * horizontal_ratio
    new_img = np.clip(new_img, 0, 255).astype(np.uint8)
    new_img = new_img.reshape(new_h, new_w, num_channels)
    
    # Sequential Code
    # new_img = np.ones((new_h, new_w, num_channels), dtype=np.uint8)
    # for i in range(new_h): # 0 <= i < scale * h
    #     for j in range(new_w): # 0 <= j < scale * w
    #         y, x = i // scale, j // scale # 0 <= y < h, 0 <= x < w
    #         ltop = img[y, x, channels]
    #         rtop = img[y, x + 1, channels] if x + 1 < w else ltop
    #         lbot = img[y + 1, x, channels] if y + 1 < h else ltop
    #         rbot = img[y + 1, x + 1, channels] if x + 1 < w and y + 1 < h else ltop
    #         vertical_ratio = i / scale - y
    #         horizontal_ratio = j / scale - x
    #         l = ltop * (1 - vertical_ratio) + lbot * vertical_ratio
    #         r = rtop * (1 - vertical_ratio) + rbot * vertical_ratio
    #         new_img[i, j, channels] = np.clip(0, 255, l * (1 - horizontal_ratio) + r * horizontal_ratio).astype(np.uint8)
    return new_img

#화면을 띄우는데 사용되는 Class 선언
class WindowClass(QDialog, form_class) :
    def __init__(self) :
        super().__init__()
        self.setupUi(self)

        self.home_path = os.getenv('USERPROFILE')

        #버튼에 기능을 연결하는 코드
        self.image_label.mousePressEvent = self.changeLabelFunction

    # image_label이 눌리면 작동할 함수
    def changeLabelFunction(self, event) :
        dlg = QFileDialog.getOpenFileName(
            self,
            'Open file',
            self.home_path,
            "Images (*.png *.pbm *.pgm *.ppm *.xbm *.xpm *.jpg *.jpeg *.bmp *.gif)"
        )
        img_path = str(dlg[0])
        image = load_image(img_path)
        fdir = os.path.dirname(img_path)
        fbasename = os.path.basename(img_path)
        bname, ext = fbasename.split(".")
        new_name = os.path.join(fdir, "upscaled_" + bname + ".webp")
        print(fdir, fbasename, new_name)
        converted_img = upscaleImage(image, 3.0)
        save_image(converted_img, new_name)
        self.qPixmap = convert2qtPixmap(converted_img)
        
        w, h = self.qPixmap.size().width(), self.qPixmap.size().height()
        print(w, h)
        r = w / h
        max_w = 370
        if max(w, h) > max_w:
            if w < h:
                h = max_w
                w = h * r
            else:
                w = max_w
                h = w / r
        w, h = int(w), int(h)
        self.image_label.setPixmap(self.qPixmap.scaled(w, h))

if __name__ == "__main__" :
    #QApplication : 프로그램을 실행시켜주는 클래스
    app = QApplication([]) 

    #WindowClass의 인스턴스 생성
    myWindow = WindowClass() 

    #프로그램 화면을 보여주는 코드
    myWindow.show()

    #프로그램을 이벤트루프로 진입시키는(프로그램을 작동시키는) 코드
    app.exec()