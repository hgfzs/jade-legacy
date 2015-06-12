/* DrawingItemGroup.cpp
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

#include <DrawingItemGroup.h>
#include <DrawingItemPoint.h>

DrawingItemGroup::DrawingItemGroup() : DrawingItem()
{
	setFlags(CanMove | CanRotate | CanFlip | MatchUnitsWithParent);
	setPlaceType(PlaceMouseDownAndUp);

	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::Control, 0));
}

DrawingItemGroup::DrawingItemGroup(const DrawingItemGroup& item) : DrawingItem(item)
{
	mItems = copyItems(item.mItems);
}

DrawingItemGroup::~DrawingItemGroup()
{
	clearItems();
}

//==================================================================================================

DrawingItem* DrawingItemGroup::copy() const
{
	return new DrawingItemGroup(*this);
}

QString DrawingItemGroup::uniqueKey() const
{
	return "group";
}

//==================================================================================================

void DrawingItemGroup::addItem(DrawingItem* item)
{
	if (item && !containsItem(item))
	{
		mItems.append(item);
		updatePoints();
	}
}

void DrawingItemGroup::removeItem(DrawingItem* item)
{
	if (item && containsItem(item))
	{
		mItems.removeAll(item);
		updatePoints();
	}
}

void DrawingItemGroup::clearItems()
{
	while (!mItems.empty()) delete mItems.takeLast();
	updatePoints();
}

int DrawingItemGroup::numberOfItems() const
{
	return mItems.size();
}

bool DrawingItemGroup::containsItem(DrawingItem* item) const
{
	bool result = false;
	if (item) result = mItems.contains(item);
	return result;
}

DrawingItem* DrawingItemGroup::item(int index) const
{
	DrawingItem* item = nullptr;
	if (0 <= index && index < mItems.size()) item = mItems[index];
	return item;
}

//==================================================================================================

void DrawingItemGroup::setItems(const QList<DrawingItem*>& items)
{
	clearItems();
	mItems = items;
	updatePoints();
}

QList<DrawingItem*> DrawingItemGroup::items() const
{
	return mItems;
}

//==================================================================================================

QRectF DrawingItemGroup::boundingRect() const
{
	QRectF rect, itemRect;
	qreal scaleFactor;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			scaleFactor = Drawing::unitsScale((*itemIter)->units(), units());

			itemRect.setTopLeft((*itemIter)->boundingRect().topLeft() * scaleFactor + (*itemIter)->pos());
			itemRect.setBottomRight((*itemIter)->boundingRect().bottomRight() * scaleFactor + (*itemIter)->pos());

			if (!rect.isValid()) rect = itemRect;
			else rect = rect.united(itemRect);
		}
	}

	return rect;
}

//==================================================================================================

void DrawingItemGroup::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			qreal scaleFactor = Drawing::unitsScale((*itemIter)->units(), units());

			painter->save();
			painter->translate((*itemIter)->pos());
			painter->scale(scaleFactor, scaleFactor);
			(*itemIter)->render(painter, styleOptions);
			painter->restore();
		}
	}
}

//==================================================================================================

void DrawingItemGroup::rotateItem(const QPointF& parentPos)
{
	DrawingItem::rotateItem(parentPos);

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateItem(QPointF(0.0, 0.0));
	updatePoints();
}

void DrawingItemGroup::rotateBackItem(const QPointF& parentPos)
{
	DrawingItem::rotateBackItem(parentPos);

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateBackItem(QPointF(0.0, 0.0));
	updatePoints();
}

void DrawingItemGroup::flipItem(const QPointF& parentPos)
{
	DrawingItem::flipItem(parentPos);

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->flipItem(QPointF(0.0, 0.0));
	updatePoints();
}

//==================================================================================================

QVariant DrawingItemGroup::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		DrawingUnits oldUnits = units();

		qreal scaleFactor = Drawing::unitsScale(oldUnits, newUnits);

		if (scaleFactor != 1.0)
		{
			for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
				(*itemIter)->setPos((*itemIter)->pos() * scaleFactor);
		}
	}

	return value;
}

//==================================================================================================

void DrawingItemGroup::writeXmlChildElements(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingItem::writeXmlChildElements(xmlWriter, items);

	xmlWriter.writeStartElement("items");
	DrawingItem::writeItemsToXml(xmlWriter, mItems);
	xmlWriter.writeEndElement();
}

void DrawingItemGroup::readXmlChildElement(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	if (xmlReader.name() == "items")
		mItems = DrawingItem::readItemsFromXml(xmlReader);
	else
		DrawingItem::readXmlChildElement(xmlReader, items);
}

//==================================================================================================

void DrawingItemGroup::updatePoints()
{
	QRectF rect = boundingRect();
	DrawingItemPoint* itemPoint;
	DrawingItemPoint* leftPoint;
	DrawingItemPoint* rightPoint;

	itemPoint = cornerPoint(Qt::TopLeftCorner);
	if (itemPoint) itemPoint->setPos(rect.topLeft());
	itemPoint = cornerPoint(Qt::BottomRightCorner);
	if (itemPoint) itemPoint->setPos(rect.bottomRight());
	itemPoint = cornerPoint(Qt::TopRightCorner);
	if (itemPoint) itemPoint->setPos(rect.topRight());
	itemPoint = cornerPoint(Qt::BottomLeftCorner);
	if (itemPoint) itemPoint->setPos(rect.bottomLeft());

	itemPoint = point(4); leftPoint = point(0); rightPoint = point(2);
	if (itemPoint && leftPoint && rightPoint) itemPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

	itemPoint = point(5); leftPoint = point(1);
	if (itemPoint && leftPoint && rightPoint) itemPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

	itemPoint = point(6); rightPoint = point(3);
	if (itemPoint && leftPoint && rightPoint) itemPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

	itemPoint = point(7); leftPoint = point(0);
	if (itemPoint && leftPoint && rightPoint) itemPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);
}

DrawingItemPoint* DrawingItemGroup::cornerPoint(Qt::Corner corner) const
{
	DrawingItemPoint* itemPoint = nullptr;
	bool lFlipped = isFlipped();
	qreal lRotationAngle = rotationAngle();

	switch (corner)
	{
	case Qt::TopRightCorner:
		if (lFlipped)
		{
			if (lRotationAngle == 90.0) itemPoint = point(3);
			else if (lRotationAngle == 180.0) itemPoint = point(1);
			else if (lRotationAngle == 270.0) itemPoint = point(2);
			else itemPoint = point(0);
		}
		else
		{
			if (lRotationAngle == 90.0) itemPoint = point(0);
			else if (lRotationAngle == 180.0) itemPoint = point(3);
			else if (lRotationAngle == 270.0) itemPoint = point(1);
			else itemPoint = point(2);
		}
		break;
	case Qt::BottomRightCorner:
		if (lFlipped)
		{
			if (lRotationAngle == 90.0) itemPoint = point(1);
			else if (lRotationAngle == 180.0) itemPoint = point(2);
			else if (lRotationAngle == 270.0) itemPoint = point(0);
			else itemPoint = point(3);
		}
		else
		{
			if (lRotationAngle == 90.0) itemPoint = point(2);
			else if (lRotationAngle == 180.0) itemPoint = point(0);
			else if (lRotationAngle == 270.0) itemPoint = point(3);
			else itemPoint = point(1);
		}
		break;
	case Qt::BottomLeftCorner:
		if (lFlipped)
		{
			if (lRotationAngle == 90.0) itemPoint = point(2);
			else if (lRotationAngle == 180.0) itemPoint = point(0);
			else if (lRotationAngle == 270.0) itemPoint = point(3);
			else itemPoint = point(1);
		}
		else
		{
			if (lRotationAngle == 90.0) itemPoint = point(1);
			else if (lRotationAngle == 180.0) itemPoint = point(2);
			else if (lRotationAngle == 270.0) itemPoint = point(0);
			else itemPoint = point(3);
		}
		break;
	default:    // Qt::TopLeftCorner
		if (lFlipped)
		{
			if (lRotationAngle == 90.0) itemPoint = point(0);
			else if (lRotationAngle == 180.0) itemPoint = point(3);
			else if (lRotationAngle == 270.0) itemPoint = point(1);
			else itemPoint = point(2);
		}
		else
		{
			if (lRotationAngle == 90.0) itemPoint = point(3);
			else if (lRotationAngle == 180.0) itemPoint = point(1);
			else if (lRotationAngle == 270.0) itemPoint = point(2);
			else itemPoint = point(0);
		}
		break;
	}

	return itemPoint;
}
