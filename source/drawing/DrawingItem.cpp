/* DrawingItem.cpp
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

#include <DrawingItem.h>
#include <DrawingScene.h>
#include <DrawingView.h>
#include <DrawingItemPoint.h>

DrawingItem::DrawingItem()
{
	mScene = nullptr;

	mPosition = QPointF(0.0, 0.0);
	mUnits = UnitsMils;

	mFlags = CanMove | CanRotate | CanFlip | CanResize | MatchUnitsWithParent;
	mPlaceType = PlaceMouseUp;

	mSelectedPoint = nullptr;
	mParent = nullptr;

	mSelected = false;
	mVisible = true;

	mRotationAngle = 0;
	mFlipped = false;
}

DrawingItem::DrawingItem(const DrawingItem& item)
{
	mScene = nullptr;

	mPosition = item.mPosition;
	mUnits = item.mUnits;

	mFlags = item.mFlags;
	mPlaceType = item.mPlaceType;

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint((*pointIter)->copy());
	mSelectedPoint = nullptr;

	mProperties = item.mProperties;

	for(auto childIter = item.mChildren.begin(); childIter != item.mChildren.end(); childIter++)
		addChild((*childIter)->copy());
	mParent = nullptr;

	mSelected = item.mSelected;
	mVisible = item.mVisible;

	mRotationAngle = item.mRotationAngle;
	mFlipped = item.mFlipped;
}

DrawingItem::~DrawingItem()
{
	mScene = nullptr;
	mSelectedPoint = nullptr;
	mParent = nullptr;

	clearPoints();
	clearChildren();
}

//==================================================================================================

DrawingScene* DrawingItem::scene() const
{
	return mScene;
}

DrawingView* DrawingItem::view() const
{
	return (mScene) ? mScene->view() : nullptr;
}

//==================================================================================================

void DrawingItem::setPos(const QPointF& parentPos)
{
	QPointF posActual = aboutToChangeEvent(PositionChange, QVariant(parentPos)).toPointF();
	mPosition = posActual;
	changedEvent(PositionChange, QVariant(posActual));
}

void DrawingItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void DrawingItem::setX(qreal x)
{
	setPos(QPointF(x, y()));
}

void DrawingItem::setY(qreal y)
{
	setPos(QPointF(x(), y));
}

QPointF DrawingItem::pos() const
{
	return mPosition;
}

qreal DrawingItem::x() const
{
	return mPosition.x();
}

qreal DrawingItem::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItem::setUnits(DrawingUnits units)
{
	DrawingUnits newUnits = (DrawingUnits)aboutToChangeEvent(UnitsChange, QVariant((int)units)).toInt();
	DrawingUnits oldUnits = mUnits;
	qreal scaleFactor = Drawing::unitsScale(oldUnits, newUnits);

	mUnits = newUnits;

	// Adjust position of points
	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() * scaleFactor);

	// Adjust position and units of children
	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
	{
		(*childIter)->setPos((*childIter)->pos() * scaleFactor);
		if ((*childIter)->shouldMatchUnitsWithParent()) (*childIter)->setUnits(units);
	}

	changedEvent(UnitsChange, QVariant((int)newUnits));
}

DrawingUnits DrawingItem::units() const
{
	return mUnits;
}

//==================================================================================================

void DrawingItem::setPlaceType(PlaceType type)
{
	mPlaceType = type;
}

DrawingItem::PlaceType DrawingItem::placeType() const
{
	return mPlaceType;
}

//==================================================================================================

void DrawingItem::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingItem::Flags DrawingItem::flags() const
{
	return mFlags;
}

bool DrawingItem::canMove() const
{
	return (mFlags & CanMove);
}

bool DrawingItem::canRotate() const
{
	return (mFlags & CanRotate);
}

bool DrawingItem::canFlip() const
{
	return (mFlags & CanFlip);
}

bool DrawingItem::canResize() const
{
	return (mFlags & CanResize);
}

bool DrawingItem::canInsertRemovePoints() const
{
	return (mFlags & CanInsertRemovePoints);
}

bool DrawingItem::shouldMatchUnitsWithParent() const
{
	return (mFlags & MatchUnitsWithParent);
}

bool DrawingItem::shouldUsePalette() const
{
	return (mFlags & UsePalette);
}

//==================================================================================================

void DrawingItem::addProperty(const QString& property, const QVariant& value)
{
	mProperties[property] = value;
}

void DrawingItem::removeProperty(const QString& property)
{
	mProperties.remove(property);
}

void DrawingItem::clearProperties()
{
	mProperties.clear();
}

int DrawingItem::numberOfProperties() const
{
	return mProperties.size();
}

bool DrawingItem::containsProperty(const QString& property) const
{
	return mProperties.contains(property);
}

QStringList DrawingItem::propertiesList() const
{
	return mProperties.keys();
}

QList<QVariant> DrawingItem::propertyValues() const
{
	return mProperties.values();
}

//==================================================================================================

void DrawingItem::setPropertyValue(const QString& property, const QVariant& value)
{
	aboutToChangeEvent(PropertyChange, QVariant());
	mProperties[property] = value;
	changedEvent(PropertyChange, QVariant());
}

QVariant DrawingItem::propertyValue(const QString& property) const
{
	QVariant value;
	if (mProperties.contains(property)) value = mProperties[property];
	return value;
}

//==================================================================================================

void DrawingItem::setProperties(const QHash<QString, QVariant>& properties)
{
	aboutToChangeEvent(PropertyChange, QVariant());
	mProperties = properties;
	changedEvent(PropertyChange, QVariant());
}

QHash<QString, QVariant> DrawingItem::properties() const
{
	return mProperties;
}

//==================================================================================================

void DrawingItem::addPoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && !containsPoint(itemPoint))
	{
		mPoints.append(itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* itemPoint)
{
	if (itemPoint && !containsPoint(itemPoint))
	{
		mPoints.insert(index, itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && containsPoint(itemPoint))
	{
		mPoints.removeAll(itemPoint);
		itemPoint->mItem = nullptr;
	}
}

void DrawingItem::clearPoints()
{
	DrawingItemPoint* itemPoint = nullptr;

	while (!mPoints.empty())
	{
		itemPoint = mPoints.first();
		removePoint(itemPoint);
		delete itemPoint;
		itemPoint = nullptr;
	}
}

QList<DrawingItemPoint*> DrawingItem::points() const
{
	return mPoints;
}

int DrawingItem::numberOfPoints() const
{
	return mPoints.size();
}

bool DrawingItem::containsPoint(DrawingItemPoint* itemPoint) const
{
	return (itemPoint) ? mPoints.contains(itemPoint) : false;
}

DrawingItemPoint* DrawingItem::point(int index) const
{
	return (0 <= index && index < mPoints.size()) ? mPoints[index] : nullptr;
}

//==================================================================================================

DrawingItemPoint* DrawingItem::pointAt(const QPointF& itemPos) const
{
	DrawingItemPoint *itemPoint = nullptr;

	for(auto pointIter = mPoints.begin(); itemPoint == nullptr && pointIter != mPoints.end(); pointIter++)
	{
		if ((*pointIter)->itemRect().contains(itemPos))
			itemPoint = *pointIter;
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& itemPos) const
{
	DrawingItemPoint *itemPoint = nullptr;
	qreal minimumDistance = 1E9, distance;

	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
	{
		distance = Drawing::magnitude((*pointIter)->pos() - itemPos);
		if (distance < minimumDistance)
		{
			itemPoint = *pointIter;
			minimumDistance = distance;
		}
	}

	return itemPoint;
}

//==================================================================================================

bool DrawingItem::hasSelectedPoint() const
{
	return (mSelectedPoint != nullptr);
}

DrawingItemPoint* DrawingItem::selectedPoint() const
{
	return mSelectedPoint;
}

//==================================================================================================

void DrawingItem::addChild(DrawingItem* childItem)
{
	if (childItem)
	{
		mChildren.append(childItem);
		childItem->mParent = this;
	}
}

void DrawingItem::insertChild(int index, DrawingItem* childItem)
{
	if (childItem)
	{
		mChildren.insert(index, childItem);
		childItem->mParent = this;
	}
}

void DrawingItem::removeChild(DrawingItem* childItem)
{
	if (childItem)
	{
		mChildren.removeAll(childItem);
		childItem->mParent = nullptr;
	}
}

void DrawingItem::clearChildren()
{
	DrawingItem* childItem = nullptr;

	while (!mChildren.empty())
	{
		childItem = mChildren.first();
		removeChild(childItem);
		delete childItem;
		childItem = nullptr;
	}
}

QList<DrawingItem*> DrawingItem::children() const
{
	return mChildren;
}

int DrawingItem::numberOfChildren() const
{
	return mChildren.size();
}

bool DrawingItem::containsChild(DrawingItem* child) const
{
	return (child) ? mChildren.contains(child) : false;
}

DrawingItem* DrawingItem::child(int index) const
{
	return (0 <= index && index < mChildren.size()) ? mChildren[index] : nullptr;
}

//==================================================================================================

DrawingItem* DrawingItem::parent() const
{
	return mParent;
}

//==================================================================================================

void DrawingItem::setSelected(bool select)
{
	select = aboutToChangeEvent(SelectedChange, QVariant(select)).toBool();

	mSelected = select;
	if (!mSelected) mSelectedPoint = nullptr;

	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
		(*childIter)->setSelected(select);

	changedEvent(SelectedChange, QVariant(select));
}

void DrawingItem::setVisible(bool visible)
{
	visible = aboutToChangeEvent(VisibleChange, QVariant(visible)).toBool();

	mVisible = visible;

	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
		(*childIter)->setVisible(visible);

	changedEvent(VisibleChange, QVariant(visible));
}

bool DrawingItem::isSelected() const
{
	return mSelected;
}

bool DrawingItem::isVisible() const
{
	return mVisible;
}

//==================================================================================================

void DrawingItem::setRotationAngle(qreal angle)
{
	mRotationAngle = angle;
	while (mRotationAngle >= 360.0) mRotationAngle -= 360.0;
	while (mRotationAngle < 0.0) mRotationAngle += 360.0;
}

void DrawingItem::setFlipped(bool flipped)
{
	mFlipped = flipped;
}

qreal DrawingItem::rotationAngle() const
{
	return mRotationAngle;
}

bool DrawingItem::isFlipped() const
{
	return mFlipped;
}

//==================================================================================================

QPointF DrawingItem::mapToParent(const QPointF& itemPos) const
{
	QPointF parentPos = itemPos;

	if (mParent) parentPos = itemPos / Drawing::unitsScale(mUnits, mParent->units()) + mPosition;
	else if (mScene) parentPos = itemPos / Drawing::unitsScale(mUnits, mScene->units()) + mPosition;

	return parentPos;
}

QPointF DrawingItem::mapFromParent(const QPointF& parentPos) const
{
	QPointF itemPos = parentPos;

	if (mParent) itemPos = (parentPos - mPosition) * Drawing::unitsScale(mUnits, mParent->units());
	else if (mScene) itemPos = (parentPos - mPosition) * Drawing::unitsScale(mUnits, mScene->units());

	return itemPos;
}

QRectF DrawingItem::mapToParent(const QRectF& itemRect) const
{
	return QRectF(mapToParent(itemRect.topLeft()), mapToParent(itemRect.bottomRight()));
}

QRectF DrawingItem::mapFromParent(const QRectF& parentRect) const
{
	return QRectF(mapFromParent(parentRect.topLeft()), mapFromParent(parentRect.bottomRight()));
}

//==================================================================================================

QPointF DrawingItem::mapToScene(const QPointF& itemPos) const
{
	QPointF scenePos = itemPos;

	if (mParent) scenePos = mParent->mapToScene(mapToParent(itemPos));
	else if (mScene) scenePos = itemPos / Drawing::unitsScale(mUnits, mScene->units()) + mPosition;

	return scenePos;
}

QPointF DrawingItem::mapFromScene(const QPointF& scenePos) const
{
	QPointF itemPos = scenePos;

	if (mParent) itemPos = mapFromParent(mParent->mapFromScene(scenePos));
	else if (mScene) itemPos = (scenePos - mPosition) * Drawing::unitsScale(mUnits, mScene->units());

	return itemPos;
}

QRectF DrawingItem::mapToScene(const QRectF& itemRect) const
{
	return QRectF(mapToScene(itemRect.topLeft()), mapToScene(itemRect.bottomRight()));
}

QRectF DrawingItem::mapFromScene(const QRectF& sceneRect) const
{
	return QRectF(mapFromScene(sceneRect.topLeft()), mapFromScene(sceneRect.bottomRight()));
}

//==================================================================================================

QPainterPath DrawingItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QPointF DrawingItem::centerPos() const
{
	return boundingRect().center();
}

bool DrawingItem::isSuperfluous() const
{
	return false;
}

//==================================================================================================

void DrawingItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	if (itemPoint) itemPoint->setPos(mapFromParent(parentPos));
}

void DrawingItem::rotateItem(const QPointF& parentPos)
{
	QPointF difference(mPosition - parentPos);

	// Calculate new position of reference point
	setPos(parentPos.x() - difference.y(), parentPos.y() + difference.x());

	// Update orientation
	if (mFlipped) setRotationAngle(mRotationAngle - 90.0);
	else setRotationAngle(mRotationAngle + 90.0);

	// Calculate new offsets of control points
	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
		(*pointIter)->setPos(-(*pointIter)->y(), (*pointIter)->x());

	// Rotate child items as well
	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
		(*childIter)->rotateItem(QPointF(0.0, 0.0));
}

void DrawingItem::rotateBackItem(const QPointF& parentPos)
{
	QPointF difference(mPosition - parentPos);

	// Calculate new position of reference point
	setPos(parentPos.x() + difference.y(), parentPos.y() - difference.x());

	// Update orientation
	if (mFlipped) setRotationAngle(mRotationAngle + 90.0);
	else setRotationAngle(mRotationAngle - 90.0);

	// Calculate new offsets of control points
	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->y(), -(*pointIter)->x());

	// Rotate child items as well
	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
		(*childIter)->rotateBackItem(QPointF(0.0, 0.0));
}

void DrawingItem::flipItem(const QPointF& parentPos)
{
	// Calculate new position of reference point
	setPos(2 * parentPos.x() - x(), y());

	// Update orientation
	mFlipped = !mFlipped;

	// Calculate new offsets of control points
	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
		(*pointIter)->setPos(-(*pointIter)->x(), (*pointIter)->y());

	// Flip child items as well
	for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
		(*childIter)->flipItem(QPointF(0.0, 0.0));
}

//==================================================================================================

void DrawingItem::insertItemPoint(int index, DrawingItemPoint* itemPoint)
{
	insertPoint(index, itemPoint);
}

void DrawingItem::removeItemPoint(DrawingItemPoint* itemPoint)
{
	removePoint(itemPoint);
}

bool DrawingItem::canInsertPoint(DrawingItemPoint* itemPoint) const
{
	return (canInsertRemovePoints() && itemPoint && !containsPoint(itemPoint));
}

bool DrawingItem::canRemovePoint(DrawingItemPoint* itemPoint) const
{
	return (canInsertRemovePoints() && itemPoint && containsPoint(itemPoint) &&	itemPoint->numberOfTargets() == 0);
}

int DrawingItem::insertPointIndex(DrawingItemPoint* itemPoint) const
{
	Q_UNUSED(itemPoint);
	return -1;
}

//==================================================================================================

void DrawingItem::contextMenuEvent(DrawingMouseEvent& event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseDoubleClickEvent(DrawingMouseEvent& event)
{
	Q_UNUSED(event);
}

void DrawingItem::mousePressEvent(DrawingMouseEvent& event)
{
	// Update mSelectedPoint
	if (mParent == nullptr)
	{
		mSelectedPoint = pointAt(mapFromScene(event.scenePos()));

		if (mSelectedPoint)
		{
			if ((mSelectedPoint->flags() & DrawingItemPoint::Control) == 0) mSelectedPoint = nullptr;
			else if ((flags() & CanResize) == 0) mSelectedPoint = nullptr;
		}
	}
	else mSelectedPoint = nullptr;
}

void DrawingItem::mouseMoveEvent(DrawingMouseEvent& event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseReleaseEvent(DrawingMouseEvent& event)
{
	Q_UNUSED(event);
}

//==================================================================================================

bool DrawingItem::createEvent(DrawingView* view)
{
	Q_UNUSED(view);
	return true;
}

QVariant DrawingItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(reason);
	return value;
}

void DrawingItem::changedEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(reason);
	Q_UNUSED(value);
}

//==================================================================================================

void DrawingItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	Q_UNUSED(items);

	xmlWriter.writeAttribute("x", QString::number(x()));
	xmlWriter.writeAttribute("y", QString::number(y()));
	xmlWriter.writeAttribute("units", QString::number((quint16)units()));
	xmlWriter.writeAttribute("flags", QString::number((quint32)flags()));
	xmlWriter.writeAttribute("placeType", QString::number((quint16)placeType()));
	xmlWriter.writeAttribute("visible", (isVisible()) ? "true" : "false");
	xmlWriter.writeAttribute("rotationAngle", QString::number(rotationAngle()));
	xmlWriter.writeAttribute("flipped", (isFlipped()) ? "true" : "false");

	// Any properties should be saved in derived class writeXmlAttributes
}

void DrawingItem::writeXmlChildElements(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	QList<DrawingItemPoint*> points = DrawingItem::points();
	DrawingItemPoint* targetItemPoint;
	DrawingItem* targetItem;
	int itemIndex, targetItemIndex;
	QString pointConnections;

	// Points
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
	{
		xmlWriter.writeStartElement("itemPoint");

		xmlWriter.writeAttribute("x", QString::number((*pointIter)->x()));
		xmlWriter.writeAttribute("y", QString::number((*pointIter)->y()));
		xmlWriter.writeAttribute("size", QString::number((*pointIter)->size()));
		xmlWriter.writeAttribute("flags", QString::number((quint32)(*pointIter)->flags()));
		xmlWriter.writeAttribute("category", QString::number((*pointIter)->category()));

		// Targets
		pointConnections = QString();
		itemIndex = items.indexOf(this);
		if (itemIndex >= 0)
		{
			for(int i = 0; i < (*pointIter)->numberOfTargets(); i++)
			{
				targetItemPoint = (*pointIter)->target(i);
				targetItem = targetItemPoint->item();

				if (targetItem) targetItemIndex = items.indexOf(targetItem);
				else targetItemIndex = -1;

				if (targetItemIndex >= 0)
				{
					pointConnections += QString::number(targetItemIndex) + ",";
					pointConnections += QString::number(targetItem->points().indexOf(targetItemPoint)) + ",";
				}
			}
		}
		pointConnections = pointConnections.left(pointConnections.size() - 1);

		if (!pointConnections.isEmpty())
			xmlWriter.writeAttribute("connections", pointConnections);

		xmlWriter.writeEndElement();
	}

	// Children should be saved in derived class writeXmlChildElements
}

void DrawingItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();
	Q_UNUSED(items);

	if (attributes.hasAttribute("units"))
		setUnits((DrawingUnits)attributes.value("units").toString().toUShort());

	if (attributes.hasAttribute("x"))
		setX(attributes.value("x").toString().toDouble());
	if (attributes.hasAttribute("y"))
		setY(attributes.value("y").toString().toDouble());

	if (attributes.hasAttribute("flags"))
		setFlags((Flags)attributes.value("flags").toString().toUInt());
	if (attributes.hasAttribute("placeType"))
		setPlaceType((PlaceType)attributes.value("placeType").toString().toUShort());

	if (attributes.hasAttribute("visible"))
		setVisible(attributes.value("visible").toString().toLower() == "true");

	if (attributes.hasAttribute("rotationAngle"))
		setRotationAngle(attributes.value("rotationAngle").toString().toDouble());
	if (attributes.hasAttribute("flipped"))
		setFlipped(attributes.value("flipped").toString().toLower() == "true");

	// Any properties should be saved in derived class writeXmlAttributes
}

void DrawingItem::readXmlChildElement(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	if (xmlReader.name() == "itemPoint")
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint();
		QXmlStreamAttributes attributes = xmlReader.attributes();

		QStringList pointConnections;
		int itemIndex, pointIndex;
		DrawingItem* targetItem;
		DrawingItemPoint* targetItemPoint;

		if (attributes.hasAttribute("x"))
			newPoint->setX(attributes.value("x").toString().toDouble());
		if (attributes.hasAttribute("y"))
			newPoint->setY(attributes.value("y").toString().toDouble());

		if (attributes.hasAttribute("size"))
			newPoint->setSize(attributes.value("size").toString().toDouble());

		if (attributes.hasAttribute("flags"))
			newPoint->setFlags((DrawingItemPoint::Flags)attributes.value("flags").toString().toUInt());
		if (attributes.hasAttribute("category"))
			newPoint->setCategory(attributes.value("category").toString().toInt());

		if (attributes.hasAttribute("connections"))
		{
			pointConnections = attributes.value("connections").toString().split(
				",", QString::SkipEmptyParts);

			for(int i = 0; i < pointConnections.size(); i += 2)
			{
				itemIndex = pointConnections[i].toInt();
				pointIndex = pointConnections[i+1].toInt();

				if (itemIndex < items.size())
				{
					targetItem = items[itemIndex];
					if (pointIndex < targetItem->numberOfPoints())
					{
						targetItemPoint = targetItem->point(pointIndex);

						newPoint->addTarget(targetItemPoint);
						targetItemPoint->addTarget(newPoint);
					}
				}
			}
		}

		xmlReader.skipCurrentElement();

		addPoint(newPoint);
	}
	else xmlReader.skipCurrentElement();

	// Children should be saved in derived class readXmlChildElements
}

//==================================================================================================

void DrawingItem::setupPainter(QPainter* painter,
	const DrawingStyleOptions& styleOptions, const QPen& itemPen, const QBrush& itemBrush)
{
	QPen pen = itemPen;
	QBrush penBrush = pen.brush();
	QBrush brush = itemBrush;

	if (flags() & UsePalette)
	{
		QColor color;
		int alpha = brush.color().alpha();

		if (isSelected())
			brush = styleOptions.outputBrush(DrawingStyleOptions::AlternateItem);
		else
			brush = styleOptions.outputBrush(DrawingStyleOptions::Item);

		penBrush = brush;

		color = brush.color();
		color.setAlpha(alpha);
		brush.setColor(color);
	}

	switch (styleOptions.colorMode())
	{
	case DrawingStyleOptions::BlackOnWhite:
		if (brush.color().alpha() != 0) brush.setColor(QColor(255, 255, 255));
		penBrush.setColor(QColor(0, 0 ,0));
		break;

	case DrawingStyleOptions::WhiteOnBlack:
		if (brush.color().alpha() != 0) brush.setColor(QColor(0, 0, 0));
		penBrush.setColor(QColor(255, 255, 255));
		break;

	default: break;
	}

	painter->setBrush(brush);

	pen.setBrush(penBrush);
	painter->setPen(pen);
}

QPainterPath DrawingItem::itemShapeFromPath(const QPainterPath& path, const QPen& pen, bool adjustOutline) const
{
	QPainterPath shape;

	if (path != QPainterPath())
	{
		QPainterPathStroker painterPathStroker;

		if (pen.widthF() > 0)
		{
			if (adjustOutline)
				painterPathStroker.setWidth(adjustOutlineForView(pen.widthF()));
			else
				painterPathStroker.setWidth(pen.widthF());
		}
		else
			painterPathStroker.setWidth(0.00000001);

		painterPathStroker.setCapStyle(pen.capStyle());
		painterPathStroker.setJoinStyle(pen.joinStyle());
		painterPathStroker.setMiterLimit(pen.miterLimit());

		shape = painterPathStroker.createStroke(path);
	}

	return shape;
}

qreal DrawingItem::adjustOutlineForView(qreal penWidth) const
{
	qreal adjustedWidth = penWidth;
	DrawingView* view = DrawingItem::view();

	if (view)
	{
		QSize lineViewSize = view->mapFromScene(mapToScene(QRectF(0, 0, penWidth, penWidth))).size();
		if (lineViewSize.width() < 8 && lineViewSize.height() < 8) adjustedWidth *= 4;
	}

	return adjustedWidth;
}

void DrawingItem::adjustReferencePoint()
{
	// Adjust position of item, points, and children so that point(0)->pos() == QPointF(0, 0)
	if (!mPoints.isEmpty() && mPoints.first())
	{
		QPointF deltaPos = -mPoints.first()->pos();
		QPointF parentPos = mapToParent(mPoints.first()->pos());

		for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
			(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

		for(auto childIter = mChildren.begin(); childIter != mChildren.end(); childIter++)
			(*childIter)->setPos((*childIter)->pos() + deltaPos);

		setPos(parentPos);
	}
}

//==================================================================================================

DrawingItem& DrawingItem::operator=(const DrawingItem& item)
{
	mScene = nullptr;

	mPosition = item.mPosition;
	mUnits = item.mUnits;

	mFlags = item.mFlags;
	mPlaceType = item.mPlaceType;

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint((*pointIter)->copy());
	mSelectedPoint = nullptr;

	mProperties = item.mProperties;

	for(auto childIter = item.mChildren.begin(); childIter != item.mChildren.end(); childIter++)
		addChild((*childIter)->copy());
	mParent = nullptr;

	mSelected = item.mSelected;
	mVisible = item.mVisible;

	mRotationAngle = item.mRotationAngle;
	mFlipped = item.mFlipped;

	return *this;
}

//==================================================================================================

QList<DrawingItem*> DrawingItem::copyItems(const QList<DrawingItem*>& items)
{
	QList<DrawingItem*> copiedItems;
	QList<DrawingItemPoint*> itemPoints;
	QList<DrawingItemPoint*> targetPoints;
	DrawingItem* targetItem;
	DrawingItem* copiedTargetItem;
	DrawingItemPoint* copiedTargetPoint;

	// Copy items
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		copiedItems.append((*itemIter)->copy());

	// Maintain connections to other items in this list
	for(int itemIndex = 0; itemIndex < items.size(); itemIndex++)
	{
		itemPoints = items[itemIndex]->points();
		for(int pointIndex = 0; pointIndex < itemPoints.size(); pointIndex++)
		{
			targetPoints = itemPoints[pointIndex]->targets();
			for(auto targetIter = targetPoints.begin(); targetIter != targetPoints.end(); targetIter++)
			{
				targetItem = (*targetIter)->item();
				if (items.contains(targetItem))
				{
					// There is a connection here that must be maintained in the copied items
					copiedTargetItem = copiedItems[items.indexOf(targetItem)];
					copiedTargetPoint =
						copiedTargetItem->point(targetItem->points().indexOf(*targetIter));

					copiedItems[itemIndex]->point(pointIndex)->addTarget(copiedTargetPoint);
				}
			}
		}

	}

	return copiedItems;
}

//==================================================================================================

void DrawingItem::writeItemsToXml(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		if ((*itemIter)->parent() == nullptr)
		{
			xmlWriter.writeStartElement((*itemIter)->uniqueKey());
			(*itemIter)->writeXmlAttributes(xmlWriter, items);
			(*itemIter)->writeXmlChildElements(xmlWriter, items);
			xmlWriter.writeEndElement();
		}
	}
}

QList<DrawingItem*> DrawingItem::readItemsFromXml(QXmlStreamReader& xmlReader)
{
	QList<DrawingItem*> items;
	DrawingItem* item;

	while (xmlReader.readNextStartElement())
	{
		item = DrawingView::itemFactory.create(xmlReader.name().toString());
		if (item)
		{
			item->clearPoints();

			item->readXmlAttributes(xmlReader, items);

			while (xmlReader.readNextStartElement())
				item->readXmlChildElement(xmlReader, items);

			items.append(item);
		}
	}

	return items;
}
