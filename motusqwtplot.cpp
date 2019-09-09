#include "motusqwtplot.h"

MotusQwtplot::MotusQwtplot(QObject *parent) : QObject(parent)
{
    for(int i=0;i<MAXLENGHT;i++)
    {
        xcoord.append(i);
        vector[0].append(0.f);
        vector[1].append(0.f);
        vector[2].append(0.f);

    }
}

//画曲线
void MotusQwtplot::lineView(float data[3])
{

    for(int i=0;i<3;i++)
    {
       vector[i].pop_front();
       vector[i].append(data[i]);
       curve[i]->setSamples(xcoord, vector[i]);
    }
    qwtView->replot();

}

void MotusQwtplot::dynamicXMinMAX(float data[3])
{
    float mindata=0;
    float maxdata=0;
    for(int i=0;i<3;i++)
    {
       if(data[i]<mindata) mindata=data[i];
       if(data[i]>maxdata) maxdata=data[i];
    }
    if(mindata>-5.f)  mindata=-5.f;
    if(maxdata<5.f)   maxdata=5.f;
    if(fabs(mindata)>fabs(maxdata)) maxdata=fabs(mindata);
    qwtView ->setAxisScale(QwtPlot::yLeft,-1.f*maxdata-1,maxdata+1,(maxdata+maxdata)/10.f);
}

//设置X轴的最大最小值
void MotusQwtplot::setXMinMAX(double min,double max,double step)
{
   qwtView ->setAxisScale(QwtPlot::yLeft,min,max,step);
}

//曲线显示
void MotusQwtplot::showItem(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = qwtView->infoToItem( itemInfo );
    if ( plotItem )
        plotItem->setVisible( on );
}

//设置曲线颜色
void MotusQwtplot::setCurve(QString name[3],QColor linecolor[3])
{
    curve[0]=new QwtPlotCurve(name[0]);
    curve[0]->setPen(linecolor[0],2);//设置曲线颜色 粗细
    curve[0]->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    curve[0]->attach(qwtView);
    curve[0]->setLegendAttribute(curve[0]->LegendShowLine);//显示图例的标志，这里显示线的颜色。

    curve[1]=new QwtPlotCurve(name[1]);
    curve[1]->setPen(linecolor[1],2);//设置曲线颜色 粗细
    curve[1]->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    curve[1]->attach(qwtView);
    curve[1]->setLegendAttribute(curve[1]->LegendShowLine);//显示图例的标志，这里显示线的颜色。

    curve[2]=new QwtPlotCurve(name[2]);
    curve[2]->setPen(linecolor[2],2);//设置曲线颜色 粗细
    curve[2]->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    curve[2]->attach(qwtView);
    curve[2]->setLegendAttribute(curve[2]->LegendShowLine);//显示图例的标志，这里显示线的颜色。

    grid= new QwtPlotGrid();
    grid->setPen(QColor(222,222,222),1);
    grid->attach(qwtView);

    legend = new QwtLegend();
    legend->setDefaultItemMode( QwtLegendData::Checkable );//图例可被点击
    qwtView->insertLegend( legend, QwtPlot::RightLegend );

    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
               SLOT( showItem( const QVariant &, bool ) ) );//点击图例操作

    QwtPlotItemList items = qwtView->itemList( QwtPlotItem::Rtti_PlotCurve );//获取画了多少条曲线,如果为获取其他形状，注意改变参数

    for ( int i = 0; i < items.size(); i++ )
    {
        items[i]->setVisible( false );
    }
}

//绑定窗口部件
void MotusQwtplot::initPara(QwtPlot *tqwtView)
{
    qwtView=tqwtView;
    qwtView->setAxisScale(QwtPlot::xBottom,0,MAXLENGHT,MAXLENGHT/10);
    qwtView->enableAxis(QwtPlot::xBottom,false);
}
