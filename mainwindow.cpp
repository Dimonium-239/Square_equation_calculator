#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setFixedWidth(this->width());
    resize(this->width(), minimumHeight());

    ui->customPlot->resize(ui->customPlot->width(), 0);
    ui->customPlot->hide();
    ui->Deltalabel->setText(QChar(0x0394)+ + ' ' + ':');

    double minVal = -1000.0, maxVal = 1000.0;
    QDoubleValidator *aVal = new QDoubleValidator(minVal, maxVal, 1000, ui->aline);
    aVal->setNotation(QDoubleValidator::StandardNotation);
    aVal->setLocale(QLocale::C);
    ui->aline->setValidator(aVal);

    QDoubleValidator *bVal = new QDoubleValidator(minVal, maxVal, 1000, ui->bline);
    bVal->setNotation(QDoubleValidator::StandardNotation);
    bVal->setLocale(QLocale::C);
    ui->bline->setValidator(bVal);

    QDoubleValidator *cVal = new QDoubleValidator(minVal, maxVal, 1000, ui->cline);
    cVal->setNotation(QDoubleValidator::StandardNotation);
    cVal->setLocale(QLocale::C);
    ui->cline->setValidator(cVal);

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_calculateButton_clicked(){
    resize(this->width(), maximumHeight());
    bool ok;
    double a_doub = ui->aline->text().toDouble(&ok);
    double b_doub = ui->bline->text().toDouble(&ok);
    double c_doub = ui->cline->text().toDouble(&ok);
    double delta = b_doub*b_doub-4*a_doub*c_doub;
    double x_1 = (-b_doub + sqrt(delta))/(2*a_doub);
    double x_2 = (-b_doub - sqrt(delta))/(2*a_doub);
    double minMaxPx = -b_doub/(2*a_doub);
    double minMaxPy = -delta/(4*a_doub);

    setupQuadratic(ui->customPlot, a_doub, b_doub, c_doub, x_1, x_2, minMaxPx, minMaxPy);
    ui->customPlot->show();

    if(delta < 0) ErrorBox();
    else if (delta >= 0){
        if(a_doub < 0.0)
            ui->min_maxLabel->setText("max point:");
        else
            ui->min_maxLabel->setText("min point:");

        QString deltaAsString = QString::number(delta);
        QString x1AsString = QString::number(x_1);
        QString x2AsString = QString::number(x_2);
        QString mmpAsString = QString::number(minMaxPx);

        ui->DeltaLine->setText(deltaAsString);
        ui->x1Line->setText(x1AsString);
        ui->x2Line->setText(x2AsString);
        ui->min_maxLine->setText(mmpAsString);
    }
}

void MainWindow::ErrorBox(){
    QMessageBox messageBox;
    messageBox.critical(nullptr,"Warning","Delta is less then 0\nYou can see only plot");
    messageBox.setFixedSize(500,200);
}

void MainWindow::on_clearButton_clicked(){
    ui->aline->setText("");
    ui->bline->setText("");
    ui->cline->setText("");
    ui->DeltaLine->setText("");
    ui->x1Line->setText("");
    ui->x2Line->setText("");
    ui->min_maxLine->setText("");
    ui->min_maxLabel->setText("min max point:");
    ui->customPlot->graph()->data()->clear();
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->hide();
    ui->customPlot->resize(ui->customPlot->width(), 0);
    resize(this->width(), minimumHeight());
}

void MainWindow::setupQuadratic(QCustomPlot *customPlot, double a, double b, double c, double x_1, double x_2, double minMaxPx, double minMaxPy){
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    QCPGraph *graph = customPlot->addGraph();
    ui->customPlot->graph()->data()->clear();
    ui->customPlot->graph(0)->data()->clear();
    int n = 50000;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; ++i){
      x[i] = (i - int(n/2));
      y[i] = a*(x[i]*x[i]) + b*x[i] + c;
    }

    graph->setData(x, y);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();
    graph->setScatterSkip(0);

    if(minMaxPy < 0) customPlot->xAxis->setRange(x_1-4, x_2+4);
    else customPlot->xAxis->setRange(minMaxPx-10, minMaxPx+10);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->xAxis->setLabel("x");

    customPlot->yAxis->setRange(minMaxPy-10, minMaxPy+10);
    customPlot->yAxis->setLabel("y");

    QVector<double> points = {
        x_1, minMaxPx, x_2
    };
    QVector<double> values;

    QCPItemTracer * tracer = new QCPItemTracer(customPlot);
    tracer->setGraph(customPlot->graph());
    tracer->setInterpolating(true);
    tracer->setVisible(false);
    for(int i = 0; i < 3; i ++){
        tracer->setGraphKey(points[i]);
        tracer->updatePosition();
        values.push_back(tracer->position->coords().y());
    }

    QCPGraph* dwPoints = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
    dwPoints->setAdaptiveSampling(false);
    dwPoints->setLineStyle(QCPGraph::lsNone);
    dwPoints->setScatterStyle(QCPScatterStyle::ssCircle);
    dwPoints->setPen(QPen(QBrush(Qt::red), 2));

    textItem = new QCPItemText(customPlot);
    connect(ui->customPlot, &QCustomPlot::mousePress, this, &MainWindow::onMouseClicking);

    dwPoints->addData(points, values);
    customPlot->replot();
}

void MainWindow::onMouseClicking(QMouseEvent *event)
{
    textItem->setText(QString(""));
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());
    double x = customPlot->xAxis->pixelToCoord(event->pos().x());
    double y = customPlot->yAxis->pixelToCoord(event->pos().y());
    textItem->setText(QString("(%1, %2)").arg(x).arg(y));
    textItem->position->setCoords(QPointF(x+1.2, y+1.2));
    textItem->setFont(QFont(font().family(), 8));
    customPlot->replot();
}


