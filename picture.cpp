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
    this->content[(this->height * y) + x] = (0.213*redColor + 0.715*greenColor + 0.072*blueColor)/255;
}

double Picture::getIntensity(const int x, const int y) const {
    // все "снаружи" нулевые(черные)
    if(y < 0 || y >= this->width || x < 0 || x >= this->height)
    {
        return 0;
    }
    return this->content[(this->height * y) + x];
}

double Picture::getIntensity(const int x, const int y, BorderMode borderMode) const {
    switch(borderMode){
        case BorderMode::OutsideBlack : return this->getOutsideBlack(x,y);
                                        break;
        case BorderMode::CopyBorderValue: return this->getCopyBoarderValue(x,y);
                                        break;
        case BorderMode::ReflectBorderValue: return this->getReflectBoarderValue(x,y);
                                        break;
        case BorderMode::WrapPicture: return this->getWrapPicture(x,y);
                                    break;
        default: this->getOutsideBlack(x,y);
                break;
    }
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

unique_ptr<Picture> Picture::useFilter(const PictureFilterContent &pictureFilterContent, BorderMode borderMode){
    const int heightPicture = this->getHeight();
    const int widthPicture = this->getWidth();

    const int widthFilter = pictureFilterContent.getWidth();
    const int heightFilter = pictureFilterContent.getHeight();

    const int centerWidthFilter = widthFilter / 2;
    const int centerHeightFilter = heightFilter / 2;

    auto resultPicture = make_unique<Picture>(heightPicture, widthPicture);
        for (int x = 0; x < heightPicture; x++) {
            for (int y = 0; y < widthPicture; y++) {
                double resultIntensity = 0;
                for (int dX = 0; dX < widthFilter; dX++) {
                    for (int dY = 0; dY < heightFilter; dY++) {
                        resultIntensity += this->getIntensity(x - dX + centerWidthFilter, y - dY + centerHeightFilter, borderMode)
                                  * pictureFilterContent.getContentCell(dX,dY);
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
    const int height = sobelX.getHeight();
    const int width = sobelX.getWidth();
    auto resultPicture = make_unique<Picture>(height,width);
    for(int x = 0;x<height;x++)
    {
        for(int y=0;y<width;y++)
        {
            double sobelXIntensity = sobelX.getIntensity(x,y);
            double sobelYIntensity = sobelY.getIntensity(x,y);
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
    if(normalizeValue != 0.) {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = this->getIntensity(x,y);
                double normalizeIntensity = (intensity - *minIntensity)/normalizeValue;
                this->setIntensity(x, y, normalizeIntensity);
            }
        }
    }
}

unique_ptr<Picture> Picture::getPictureNormalize(){
    const int height = this->getHeight();
    const int width = this->getWidth();
    auto minIntensity = min_element(&this->content[0], &this->content[height*width]);
    auto maxIntensity = max_element(&this->content[0], &this->content[height*width]);
    double normalizeValue = *maxIntensity - *minIntensity;
    auto resultPicture = make_unique<Picture>(height,width);
    if(normalizeValue != 0.) {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = this->getIntensity(x,y);
                double normalizeIntensity = (intensity - *minIntensity)/normalizeValue;
                resultPicture->setIntensity(x, y, normalizeIntensity);
            }
        }

    }
    else
    {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = this->getIntensity(x,y);
                resultPicture->setIntensity(x, y, intensity);
            }
        }
    }
    return resultPicture;
}

double Picture::getOutsideBlack(const int x, const int y) const {
    if(y < 0 || y >= this->width || x < 0 || x >= this->height)
    {
        return 0;
    }
    else
    {
        return this->getIntensity(x,y);
    }
}

double Picture::getCopyBoarderValue(const int x, const int y) const {
    const int maxY = max(0, y);
    const int resultY = min(maxY,this->getWidth()-1);
    const int maxX = max(0, x);
    const int resultX = min(maxX,this->getHeight()-1);
    return this->getIntensity(resultX, resultY);
}

double Picture::getReflectBoarderValue(const int x, const int y) const {
    int resultX = 0;
    int resultY = 0;
    int const width = this->getWidth();
    if(y>=width)
    {
        resultY = 2*(width - 1) - y;
    }
    else
    {
        resultY = abs(y);
    }
    int const height = this->getHeight();
    if(x>=height)
    {
        resultX = 2*(height - 1) - x;
    }
    else
    {
        resultX = abs(x);
    }
    return this->getIntensity(resultX, resultY);
}

double Picture::getWrapPicture(const int x, const int y) const {
    int resultX = 0;
    int resultY = 0;
    int const width = this->getWidth();
    if(y>=width || y<0)
    {
        if(y>=width)
        {
            resultY = y - width;
        }
        else
        {
            resultY = width - 1 - abs(y);
        }

    }
    else
    {
        resultY = y;
    }
    if(x>=height || x<0)
    {
        if(x>=height)
        {
            resultX = x - height;
        }
        else
        {
            resultX = height - 1 - abs(x);
        }

    }
    else
    {
        resultX = x;
    }
    return this->getIntensity(resultX, resultY);
}


