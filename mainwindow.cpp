#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

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

void MainWindow::on_pushButton_clicked()
{
    const QString dirs = QFileDialog::getOpenFileName(0, "Выбор изображения...", "", "*.jpg");
    QImage image(dirs);
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    QGraphicsItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(pixmap_item);
    const int height = image.height();
    const int width = image.width();
    ui->graphicsView->fitInView(pixmap_item, Qt::KeepAspectRatio);
}
