/****************************************************************************
**
** Copyright (C) 2012 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the Qt Commercial Charts Add-on.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QValuesAxis>
#include <QAbstractAxis>
#include <QDebug>

#include "chart.h"

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags, QLineSeries *series)
    : QChart(parent, wFlags), m_series(series)
{
    m_clicked = false;
}

Chart::~Chart()
{
}

void Chart::clickPoint(const QPointF &point)
{
    //Get all points from the series.
    QList<QPointF> points = m_series->points();
    //Construct a small rectangle around the clicked point
    //to identify the real point clicked from the series.
    QRectF clickRect(point.x() - 0.5, point.y() - 0.5, 1.0, 1.0);

    //Find the clicked point to be moved.
    foreach (QPointF p, points) {
        if (clickRect.contains(p)) {
            m_movingPoint = p;
            m_clicked = true;
            return;
        }
    }
}

void Chart::setPointClicked(bool clicked)
{
   m_clicked = clicked;
}

void Chart::handlePointMove(const QPoint &point)
{
    if (m_clicked) {
        //Map the point clicked from the ChartView
        //to the area occupied by the chart.
        QPoint mappedPoint = point;
        mappedPoint.setX(point.x()-this->plotArea().x());
        mappedPoint.setY(point.y()-this->plotArea().y());

        //Get the x- and y axis to be able to convert the mapped
        //coordinate point to the charts scale.
        QAbstractAxis * axisx = this->axisX();
        QValuesAxis* haxis = 0;
        if (axisx->type() == QAbstractAxis::AxisTypeValues)
            haxis = qobject_cast<QValuesAxis*>(axisx);

        QAbstractAxis * axisy = this->axisY();
        QValuesAxis* vaxis = 0;
        if (axisy->type() == QAbstractAxis::AxisTypeValues)
            vaxis = qobject_cast<QValuesAxis*>(axisy);

        if (haxis && vaxis) {
            //Calculate the "unit" between points on the x
            //y axis.
            double xUnit = this->plotArea().width()/haxis->max();
            double yUnit = this->plotArea().height()/vaxis->max();

            //Convert the mappedPoint to the actual chart scale.
            double x = mappedPoint.x()/xUnit;
            double y = vaxis->max() - mappedPoint.y()/yUnit;

            //Replace the old point with the new one.
            m_series->replace(m_movingPoint, QPointF(x, y));

            //Update the m_movingPoint so we are able to
            //do the replace also during mousemoveEvent.
            m_movingPoint.setX(x);
            m_movingPoint.setY(y);
        }
    }
}

