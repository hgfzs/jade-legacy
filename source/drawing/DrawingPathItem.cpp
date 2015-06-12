/* DrawingPathItem.cpp
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

#include <DrawingPathItem.h>
#include <DrawingItemPoint.h>

DrawingPathItem::DrawingPathItem() : DrawingRectResizeItem()
{
	QRect rect(-100, -100, 200, 200);

	mUniqueKey = "path";

	setPlaceType(PlaceMouseUp);

	for(int i = 0; i < 8; i++) point(i)->setFlags(DrawingItemPoint::Control);

	point(0)->setPos(rect.left(), rect.top());
	point(1)->setPos(rect.right(), rect.bottom());
	point(2)->setPos(rect.right(), rect.top());
	point(3)->setPos(rect.left(), rect.bottom());
	point(4)->setPos((point(0)->pos() + point(2)->pos()) / 2);
	point(5)->setPos((point(2)->pos() + point(1)->pos()) / 2);
	point(6)->setPos((point(1)->pos() + point(3)->pos()) / 2);
	point(7)->setPos((point(3)->pos() + point(0)->pos()) / 2);
}

DrawingPathItem::DrawingPathItem(const DrawingPathItem& item) : DrawingRectResizeItem(item)
{
	mPath = item.mPath;
	mUniqueKey = item.mUniqueKey;
}

DrawingPathItem::~DrawingPathItem() { }

//==================================================================================================

DrawingItem* DrawingPathItem::copy() const
{
	return new DrawingPathItem(*this);
}

//==================================================================================================

void DrawingPathItem::setUniqueKey(const QString& key)
{
	mUniqueKey = key;
}

QString DrawingPathItem::uniqueKey() const
{
	return mUniqueKey;
}

//==================================================================================================

void DrawingPathItem::setPath(const QPainterPath& path)
{
	mPath = path;
}

QPainterPath DrawingPathItem::path() const
{
	return mPath;
}

//==================================================================================================

QPointF DrawingPathItem::mapToPath(const QPointF& itemPos) const
{
	QPointF newPoint;

	QRectF pathRect = mPath.boundingRect();
	QRectF bounds = boundingRect();
	QPointF topLeft = DrawingItem::point(0)->pos();
	QSizeF size;
	qreal cosAngle, sinAngle;
	qreal ratioX, ratioY;

	if (rotationAngle() == 90 || rotationAngle() == 270)
	{
		size.setWidth(bounds.height());
		size.setHeight(bounds.width());
	}
	else size = bounds.size();

	if (rotationAngle() == 90.0) { cosAngle = 0; sinAngle = 1; }
	else if (rotationAngle() == 180.0) { cosAngle = -1; sinAngle = 0; }
	else if (rotationAngle() == 270.0) { cosAngle = 0; sinAngle = -1; }
	else { cosAngle = 1; sinAngle = 0; }

	if (isFlipped())
	{
		ratioX = (-cosAngle * (itemPos.x() - topLeft.x()) + sinAngle * (itemPos.y() - topLeft.y())) / size.width();
		ratioY = (sinAngle * (itemPos.x() - topLeft.x()) + cosAngle * (itemPos.y() - topLeft.y())) / size.height();
	}
	else
	{
		ratioX = (cosAngle * (itemPos.x() - topLeft.x()) + sinAngle * (itemPos.y() - topLeft.y())) / size.width();
		ratioY = (-sinAngle * (itemPos.x() - topLeft.x()) + cosAngle * (itemPos.y() - topLeft.y())) / size.height();
	}

	newPoint.setX(ratioX * pathRect.width() + pathRect.left());
	newPoint.setY(ratioY * pathRect.height() + pathRect.top());

	return newPoint;
}

QRectF DrawingPathItem::mapToPath(const QRectF& itemRect) const
{
	return QRectF(mapToPath(itemRect.topLeft()), mapToPath(itemRect.bottomRight()));
}

QPointF DrawingPathItem::mapFromPath(const QPointF& pathPos) const
{
	QPointF newPoint;

	QRectF pathRect = mPath.boundingRect();
	QRectF bounds = boundingRect();
	QPointF topLeft = DrawingItem::point(0)->pos();
	QSizeF size;
	qreal cosAngle, sinAngle;
	qreal ratioX, ratioY;

	if (rotationAngle() == 90 || rotationAngle() == 270)
	{
		size.setWidth(bounds.height());
		size.setHeight(bounds.width());
	}
	else size = bounds.size();

	if (rotationAngle() == 90.0) { cosAngle = 0; sinAngle = 1; }
	else if (rotationAngle() == 180.0) { cosAngle = -1; sinAngle = 0; }
	else if (rotationAngle() == 270.0) { cosAngle = 0; sinAngle = -1; }
	else { cosAngle = 1; sinAngle = 0; }

	ratioX = (pathPos.x() - pathRect.left()) / pathRect.width();
	ratioY = (pathPos.y() - pathRect.top()) / pathRect.height();

	if (isFlipped())
		newPoint.setX(topLeft.x() - cosAngle * size.width() * ratioX + sinAngle * size.height() * ratioY);
	else
		newPoint.setX(topLeft.x() + cosAngle * size.width() * ratioX - sinAngle * size.height() * ratioY);

	newPoint.setY(topLeft.y() + sinAngle * size.width() * ratioX + cosAngle * size.height() * ratioY);

	return newPoint;
}

QRectF DrawingPathItem::mapFromPath(const QRectF& pathRect) const
{
	return QRectF(mapFromPath(pathRect.topLeft()), mapFromPath(pathRect.bottomRight()));
}

//==================================================================================================

QRectF DrawingPathItem::boundingRect() const
{
	return Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());
}

//==================================================================================================

void DrawingPathItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QPainterPath transformedPath;
	QList<QPointF> curveDataPoints;

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif

	for(int i = 0; i < mPath.elementCount(); i++)
	{
		QPainterPath::Element element = mPath.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			transformedPath.moveTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::LineToElement:
			transformedPath.lineTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToElement:
			curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToDataElement:
			if (curveDataPoints.size() >= 2)
			{
				transformedPath.cubicTo(curveDataPoints[0], curveDataPoints[1],
					mapFromPath(QPointF(element.x, element.y)));
				curveDataPoints.pop_front();
				curveDataPoints.pop_front();
			}
			else curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		}
	}

	setupPainter(painter, styleOptions, pen());
	painter->drawPath(transformedPath);
}

//==================================================================================================

void DrawingPathItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingRectResizeItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("path", Drawing::pathToString(path()));
}

void DrawingPathItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingRectResizeItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("path"))
		setPath(Drawing::pathFromString(attributes.value("path").toString()));
}

//==================================================================================================

void DrawingPathItem::setInitialPath(const QPainterPath& path)
{
	QRectF rect = path.boundingRect();

	point(0)->setPos(rect.left(), rect.top());
	point(1)->setPos(rect.right(), rect.bottom());
	point(2)->setPos(rect.right(), rect.top());
	point(3)->setPos(rect.left(), rect.bottom());
	point(4)->setPos((point(0)->pos() + point(2)->pos()) / 2);
	point(5)->setPos((point(2)->pos() + point(1)->pos()) / 2);
	point(6)->setPos((point(1)->pos() + point(3)->pos()) / 2);
	point(7)->setPos((point(3)->pos() + point(0)->pos()) / 2);

	setPath(path);
}

void DrawingPathItem::addConnectionPoint(const QPointF& itemPos)
{
	bool pointFound = false;

	QList<DrawingItemPoint*> lPoints = points();
	for(auto iter = lPoints.begin(); !pointFound && iter != lPoints.end(); iter++)
	{
		pointFound = ((*iter)->pos() == itemPos);
		if (pointFound) (*iter)->setFlags((*iter)->flags() | DrawingItemPoint::Connection);
	}

	if (!pointFound)
		addPoint(new DrawingItemPoint(itemPos, DrawingItemPoint::Connection, 0));
}

void DrawingPathItem::addConnectionPoint(qreal x, qreal y)
{
	addConnectionPoint(QPointF(x, y));
}

//==================================================================================================

void DrawingPathItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	if (point == nullptr) point = selectedPoint();
	if (point)
	{
		DrawingItemPoint* startPoint = DrawingItem::point(0);
		DrawingItemPoint* endPoint = DrawingItem::point(1);

		QPointF originalPos = mapToScene(point->pos());
		QPointF actualPos = parentPos;
		bool wasEndXGreater = (startPoint->x() < endPoint->x());
		bool wasEndYGreater = (startPoint->y() < endPoint->y());
		QList<QPointF> additionalConnectionPoints;

		// Save locations of "additional" connection points
		for(int i = 8; i < numberOfPoints(); i++)
			additionalConnectionPoints.append(mapToPath(DrawingItem::point(i)->pos()));

		DrawingItem::resizeItem(point, parentPos);
		adjustBoxControlPoints(point);
		adjustReferencePoint();
		for(int i = 8; i < numberOfPoints(); i++)
			DrawingItem::point(i)->setPos(mapFromPath(additionalConnectionPoints[i-8]));

		if ((wasEndXGreater && startPoint->x() >= endPoint->x()) ||
			(!wasEndXGreater && startPoint->x() <= endPoint->x()))
		{
			actualPos.setX(originalPos.x());
		}
		if ((wasEndYGreater && startPoint->y() >= endPoint->y()) ||
			(!wasEndYGreater && startPoint->y() <= endPoint->y()))
		{
			actualPos.setY(originalPos.y());
		}

		if (actualPos != parentPos)
		{
			DrawingItem::resizeItem(point, actualPos);
			adjustBoxControlPoints(point);
			adjustReferencePoint();
			for(int i = 8; i < numberOfPoints(); i++)
				DrawingItem::point(i)->setPos(mapFromPath(additionalConnectionPoints[i-8]));
		}
	}
}

//==================================================================================================

QVariant DrawingPathItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	DrawingRectResizeItem::aboutToChangeEvent(reason, value);

	if (reason == AddNewItemToScene) adjustReferencePoint();

	return value;
}
