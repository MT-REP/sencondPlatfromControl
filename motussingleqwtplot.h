#ifndef MOTUSSINGLEQWTPLOT_H
#define MOTUSSINGLEQWTPLOT_H

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

#define MaxSingleQwtplot 200
class MotusSingleQwtplot : public QObject
{
    Q_OBJECT
public:
    explicit MotusSingleQwtplot(QObject *parent = nullptr);
    void initPara(QwtPlot *tqwtView);
    //设置曲线颜色和名字
    void setCurve(QString name,QColor linecolor);
    //画曲线
    void lineView(float data);
    //
    void dynamicXMinMAX(float mindata,float maxdata);
private:
    QwtPlot *qwtView;
    QwtPlotCurve *curve;
    QwtPlotGrid* grid;
    QVector<double> xcoord;
    QVector<double> vector;
signals:

public slots:
};

#endif // MOTUSSINGLEQWTPLOT_H
