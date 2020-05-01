#include "motusqwtplot.h"

MotusQwtplot::MotusQwtplot(QObject *parent) : QObject(parent)
{
    for(int i=0;i<MAXLENGHT;i++)
    {
        xcoord.append(i);
        vector[0].append(0.f);
        vector[1].append(0.f);
        vector[2].append(0.f);
        vector[3].append(0.f);
        vector[4].append(0.f);
        vector[5].append(0.f);
    }
}

//画曲线
void MotusQwtplot::lineView(float data[6])
{
    for(int i=0;i<6;i++)
    {
       vector[i].pop_front();
       vector[i].append(data[i]);
       curve[i]->setSamples(xcoord, vector[i]);
    }
    float mindata =vector[0].at(0);
    float maxdata =vector[0].at(0);
    for(int j=0;j<6;j++)
    {
        for(int i=0;i<vector[j].size();i++)
        {
            if(mindata>vector[j].at(i))
                mindata=vector[j].at(i);
            if(maxdata<vector[j].at(i))
                maxdata=vector[j].at(i);
        }
    }
    if(fabs(maxdata-mindata)<1)
    {
       maxdata+=1;
       mindata-=1;
    }
    setXMinMAX(mindata,maxdata,10);
    qwtView->replot();

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
void MotusQwtplot::setCurve(QString name[6],QColor linecolor[6])
{
    for(int i=0;i<6;i++)
    {
        curve[i]=new QwtPlotCurve(name[i]);
        curve[i]->setPen(linecolor[i],2);//设置曲线颜色 粗细
        curve[i]->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
        curve[i]->attach(qwtView);
        curve[i]->setLegendAttribute(curve[i]->LegendShowLine);//显示图例的标志，这里显示线的颜色。
    }

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
