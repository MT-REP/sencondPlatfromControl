#ifndef MOTUSQWTPLOT_H
#define MOTUSQWTPLOT_H

#include <QObject>
#include<qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include <qpen.h>
#include <qwt_symbol.h>
#include <qwt_picker_machine.h>

#define MAXLENGHT 1800

class MotusQwtplot : public QObject
{
    Q_OBJECT
public:
    explicit MotusQwtplot(QObject *parent = nullptr);
    void initPara(QwtPlot *tqwtView);
    //设置X轴的最大最小值
    void setXMinMAX(double min,double max,double step);
    //设置曲线颜色和名字
    void setCurve(QString name[6],QColor linecolor[6]);
    //画曲线
    void lineView(float data[6]);
private:
    QwtPlot *qwtView;
    QwtPlotCurve *curve[6];
    QwtLegend *legend;
    QwtPlotGrid* grid;
    QVector<double> xcoord;
    QVector<double> vector[6];
signals:

public slots:
    void showItem(const QVariant &itemInfo, bool on);
};

#endif // MOTUSQWTPLOT_H
