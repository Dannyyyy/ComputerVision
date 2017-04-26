#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <picture.h>
#include <gaussianpyramid.h>
#include <descriptorsearch.h>
#include <memory>
#include <cvhelper.h>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void lab1();
    void lab2();
    void lab3();
    void lab4();
    void lab6();
    Picture loadPicture(QString filePath);
    Picture loadQImage(const QImage &imageInitial);
    void outputPyramid(const GaussianPyramid &pyramid, QString filePath) const;
    void applyHomography(const QImage &first, const QImage &second, const vector<NearestDescriptors> &overlaps, QString filePath);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Picture picture;
};

#endif // MAINWINDOW_H
