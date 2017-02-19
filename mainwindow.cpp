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
        QImage image(filePath);
        QGraphicsScene *scene = new QGraphicsScene(this);
        ui->graphicsViewInitial->setScene(scene);
        QGraphicsItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        scene->addItem(pixmap_item);
        const int height = image.height();
        const int width = image.width();
        ui->graphicsViewInitial->fitInView(pixmap_item, Qt::KeepAspectRatio);
        picture = make_unique<Picture>(height, width);
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                auto intensity = image.pixel(j,i);
                picture->setIntensity(i,j,qRed(intensity),qGreen(intensity),qBlue(intensity));
            }
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Изображение не выбрано.");
        msgBox.exec();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    auto image = picture->getImage();
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsViewResult->setScene(scene);
    QGraphicsItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(pixmap_item);
    ui->graphicsViewResult->fitInView(pixmap_item, Qt::KeepAspectRatio);
}
