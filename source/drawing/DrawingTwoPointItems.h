/* DrawingTwoPointItems.h
 *
 * Copyright (C) 2013-2014 Jason Allen
 *
 * This file is part of the Jade Diagram Editor.
 *
 * Jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGTWOPOINTITEMS_H
#define DRAWINGTWOPOINTITEMS_H

#include <DrawingItem.h>

class DrawingTwoPointItem : public DrawingItem
{
public:
	DrawingTwoPointItem();
	DrawingTwoPointItem(const DrawingTwoPointItem& item);
	virtual ~DrawingTwoPointItem();

	void setPointPos(int pointIndex, const QPointF& scenePos);
	void setPointPos(int pointIndex, qreal sceneX, qreal sceneY);
	void setPointX(int pointIndex, qreal sceneX);
	void setPointY(int pointIndex, qreal sceneY);
	QPointF pointPos(int pointIndex) const;
	qreal pointX(int pointIndex) const;
	qreal pointY(int pointIndex) const;

	void setPen(const QPen& pen);
	void setPenColor(const QColor& color);
	void setPenWidth(qreal width);
	void setPenStyle(Qt::PenStyle style);
	void setPenCapStyle(Qt::PenCapStyle style);
	void setPenJoinStyle(Qt::PenJoinStyle style);
	QPen pen() const;
	QColor penColor() const;
	qreal penWidth() const;
	Qt::PenStyle penStyle() const;
	Qt::PenCapStyle penCapStyle() const;
	Qt::PenJoinStyle penJoinStyle() const;

	void setStartArrow(const DrawingArrow& arrow);
	void setStartArrowStyle(DrawingArrow::Style style);
	void setStartArrowSize(qreal size);
	DrawingArrow startArrow() const;
	DrawingArrow::Style startArrowStyle() const;
	qreal startArrowSize() const;

	void setEndArrow(const DrawingArrow& arrow);
	void setEndArrowStyle(DrawingArrow::Style style);
	void setEndArrowSize(qreal size);
	DrawingArrow endArrow() const;
	DrawingArrow::Style endArrowStyle() const;
	qreal endArrowSize() const;

	DrawingItemPoint* startPoint() const;
	DrawingItemPoint* endPoint() const;
	virtual bool isSuperfluous() const;

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

//==================================================================================================

class DrawingLineItem : public DrawingTwoPointItem
{
public:
	DrawingLineItem();
	DrawingLineItem(const DrawingLineItem& item);
	virtual ~DrawingLineItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);
};

//==================================================================================================

class DrawingArcItem : public DrawingTwoPointItem
{
public:
	DrawingArcItem();
	DrawingArcItem(const DrawingArcItem& item);
	virtual ~DrawingArcItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

protected:
	qreal orientedCenterX() const;
	qreal orientedCenterY() const;
	qreal orientedRadiusX() const;
	qreal orientedRadiusY() const;
	qreal orientedStartAngle() const;
	qreal orientedSpanAngle() const;
};

//==================================================================================================

class DrawingCurveItem : public DrawingTwoPointItem
{
public:
	DrawingCurveItem();
	DrawingCurveItem(const DrawingCurveItem& item);
	virtual ~DrawingCurveItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isSuperfluous() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);

protected:
	QPointF pointFromRatio(qreal ratio) const;
};

#endif
