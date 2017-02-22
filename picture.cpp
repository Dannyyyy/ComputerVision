#include <QImage>
#include <QRgb>
#include <picture.h>
#include <picturefilter.h>
#include <math.h>

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
    this->content[(this->height * y) + x] = (0.213*redColor + 0.715*greenColor + 0.072*blueColor)/255.;
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
                for (int dX = 0; dX < pictureFilterContent.width; dX++) {
                    for (int dY = 0; dY < pictureFilterContent.height; dY++) {
                        resultIntensity += this->getIntensity(x - dX + pictureFilterContent.width / 2, y - dY + pictureFilterContent.height / 2)
                                  * pictureFilterContent.content[(dY * pictureFilterContent.width) + dX];
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

unique_ptr<Picture> Picture::calculationGradient(const Picture &sobelX, const Picture &sobelY){
    const int height = sobelX.height;
    const int width = sobelX.width;
    auto resultPicture = make_unique<Picture>(height,width);
    for(int x = 0;x<height;x++)
    {
        for(int y=0;y<width;y++)
        {
            double sobelXIntensity = sobelX.content[(height * y) + x];
            double sobelYIntensity = sobelY.content[(height * y) + x];
            double resultIntensity = sqrt(pow(sobelXIntensity,2)+pow(sobelYIntensity,2));
            resultPicture->setIntensity(x, y, resultIntensity);
        }
    }
    return resultPicture;
}

void Picture::pictureNormalize(){
    const int height = this->getHeight();
    const int width = this->getWidth();
    auto minIntensity = min_element(&this->content[0], &this->content[height*width]);
    auto maxIntensity = max_element(&this->content[0], &this->content[height*width]);
    double normalizeValue = *maxIntensity - *minIntensity;
    if(normalizeValue == 0){
        normalizeValue = 1;
    }
    for(int x = 0;x<height;x++)
    {
        for(int y=0;y<width;y++)
        {
            double intensity = this->getIntensity(x,y);
            intensity = (intensity - *minIntensity)/normalizeValue;
            this->setIntensity(x, y, intensity);
        }
    }
}
