#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <picture.h>
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
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    unique_ptr<Picture> picture;
};

#endif // MAINWINDOW_H
