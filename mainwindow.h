#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>

#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onSockReadyRead();
    void onSockDisplayError(QAbstractSocket::SocketError socketError);

private:
    Ui::MainWindow *ui;

    QwtLegend *leg, *leg2;      // legend on plots
    QwtPlotGrid *grid, *grid2;  // grids on plots
    QwtPlotCurve *curv_I, *curv_Q, *curv_S; // curves on plots

    QTcpSocket * mTcpSocket;
};

#endif // MAINWINDOW_H
