#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QColor>
#include <QRgb>
#include <picturefilter.h>
#include <border.h>
#include <gaussianpyramid.h>
#include <pointsearch.h>
#include <descriptorsearch.h>
#include <iostream>
#include <QPainter>
#include <homographysearch.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::outputPyramid(const GaussianPyramid &pyramid, QString filePath) const{
    int countOctaves = pyramid.getCountOctaves();
    int countLevelsInOctave = pyramid.getCountLevelsInOctave();
    for(int i = 0; i < countOctaves; i++) {
        for (int j = 0; j <= countLevelsInOctave+2; j++) {
            QString fileName =  filePath + QString("octave_%1_level_%2").arg(QString::number(i),QString::number(j));
            pyramid.savePicture(i,j,fileName);
        }
    }
}

Picture MainWindow::loadPicture(QString filePath){
    QImage imageInitial(filePath);
    const int height = imageInitial.height();
    const int width = imageInitial.width();
    auto initialPicture = Picture(height, width);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            auto intensity = imageInitial.pixel(j,i);
            initialPicture.setIntensity(i,j,qRed(intensity),qGreen(intensity),qBlue(intensity));
        }
    }
    return initialPicture;
}

void MainWindow::lab1(){
    QString filePath = "C:\\AGTU\\pictures\\";
    // Собель
    auto pictureSobelX = picture.useFilter(PictureFilter::getSobelGX(),BorderMode::ReflectBorderValue);
    pictureSobelX.saveImage(filePath + "sobelX");
    auto pictureSobelY = picture.useFilter(PictureFilter::getSobelGY(),BorderMode::ReflectBorderValue);
    pictureSobelY.saveImage(filePath + "sobelY");
    auto &pictureSobel = Picture::calculationGradient(pictureSobelX,pictureSobelY);
    pictureSobel.pictureNormalize();
    pictureSobel.saveImage(filePath + "sobel");
    // Гаусс
    auto pictureGauss = picture.useFilter(PictureFilter::getGaussX(5),PictureFilter::getGaussY(5),BorderMode::ReflectBorderValue);
    pictureGauss.saveImage(filePath + "gauss");
    //
    auto imageResult = pictureSobel.getImage();
    QGraphicsScene *sceneResult = new QGraphicsScene(this);
    ui->graphicsViewResult->setScene(sceneResult);
    QGraphicsItem *pixmapItemResult = new QGraphicsPixmapItem(QPixmap::fromImage(imageResult));
    sceneResult->addItem(pixmapItemResult);
    ui->graphicsViewResult->fitInView(pixmapItemResult, Qt::KeepAspectRatio);
}

void MainWindow::lab2(){
    auto pyramid = new GaussianPyramid(picture,5);
    outputPyramid(*pyramid,"C:\\AGTU\\pictures\\");
}

void MainWindow::lab3(){
    auto interestPointsMoravek = new PointSearch(picture);
    interestPointsMoravek->moravek(BorderMode::ReflectBorderValue, 0.02);
    interestPointsMoravek->adaptiveNonMaxSuppression(500);
    interestPointsMoravek->drawAndSaveInterestPoints("C:\\AGTU\\pictures\\moravek.jpg");

    auto interestPointsHarris = new PointSearch(picture);
    interestPointsHarris->harris(BorderMode::ReflectBorderValue, 0.075);
    interestPointsHarris->adaptiveNonMaxSuppression(500);
    interestPointsHarris->drawAndSaveInterestPoints("C:\\AGTU\\pictures\\harris.jpg");
}

void MainWindow::lab4(){
    const double treshold = 0.075;
        const int pointsCount = 500;
        auto border = BorderMode::ReflectBorderValue;

        auto fPicture = loadPicture("C:\\AGTU\\pictures\\first.jpg");
        auto fInterestPoints = new PointSearch(fPicture);
        fInterestPoints->harris(border, treshold);
        fInterestPoints->adaptiveNonMaxSuppression(pointsCount);

        auto sPicture = loadPicture("C:\\AGTU\\pictures\\second.jpg");
        auto sInterestPoints = new PointSearch(sPicture);
        sInterestPoints->harris(border, treshold);
        sInterestPoints->adaptiveNonMaxSuppression(pointsCount);

        auto sobelGX = PictureFilter::getSobelGX();
        auto sobelGY = PictureFilter::getSobelGY();

        auto fSobelX = fPicture.useFilter(sobelGX,border);
        auto fSobelY = fPicture.useFilter(sobelGY,border);
        auto sSobelX = sPicture.useFilter(sobelGX,border);
        auto sSobelY = sPicture.useFilter(sobelGY,border);

        auto fPoints = fInterestPoints->Points();
        auto sPoints = sInterestPoints->Points();

        auto fDescriptors = new DescriptorSearch(fSobelX, fSobelY, border, fPoints);
        auto sDescriptors = new DescriptorSearch(sSobelX, sSobelY, border, sPoints);

        vector<NearestDescriptors> overlaps = DescriptorSearch::searchOverlap(*fDescriptors, *sDescriptors);

        const int fHeight = fPicture.getHeight();
        const int fWidth = fPicture.getWidth();
        const int sHeight = sPicture.getHeight();
        const int sWidth = sPicture.getWidth();
        const int rHeight = max(fHeight,sHeight);
        const int rWidth = fWidth + sWidth;
        QImage resultImage = QImage(rWidth, rHeight, QImage::Format_RGB32);

        for (int x = 0; x < fHeight; x++) {
           for (int y = 0; y < fWidth; y++) {
                   int intensity = (int)(fPicture.getIntensity(x, y) * 255);
                   resultImage.setPixel(y, x, qRgb(intensity, intensity, intensity));
           }
        }
        for (int x = 0; x < sHeight; x++) {
           for (int y = 0; y < sWidth; y++) {
                   int intensity = (int)(sPicture.getIntensity(x, y) * 255);
                   resultImage.setPixel(y+fWidth, x, qRgb(intensity, intensity, intensity));
           }
        }
        DescriptorSearch::saveOverlaps(resultImage,"C:\\AGTU\\pictures\\",overlaps,fWidth);
}

void MainWindow::lab6(){
    /*
    const double treshold = 0.075;
    const int pointsCount = 500;
    auto border = BorderMode::ReflectBorderValue;
    auto fPicture = loadPicture("C:\\AGTU\\pictures\\circle.jpg");
    auto pyramid = new GaussianPyramid(fPicture,8);
    pyramid->calculateDifferences();
    outputPyramid(*pyramid,"C:\\AGTU\\pictures\\");
    auto fInterestPoints = new PointSearch(fPicture);
    fInterestPoints->blob(*pyramid, border);
    fInterestPoints->drawAndSaveInterestPointsBlob("C:\\AGTU\\pictures\\blob.jpg");
    */
    const double treshold = 0.075;
        const int pointsCount = 500;
        auto border = BorderMode::ReflectBorderValue;

        auto fPicture = loadPicture("C:\\AGTU\\pictures\\first.jpg");
        auto fInterestPoints = new PointSearch(fPicture);
        fInterestPoints->harris(border, treshold);
        fInterestPoints->adaptiveNonMaxSuppression(pointsCount);

        auto sPicture = loadPicture("C:\\AGTU\\pictures\\second.jpg");
        auto sInterestPoints = new PointSearch(sPicture);
        sInterestPoints->harris(border, treshold);
        sInterestPoints->adaptiveNonMaxSuppression(pointsCount);

        auto sobelGX = PictureFilter::getSobelGX();
        auto sobelGY = PictureFilter::getSobelGY();

        auto fSobelX = fPicture.useFilter(sobelGX,border);
        auto fSobelY = fPicture.useFilter(sobelGY,border);
        auto sSobelX = sPicture.useFilter(sobelGX,border);
        auto sSobelY = sPicture.useFilter(sobelGY,border);

        auto fPoints = fInterestPoints->Points();
        auto sPoints = sInterestPoints->Points();

        auto fDescriptors = new DescriptorSearch(fSobelX, fSobelY, border, fPoints);
        auto sDescriptors = new DescriptorSearch(sSobelX, sSobelY, border, sPoints);

        vector<NearestDescriptors> overlaps = DescriptorSearch::searchOverlap(*fDescriptors, *sDescriptors);

        // lab 5
        const int fHeight = fPicture.getHeight();
        const int fWidth = fPicture.getWidth();
        const int sHeight = sPicture.getHeight();
        const int sWidth = sPicture.getWidth();
        const int rHeight = max(fHeight,sHeight);
        const int rWidth = fWidth + sWidth;
        QImage resultImage = QImage(rWidth, rHeight, QImage::Format_RGB32);

        for (int x = 0; x < fHeight; x++) {
           for (int y = 0; y < fWidth; y++) {
                   int intensity = (int)(fPicture.getIntensity(x, y) * 255);
                   resultImage.setPixel(y, x, qRgb(intensity, intensity, intensity));
           }
        }
        for (int x = 0; x < sHeight; x++) {
           for (int y = 0; y < sWidth; y++) {
                   int intensity = (int)(sPicture.getIntensity(x, y) * 255);
                   resultImage.setPixel(y+fWidth, x, qRgb(intensity, intensity, intensity));
           }
        }
        DescriptorSearch::saveOverlaps(resultImage,"C:\\AGTU\\pictures\\",overlaps,fWidth);
        // lab 8
        auto homography = HomographySearch();
        auto ransac = homography.ransac(overlaps);

        const int resultWidth = fWidth + sWidth; //+ 100;
        const int resultHeight = fHeight + sHeight; //+ 100;

        QImage qImage(resultWidth, resultHeight, QImage::Format::Format_RGB32);
        QPainter painter(&qImage);
        painter.drawImage(0, 0, fPicture.getImage());
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::Antialiasing);
        QTransform ransacTransform(ransac[0], ransac[1], ransac[2], ransac[3],
                                ransac[4], ransac[5], ransac[6], ransac[7], ransac[8]);
        cout<<ransac[0]<<" : "<<ransac[1]<<" : "<<ransac[2]<<" : "<<ransac[3]<<" : "<<ransac[4]<<" : "<<ransac[5]<<" : "<<ransac[6]<<" : "<<ransac[7]<<" : "<<ransac[8]<<endl;
        painter.setTransform(ransacTransform);
        painter.drawImage(0, 0, sPicture.getImage());
        qImage.save("C:\\AGTU\\pictures\\ransac.jpg", "jpg");}


//image load
void MainWindow::on_pushButton_clicked()
{
    //lab4();
    lab6();
    /*
    const QString filePath = QFileDialog::getOpenFileName(0, "Выбор изображения...", "", "*.jpg");
    if(filePath != "")
    {
        QImage imageInitial(filePath);
        QGraphicsScene *sceneInitial = new QGraphicsScene(this);
        ui->graphicsViewInitial->setScene(sceneInitial);
        QGraphicsItem *pixmapItemInitial = new QGraphicsPixmapItem(QPixmap::fromImage(imageInitial));
        sceneInitial->addItem(pixmapItemInitial);
        const int height = imageInitial.height();
        const int width = imageInitial.width();
        ui->graphicsViewInitial->fitInView(pixmapItemInitial, Qt::KeepAspectRatio);
        picture = Picture(height, width);
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                auto intensity = imageInitial.pixel(j,i);
                picture.setIntensity(i,j,qRed(intensity),qGreen(intensity),qBlue(intensity));
            }
        }
        lab3();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Изображение не выбрано.");
        msgBox.exec();
    }
    */
}
