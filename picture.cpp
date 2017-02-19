#include <picture.h>

Picture::Picture(){
    this->width = 0;
    this->height = 0;
    this->content = nullptr;
}

Picture::Picture(const int width, const int height){
    this->width = width;
    this->height = height;
    this->content = make_unique<double []>(width * height);
}
