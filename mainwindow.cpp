#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QMessageBox>

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
        ui->graphicsView->setScene(scene);
        QGraphicsItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        scene->addItem(pixmap_item);
        const int height = image.height();
        const int width = image.width();
        ui->graphicsView->fitInView(pixmap_item, Qt::KeepAspectRatio);
        picture = make_unique<Picture>(height, width);
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                //picture->content[i][j] = image.pixel(i,j);
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
