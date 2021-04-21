#ifndef UPLOADTEST_H
#define UPLOADTEST_H

#include <QObject>
#include <QTcpSocket>

class UploadTest : public QObject
{
    Q_OBJECT
public:
    explicit UploadTest(QObject *parent = nullptr);

    void sendData(int64_t size);

signals:    
    void finished(double bitpersec);

private slots:
    void disconnected();    
    void bytesWritten(int64_t bytes);

private:
    struct timeval tv1;
    struct timeval tv2;
    QTcpSocket socket;
    int64_t size;

};

#endif // UPLOADTEST_H
