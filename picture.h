#ifndef PICTURE_H
#define PICTURE_H

#include <QImage>
#include <memory>
#include <picturefilter.h>
#include <border.h>

using namespace std;

class Picture
{
private:
    int height;
    int width;
    unique_ptr<double []> content;
public:
    Picture();
    Picture(const int height, const int width);
    int getHeight() const { return this->height; }
    int getWidth() const { return this->width; }
    void setIntensity(const int x, const int y, const int redColor, const int greenColor, const int blueColor);
    void setIntensity(const int x, const int y, const double intensity);
    double getIntensity(const int x, const int y) const;
    double getIntensity(const int x, const int y, BorderMode borderMode) const;
    QImage getImage();
    unique_ptr<Picture> useFilter(const PictureFilterContent &pictureFilterContent, BorderMode borderMode);
    void saveImage(QString fileName);
    static unique_ptr<Picture> calculationGradient(const Picture &sobelX, const Picture &sobelY);
    void pictureNormalize();
    unique_ptr<Picture> getPictureNormalize();
    double getOutsideBlack(const int x, const int y) const;
    double getCopyBoarderValue(const int x, const int y) const;
    double getReflectBoarderValue(const int x, const int y) const;
    double getWrapPicture(const int x, const int y) const;
};
#endif // PICTURE_H
