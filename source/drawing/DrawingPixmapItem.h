/* DrawingPixmapItem.h
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

#ifndef DRAWINGPIXMAPITEM_H
#define DRAWINGPIXMAPITEM_H

#include <DrawingRectItems.h>

class DrawingPixmapItem : public DrawingRectResizeItem
{
public:
	DrawingPixmapItem();
	DrawingPixmapItem(const DrawingPixmapItem& item);
	virtual ~DrawingPixmapItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	void setPixmap(const QPixmap& pixmap);
	QPixmap pixmap() const;

	virtual QRectF boundingRect() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
};

#endif
