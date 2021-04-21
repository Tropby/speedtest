#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include "downloadtest.h"
#include "uploadtest.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void dlStep(double bitpersec);
    void dlFinished(double bitpersec);
    void ulFinished(double bitpersec);

    void on_pushButtonStartDownloadTest_clicked();

    void timeout();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButtonUpload_clicked();

    void on_pushButtonAbout_clicked();

private:
    Ui::MainWindow *ui;
    DownloadTest * downloadTest;
    UploadTest * uploadTest;

    double maxDl;
    double maxResultDl;
    double maxResultUl;

    QTimer timer;
    int nextDlTest;

    QtCharts::QLineSeries lineSeries;
    QtCharts::QChart chart;
    QtCharts::QChartView chartView;

    QtCharts::QSplineSeries resultLineSeries;
    QtCharts::QSplineSeries resultLineSeriesUpload;
    QtCharts::QChart resultChart;
    QtCharts::QChartView resultChartView;

};
#endif // MAINWINDOW_H
