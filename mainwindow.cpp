#include <QThread>

#include <liquid/liquid.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

static const int samplesToRead = 8192;
static const int channels = 2;
static const int adcMaxValue = 128;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mTcpSocket = new QTcpSocket(this);
    connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(onSockReadyRead()));
    connect(mTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSockDisplayError(QAbstractSocket::SocketError)));
    mTcpSocket->connectToHost("localhost", 1234);

    // Set plot title
    ui->iqPlot->setTitle(QString::fromLocal8Bit("Real Time Plot:"));

    // Create legend
    leg = new QwtLegend;
    // Make legend readonly
    leg->setDefaultItemMode(QwtLegendData::ReadOnly);
    // Put the legend at the top
    ui->iqPlot->insertLegend(leg,QwtPlot::TopLegend);

    // Create grid
    grid = new QwtPlotGrid;
    // Enable grid lines
    grid->enableXMin(true);
    // Set grid color
    grid->setMajorPen(QPen(Qt::black,0,Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));

    // Bind grid to plot
    grid->attach(ui->iqPlot);

    // Axes names
    ui->iqPlot->setAxisTitle(
        QwtPlot::xBottom,QString::fromLocal8Bit("time, 1/Fs"));
    ui->iqPlot->setAxisScale(QwtPlot::xBottom,-1,samplesToRead+1);
    ui->iqPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("U, V"));
    ui->iqPlot->setAxisScale(QwtPlot::yLeft,-adcMaxValue-1,adcMaxValue+1);

    // Create curve
    curv_I = new QwtPlotCurve(QString("I"));
    curv_I->setPen(QPen(Qt::green));

    curv_Q = new QwtPlotCurve(QString("Q"));
    curv_Q->setPen(QPen(Qt::red));

    // Put curve to the plot
    curv_I->attach(ui->iqPlot);
    curv_Q->attach(ui->iqPlot);

    ui->iqPlot->replot();
    ui->iqPlot->canvas()->setCursor(Qt::ArrowCursor);

    //=====================================================================
    ui->psdPlot->setTitle(QString::fromLocal8Bit("Power Spectral Density:"));

    leg2 = new QwtLegend;
    leg2->setDefaultItemMode(QwtLegendData::ReadOnly);
    ui->psdPlot->insertLegend(leg2,QwtPlot::TopLegend);

    grid2 = new QwtPlotGrid;
    grid2->enableXMin(true);
    grid2->setMajorPen(QPen(Qt::black,0,Qt::DotLine));
    grid2->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));

    grid2->attach(ui->psdPlot);


    ui->psdPlot->setAxisTitle(
        QwtPlot::xBottom,QString::fromLocal8Bit("frequency/Fs"));
    ui->psdPlot->setAxisScale(QwtPlot::xBottom,-0.5,0.5);
    ui->psdPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("PSD, dB"));
    ui->psdPlot->setAxisScale(QwtPlot::yLeft,-50,10);

    curv_S = new QwtPlotCurve(QString("U[Q](f)"));
    curv_S->setPen(QPen(Qt::blue));

    curv_S->attach(ui->psdPlot);

    ui->psdPlot->replot();

    ui->psdPlot->canvas()->setCursor(Qt::ArrowCursor);
}

void MainWindow::onSockDisplayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(this, "Error", "The following error occurred: "+mTcpSocket->errorString());
    }
}

void MainWindow::onSockReadyRead()
{
    //qDebug() << "Data ready to read";
    const int bytesToRead = samplesToRead * channels;
    if(mTcpSocket->bytesAvailable()>bytesToRead)
    {
        QByteArray array = mTcpSocket->read(bytesToRead);
        mTcpSocket->readAll();
        QVector<double> x;
        QVector<double> I;
        QVector<double> Q;
        QVector<double> S;
        QVector<double> F;

        int nfft = array.length() / channels;
        spgramf q = spgramf_create_default(nfft);

        //qDebug() << "Data bytes read " << array.length();
        for (int i = 0; i < array.length(); i++) {
            float v = (unsigned char)array.at(i) - 127;
            if (i % channels == 0) {
                I << v;
                x << i/channels;
                spgramf_push(q, v / adcMaxValue);
            } else
                Q << v;
        }
        float psd_dB[nfft];
        spgramf_get_psd(q, psd_dB);
        spgramf_destroy(q);
        for (int i = 0; i < nfft; i++) {
            S << psd_dB[i];
            F << -0.5 + (float)i/nfft;
        }

        curv_I->setSamples(x, I);
        curv_Q->setSamples(x, Q);
        ui->iqPlot->replot();

        curv_S->setSamples(F, S);
        ui->psdPlot->replot();

    //    QThread::msleep(50);

    }
}

MainWindow::~MainWindow()
{
    delete leg;
    delete leg2;
    delete grid;
    delete grid2;
    delete curv_I;
    delete curv_Q;
    delete curv_S;
    delete ui;
}
