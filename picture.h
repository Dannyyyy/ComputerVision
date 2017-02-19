#ifndef PICTURE_H
#define PICTURE_H

#include <QImage>
#include <memory>
#include <vector>

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
    int getHeight(){ return this->height; }
    int getWidth(){ return this->width; }
    void setIntensity(const int x, const int y, const int redColor, const int greenColor, const int blueColor);
    double getIntensity(const int x, const int y);
    QImage getImage();
};
#endif // PICTURE_H
