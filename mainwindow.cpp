#include "mainwindow.h"
#include "uploadtest.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      downloadTest(nullptr),
      uploadTest(nullptr),
      maxResultDl(-1),
      maxResultUl(-1),
      nextDlTest(10),
      chartView(&chart),
      resultChartView(&resultChart)
{
    ui->setupUi(this);

    connect(&timer, &QTimer::timeout, this, &MainWindow::timeout );
    lineSeries.setName("Download");

    chart.addSeries(&lineSeries);
    chart.setTitle("Speed Diagram");

    QtCharts::QValueAxis * axisY = new QtCharts::QValueAxis();
    axisY->setMin(0);
    axisY->setMax(1000000000);
    axisY->setTitleText("Download Mbit/s");
    chart.addAxis(axisY, Qt::AlignLeft);

    QtCharts::QValueAxis * axisX = new QtCharts::QValueAxis();
    axisX->setMin(0);
    axisX->setMax(30);
    chart.addAxis(axisX, Qt::AlignBottom);

    lineSeries.attachAxis(axisX);
    lineSeries.attachAxis(axisY);

    chartView.setRenderHint(QPainter::Antialiasing);
    ui->horizontalLayoutChart->addWidget(&chartView);

    ///////////////////////////////////////////////////////

    resultLineSeries.setName("Download Results");
    resultLineSeriesUpload.setName("Upload Results");

    resultChart.addSeries(&resultLineSeries);
    resultChart.addSeries(&resultLineSeriesUpload);
    resultChart.setTitle("Result Speed Diagram");

    QtCharts::QValueAxis * raxisY = new QtCharts::QValueAxis();
    raxisY->setMin(0);
    raxisY->setMax(1000);
    raxisY->setTitleText("Download Mbit/s");
    resultChart.addAxis(raxisY, Qt::AlignLeft);

    QtCharts::QValueAxis * raxisY2 = new QtCharts::QValueAxis();
    raxisY2->setMin(0);
    raxisY2->setMax(1000);
    raxisY2->setTitleText("Upload Mbit/s");
    resultChart.addAxis(raxisY2, Qt::AlignRight);

    QtCharts::QDateTimeAxis * raxisX = new QtCharts::QDateTimeAxis();
    raxisX->setMin(QDateTime::currentDateTime());
    raxisX->setMax(QDateTime::currentDateTime().addSecs(10));
    raxisX->setTitleText("Date/Time");
    resultChart.addAxis(raxisX, Qt::AlignBottom);

    resultLineSeries.attachAxis(raxisX);
    resultLineSeries.attachAxis(raxisY);

    resultLineSeriesUpload.attachAxis(raxisX);
    resultLineSeriesUpload.attachAxis(raxisY2);

    resultChartView.setRenderHint(QPainter::Antialiasing);
    ui->horizontalLayoutChartResult->addWidget(&resultChartView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dlStep(double bitpersec)
{
    if( bitpersec > maxDl )
        maxDl = bitpersec;

    lineSeries.append(QPoint(lineSeries.count(), bitpersec / ( 1024 * 1024 )));
    chart.axisX()->setMax(lineSeries.count());
    chart.axisY()->setMax(maxDl / ( 1024 * 1024 ) + 50.0);
    chartView.repaint();

    int index = 0;
    while( bitpersec > 1024 )
    {
        index++;
        bitpersec /= 1024;
    }

    QStringList sl;
    sl << "" << "k" << "M" << "G" << "T";

    qDebug() << QString::asprintf("%.2f", bitpersec) + " " + sl[index] + " bit/s";

    ui->labelStep->setText(QString::asprintf("%.2f", bitpersec) + " " + sl[index] + "bit/s");
}

void MainWindow::dlFinished(double bitpersec)
{
    if( bitpersec > maxResultDl )
        maxResultDl = bitpersec;

    resultLineSeries.append(QDateTime::currentDateTime().toMSecsSinceEpoch(), bitpersec / ( 1024 * 1024 ));
    resultChart.axisX()->setMax(QDateTime::currentDateTime().addSecs(10));
    resultChart.axisY()->setMax(maxResultDl / ( 1024 * 1024 ) + 50.0);
    resultChartView.repaint();

    if(ui->checkBoxSaveToFile->isChecked())
    {
        QFile f;
        f.setFileName("downloadstat.csv");
        f.open(QIODevice::Append);
        f.write(QString::asprintf("%s;%.2f;Mbit/s\n",
          QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate).toLatin1().data(),
          bitpersec / (1024 * 1024)).toUtf8());
        f.close();
    }

    int index = 0;
    while( bitpersec > 1024 )
    {
        index++;
        bitpersec /= 1024;
    }

    QStringList sl;
    sl << "" << "k" << "M" << "G" << "T";

    ui->labelResult->setText(QString::asprintf("%.2f", bitpersec) + " " + sl[index] + "bit/s");

    on_pushButtonUpload_clicked();
}

void MainWindow::ulFinished(double bitpersec)
{
    if( bitpersec > maxResultUl )
        maxResultUl = bitpersec;

    resultLineSeriesUpload.append(QDateTime::currentDateTime().toMSecsSinceEpoch(), bitpersec / ( 1024 * 1024 ));
    resultChart.axisX()->setMax(QDateTime::currentDateTime().addSecs(10));
    resultChart.axes(Qt::Vertical)[1]->setMax(maxResultUl / ( 1024 * 1024 ) + 50.0);
    resultChartView.repaint();

    if(ui->checkBoxSaveToFile->isChecked())
    {
        QFile f;
        f.setFileName("uploadstat.csv");
        f.open(QIODevice::Append);
        f.write(QString::asprintf("%s;%.2f;Mbit/s\n",
          QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate).toLatin1().data(),
          bitpersec / (1024 * 1024)).toUtf8());
        f.close();
    }

    int index = 0;
    while( bitpersec > 1024 )
    {
        index++;
        bitpersec /= 1024;
    }

    QStringList sl;
    sl << "" << "k" << "M" << "G" << "T";

    ui->labelUpload->setText(QString::asprintf("%.2f", bitpersec) + " " + sl[index] + "bit/s");
    ui->labelStep->setText("finished.");
    ui->pushButtonStartDownloadTest->setEnabled(true);
}

void MainWindow::on_pushButtonStartDownloadTest_clicked()
{
    ui->pushButtonStartDownloadTest->setEnabled(false);

    maxDl = -1;
    lineSeries.clear();

    if( downloadTest != nullptr )
        downloadTest->deleteLater();

    downloadTest = new DownloadTest(this);
    connect(downloadTest, &DownloadTest::finished, this, &MainWindow::dlFinished );
    connect(downloadTest, &DownloadTest::step, this, &MainWindow::dlStep );
    downloadTest->start();
}

void MainWindow::timeout()
{
    nextDlTest--;
    if( nextDlTest <= 0 )
    {
        on_pushButtonStartDownloadTest_clicked();
        nextDlTest = ui->lineEdit->text().toInt();
    }
    ui->labelNext->setText("Start in: " + QString::number(nextDlTest));
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if( arg1 )
        timer.start(1000);
    else
    {
        timer.stop();
        ui->labelNext->setText("Automode Off");
    }
}

void MainWindow::on_pushButtonUpload_clicked()
{
    if( uploadTest != nullptr )
        uploadTest->deleteLater();
    uploadTest = new UploadTest(this);
    connect(uploadTest, &UploadTest::finished, this, &MainWindow::ulFinished);
    uploadTest->sendData(20 * 1024 * 1024);
    ui->labelUpload->setText("uploading...");
    ui->labelStep->setText("uploading...");
}

void MainWindow::on_pushButtonAbout_clicked()
{
    QApplication::aboutQt();
}
