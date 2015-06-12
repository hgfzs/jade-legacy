/* DrawingPolyItems.h
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

#ifndef DRAWINGPOLYITEMS_H
#define DRAWINGPOLYITEMS_H

#include <DrawingItem.h>

class DrawingPolyItem : public DrawingItem
{
public:
	DrawingPolyItem();
	DrawingPolyItem(const DrawingPolyItem& item);
	virtual ~DrawingPolyItem();

	void setPointPos(int index, const QPointF& scenePos);
	void setPointPos(int index, qreal sceneX, qreal sceneY);
	void setPointX(int index, qreal sceneX);
	void setPointY(int index, qreal sceneY);
	QPointF pointPos(int index) const;
	qreal pointX(int index) const;
	qreal pointY(int index) const;

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

	virtual bool isSuperfluous() const;

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

//==================================================================================================

class DrawingPolylineItem : public DrawingPolyItem
{
public:
	DrawingPolylineItem();
	DrawingPolylineItem(const DrawingPolylineItem& item);
	virtual ~DrawingPolylineItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

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

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual bool canInsertPoint(DrawingItemPoint* point) const;
	virtual bool canRemovePoint(DrawingItemPoint* point) const;
	virtual int insertPointIndex(DrawingItemPoint* point) const;

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

//==================================================================================================

class DrawingPolygonItem : public DrawingPolyItem
{
public:
	DrawingPolygonItem();
	DrawingPolygonItem(const DrawingPolygonItem& item);
	virtual ~DrawingPolygonItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	void setBrush(const QBrush& brush);
	void setBrushColor(const QColor& color);
	QBrush brush() const;
	QColor brushColor() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual bool canInsertPoint(DrawingItemPoint* point) const;
	virtual bool canRemovePoint(DrawingItemPoint* point) const;
	virtual int insertPointIndex(DrawingItemPoint* point) const;

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

#endif
