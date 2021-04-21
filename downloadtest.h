#ifndef DOWNLOADTEST_H
#define DOWNLOADTEST_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class DownloadTest : public QObject
{
    Q_OBJECT
public:
    explicit DownloadTest(QObject *parent = nullptr);
    void start();
    void reset();

signals:
    void finished(double bitpersec);
    void step(double bitpersec);

private slots:
    void readReady();
    void timeout();

private:
    QTcpSocket socket;
    QTimer timer;

    int64_t read;
    double readAll;

    struct timeval tv;
    struct timeval tvnow;

    void evauate();

};

#endif // DOWNLOADTEST_H
