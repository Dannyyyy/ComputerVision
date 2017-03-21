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
        for (int j = 0; j <= countLevelsInOctave; j++) {
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
    interestPointsHarris->harris(BorderMode::ReflectBorderValue, 0.01);
    interestPointsHarris->adaptiveNonMaxSuppression(500);
    interestPointsHarris->drawAndSaveInterestPoints("C:\\AGTU\\pictures\\harris.jpg");
}

void MainWindow::lab4(){
    auto firstPicture = loadPicture("C:\\AGTU\\pictures\\first.jpg");
    auto secondPicture = loadPicture("C:\\AGTU\\pictures\\second.jpg");
    auto firstDescriptors = new DescriptorSearch(firstPicture, BorderMode::ReflectBorderValue);
    auto secondDescriptors = new DescriptorSearch(secondPicture, BorderMode::ReflectBorderValue);
    vector<NearestDescriptors> overlaps = DescriptorSearch::searchOverlap(*firstDescriptors, *secondDescriptors);
    const int fHeight = firstPicture.getHeight();
    const int fWidth = firstPicture.getWidth();
    const int sHeight = secondPicture.getHeight();
    const int sWidth = secondPicture.getWidth();
    const int rHeight = max(fHeight,sHeight);
    const int rWidth = fWidth + sWidth;
    QImage resultImage = QImage(rWidth, rHeight, QImage::Format_RGB32);

    for (int x = 0; x < fHeight; x++) {
       for (int y = 0; y < fWidth; y++) {
               int intensity = (int)(firstPicture.getIntensity(x, y) * 255);
               resultImage.setPixel(y, x, qRgb(intensity, intensity, intensity));
       }
    }
    for (int x = 0; x < sHeight; x++) {
       for (int y = 0; y < sWidth; y++) {
               int intensity = (int)(secondPicture.getIntensity(x, y) * 255);
               resultImage.setPixel(y+fWidth, x, qRgb(intensity, intensity, intensity));
       }
    }
    DescriptorSearch::saveOverlaps(resultImage,"C:\\AGTU\\pictures\\",overlaps,fWidth);
}


//image load
void MainWindow::on_pushButton_clicked()
{
    lab4();
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
        //lab3();
        auto descriptor = new DescriptorSearch(picture, BorderMode::ReflectBorderValue);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Изображение не выбрано.");
        msgBox.exec();
    }
    */
}
