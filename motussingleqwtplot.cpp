#include "motussingleqwtplot.h"

MotusSingleQwtplot::MotusSingleQwtplot(QObject *parent) : QObject(parent)
{
    for(int i=0;i<MaxSingleQwtplot;i++)
    {
        xcoord.append(i);
        vector.append(0.f);
    }
}

//画曲线
void MotusSingleQwtplot::lineView(float data)
{
    float mindata =vector.at(0);
    float maxdata =vector.at(0);
    vector.pop_front();
    vector.append(data);
    for(int i=0;i<vector.size();i++)
    {
        if(mindata>vector.at(i))
            mindata=vector.at(i);
        if(maxdata<vector.at(i))
            maxdata=vector.at(i);
    }
    dynamicXMinMAX(mindata,maxdata);
    curve->setSamples(xcoord,vector);
    qwtView->replot();
}

void MotusSingleQwtplot::dynamicXMinMAX(float mindata,float maxdata)
{
    if(fabs(maxdata-mindata)<1)
    {
       maxdata+=1;
       mindata-=1;
    }
    qwtView ->setAxisScale(QwtPlot::yLeft,mindata,maxdata,(maxdata-mindata)/10.f);
}


//设置曲线颜色
void MotusSingleQwtplot::setCurve(QString name,QColor linecolor)
{
    curve=new QwtPlotCurve(name);
    curve->setPen(linecolor,2);//设置曲线颜色 粗细
    curve->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    curve->attach(qwtView);
    curve->setLegendAttribute(curve->LegendShowLine);//显示图例的标志，这里显示线的颜色。

    grid= new QwtPlotGrid();
    grid->setPen(QColor(222,222,222),1);
    grid->attach(qwtView);
}

//绑定窗口部件
void MotusSingleQwtplot::initPara(QwtPlot *tqwtView)
{
    qwtView=tqwtView;
    qwtView->setAxisScale(QwtPlot::xBottom,0,MaxSingleQwtplot,MaxSingleQwtplot/20);
    dynamicXMinMAX(-2,2);
    qwtView->enableAxis(QwtPlot::xBottom,false);
}
