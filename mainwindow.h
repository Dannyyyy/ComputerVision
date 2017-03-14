#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <picture.h>
#include <gaussianpyramid.h>
#include <memory>

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
    void outputPyramid(const GaussianPyramid &pyramid, QString filePath) const;
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Picture picture;
};

#endif // MAINWINDOW_H
