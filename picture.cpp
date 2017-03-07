#include <QImage>
#include <QRgb>
#include <picture.h>
#include <picturefilter.h>
#include <math.h>

Picture::Picture(): width(0),height(0),content(nullptr) {}

Picture::Picture(const int height, const int width): width(width), height(height){
    content = make_unique<double []>(height * width);
}

Picture::Picture(const Picture &picture): width(picture.getWidth()), height(picture.getHeight()){
    content = make_unique<double []>((width * height));
    for (int i = 0; i < width * height; ++i)
        content[i] = picture.content[i];
}

void Picture::setIntensity(const int x, const int y, const double intensity){
    this->content[(height * y) + x] = intensity;
}

void Picture::setIntensity(const int x, const int y, const int redColor, const int greenColor, const int blueColor){
    this->content[(height * y) + x] = (0.213*redColor + 0.715*greenColor + 0.072*blueColor)/255;
}

double Picture::getIntensity(const int x, const int y) const {
    return content[(height * y) + x];
}

double Picture::getIntensity(const int x, const int y, BorderMode borderMode) const {
    switch(borderMode){
        case BorderMode::OutsideBlack : return getOutsideBlack(x,y); break;
        case BorderMode::CopyBorderValue: return getCopyBoarderValue(x,y); break;
        case BorderMode::ReflectBorderValue: return getReflectBoarderValue(x,y); break;
        case BorderMode::WrapPicture: return getWrapPicture(x,y); break;
        default: getOutsideBlack(x,y);
    }
}

QImage Picture::getImage() const{
    const int height = getHeight();
    const int width = getWidth();
    QImage image = QImage(width, height, QImage::Format_RGB32);
    for (int x = 0; x < height; x++) {
       for (int y = 0; y < width; y++) {
               int intensity = (int)(getIntensity(x, y) * 255);
               image.setPixel(y, x, qRgb(intensity, intensity, intensity));
       }
    }
    return image;
}

void Picture::saveImage(QString filePath) const{
    QImage image = getImage();
    image.save(filePath + ".jpg", "jpg");
}

Picture Picture::scalePicture() const{
    const int halfHeight = getHeight()/2;
    const int halfWidth = getWidth()/2;

    auto resultPicture = Picture(halfHeight, halfWidth);
    for(int x=0;x<halfHeight;x++){
        for(int y=0;y<halfWidth;y++){
            double scaleIntensity = (getIntensity(x*2,y*2,BorderMode::ReflectBorderValue)+
                                        getIntensity(x*2,y*2+1,BorderMode::ReflectBorderValue)+
                                        getIntensity(x*2+1,y*2,BorderMode::ReflectBorderValue)+
                                        getIntensity(x*2+1,y*2+1,BorderMode::ReflectBorderValue))/4.;
            resultPicture.setIntensity(x,y,scaleIntensity);
        }
    }
    return resultPicture;
}

Picture Picture::useFilter(const PictureFilterContent &pictureFilterContent, BorderMode border) const{
    const int heightPicture = getHeight();
    const int widthPicture = getWidth();

    const int widthFilter = pictureFilterContent.getWidth();
    const int heightFilter = pictureFilterContent.getHeight();

    const int centerWidthFilter = widthFilter / 2;
    const int centerHeightFilter = heightFilter / 2;

    auto resultPicture = Picture(heightPicture, widthPicture);
        for (int x = 0; x < heightPicture; x++) {
            for (int y = 0; y < widthPicture; y++) {
                double resultIntensity = 0;
                for (int dX = 0; dX < widthFilter; dX++) {
                    for (int dY = 0; dY < heightFilter; dY++) {
                        auto X = x - dX + centerWidthFilter;
                        auto Y = y - dY + centerHeightFilter;
                        resultIntensity += getIntensity(X, Y, border) * pictureFilterContent.getContentCell(dX,dY);
                    }
                }
                resultPicture.setIntensity(x, y, resultIntensity);
            }
        }
        return resultPicture;
}

Picture Picture::useTwoFilter(const PictureFilterContent &fFilter, const PictureFilterContent &sFilter, BorderMode border) const{
    auto resultPicture = useFilter(fFilter, border);
    resultPicture = resultPicture.useFilter(sFilter, border);
    return resultPicture;
}


Picture Picture::calculationGradient(const Picture &sobelX, const Picture &sobelY){
    const int height = sobelX.getHeight();
    const int width = sobelX.getWidth();
    auto resultPicture = Picture(height,width);
    for(int x = 0;x<height;x++)
    {
        for(int y=0;y<width;y++)
        {
            double sobelXIntensity = sobelX.getIntensity(x,y);
            double sobelYIntensity = sobelY.getIntensity(x,y);
            double resultIntensity = sqrt(pow(sobelXIntensity,2)+pow(sobelYIntensity,2));
            resultPicture.setIntensity(x, y, resultIntensity);
        }
    }
    return resultPicture;
}

void Picture::pictureNormalize(){
    const int height = getHeight();
    const int width = getWidth();
    auto minIntensity = min_element(&content[0], &content[height*width]);
    auto maxIntensity = max_element(&content[0], &content[height*width]);
    double normalizeValue = *maxIntensity - *minIntensity;
    if(normalizeValue != 0.) {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = getIntensity(x,y);
                double normalizeIntensity = (intensity - *minIntensity)/normalizeValue;
                this->setIntensity(x, y, normalizeIntensity);
            }
        }
    }
}

Picture Picture::getPictureNormalize() const{
    const int height = getHeight();
    const int width = getWidth();
    auto minIntensity = min_element(&content[0], &content[height*width]);
    auto maxIntensity = max_element(&content[0], &content[height*width]);
    double normalizeValue = *maxIntensity - *minIntensity;
    auto resultPicture = Picture(height,width);
    if(normalizeValue != 0.) {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = getIntensity(x,y);
                double normalizeIntensity = (intensity - *minIntensity)/normalizeValue;
                resultPicture.setIntensity(x, y, normalizeIntensity);
            }
        }

    }
    else
    {
        for(int x = 0;x<height;x++)
        {
            for(int y=0;y<width;y++)
            {
                double intensity = getIntensity(x,y);
                resultPicture.setIntensity(x, y, intensity);
            }
        }
    }
    return resultPicture;
}

double Picture::getOutsideBlack(const int x, const int y) const {
    if(y < 0 || y >= width || x < 0 || x >= height)
    {
        return 0;
    }
    else
    {
        return getIntensity(x,y);
    }
}

double Picture::getCopyBoarderValue(const int x, const int y) const {
    const int maxY = max(0, y);
    const int resultY = min(maxY,getWidth()-1);
    const int maxX = max(0, x);
    const int resultX = min(maxX,getHeight()-1);
    return getIntensity(resultX, resultY);
}

double Picture::getReflectBoarderValue(const int x, const int y) const {
    int resultX = 0;
    int resultY = 0;
    int const width = getWidth();
    if(y>=width)
    {
        resultY = 2*(width - 1) - y;
    }
    else
    {
        resultY = abs(y);
    }
    int const height = getHeight();
    if(x>=height)
    {
        resultX = 2*(height - 1) - x;
    }
    else
    {
        resultX = abs(x);
    }
    return getIntensity(resultX, resultY);
}

double Picture::getWrapPicture(const int x, const int y) const {
    int resultX = 0;
    int resultY = 0;
    int const width = getWidth();
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
    return getIntensity(resultX, resultY);
}



