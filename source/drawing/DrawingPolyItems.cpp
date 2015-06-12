/* DrawingPolyItems.cpp
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

#include <DrawingPolyItems.h>
#include <DrawingItemPoint.h>

DrawingPolyItem::DrawingPolyItem() : DrawingItem()
{
	addProperty("Pen Color", QColor(255, 0, 255));
	addProperty("Pen Width", 12.0);
	addProperty("Pen Style", (unsigned int)Qt::SolidLine);
	addProperty("Pen Cap Style", (unsigned int)Qt::RoundCap);
	addProperty("Pen Join Style", (unsigned int)Qt::RoundJoin);

	setFlags(CanMove | CanRotate | CanFlip | CanResize | CanInsertRemovePoints | MatchUnitsWithParent);
	setPlaceType(PlaceMouseDownAndUp);
}

DrawingPolyItem::DrawingPolyItem(const DrawingPolyItem& item) : DrawingItem(item) { }

DrawingPolyItem::~DrawingPolyItem() { }

//==================================================================================================

void DrawingPolyItem::setPointPos(int index, const QPointF& scenePos)
{
	resizeItem(point(index), mapToParent(mapFromScene(scenePos)));
}

void DrawingPolyItem::setPointPos(int index, qreal sceneX, qreal sceneY)
{
	setPointPos(index, QPointF(sceneX, sceneY));
}

void DrawingPolyItem::setPointX(int index, qreal sceneX)
{
	setPointPos(index, QPointF(sceneX, pointY(index)));
}

void DrawingPolyItem::setPointY(int index, qreal sceneY)
{
	setPointPos(index, QPointF(pointX(index), sceneY));
}

QPointF DrawingPolyItem::pointPos(int index) const
{
	QPointF scenePos;

	DrawingItemPoint* itemPoint = point(index);
	if (itemPoint) scenePos = mapToScene(itemPoint->pos());

	return scenePos;
}

qreal DrawingPolyItem::pointX(int index) const
{
	return pointPos(index).x();
}

qreal DrawingPolyItem::pointY(int index) const
{
	return pointPos(index).y();
}

//==================================================================================================

void DrawingPolyItem::setPen(const QPen& pen)
{
	setPenColor(pen.brush().color());
	setPenWidth(pen.widthF());
	setPenStyle(pen.style());
	setPenCapStyle(pen.capStyle());
	setPenJoinStyle(pen.joinStyle());
}

void DrawingPolyItem::setPenColor(const QColor& color)
{
	setPropertyValue("Pen Color", color);
}

void DrawingPolyItem::setPenWidth(qreal width)
{
	setPropertyValue("Pen Width", width);
}

void DrawingPolyItem::setPenStyle(Qt::PenStyle style)
{
	setPropertyValue("Pen Style", (unsigned int)style);
}

void DrawingPolyItem::setPenCapStyle(Qt::PenCapStyle style)
{
	setPropertyValue("Pen Cap Style", (unsigned int)style);
}

void DrawingPolyItem::setPenJoinStyle(Qt::PenJoinStyle style)
{
	setPropertyValue("Pen Join Style", (unsigned int)style);
}

QPen DrawingPolyItem::pen() const
{
	return QPen(penColor(), penWidth(), penStyle(), penCapStyle(), penJoinStyle());
}

QColor DrawingPolyItem::penColor() const
{
	return propertyValue("Pen Color").value<QColor>();
}

qreal DrawingPolyItem::penWidth() const
{
	return propertyValue("Pen Width").toDouble();
}

Qt::PenStyle DrawingPolyItem::penStyle() const
{
	return (Qt::PenStyle)propertyValue("Pen Style").toUInt();
}

Qt::PenCapStyle DrawingPolyItem::penCapStyle() const
{
	return (Qt::PenCapStyle)propertyValue("Pen Cap Style").toUInt();
}

Qt::PenJoinStyle DrawingPolyItem::penJoinStyle() const
{
	return (Qt::PenJoinStyle)propertyValue("Pen Join Style").toUInt();
}

//==================================================================================================

bool DrawingPolyItem::isSuperfluous() const
{
	bool superfluous = true;
	QPointF pos = point(0)->pos();
	QList<DrawingItemPoint*> lPoints = points();

	for(auto pointIter = lPoints.begin(); superfluous && pointIter != lPoints.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return superfluous;
}

//==================================================================================================

void DrawingPolyItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	DrawingItem::resizeItem(point, parentPos);
	adjustReferencePoint();
}

//==================================================================================================

QVariant DrawingPolyItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setPenWidth(penWidth() * scaleFactor);
	}

	return value;
}

//==================================================================================================

void DrawingPolyItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("penColor", Drawing::colorToString(penColor()));
	xmlWriter.writeAttribute("penWidth", QString::number(penWidth()));
	xmlWriter.writeAttribute("penStyle", QString::number((unsigned int)penStyle()));
	xmlWriter.writeAttribute("penCapStyle", QString::number((unsigned int)penCapStyle()));
	xmlWriter.writeAttribute("penJoinStyle", QString::number((unsigned int)penJoinStyle()));
}

void DrawingPolyItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("penColor"))
		setPenColor(Drawing::colorFromString(attributes.value("penColor").toString()));
	if (attributes.hasAttribute("penWidth"))
		setPenWidth(attributes.value("penWidth").toString().toDouble());
	if (attributes.hasAttribute("penStyle"))
		setPenStyle((Qt::PenStyle)attributes.value("penStyle").toString().toUInt());
	if (attributes.hasAttribute("penCapStyle"))
		setPenCapStyle((Qt::PenCapStyle)attributes.value("penCapStyle").toString().toUInt());
	if (attributes.hasAttribute("penJoinStyle"))
		setPenJoinStyle((Qt::PenJoinStyle)attributes.value("penJoinStyle").toString().toUInt());
}
//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPolylineItem::DrawingPolylineItem() : DrawingPolyItem()
{
	QVariant variant;
	variant.setValue(DrawingArrow());

	addProperty("Start Arrow Style", DrawingArrow::None);
	addProperty("Start Arrow Size", 100.0);
	addProperty("End Arrow Style", DrawingArrow::None);
	addProperty("End Arrow Size", 100.0);

	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free, 0));
	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free, 0));
}

DrawingPolylineItem::DrawingPolylineItem(const DrawingPolylineItem& item) : DrawingPolyItem(item) { }

DrawingPolylineItem::~DrawingPolylineItem() { }

//==================================================================================================

DrawingItem* DrawingPolylineItem::copy() const
{
	return new DrawingPolylineItem(*this);
}

QString DrawingPolylineItem::uniqueKey() const
{
	return "polyline";
}

//==================================================================================================

void DrawingPolylineItem::setStartArrow(const DrawingArrow& arrow)
{
	setStartArrowStyle(arrow.style());
	setStartArrowSize(arrow.size());
}

void DrawingPolylineItem::setStartArrowStyle(DrawingArrow::Style style)
{
	setPropertyValue("Start Arrow Style", (unsigned int)style);
}

void DrawingPolylineItem::setStartArrowSize(qreal size)
{
	setPropertyValue("Start Arrow Size", size);
}

DrawingArrow DrawingPolylineItem::startArrow() const
{
	return DrawingArrow(startArrowStyle(), startArrowSize());
}

DrawingArrow::Style DrawingPolylineItem::startArrowStyle() const
{
	return (DrawingArrow::Style)propertyValue("Start Arrow Style").toUInt();
}

qreal DrawingPolylineItem::startArrowSize() const
{
	return propertyValue("Start Arrow Size").toDouble();
}

//==================================================================================================

void DrawingPolylineItem::setEndArrow(const DrawingArrow& arrow)
{
	setEndArrowStyle(arrow.style());
	setEndArrowSize(arrow.size());
}

void DrawingPolylineItem::setEndArrowStyle(DrawingArrow::Style style)
{
	setPropertyValue("End Arrow Style", (unsigned int)style);
}

void DrawingPolylineItem::setEndArrowSize(qreal size)
{
	setPropertyValue("End Arrow Size", size);
}

DrawingArrow DrawingPolylineItem::endArrow() const
{
	return DrawingArrow(endArrowStyle(), endArrowSize());
}

DrawingArrow::Style DrawingPolylineItem::endArrowStyle() const
{
	return (DrawingArrow::Style)propertyValue("End Arrow Style").toUInt();
}

qreal DrawingPolylineItem::endArrowSize() const
{
	return propertyValue("End Arrow Size").toDouble();
}

//==================================================================================================

DrawingItemPoint* DrawingPolylineItem::startPoint() const
{
	return point(0);
}

DrawingItemPoint* DrawingPolylineItem::endPoint() const
{
	return point(numberOfPoints() - 1);
}

//==================================================================================================

QRectF DrawingPolylineItem::boundingRect() const
{
	qreal l = 1E9, t = 1E9, r = -1E9, b = -1E9;
	QList<DrawingItemPoint*> lPoints = points();

	for(auto pointIter = lPoints.begin(); pointIter != lPoints.end(); pointIter++)
	{
		l = std::min(l, (*pointIter)->x());
		t = std::min(t, (*pointIter)->y());
		r = std::max(r, (*pointIter)->x());
		b = std::max(b, (*pointIter)->y());
	}

	return QRectF(l, t, r - l, b - t);
}

QPainterPath DrawingPolylineItem::shape() const
{
	QPainterPath path;
	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();

	path.moveTo(points.first()->pos());
	for(auto pointIter = points.begin()+1; pointIter != points.end(); pointIter++)
		path.lineTo((*pointIter)->pos());

	return itemShapeFromPath(path, pen());
}

//==================================================================================================

void DrawingPolylineItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	DrawingItemPoint* point0 = point(0);
	DrawingItemPoint* point1 = point(numberOfPoints() - 1);
	QList<DrawingItemPoint*> lPoints = points();
	QPolygonF polygon;
	DrawingItemPoint* otherPoint;
	qreal theta;

	// Polyline
	for(auto pointIter = lPoints.begin(); pointIter != lPoints.end(); pointIter++)
		polygon.append((*pointIter)->pos());

	setupPainter(painter, styleOptions, pen());
	painter->drawPolyline(polygon);

	// Arrows
	if (pen().style() != Qt::NoPen)
	{
		QPen arrowPen = pen();
		arrowPen.setStyle(Qt::SolidLine);
		setupPainter(painter, styleOptions, arrowPen, styleOptions.outputBrush(DrawingStyleOptions::Background));

		otherPoint = point(1);
		if (otherPoint)
		{
			theta = qAtan2(otherPoint->y() - point0->y(),
				otherPoint->x() - point0->x()) * 180.0 / 3.1414592654;

			if (Drawing::magnitude(otherPoint->pos() - point0->pos()) > startArrowSize())
				startArrow().render(painter, point0->pos(), theta);
		}

		otherPoint = point(numberOfPoints() - 2);
		if (otherPoint)
		{
			theta = qAtan2(otherPoint->y() - point1->y(),
				otherPoint->x() - point1->x()) * 180.0 / 3.1414592654;

			if (Drawing::magnitude(otherPoint->pos() - point1->pos()) > endArrowSize())
				endArrow().render(painter, point1->pos(), theta);
		}
	}

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

bool DrawingPolylineItem::canInsertPoint(DrawingItemPoint* point) const
{
	return DrawingPolyItem::canInsertPoint(point);
}

bool DrawingPolylineItem::canRemovePoint(DrawingItemPoint* point) const
{
	return (DrawingPolyItem::canRemovePoint(point) && numberOfPoints() > 2);
}

int DrawingPolylineItem::insertPointIndex(DrawingItemPoint* point) const
{
	int index = -1;

	if (point)
	{
		QList<DrawingItemPoint*> lPoints = points();
		QList<DrawingItemPoint*>::Iterator prevIter = lPoints.begin();
		QList<DrawingItemPoint*>::Iterator nextIter = lPoints.begin();
		nextIter++;
		int currentIndex = 0;

		int minimumIndex = -1;
		qreal distance, minimumDistance = 1E9;

		point->setFlags(DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free);
		point->setCategory(0);

		// Find where to insert point within polygon
		while (prevIter != lPoints.end() && nextIter != lPoints.end())
		{
			distance = Drawing::distanceFromPointToLineSegment(point->pos(),
				QLineF((*prevIter)->pos(), (*nextIter)->pos()));

			if (distance < minimumDistance)
			{
				minimumDistance = distance;
				minimumIndex = currentIndex;
			}

			prevIter++;
			nextIter++;
			currentIndex++;
		}

		index = minimumIndex + 1;
	}

	return index;
}

//==================================================================================================

QVariant DrawingPolylineItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingPolyItem::aboutToChangeEvent(reason, value);

	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);
		DrawingArrow lArrow;

		setStartArrowSize(startArrowSize() * scaleFactor);
		setEndArrowSize(endArrowSize() * scaleFactor);
	}

	return result;
}

//==================================================================================================

void DrawingPolylineItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingPolyItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("startArrowStyle", QString::number((unsigned int)startArrowStyle()));
	xmlWriter.writeAttribute("startArrowSize", QString::number(startArrowSize()));
	xmlWriter.writeAttribute("endArrowStyle", QString::number((unsigned int)endArrowStyle()));
	xmlWriter.writeAttribute("endArrowSize", QString::number(endArrowSize()));
}

void DrawingPolylineItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingPolyItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("startArrowStyle"))
		setStartArrowStyle((DrawingArrow::Style)attributes.value("startArrowStyle").toString().toUInt());
	if (attributes.hasAttribute("startArrowSize"))
		setStartArrowSize(attributes.value("startArrowSize").toString().toDouble());

	if (attributes.hasAttribute("endArrowStyle"))
		setEndArrowStyle((DrawingArrow::Style)attributes.value("endArrowStyle").toString().toUInt());
	if (attributes.hasAttribute("endArrowSize"))
		setEndArrowSize(attributes.value("endArrowSize").toString().toDouble());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPolygonItem::DrawingPolygonItem() : DrawingPolyItem()
{
	addProperty("Brush Color", QColor(0, 0, 0, 0));
	setPlaceType(PlaceMouseUp);

	addPoint(new DrawingItemPoint(QPointF(-200.0, -200.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection, 0));
	addPoint(new DrawingItemPoint(QPointF(-200.0, 200.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection, 0));
	addPoint(new DrawingItemPoint(QPointF(200.0, 0.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection, 0));
}

DrawingPolygonItem::DrawingPolygonItem(const DrawingPolygonItem& item) : DrawingPolyItem(item) { }

DrawingPolygonItem::~DrawingPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingPolygonItem::copy() const
{
	return new DrawingPolygonItem(*this);
}

QString DrawingPolygonItem::uniqueKey() const
{
	return "polygon";
}

//==================================================================================================

void DrawingPolygonItem::setBrush(const QBrush& brush)
{
	setBrushColor(brush.color());
}

void DrawingPolygonItem::setBrushColor(const QColor& color)
{
	setPropertyValue("Brush Color", color);
}

QBrush DrawingPolygonItem::brush() const
{
	return QBrush(brushColor());
}

QColor DrawingPolygonItem::brushColor() const
{
	return propertyValue("Brush Color").value<QColor>();
}

//==================================================================================================

QRectF DrawingPolygonItem::boundingRect() const
{
	qreal l = 1E9, t = 1E9, r = -1E9, b = -1E9;
	QList<DrawingItemPoint*> lPoints = points();

	for(auto pointIter = lPoints.begin(); pointIter != lPoints.end(); pointIter++)
	{
		l = std::min(l, (*pointIter)->x());
		t = std::min(t, (*pointIter)->y());
		r = std::max(r, (*pointIter)->x());
		b = std::max(b, (*pointIter)->y());
	}

	return QRectF(l, t, r - l, b - t);
}

QPainterPath DrawingPolygonItem::shape() const
{
	// This code should work, but doesn't
	QPainterPath path;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	path.moveTo(points.first()->pos());
	for(auto pointIter = points.begin()+1; pointIter != points.end(); pointIter++)
		path.lineTo((*pointIter)->pos());
	path.closeSubpath();

	QPainterPath shape = itemShapeFromPath(path, pen());
	if (brush().color().alpha() > 0) shape = shape.united(path);
	return shape;
}

//==================================================================================================

void DrawingPolygonItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QList<DrawingItemPoint*> lPoints = points();
	QPolygonF points;

	for(auto pointIter = lPoints.begin(); pointIter != lPoints.end(); pointIter++)
		points.append((*pointIter)->pos());

	setupPainter(painter, styleOptions, pen(), brush());
	painter->drawPolygon(points);

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

bool DrawingPolygonItem::canInsertPoint(DrawingItemPoint* point) const
{
	return DrawingPolyItem::canInsertPoint(point);
}

bool DrawingPolygonItem::canRemovePoint(DrawingItemPoint* point) const
{
	return (DrawingPolyItem::canRemovePoint(point) && numberOfPoints() > 3);
}

int DrawingPolygonItem::insertPointIndex(DrawingItemPoint* point) const
{
	int index = -1;

	if (point)
	{
		QList<DrawingItemPoint*> lPoints = points();
		QList<DrawingItemPoint*>::Iterator prevIter = lPoints.begin();
		QList<DrawingItemPoint*>::Iterator nextIter = lPoints.begin();
		nextIter++;
		int currentIndex = 0;

		int minimumIndex = -1;
		qreal distance, minimumDistance = 1E9;

		point->setFlags(DrawingItemPoint::Control | DrawingItemPoint::Connection);
		point->setCategory(0);

		// Find where to insert point within polygon
		while (prevIter != lPoints.end())
		{
			if (nextIter == lPoints.end()) nextIter = lPoints.begin();

			distance = Drawing::distanceFromPointToLineSegment(point->pos(),
				QLineF((*prevIter)->pos(), (*nextIter)->pos()));

			if (distance < minimumDistance)
			{
				minimumDistance = distance;
				minimumIndex = currentIndex;
			}

			prevIter++;
			nextIter++;
			currentIndex++;
		}

		index = minimumIndex + 1;
	}

	return index;
}

//==================================================================================================

QVariant DrawingPolygonItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingPolyItem::aboutToChangeEvent(reason, value);
	if (reason == AddNewItemToScene) adjustReferencePoint();
	return result;
}

//==================================================================================================

void DrawingPolygonItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingPolyItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("brushColor", Drawing::colorToString(brushColor()));
}

void DrawingPolygonItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingPolyItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("brushColor"))
		setBrushColor(Drawing::colorFromString(attributes.value("brushColor").toString()));
}
