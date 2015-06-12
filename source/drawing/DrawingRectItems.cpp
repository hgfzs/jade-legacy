/* DrawingRectItems.cpp
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

#include <DrawingRectItems.h>
#include <DrawingItemPoint.h>

DrawingRectResizeItem::DrawingRectResizeItem() : DrawingItem()
{
	addProperty("Pen Color", QColor(255, 0, 255));
	addProperty("Pen Width", 12.0);
	addProperty("Pen Style", (unsigned int)Qt::SolidLine);
	addProperty("Pen Cap Style", (unsigned int)Qt::RoundCap);
	addProperty("Pen Join Style", (unsigned int)Qt::RoundJoin);

	setFlags(CanMove | CanRotate | CanFlip | CanResize | MatchUnitsWithParent);
	setPlaceType(PlaceMouseDownAndUp);

	for(int i = 0; i < 4; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::Control, 0));

	for(int i = 4; i < 8; i++)
	{
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0),
			DrawingItemPoint::Control | DrawingItemPoint::Connection, 0));
	}
}

DrawingRectResizeItem::DrawingRectResizeItem(const DrawingRectResizeItem& item) : DrawingItem(item) { }

DrawingRectResizeItem::~DrawingRectResizeItem() { }

//==================================================================================================

void DrawingRectResizeItem::setRect(const QRectF& sceneRect)
{
	resizeItem(point(0), mapToParent(mapFromScene(sceneRect.topLeft())));
	resizeItem(point(1), mapToParent(mapFromScene(sceneRect.bottomRight())));
}

QRectF DrawingRectResizeItem::rect() const
{
	return mapToScene(boundingRect());
}

//==================================================================================================

void DrawingRectResizeItem::setPen(const QPen& pen)
{
	setPenColor(pen.brush().color());
	setPenWidth(pen.widthF());
	setPenStyle(pen.style());
	setPenCapStyle(pen.capStyle());
	setPenJoinStyle(pen.joinStyle());
}

void DrawingRectResizeItem::setPenColor(const QColor& color)
{
	setPropertyValue("Pen Color", color);
}

void DrawingRectResizeItem::setPenWidth(qreal width)
{
	setPropertyValue("Pen Width", width);
}

void DrawingRectResizeItem::setPenStyle(Qt::PenStyle style)
{
	setPropertyValue("Pen Style", (unsigned int)style);
}

void DrawingRectResizeItem::setPenCapStyle(Qt::PenCapStyle style)
{
	setPropertyValue("Pen Cap Style", (unsigned int)style);
}

void DrawingRectResizeItem::setPenJoinStyle(Qt::PenJoinStyle style)
{
	setPropertyValue("Pen Join Style", (unsigned int)style);
}

QPen DrawingRectResizeItem::pen() const
{
	return QPen(penColor(), penWidth(), penStyle(), penCapStyle(), penJoinStyle());
}

QColor DrawingRectResizeItem::penColor() const
{
	return propertyValue("Pen Color").value<QColor>();
}

qreal DrawingRectResizeItem::penWidth() const
{
	return propertyValue("Pen Width").toDouble();
}

Qt::PenStyle DrawingRectResizeItem::penStyle() const
{
	return (Qt::PenStyle)propertyValue("Pen Style").toUInt();
}

Qt::PenCapStyle DrawingRectResizeItem::penCapStyle() const
{
	return (Qt::PenCapStyle)propertyValue("Pen Cap Style").toUInt();
}

Qt::PenJoinStyle DrawingRectResizeItem::penJoinStyle() const
{
	return (Qt::PenJoinStyle)propertyValue("Pen Join Style").toUInt();
}

//==================================================================================================

bool DrawingRectResizeItem::isSuperfluous() const
{
	return (point(0)->pos() == point(1)->pos());
}

//==================================================================================================

void DrawingRectResizeItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	DrawingItem::resizeItem(point, parentPos);
	adjustBoxControlPoints(point);
	adjustReferencePoint();
}

//==================================================================================================

QVariant DrawingRectResizeItem::aboutToChangeEvent(Reason reason, const QVariant& value)
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

void DrawingRectResizeItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("penColor", Drawing::colorToString(penColor()));
	xmlWriter.writeAttribute("penWidth", QString::number(penWidth()));
	xmlWriter.writeAttribute("penStyle", QString::number((unsigned int)penStyle()));
	xmlWriter.writeAttribute("penCapStyle", QString::number((unsigned int)penCapStyle()));
	xmlWriter.writeAttribute("penJoinStyle", QString::number((unsigned int)penJoinStyle()));
}

void DrawingRectResizeItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
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

void DrawingRectResizeItem::adjustBoxControlPoints(DrawingItemPoint* activePoint)
{
	if (activePoint == nullptr) activePoint = selectedPoint();

	if (activePoint)
	{
		int index = points().indexOf(activePoint);
		bool rotated = (rotationAngle() == 90.0 || rotationAngle() == 270.0);
		DrawingItemPoint* leftPoint = nullptr;
		DrawingItemPoint* rightPoint = nullptr;
		DrawingItemPoint* midPoint = nullptr;

		if (0 <= index && index < 4)
		{
			DrawingItemPoint* adjustPosXPoint = nullptr;
			DrawingItemPoint* adjustPosYPoint = nullptr;

			switch (index)
			{
			case 0:
				adjustPosXPoint	= (rotated) ? point(2) : point(3);
				adjustPosYPoint	= (rotated) ? point(3) : point(2);
				break;
			case 1:
				adjustPosXPoint	= (rotated) ? point(3) : point(2);
				adjustPosYPoint	= (rotated) ? point(2) : point(3);
				break;
			case 2:
				adjustPosXPoint	= (rotated) ? point(0) : point(1);
				adjustPosYPoint	= (rotated) ? point(1) : point(0);
				break;
			case 3:
				adjustPosXPoint	= (rotated) ? point(1) : point(0);
				adjustPosYPoint	= (rotated) ? point(0) : point(1);
				break;
			default:
				break;
			}

			if (adjustPosXPoint) adjustPosXPoint->setX(activePoint->x());
			if (adjustPosYPoint) adjustPosYPoint->setY(activePoint->y());
		}
		else if (4 <= index && index < 8)
		{
			switch (index)
			{
			case 4: leftPoint = point(0); rightPoint = point(2); break;
			case 5: leftPoint = point(2); rightPoint = point(1); break;
			case 6: leftPoint = point(1); rightPoint = point(3); break;
			case 7: leftPoint = point(3); rightPoint = point(0); break;
			}

			if (leftPoint && rightPoint)
			{
				if ((rotated && index % 2 == 0) || (!rotated && index % 2 == 1))
				{
					leftPoint->setX(activePoint->x());
					rightPoint->setX(activePoint->x());
				}
				else
				{
					leftPoint->setY(activePoint->y());
					rightPoint->setY(activePoint->y());
				}
			}
		}

		// Update midpoints
		midPoint = point(4); leftPoint = point(0); rightPoint = point(2);
		if (midPoint && leftPoint && rightPoint)
			midPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

		midPoint = point(5); leftPoint = point(2); rightPoint = point(1);
		if (midPoint && leftPoint && rightPoint)
			midPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

		midPoint = point(6); leftPoint = point(1); rightPoint = point(3);
		if (midPoint && leftPoint && rightPoint)
			midPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);

		midPoint = point(7); leftPoint = point(3); rightPoint = point(0);
		if (midPoint && leftPoint && rightPoint)
			midPoint->setPos((leftPoint->pos() + rightPoint->pos()) / 2);
	}
}

void DrawingRectResizeItem::adjustEllipseControlPoints(DrawingItemPoint* activePoint)
{
	const qreal PI = 3.14159265358;
	DrawingItemPoint* startPoint = point(0);
	DrawingItemPoint* endPoint = point(1);

	adjustBoxControlPoints(activePoint);

	if (startPoint && endPoint)
	{
		// Adjust points at 45 degree offsets
		QRectF rect = QRectF(startPoint->pos(), endPoint->pos());
		qreal radiusX = rect.width() / 2;
		qreal radiusY = rect.height() / 2;

		DrawingItemPoint* cornerPoint = nullptr;
		qreal angle = -3 * PI / 4;
		qreal angleStep = (rotationAngle() == 90.0 || rotationAngle() == 270.0) ? -PI / 2 : PI / 2;

		for(int i = 8; i < 12; i++)
		{
			cornerPoint = point(i);
			if (cornerPoint)
			{
				cornerPoint->setPos(rect.center() +
										 QPointF(radiusX * qCos(angle), radiusY * qSin(angle)));
			}
			angle += angleStep;
		}
	}
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRectItem::DrawingRectItem() : DrawingRectResizeItem()
{
	addProperty("Brush Color", QColor(0, 0, 0, 0));
	addProperty("Corner X-Radius", QVariant(0.0));
	addProperty("Corner Y-Radius", QVariant(0.0));

	for(int i = 0; i < 4; i++)
		point(i)->setFlags(DrawingItemPoint::Control | DrawingItemPoint::Connection);
}

DrawingRectItem::DrawingRectItem(const DrawingRectItem& item) : DrawingRectResizeItem(item) { }

DrawingRectItem::~DrawingRectItem() { }

//==================================================================================================

DrawingItem* DrawingRectItem::copy() const
{
	return new DrawingRectItem(*this);
}

QString DrawingRectItem::uniqueKey() const
{
	return "rect";
}

//==================================================================================================

void DrawingRectItem::setBrush(const QBrush& brush)
{
	setBrushColor(brush.color());
}

void DrawingRectItem::setBrushColor(const QColor& color)
{
	setPropertyValue("Brush Color", color);
}

QBrush DrawingRectItem::brush() const
{
	return QBrush(brushColor());
}

QColor DrawingRectItem::brushColor() const
{
	return propertyValue("Brush Color").value<QColor>();
}

//==================================================================================================

void DrawingRectItem::setCornerRadii(qreal xRadius, qreal yRadius)
{
	setPropertyValue("Corner X-Radius", xRadius);
	setPropertyValue("Corner Y-Radius", yRadius);
}

void DrawingRectItem::setCornerRadiusX(qreal radius)
{
	setPropertyValue("Corner X-Radius", radius);
}

void DrawingRectItem::setCornerRadiusY(qreal radius)
{
	setPropertyValue("Corner Y-Radius", radius);
}

qreal DrawingRectItem::cornerRadiusX() const
{
	return propertyValue("Corner X-Radius").toDouble();
}

qreal DrawingRectItem::cornerRadiusY() const
{
	return propertyValue("Corner Y-Radius").toDouble();
}

//==================================================================================================

QRectF DrawingRectItem::boundingRect() const
{
	return Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());
}

QPainterPath DrawingRectItem::shape() const
{
	QPainterPath path;
	QRectF bounds = boundingRect();

	path.addRoundedRect(bounds, orientedCornerRadiusX(), orientedCornerRadiusY());

	QPainterPath shape = itemShapeFromPath(path, pen());
	if (brush().color().alpha() > 0) shape = shape.united(path);
	return shape;
}

//==================================================================================================

void DrawingRectItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QRectF bounds = boundingRect();
	qreal xRadius = orientedCornerRadiusX();
	qreal yRadius = orientedCornerRadiusY();

	setupPainter(painter, styleOptions, pen(), brush());

	if (xRadius > 0 || yRadius > 0) painter->drawRoundedRect(bounds, xRadius, yRadius);
	else painter->drawRect(bounds);

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

QVariant DrawingRectItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingRectResizeItem::aboutToChangeEvent(reason, value);

	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setCornerRadii(cornerRadiusX() * scaleFactor, cornerRadiusY() * scaleFactor);
	}

	return result;
}

//==================================================================================================

void DrawingRectItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingRectResizeItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("brushColor", Drawing::colorToString(brushColor()));
	xmlWriter.writeAttribute("cornerRadiusX", QString::number(cornerRadiusX()));
	xmlWriter.writeAttribute("cornerRadiusY", QString::number(cornerRadiusY()));
}

void DrawingRectItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingRectResizeItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("brushColor"))
		setBrushColor(Drawing::colorFromString(attributes.value("brushColor").toString()));
	if (attributes.hasAttribute("cornerRadiusX"))
		setCornerRadiusX(attributes.value("cornerRadiusX").toString().toDouble());
	if (attributes.hasAttribute("cornerRadiusY"))
		setCornerRadiusY(attributes.value("cornerRadiusY").toString().toDouble());
}

//==================================================================================================

qreal DrawingRectItem::orientedCornerRadiusX() const
{
	qreal radius = cornerRadiusX();
	if (rotationAngle() == 90.0 || rotationAngle() == 270.0) radius = cornerRadiusY();
	return radius;
}

qreal DrawingRectItem::orientedCornerRadiusY() const
{
	qreal radius = cornerRadiusY();
	if (rotationAngle() == 90.0 || rotationAngle() == 270.0) radius = cornerRadiusX();
	return radius;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingEllipseItem::DrawingEllipseItem() : DrawingRectResizeItem()
{
	addProperty("Brush Color", QColor(0, 0, 0, 0));

	for(int i = 8; i < 12; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::Connection, 0));
}

DrawingEllipseItem::DrawingEllipseItem(const DrawingEllipseItem& item) : DrawingRectResizeItem(item) { }

DrawingEllipseItem::~DrawingEllipseItem() { }

//==================================================================================================

DrawingItem* DrawingEllipseItem::copy() const
{
	return new DrawingEllipseItem(*this);
}

QString DrawingEllipseItem::uniqueKey() const
{
	return "ellipse";
}

//==================================================================================================

void DrawingEllipseItem::setBrush(const QBrush& brush)
{
	setBrushColor(brush.color());
}

void DrawingEllipseItem::setBrushColor(const QColor& color)
{
	setPropertyValue("Brush Color", color);
}

QBrush DrawingEllipseItem::brush() const
{
	return QBrush(brushColor());
}

QColor DrawingEllipseItem::brushColor() const
{
	return propertyValue("Brush Color").value<QColor>();
}

//==================================================================================================

QRectF DrawingEllipseItem::boundingRect() const
{
	return Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());
}

QPainterPath DrawingEllipseItem::shape() const
{
	QPainterPath path;
	QRectF bounds = Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());

	path.addEllipse(bounds.center(), bounds.width()/2, bounds.height()/2);

	QPainterPath shape = itemShapeFromPath(path, pen());
	if (brush().color().alpha() > 0) shape = shape.united(path);
	return shape;
}

//==================================================================================================

void DrawingEllipseItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QRectF bounds = Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());

	setupPainter(painter, styleOptions, pen(), brush());
	painter->drawEllipse(bounds);

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

void DrawingEllipseItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	DrawingRectResizeItem::resizeItem(point, parentPos);
	adjustEllipseControlPoints(point);
}

//==================================================================================================

void DrawingEllipseItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingRectResizeItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("brushColor", Drawing::colorToString(brushColor()));
}

void DrawingEllipseItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingRectResizeItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("brushColor"))
		setBrushColor(Drawing::colorFromString(attributes.value("brushColor").toString()));
}

