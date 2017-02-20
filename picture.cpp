#include <QImage>
#include <QRgb>
#include <picture.h>
#include <picturefilter.h>

Picture::Picture(){
    this->width = 0;
    this->height = 0;
    this->content = nullptr;
}

Picture::Picture(const int height, const int width){
    this->width = width;
    this->height = height;
    this->content = make_unique<double []>(height * width);
}

void Picture::setIntensity(const int x, const int y, const double intensity){
    this->content[(this->height * y) + x] = intensity;
}

void Picture::setIntensity(const int x, const int y, const int redColor, const int greenColor, const int blueColor){
    this->content[(this->height * y) + x] = (0,213*redColor + 0,715*greenColor + 0,072*blueColor)/255;
}

double Picture::getIntensity(const int x, const int y){
    // все "снаружи" нулевые(черные)
    if(y < 0 || y >= this->width || x < 0 || x >= this->height)
    {
        return 0;
    }
    return this->content[(this->height * y) + x];
}

QImage Picture::getImage(){
    const int height = this->getHeight();
    const int width = this->getWidth();
    QImage image = QImage(width, height, QImage::Format_RGB32);
    for (int x = 0; x < height; x++) {
       for (int y = 0; y < width; y++) {
               int intensity = (int) (this->getIntensity(x, y) * 255);
               image.setPixel(y, x, qRgb(intensity, intensity, intensity));
       }
    }
    return image;
}

unique_ptr<Picture> Picture::useFilter(const PictureFilterContent &pictureFilterContent){
    const int height = this->getHeight();
    const int width = this->getWidth();
    auto resultPicture = make_unique<Picture>(height, width);
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                double resultIntensity = 0;
                for (int pictureFilterContentPositionX = 0; pictureFilterContentPositionX < pictureFilterContent.width; pictureFilterContentPositionX++) {
                    for (int pictureFilterContentPositionY = 0; pictureFilterContentPositionY < pictureFilterContent.height; pictureFilterContentPositionY++) {
                        resultIntensity += this->getIntensity(x - pictureFilterContentPositionX + pictureFilterContent.width / 2, y - pictureFilterContentPositionY + pictureFilterContent.height / 2)
                                  * pictureFilterContent.content[(pictureFilterContentPositionY * pictureFilterContent.width) + pictureFilterContentPositionX];
                    }
                }
                resultPicture->setIntensity(x, y, resultIntensity);
            }
        }
        return resultPicture;
}

void Picture::saveImage(QString fileName){
    QImage image = this->getImage();
    image.save("C:\\AGTU\\pictures\\" + fileName + ".jpg", "jpg");
}
