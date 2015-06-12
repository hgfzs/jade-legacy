/* DrawingPixmapItem.cpp
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

#include <DrawingPixmapItem.h>
#include <DrawingItemPoint.h>

DrawingPixmapItem::DrawingPixmapItem() : DrawingRectResizeItem()
{
	QRect rect(-200, -200, 400, 400);

	addProperty("Image", QPixmap());

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

DrawingPixmapItem::DrawingPixmapItem(const DrawingPixmapItem& item) : DrawingRectResizeItem(item) { }

DrawingPixmapItem::~DrawingPixmapItem() { }

//==================================================================================================

DrawingItem* DrawingPixmapItem::copy() const
{
	return new DrawingPixmapItem(*this);
}

QString DrawingPixmapItem::uniqueKey() const
{
	return "image";
}

//==================================================================================================

void DrawingPixmapItem::setPixmap(const QPixmap& pixmap)
{
	setPropertyValue("Image", pixmap);
}

QPixmap DrawingPixmapItem::pixmap() const
{
	return propertyValue("Image").value<QPixmap>();
}

//==================================================================================================

QRectF DrawingPixmapItem::boundingRect() const
{
	return Drawing::rectFromPoints(point(0)->pos(), point(1)->pos());
}

//==================================================================================================

void DrawingPixmapItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QPixmap pixmap = DrawingPixmapItem::pixmap();
	QRectF sourceRect;
	
#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif

	if (pixmap.isNull())
	{
		qreal deviceFactor = 1.0;
		qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);

		setupPainter(painter, styleOptions, pen());
		painter->setBrush(Qt::transparent);

		if (painter->pen().style() == Qt::NoPen || painter->pen().widthF() == 0)
		{
			QPen pen = painter->pen();
			pen.setWidth(1);
			pen.setCosmetic(true);
			pen.setStyle(Qt::DashLine);
			painter->setPen(pen);
		}

		QFont font("Arial", qMin(boundingRect().width(), boundingRect().height()) / 6);
		if (painter->paintEngine()->paintDevice())
			deviceFactor = 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX();
		font.setPointSizeF(font.pointSizeF() * 0.72 / scaleFactor);       // Scale to workspace
		font.setPointSizeF(font.pointSizeF() * deviceFactor);             // Scale to device
		painter->setFont(font);

		painter->drawRect(boundingRect());
		painter->drawText(boundingRect(), Qt::AlignCenter, "<no image>");
	}
	else
	{
		setupPainter(painter, styleOptions, pen());
		painter->setBrush(Qt::transparent);

		painter->rotate(rotationAngle());
		if (isFlipped()) painter->scale(-1.0, 1.0);

		if (rotationAngle() == 90 || rotationAngle() == 270)
			sourceRect = QRectF(0, 0, pixmap.height(), pixmap.width());
		else
			sourceRect = QRectF(0, 0, pixmap.width(), pixmap.height());

		painter->drawPixmap(boundingRect(), pixmap, sourceRect);
		painter->drawRect(boundingRect());
	}
}

//==================================================================================================

void DrawingPixmapItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingRectResizeItem::writeXmlAttributes(xmlWriter, items);

	QPixmap pixmap = DrawingPixmapItem::pixmap();
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	buffer.close();

	xmlWriter.writeAttribute("data", buffer.buffer().toPercentEncoding());
}

void DrawingPixmapItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingRectResizeItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("data"))
	{
		QPixmap pixmap;
		QByteArray data = QByteArray::fromPercentEncoding(attributes.value("data").toLocal8Bit());
		if (pixmap.loadFromData(data)) setPixmap(pixmap);
	}
}

//==================================================================================================

QVariant DrawingPixmapItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	DrawingRectResizeItem::aboutToChangeEvent(reason, value);

	if (reason == AddNewItemToScene) adjustReferencePoint();

	return value;
}
