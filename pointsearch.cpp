#include <pointsearch.h>

void PointSearch::harris() const{
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);

    const double sigma = 1;
    auto gauss = PictureFilter::getGaussXY(sigma);
    const int centerGauss = gauss.getHeight()/2;

    BorderMode border = BorderMode::ReflectBorderValue;
    auto filterSobelX = PictureFilter::getSobelGX();
    auto sobelX = picture.useFilter(filterSobelX,border);
    auto filterSobelY = PictureFilter::getSobelGY();
    auto sobelY = picture.useFilter(filterSobelY,border);

    double A,B,C;
    const double k = 0.06;
    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {
             A=B=C=0;
             for(int u=-centerGauss;u<centerGauss;u++)
             {
                 for(int v=-centerGauss;v<centerGauss;v++)
                 {
                     auto gaussW = gauss.getContentCell(u,v);
                     auto Ix = sobelX.getIntensity(x,y,border);
                     auto Iy = sobelY.getIntensity(x,y,border);
                     A += gaussW * pow(Ix,2);
                     B += gaussW * Ix * Iy;
                     C += gaussW * pow(Iy,2);
                 }
             }
             double f = A*C - pow(B,2) - k*pow((A+C),2);
             resultPicture.setIntensity(x,y,f);
        }
     }
}

void PointSearch::moravek() const{
    const int heightPicture = picture.getHeight();
    const int widthPicture = picture.getWidth();
    auto resultPicture = Picture(heightPicture, widthPicture);

    for (int x = 0; x < heightPicture; x++) {
        for (int y = 0; y < widthPicture; y++) {

        }
    }
}
