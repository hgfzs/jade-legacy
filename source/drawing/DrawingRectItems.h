/* DrawingRectItems.h
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

#ifndef DRAWINGRECTITEMS_H
#define DRAWINGRECTITEMS_H

#include <DrawingItem.h>

class DrawingRectResizeItem : public DrawingItem
{
public:
	DrawingRectResizeItem();
	DrawingRectResizeItem(const DrawingRectResizeItem& item);
	virtual ~DrawingRectResizeItem();

	void setRect(const QRectF& sceneRect);
	QRectF rect() const;

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

	void adjustBoxControlPoints(DrawingItemPoint* activePoint);
	void adjustEllipseControlPoints(DrawingItemPoint* activePoint);
};

//==================================================================================================

class DrawingRectItem : public DrawingRectResizeItem
{
public:
	DrawingRectItem();
	DrawingRectItem(const DrawingRectItem& item);
	virtual ~DrawingRectItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	void setBrush(const QBrush& brush);
	void setBrushColor(const QColor& color);
	QBrush brush() const;
	QColor brushColor() const;

	void setCornerRadii(qreal xRadius, qreal yRadius);
	void setCornerRadiusX(qreal radius);
	void setCornerRadiusY(qreal radius);
	qreal cornerRadiusX() const;
	qreal cornerRadiusY() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	qreal orientedCornerRadiusX() const;
	qreal orientedCornerRadiusY() const;
};

//==================================================================================================

class DrawingEllipseItem : public DrawingRectResizeItem
{
public:
	DrawingEllipseItem();
	DrawingEllipseItem(const DrawingEllipseItem& item);
	virtual ~DrawingEllipseItem();
	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	void setBrush(const QBrush& brush);
	void setBrushColor(const QColor& color);
	QBrush brush() const;
	QColor brushColor() const;


	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);

protected:
	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

#endif
