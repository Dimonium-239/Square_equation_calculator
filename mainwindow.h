#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"
#include <QMainWindow>
#include <math.h>
#include <QDoubleValidator>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setupQuadratic(QCustomPlot *customPlot, double a, double b, double c, double x_1, double x_2, double minMaxPx, double minMaxPy);
    ~MainWindow();

private slots:

    void onMouseClicking(QMouseEvent* event);

    void on_calculateButton_clicked();

    void on_clearButton_clicked();

    void ErrorBox();


private:
    Ui::MainWindow *ui;
    QCPItemText *textItem;
};


#endif // MAINWINDOW_H
