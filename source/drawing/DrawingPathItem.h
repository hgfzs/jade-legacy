/* DrawingPathItem.h
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

#ifndef DRAWINGPATHITEM_H
#define DRAWINGPATHITEM_H

#include <DrawingRectItems.h>

class DrawingPathItem : public DrawingRectResizeItem
{
private:
	QPainterPath mPath;
	QString mUniqueKey;

public:
	DrawingPathItem();
	DrawingPathItem(const DrawingPathItem& item);
	~DrawingPathItem();

	virtual DrawingItem* copy() const;

	void setUniqueKey(const QString& key);
	virtual QString uniqueKey() const;

	void setPath(const QPainterPath& path);
	QPainterPath path() const;

	QPointF mapToPath(const QPointF& itemPos) const;
	QRectF mapToPath(const QRectF& itemRect) const;
	QPointF mapFromPath(const QPointF& pathPos) const;
	QRectF mapFromPath(const QRectF& pathRect) const;

	virtual QRectF boundingRect() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	void setInitialPath(const QPainterPath& path);
	void addConnectionPoint(const QPointF& itemPos);
	void addConnectionPoint(qreal x, qreal y);

	virtual void resizeItem(DrawingItemPoint* point, const QPointF& parentPos);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

#endif
