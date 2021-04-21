#include "uploadtest.h"

#include <sys/time.h>

#include <QTcpSocket>

UploadTest::UploadTest(QObject *parent) : QObject(parent)
{
    connect(&socket, &QTcpSocket::readyRead, this, &UploadTest::disconnected);
    connect(&socket, &QTcpSocket::bytesWritten, this, &UploadTest::bytesWritten);
}

void UploadTest::sendData( int64_t size )
{
    this->size = size;
    socket.connectToHost("speedtest.tele2.net", 80);
    if(!socket.waitForConnected())
    {
        qDebug() << "Can not connect to host!";
        return;
    }

    socket.write(QString("GET /upload.php HTTP/1.0\r\nContent-Length:" + QString::number(size) + "\r\nHost: speedtest.tele2.net\r\n\r\n").toLatin1());

    char * data = new char[size];
    gettimeofday(&tv1, NULL);
    socket.write(data, size);
    delete[] data;
}

void UploadTest::disconnected()
{
    qDebug() << socket.readAll();

    gettimeofday(&tv2, NULL);

    int64_t t = tv2.tv_usec - tv1.tv_usec;
    t += (int64_t)tv2.tv_sec * 1000000UL - (int64_t)tv1.tv_sec * 1000000UL;
    double tt = t / 1000000.0;

    double ra = size / tt * 8.0;

    emit finished(ra);

    int index = 0;
    QStringList sl;
    sl << "" << "k" << "M" << "G" << "T";
    while( ra > 1024 )
    {
        index++;
        ra /= 1024;
    }

    qDebug() << ra << sl[index] + "bit/s";
}

void UploadTest::bytesWritten(int64_t bytes)
{
    qDebug() << "Written" << bytes;
}
