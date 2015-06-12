/* DrawingItemPoint.h
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

#ifndef DRAWINGITEMPOINT_H
#define DRAWINGITEMPOINT_H

#include <DrawingTypes.h>

/* A DrawingItemPoint represents a point of interest within a DrawingPointsItem.
 *
 * Often clicking an item will bring up a small box in each corner so that the width or height of
 * the item can be changed by clicking and dragging.  Control points are provided for this purpose.
 *
 * Sometimes a user may want to connect two items together, such as two lines for example.  When
 * one of the lines is moved, the other is resized so as to maintain the connection visually.
 * Connection points are provided for this purpose.
 *
 * Free points are used to indicate whether a particular point is free to be resized in order to
 * try to maintain a connection visually.  This typically only applies to points that are both
 * Control and Connection points.
 *
 * The position of the point within its parent item is given in terms of the item's local
 * coordinates.
 */
class DrawingItemPoint
{
	friend class DrawingItem;

public:
	enum Flag { Control = 0x01, Connection = 0x02, Free = 0x04 };
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingItem* mItem;

	QPointF mPosition;
	int mSize;
	int mCategory;
	Flags mFlags;

	QList<DrawingItemPoint*> mTargets;

public:
	DrawingItemPoint(const QPointF& position = QPointF(), Flags flags = Control, int category = 0);
	DrawingItemPoint(const DrawingItemPoint& other);
	virtual ~DrawingItemPoint();

	virtual DrawingItemPoint* copy() const;

	DrawingItem* item() const;

	// Properties
	void setPos(const QPointF& pos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setSize(int size);
	int size() const;

	void setFlags(Flags flags);
	Flags flags() const;
	bool isControlPoint() const;
	bool isConnectionPoint() const;
	bool isFree() const;

	void setCategory(int category);
	int category() const;

	// Targets
	void addTarget(DrawingItemPoint* itemPoint);
	void removeTarget(DrawingItemPoint* itemPoint);
	void clearTargets();
	QList<DrawingItemPoint*> targets() const;
	int numberOfTargets() const;
	bool isTarget(DrawingItemPoint* itemPoint) const;
	bool isTarget(DrawingItem* item) const;
	DrawingItemPoint* target(int index) const;

	// Convenience functions
	virtual QRectF sceneRect() const;
	virtual QRectF itemRect() const;

	virtual bool shouldConnect(DrawingItemPoint* otherItemPoint, DrawingItemPlaceMode placeMode) const;
	virtual bool shouldDisconnect(DrawingItemPoint* otherItemPoint) const;

	DrawingItemPoint& operator=(const DrawingItemPoint& other);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItemPoint::Flags)

#endif
