/* DrawingItemPoint.cpp
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

#include <DrawingItemPoint.h>
#include <DrawingView.h>
#include <DrawingScene.h>
#include <DrawingItem.h>

DrawingItemPoint::DrawingItemPoint(const QPointF& position, Flags flags, int category)
{
	mItem = nullptr;

	mPosition = position;
	mSize = 4;
	mFlags = flags;
	mCategory = category;
}

DrawingItemPoint::DrawingItemPoint(const DrawingItemPoint& point)
{
	mItem = nullptr;

	mPosition = point.mPosition;
	mSize = point.mSize;
	mFlags = point.mFlags;
	mCategory = point.mCategory;
}

DrawingItemPoint::~DrawingItemPoint()
{
	clearTargets();
}

//==================================================================================================

DrawingItemPoint* DrawingItemPoint::copy() const
{
	return new DrawingItemPoint(*this);
}

//==================================================================================================

DrawingItem* DrawingItemPoint::item() const
{
	return mItem;
}

//==================================================================================================

void DrawingItemPoint::setPos(const QPointF& pos)
{
	mPosition = pos;
}

void DrawingItemPoint::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void DrawingItemPoint::setX(qreal x)
{
	setPos(QPointF(x, y()));
}

void DrawingItemPoint::setY(qreal y)
{
	setPos(QPointF(x(), y));
}

QPointF DrawingItemPoint::pos() const
{
	return mPosition;
}

qreal DrawingItemPoint::x() const
{
	return mPosition.x();
}

qreal DrawingItemPoint::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItemPoint::setSize(int size)
{
	mSize = size;
}

int DrawingItemPoint::size() const
{
	return mSize;
}

//==================================================================================================

void DrawingItemPoint::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingItemPoint::Flags DrawingItemPoint::flags() const
{
	return mFlags;
}

bool DrawingItemPoint::isControlPoint() const
{
	return ((mFlags & Control) > 0);
}

bool DrawingItemPoint::isConnectionPoint() const
{
	return ((mFlags & Connection) > 0);
}

bool DrawingItemPoint::isFree() const
{
	return ((mFlags & Free) > 0);
}

//==================================================================================================

void DrawingItemPoint::setCategory(int category)
{
	mCategory = category;
}

int DrawingItemPoint::category() const
{
	return mCategory;
}

//==================================================================================================

void DrawingItemPoint::addTarget(DrawingItemPoint* point)
{
	if (point && !isTarget(point)) mTargets.append(point);
}

void DrawingItemPoint::removeTarget(DrawingItemPoint* point)
{
	if (point) mTargets.removeAll(point);
}

void DrawingItemPoint::clearTargets()
{
	DrawingItemPoint* point;

	while (numberOfTargets() > 0)
	{
		point = mTargets.first();

		removeTarget(point);
		point->removeTarget(this);
	}
}

QList<DrawingItemPoint*> DrawingItemPoint::targets() const
{
	return mTargets;
}

int DrawingItemPoint::numberOfTargets() const
{
	return mTargets.size();
}

bool DrawingItemPoint::isTarget(DrawingItemPoint* point) const
{
	return (point) ? mTargets.contains(point) : false;
}

bool DrawingItemPoint::isTarget(DrawingItem* item) const
{
	bool target = false;

	if (item)
	{
		for(auto pointIter = mTargets.begin(); !target && pointIter != mTargets.end(); pointIter++)
			target = ((*pointIter)->item() == item);
	}

	return target;
}

DrawingItemPoint* DrawingItemPoint::target(int index) const
{
	return (0 <= index && index < numberOfTargets()) ? mTargets[index] : nullptr;
}

//==================================================================================================

QRectF DrawingItemPoint::sceneRect() const
{
	QRectF rect;
	DrawingScene* scene = (mItem) ? mItem->scene() : nullptr;
	DrawingView* view = (scene) ? scene->view() : nullptr;

	if (mItem && view)
	{
		QPoint centerPoint = view->mapFromScene(mItem->mapToScene(pos()));
		QPoint delta(mSize, mSize);
		rect = QRectF(view->mapToScene(centerPoint - delta), view->mapToScene(centerPoint + delta));
	}

	return rect;
}

QRectF DrawingItemPoint::itemRect() const
{
	return (mItem) ? mItem->mapFromScene(sceneRect()) : QRectF();
}

//==================================================================================================

bool DrawingItemPoint::shouldConnect(DrawingItemPoint* otherItemPoint, DrawingItemPlaceMode placeMode) const
{
	// Assume both points are members of different items in the same scene
	bool result = false;
	DrawingScene* scene = mItem->scene();

	if (scene && placeMode != DoNotPlace)
	{
		//qreal threshold = (placeMode == PlaceLoose) ? scene->grid() / 2 : 0.001;
		qreal threshold = (placeMode == PlaceLoose) ? scene->grid() / 4 : 0.001;
		qreal distance = Drawing::magnitude(mItem->mapToScene(pos()) -
			otherItemPoint->item()->mapToScene(otherItemPoint->pos()));

		result = (isConnectionPoint() && otherItemPoint->isConnectionPoint() &&
			(isFree() || otherItemPoint->isFree()) && category() == otherItemPoint->category() &&
			!isTarget(otherItemPoint) && !isTarget(otherItemPoint->item()) && distance < threshold);
	}

	return result;
}

bool DrawingItemPoint::shouldDisconnect(DrawingItemPoint* otherItemPoint) const
{
	return !(mItem->mapToScene(pos()) !=
		otherItemPoint->item()->mapToScene(otherItemPoint->pos()) &&
		otherItemPoint->item()->canResize() && otherItemPoint->isControlPoint());
}

//==================================================================================================

DrawingItemPoint& DrawingItemPoint::operator=(const DrawingItemPoint& point)
{
	mItem = nullptr;

	mPosition = point.mPosition;
	mSize = point.mSize;
	mFlags = point.mFlags;
	mCategory = point.mCategory;

	return *this;
}
