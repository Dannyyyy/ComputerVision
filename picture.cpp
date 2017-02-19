#include <picture.h>

Picture::Picture(){
    this->width = 0;
    this->height = 0;
}

Picture::Picture(const int width, const int height){
    this->width = width;
    this->height = height;
}
