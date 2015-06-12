/* DrawingItemGroup.h
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

#ifndef DRAWINGITEMGROUP_H
#define DRAWINGITEMGROUP_H

#include <DrawingItem.h>

class DrawingItemGroup : public DrawingItem
{
private:
	QList<DrawingItem*> mItems;

public:
	DrawingItemGroup();
	DrawingItemGroup(const DrawingItemGroup& item);
	virtual ~DrawingItemGroup();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

	void addItem(DrawingItem* item);
	void removeItem(DrawingItem* item);
	void clearItems();
	int numberOfItems() const;
	bool containsItem(DrawingItem* item) const;
	DrawingItem* item(int index) const;

	void setItems(const QList<DrawingItem*>& items);
	QList<DrawingItem*> items() const;

	virtual QRectF boundingRect() const;

	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual void rotateItem(const QPointF& parentPos);
	virtual void rotateBackItem(const QPointF& parentPos);
	virtual void flipItem(const QPointF& parentPos);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);

	virtual void writeXmlChildElements(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlChildElement(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	void updatePoints();
	DrawingItemPoint* cornerPoint(Qt::Corner corner) const;
};

#endif
