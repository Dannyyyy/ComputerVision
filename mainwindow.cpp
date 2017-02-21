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

//image load
void MainWindow::on_pushButton_clicked()
{
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
        picture = make_unique<Picture>(height, width);
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                auto intensity = imageInitial.pixel(j,i);
                picture->setIntensity(i,j,qRed(intensity),qGreen(intensity),qBlue(intensity));
            }
        }
        auto pictureSobelX = picture->useFilter(*PictureFilter::getSobelGX());
        pictureSobelX->saveImage("sobelX");
        auto pictureSobelY = picture->useFilter(*PictureFilter::getSobelGY());
        pictureSobelY->saveImage("sobelY");
        auto pictureSobel = Picture::calculationGradient(*pictureSobelX,*pictureSobelY);
        pictureSobel->pictureNormalize();
        pictureSobel->saveImage("sobel");
        //
        auto imageResult = pictureSobel->getImage();
        QGraphicsScene *sceneResult = new QGraphicsScene(this);
        ui->graphicsViewResult->setScene(sceneResult);
        QGraphicsItem *pixmapItemResult = new QGraphicsPixmapItem(QPixmap::fromImage(imageResult));
        sceneResult->addItem(pixmapItemResult);
        ui->graphicsViewResult->fitInView(pixmapItemResult, Qt::KeepAspectRatio);

    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Изображение не выбрано.");
        msgBox.exec();
    }
}
