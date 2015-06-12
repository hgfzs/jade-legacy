/* DrawingTwoPointItems.cpp
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

#include <DrawingTwoPointItems.h>
#include <DrawingItemPoint.h>

DrawingTwoPointItem::DrawingTwoPointItem() : DrawingItem()
{
	QVariant variant;
	variant.setValue(DrawingArrow());

	addProperty("Pen Color", QColor(255, 0, 255));
	addProperty("Pen Width", 12.0);
	addProperty("Pen Style", (unsigned int)Qt::SolidLine);
	addProperty("Pen Cap Style", (unsigned int)Qt::RoundCap);
	addProperty("Pen Join Style", (unsigned int)Qt::RoundJoin);
	addProperty("Start Arrow Style", DrawingArrow::None);
	addProperty("Start Arrow Size", 100.0);
	addProperty("End Arrow Style", DrawingArrow::None);
	addProperty("End Arrow Size", 100.0);

	setFlags(CanMove | CanRotate | CanFlip | CanResize | MatchUnitsWithParent);
	setPlaceType(PlaceMouseDownAndUp);

	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free, 0));
	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0),
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free, 0));
}

DrawingTwoPointItem::DrawingTwoPointItem(const DrawingTwoPointItem& item) : DrawingItem(item) { }

DrawingTwoPointItem::~DrawingTwoPointItem() { }

//==================================================================================================

void DrawingTwoPointItem::setPointPos(int pointIndex, const QPointF& scenePos)
{
	resizeItem(point(pointIndex), mapToParent(mapFromScene(scenePos)));
}

void DrawingTwoPointItem::setPointPos(int pointIndex, qreal sceneX, qreal sceneY)
{
	setPointPos(pointIndex, QPointF(sceneX, sceneY));
}

void DrawingTwoPointItem::setPointX(int pointIndex, qreal sceneX)
{
	setPointPos(pointIndex, QPointF(sceneX, pointY(pointIndex)));
}

void DrawingTwoPointItem::setPointY(int pointIndex, qreal sceneY)
{
	setPointPos(pointIndex, QPointF(pointX(pointIndex), sceneY));
}

QPointF DrawingTwoPointItem::pointPos(int pointIndex) const
{
	QPointF scenePos;

	DrawingItemPoint* itemPoint = point(pointIndex);
	if (itemPoint) scenePos = mapToScene(itemPoint->pos());

	return scenePos;
}

qreal DrawingTwoPointItem::pointX(int pointIndex) const
{
	return pointPos(pointIndex).x();
}

qreal DrawingTwoPointItem::pointY(int pointIndex) const
{
	return pointPos(pointIndex).y();
}

//==================================================================================================

void DrawingTwoPointItem::setPen(const QPen& pen)
{
	setPenColor(pen.brush().color());
	setPenWidth(pen.widthF());
	setPenStyle(pen.style());
	setPenCapStyle(pen.capStyle());
	setPenJoinStyle(pen.joinStyle());
}

void DrawingTwoPointItem::setPenColor(const QColor& color)
{
	setPropertyValue("Pen Color", color);
}

void DrawingTwoPointItem::setPenWidth(qreal width)
{
	setPropertyValue("Pen Width", width);
}

void DrawingTwoPointItem::setPenStyle(Qt::PenStyle style)
{
	setPropertyValue("Pen Style", (unsigned int)style);
}

void DrawingTwoPointItem::setPenCapStyle(Qt::PenCapStyle style)
{
	setPropertyValue("Pen Cap Style", (unsigned int)style);
}

void DrawingTwoPointItem::setPenJoinStyle(Qt::PenJoinStyle style)
{
	setPropertyValue("Pen Join Style", (unsigned int)style);
}

QPen DrawingTwoPointItem::pen() const
{
	return QPen(penColor(), penWidth(), penStyle(), penCapStyle(), penJoinStyle());
}

QColor DrawingTwoPointItem::penColor() const
{
	return propertyValue("Pen Color").value<QColor>();
}

qreal DrawingTwoPointItem::penWidth() const
{
	return propertyValue("Pen Width").toDouble();
}

Qt::PenStyle DrawingTwoPointItem::penStyle() const
{
	return (Qt::PenStyle)propertyValue("Pen Style").toUInt();
}

Qt::PenCapStyle DrawingTwoPointItem::penCapStyle() const
{
	return (Qt::PenCapStyle)propertyValue("Pen Cap Style").toUInt();
}

Qt::PenJoinStyle DrawingTwoPointItem::penJoinStyle() const
{
	return (Qt::PenJoinStyle)propertyValue("Pen Join Style").toUInt();
}

//==================================================================================================

void DrawingTwoPointItem::setStartArrow(const DrawingArrow& arrow)
{
	setStartArrowStyle(arrow.style());
	setStartArrowSize(arrow.size());
}

void DrawingTwoPointItem::setStartArrowStyle(DrawingArrow::Style style)
{
	setPropertyValue("Start Arrow Style", (unsigned int)style);
}

void DrawingTwoPointItem::setStartArrowSize(qreal size)
{
	setPropertyValue("Start Arrow Size", size);
}

DrawingArrow DrawingTwoPointItem::startArrow() const
{
	return DrawingArrow(startArrowStyle(), startArrowSize());
}

DrawingArrow::Style DrawingTwoPointItem::startArrowStyle() const
{
	return (DrawingArrow::Style)propertyValue("Start Arrow Style").toUInt();
}

qreal DrawingTwoPointItem::startArrowSize() const
{
	return propertyValue("Start Arrow Size").toDouble();
}

//==================================================================================================

void DrawingTwoPointItem::setEndArrow(const DrawingArrow& arrow)
{
	setEndArrowStyle(arrow.style());
	setEndArrowSize(arrow.size());
}

void DrawingTwoPointItem::setEndArrowStyle(DrawingArrow::Style style)
{
	setPropertyValue("End Arrow Style", (unsigned int)style);
}

void DrawingTwoPointItem::setEndArrowSize(qreal size)
{
	setPropertyValue("End Arrow Size", size);
}

DrawingArrow DrawingTwoPointItem::endArrow() const
{
	return DrawingArrow(endArrowStyle(), endArrowSize());
}

DrawingArrow::Style DrawingTwoPointItem::endArrowStyle() const
{
	return (DrawingArrow::Style)propertyValue("End Arrow Style").toUInt();
}

qreal DrawingTwoPointItem::endArrowSize() const
{
	return propertyValue("End Arrow Size").toDouble();
}

//==================================================================================================

DrawingItemPoint* DrawingTwoPointItem::startPoint() const
{
	return point(0);
}

DrawingItemPoint* DrawingTwoPointItem::endPoint() const
{
	return point(1);
}

bool DrawingTwoPointItem::isSuperfluous() const
{
	return (startPoint()->pos() == endPoint()->pos());
}

//==================================================================================================

void DrawingTwoPointItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	DrawingItem::resizeItem(point, parentPos);
	adjustReferencePoint();
}

//==================================================================================================

QVariant DrawingTwoPointItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);
		QPen lPen = pen();
		DrawingArrow lArrow;

		setPenWidth(penWidth() * scaleFactor);
		setStartArrowSize(startArrowSize() * scaleFactor);
		setEndArrowSize(endArrowSize() * scaleFactor);
	}
	else if (reason == AddNewItemToScene) adjustReferencePoint();

	return value;
}

//==================================================================================================

void DrawingTwoPointItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("penColor", Drawing::colorToString(penColor()));
	xmlWriter.writeAttribute("penWidth", QString::number(penWidth()));
	xmlWriter.writeAttribute("penStyle", QString::number((unsigned int)penStyle()));
	xmlWriter.writeAttribute("penCapStyle", QString::number((unsigned int)penCapStyle()));
	xmlWriter.writeAttribute("penJoinStyle", QString::number((unsigned int)penJoinStyle()));

	xmlWriter.writeAttribute("startArrowStyle", QString::number((unsigned int)startArrowStyle()));
	xmlWriter.writeAttribute("startArrowSize", QString::number(startArrowSize()));
	xmlWriter.writeAttribute("endArrowStyle", QString::number((unsigned int)endArrowStyle()));
	xmlWriter.writeAttribute("endArrowSize", QString::number(endArrowSize()));
}

void DrawingTwoPointItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
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

DrawingLineItem::DrawingLineItem() : DrawingTwoPointItem()
{
	// Midpoint
	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::Connection, 0));
}

DrawingLineItem::DrawingLineItem(const DrawingLineItem& item) : DrawingTwoPointItem(item) { }

DrawingLineItem::~DrawingLineItem() { }

//==================================================================================================

DrawingItem* DrawingLineItem::copy() const
{
	return new DrawingLineItem(*this);
}

QString DrawingLineItem::uniqueKey() const
{
	return "line";
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	return Drawing::rectFromPoints(startPoint()->pos(), endPoint()->pos());
}

QPainterPath DrawingLineItem::shape() const
{
	QPainterPath path;

	path.moveTo(startPoint()->pos());
	path.lineTo(endPoint()->pos());

	return itemShapeFromPath(path, pen());
}

//==================================================================================================

void DrawingLineItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	// Line
	setupPainter(painter, styleOptions, pen());
	painter->drawLine(startPoint()->pos(), endPoint()->pos());

	// Arrows
	if (pen().style() != Qt::NoPen)
	{
		QPen arrowPen = pen();
		arrowPen.setStyle(Qt::SolidLine);
		setupPainter(painter, styleOptions, arrowPen, styleOptions.outputBrush(DrawingStyleOptions::Background));

		qreal theta = qAtan2(endPoint()->y() - startPoint()->y(),
					   endPoint()->x() - startPoint()->x()) * 180.0 / 3.1414592654;

		if (Drawing::magnitude(endPoint()->pos() - startPoint()->pos()) > startArrowSize())
			startArrow().render(painter, startPoint()->pos(), theta);
		if (Drawing::magnitude(endPoint()->pos() - startPoint()->pos()) > endArrowSize())
			endArrow().render(painter, endPoint()->pos(), theta - 180.0);
	}

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

void DrawingLineItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	DrawingTwoPointItem::resizeItem(point, parentPos);
	DrawingItem::point(2)->setPos((startPoint()->pos() + endPoint()->pos()) / 2.0);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingArcItem::DrawingArcItem() : DrawingTwoPointItem() { }

DrawingArcItem::DrawingArcItem(const DrawingArcItem& item) : DrawingTwoPointItem(item) { }

DrawingArcItem::~DrawingArcItem() { }

//==================================================================================================

DrawingItem* DrawingArcItem::copy() const
{
	return new DrawingArcItem(*this);
}

QString DrawingArcItem::uniqueKey() const
{
	return "arc";
}

//==================================================================================================

QRectF DrawingArcItem::boundingRect() const
{
	return Drawing::rectFromPoints(startPoint()->pos(), endPoint()->pos());
}

QPainterPath DrawingArcItem::shape() const
{
	QPainterPath path;

	qreal arcRadiusX = orientedRadiusX();
	qreal arcRadiusY = orientedRadiusY();
	qreal arcCenterX = orientedCenterX();
	qreal arcCenterY = orientedCenterY();
	qreal arcStartAngle = orientedStartAngle();
	qreal arcSpanAngle = orientedSpanAngle();

	path.arcMoveTo(QRectF(arcCenterX - arcRadiusX, arcCenterY - arcRadiusY,
		arcRadiusX * 2, arcRadiusY * 2), arcStartAngle);
	path.arcTo(QRectF(arcCenterX - arcRadiusX, arcCenterY - arcRadiusY,
		arcRadiusX * 2, arcRadiusY * 2), arcStartAngle, arcSpanAngle);

	return itemShapeFromPath(path, pen());
}

//==================================================================================================

void DrawingArcItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	qreal arcRadiusX = orientedRadiusX();
	qreal arcRadiusY = orientedRadiusY();
	qreal arcCenterX = orientedCenterX();
	qreal arcCenterY = orientedCenterY();
	qreal arcStartAngle = orientedStartAngle();
	qreal arcSpanAngle = orientedSpanAngle();

	// Arc
	setupPainter(painter, styleOptions, pen());
	painter->drawArc(QRectF(arcCenterX - arcRadiusX, arcCenterY - arcRadiusY,
		arcRadiusX * 2, arcRadiusY * 2), (int)(arcStartAngle * 16), (int)(arcSpanAngle * 16));

	// Arrows
	if (pen().style() != Qt::NoPen)
	{
		QPen arrowPen = pen();
		arrowPen.setStyle(Qt::SolidLine);
		setupPainter(painter, styleOptions, arrowPen, styleOptions.outputBrush(DrawingStyleOptions::Background));

		DrawingItemPoint* itemPoint0 = startPoint();
		DrawingItemPoint* itemPoint1 = endPoint();

		if (Drawing::magnitude(itemPoint1->pos() - itemPoint0->pos()) > startArrowSize())
		{
			if (isFlipped()) startArrow().render(painter, itemPoint0->pos(), 90 - arcStartAngle);
			else startArrow().render(painter, itemPoint0->pos(), -90 - arcStartAngle);
		}
		if (Drawing::magnitude(itemPoint1->pos() - itemPoint0->pos()) > endArrowSize())
			endArrow().render(painter, itemPoint1->pos(), -arcStartAngle);
	}

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

qreal DrawingArcItem::orientedCenterX() const
{
	DrawingItemPoint* itemPoint0 = startPoint();
	DrawingItemPoint* itemPoint1 = endPoint();
	qreal centerX = 0.0;

	if (itemPoint1->x() == itemPoint0->x()) centerX = itemPoint0->x();
	else
	{
		qreal slope = (itemPoint1->y() - itemPoint0->y()) /
					  (itemPoint1->x() - itemPoint0->x());

		if ((!isFlipped() && slope < 0) || (isFlipped() && slope > 0))
			centerX = itemPoint0->x();
		else centerX = itemPoint1->x();
	}

	return centerX;
}

qreal DrawingArcItem::orientedCenterY() const
{
	DrawingItemPoint* itemPoint0 = startPoint();
	DrawingItemPoint* itemPoint1 = endPoint();
	qreal centerY = 0.0;

	if (itemPoint1->x() == itemPoint0->x()) centerY = itemPoint1->y();
	else
	{
		qreal slope = (itemPoint1->y() - itemPoint0->y()) /
					  (itemPoint1->x() - itemPoint0->x());

		if ((!isFlipped() && slope < 0) || (isFlipped() && slope > 0))
			centerY = itemPoint1->y();
		else centerY = itemPoint0->y();
	}

	return centerY;
}

qreal DrawingArcItem::orientedRadiusX() const
{
	return boundingRect().width();
}

qreal DrawingArcItem::orientedRadiusY() const
{
	return boundingRect().height();
}

qreal DrawingArcItem::orientedStartAngle() const
{
	qreal angle = 0.0;

	DrawingItemPoint* itemPoint0 = startPoint();
	angle = qAtan2(orientedCenterY() - itemPoint0->y(),
				   itemPoint0->x() - orientedCenterX()) * 180 / 3.14159;

	return angle;
}

qreal DrawingArcItem::orientedSpanAngle() const
{
	qreal angle = 90.0;
	if (isFlipped()) angle = -90.0;
	return angle;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingCurveItem::DrawingCurveItem() : DrawingTwoPointItem()
{
	setPlaceType(PlaceMouseUp);

	// Control points
	point(0)->setPos(QPointF(-200.0, -200.0));
	point(1)->setPos(QPointF( 200.0,  200.0));
	addPoint(new DrawingItemPoint(QPointF(0.0, -200.0), DrawingItemPoint::Control, 0));
	addPoint(new DrawingItemPoint(QPointF(0.0, 200.0), DrawingItemPoint::Control, 0));
}

DrawingCurveItem::DrawingCurveItem(const DrawingCurveItem& item) : DrawingTwoPointItem(item) { }

DrawingCurveItem::~DrawingCurveItem() { }

//==================================================================================================

DrawingItem* DrawingCurveItem::copy() const
{
	return new DrawingCurveItem(*this);
}

QString DrawingCurveItem::uniqueKey() const
{
	return "curve";
}

//==================================================================================================

QRectF DrawingCurveItem::boundingRect() const
{
	QRectF rect;
	QPainterPath path;

	path.moveTo(point(0)->pos());
	path.cubicTo(point(2)->pos(), point(3)->pos(), point(1)->pos());
	rect = path.boundingRect();

	return rect;
}

QPainterPath DrawingCurveItem::shape() const
{
	QPainterPath path;

	path.moveTo(startPoint()->pos());
	path.cubicTo(point(2)->pos(), point(3)->pos(), endPoint()->pos());

	return itemShapeFromPath(path, pen());
}

bool DrawingCurveItem::isSuperfluous() const
{
	return !boundingRect().isValid();
}

//==================================================================================================

void DrawingCurveItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QPainterPath path;
	DrawingItemPoint* itemPoint0 = point(0);
	DrawingItemPoint* itemPoint1 = point(1);
	DrawingItemPoint* itemPoint2 = point(2);
	DrawingItemPoint* itemPoint3 = point(3);

	// Curve
	setupPainter(painter, styleOptions, pen());
	path.moveTo(itemPoint0->pos());
	path.cubicTo(itemPoint2->pos(), itemPoint3->pos(), itemPoint1->pos());
	painter->drawPath(path);

	// Arrows
	if (pen().style() != Qt::NoPen)
	{
		QPen arrowPen = pen();
		arrowPen.setStyle(Qt::SolidLine);
		setupPainter(painter, styleOptions, arrowPen, styleOptions.outputBrush(DrawingStyleOptions::Background));

		qreal theta = qAtan2(itemPoint1->y() - itemPoint0->y(),
					  itemPoint1->x() - itemPoint0->x()) * 180.0 / 3.1414592654;

		//if (path.boundingRect().width() >= arrow(0).size() && path.boundingRect().height() >= arrow(0).size())
		if (path.boundingRect().width() > 0 || path.boundingRect().height() > 0)
		{
			QPointF p = pointFromRatio(0.05);
			theta = qAtan2(p.y() - itemPoint0->y(),
						  p.x() - itemPoint0->x()) * 180.0 / 3.14159;
			startArrow().render(painter, itemPoint0->pos(), theta);
		}
		//if (path.boundingRect().width() >= arrow(1).size() && path.boundingRect().height() >= arrow(1).size())
		if (path.boundingRect().width() > 0 || path.boundingRect().height() > 0)
		{
			QPointF p = pointFromRatio(0.95);
			theta = qAtan2(p.y() - itemPoint1->y(),
				 p.x() - itemPoint1->x()) * 180.0 / 3.14159;
			endArrow().render(painter, itemPoint1->pos(), theta);
		}
	}

	setupPainter(painter, styleOptions, pen());
	if (isSelected())
	{
		QPen dottedPen = painter->pen();
		dottedPen.setStyle(Qt::DotLine);
		dottedPen.setWidthF(0.75 * dottedPen.widthF());
		painter->setPen(dottedPen);

		painter->drawLine(itemPoint0->pos(), itemPoint2->pos());
		painter->drawLine(itemPoint1->pos(), itemPoint3->pos());
	}

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif
}

//==================================================================================================

void DrawingCurveItem::resizeItem(DrawingItemPoint* point, const QPointF& parentPos)
{
	if (point && point->isControlPoint())
	{
		QPointF deltaPosition = parentPos - mapToParent(point->pos());
		DrawingItemPoint* otherPoint = nullptr;

		if (point == DrawingCurveItem::point(0)) otherPoint = DrawingCurveItem::point(2);
		else if (point == DrawingCurveItem::point(1)) otherPoint = DrawingCurveItem::point(3);

		if (otherPoint) otherPoint->setPos(otherPoint->pos() + deltaPosition);
		point->setPos(point->pos() + deltaPosition);

		adjustReferencePoint();
	}
}

//==================================================================================================

QPointF DrawingCurveItem::pointFromRatio(qreal ratio) const
{
	QPointF pos;
	DrawingItemPoint* point0 = point(0);
	DrawingItemPoint* point1 = point(1);
	DrawingItemPoint* point2 = point(2);
	DrawingItemPoint* point3 = point(3);

	pos.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * point0->x() +
		3*ratio*(1 - ratio)*(1 - ratio) * point2->x() +
		3*ratio*ratio*(1 - ratio) * point3->x() +
		ratio*ratio*ratio * point1->x());

	pos.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * point0->y() +
		3*ratio*(1 - ratio)*(1 - ratio) * point2->y() +
		3*ratio*ratio*(1 - ratio) * point3->y() +
		ratio*ratio*ratio * point1->y());

	return pos;
}
