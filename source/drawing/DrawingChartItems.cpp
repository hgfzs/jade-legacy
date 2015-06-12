/* DrawingChartItems.cpp
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

#include <DrawingChartItems.h>
#include <DrawingItemPoint.h>

DrawingChartRectItem::DrawingChartRectItem() : DrawingRectItem()
{
	addProperty("Font Family", "Arial");
	addProperty("Font Size", 100.0);
	addProperty("Font Bold", false);
	addProperty("Font Italic", false);
	addProperty("Font Underline", false);
	addProperty("Font Overline", false);
	addProperty("Font Strike-Out", false);
	addProperty("Text Color", QColor(255, 0, 255));
	addProperty("Caption", "Label");

	setFlags(CanMove | CanRotate | CanFlip | CanResize | MatchUnitsWithParent);
	setPlaceType(PlaceMouseUp);

	point(0)->setPos(-400.0f, -200.0f);
	point(1)->setPos(400.0f, 200.0f);
	point(2)->setPos(400.0f, -200.0f);
	point(3)->setPos(-400.0f, 200.0f);
	point(4)->setPos(0.0f, -200.0f);
	point(5)->setPos(400.0f, 0.0f);
	point(6)->setPos(0.0f, 200.0f);
	point(7)->setPos(-400.0f, 0.0f);
}

DrawingChartRectItem::DrawingChartRectItem(const DrawingChartRectItem& item) : DrawingRectItem(item) { }

DrawingChartRectItem::~DrawingChartRectItem() { }

//==================================================================================================

DrawingItem* DrawingChartRectItem::copy() const
{
	return new DrawingChartRectItem(*this);
}

QString DrawingChartRectItem::uniqueKey() const
{
	return "ChartRect";
}

//==================================================================================================

void DrawingChartRectItem::setFont(const QFont& font)
{
	setFontFamily(font.family());
	setFontSize(font.pointSizeF());
	setFontBold(font.bold());
	setFontItalic(font.italic());
	setFontUnderline(font.underline());
	setFontOverline(font.overline());
	setFontStrikeOut(font.strikeOut());
}

void DrawingChartRectItem::setFontFamily(const QString& family)
{
	setPropertyValue("Font Family", family);
}

void DrawingChartRectItem::setFontSize(qreal size)
{
	setPropertyValue("Font Size", size);
}

void DrawingChartRectItem::setFontBold(bool bold)
{
	setPropertyValue("Font Bold", bold);
}

void DrawingChartRectItem::setFontItalic(bool italic)
{
	setPropertyValue("Font Italic", italic);
}

void DrawingChartRectItem::setFontUnderline(bool underline)
{
	setPropertyValue("Font Underline", underline);
}

void DrawingChartRectItem::setFontOverline(bool overline)
{
	setPropertyValue("Font Overline", overline);
}

void DrawingChartRectItem::setFontStrikeOut(bool strikeOut)
{
	setPropertyValue("Font Strike-Out", strikeOut);
}

QFont DrawingChartRectItem::font() const
{
	QFont font;

	font.setFamily(fontFamily());
	font.setPointSizeF(fontSize());
	font.setBold(isFontBold());
	font.setItalic(isFontItalic());
	font.setUnderline(isFontUnderline());
	font.setOverline(isFontOverline());
	font.setStrikeOut(isFontStrikeOut());

	return font;
}

QString DrawingChartRectItem::fontFamily() const
{
	return propertyValue("Font Family").toString();
}

qreal DrawingChartRectItem::fontSize() const
{
	return propertyValue("Font Size").toDouble();
}

bool DrawingChartRectItem::isFontBold() const
{
	return propertyValue("Font Bold").toBool();
}

bool DrawingChartRectItem::isFontItalic() const
{
	return propertyValue("Font Italic").toBool();
}

bool DrawingChartRectItem::isFontUnderline() const
{
	return propertyValue("Font Underline").toBool();
}

bool DrawingChartRectItem::isFontOverline() const
{
	return propertyValue("Font Overline").toBool();
}

bool DrawingChartRectItem::isFontStrikeOut() const
{
	return propertyValue("Font Strike-Out").toBool();
}

//==================================================================================================

void DrawingChartRectItem::setTextColor(const QColor& color)
{
	setPropertyValue("Text Color", color);
}

QColor DrawingChartRectItem::textColor() const
{
	return propertyValue("Text Color").value<QColor>();
}

//==================================================================================================

void DrawingChartRectItem::setCaption(const QString& caption)
{
	setPropertyValue("Caption", QVariant(caption));
}

QString DrawingChartRectItem::caption() const
{
	return propertyValue("Caption").toString();
}

//==================================================================================================

void DrawingChartRectItem::markDirty()
{
	mBoundingRect = QRectF();
}

//==================================================================================================

QRectF DrawingChartRectItem::boundingRect() const
{
	return DrawingRectItem::boundingRect().united(mBoundingRect);
}

QPainterPath DrawingChartRectItem::shape() const
{
	QPainterPath shape = DrawingRectItem::shape();

	if (brush().color().alpha() == 0)
		shape.addRect(mBoundingRect);

	return shape;
}

bool DrawingChartRectItem::isSuperfluous() const
{
	return (DrawingRectItem::isSuperfluous() && caption() == "");
}

//==================================================================================================

void DrawingChartRectItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QFont lFont = font();
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);
	qreal deviceFactor = 1.0;

	DrawingRectItem::render(painter, styleOptions);

	if (painter->paintEngine()->paintDevice())
		deviceFactor = 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX();

	lFont.setPointSizeF(lFont.pointSizeF() * 0.72 / scaleFactor);       // Scale to workspace
	lFont.setPointSizeF(lFont.pointSizeF() * deviceFactor);             // Scale to device

	if (!mBoundingRect.isValid()) updateLabel(lFont, painter->device());

	painter->setFont(lFont);
	setupPainter(painter, styleOptions, QPen(textColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), textColor());
	painter->translate(DrawingRectItem::boundingRect().center());
	painter->rotate(orientedTextAngle());
	painter->scale(scaleFactor, scaleFactor);
	painter->drawText(mTextRect, Qt::AlignCenter, caption());
}

//==================================================================================================

void DrawingChartRectItem::rotateItem(const QPointF& parentPos)
{
	DrawingRectItem::rotateItem(parentPos);
	markDirty();
}

void DrawingChartRectItem::rotateBackItem(const QPointF& parentPos)
{
	DrawingRectItem::rotateBackItem(parentPos);
	markDirty();
}

void DrawingChartRectItem::flipItem(const QPointF& parentPos)
{
	DrawingRectItem::flipItem(parentPos);
	markDirty();
}

void DrawingChartRectItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingRectItem::resizeItem(itemPoint, parentPos);
	markDirty();
}

//==================================================================================================

QVariant DrawingChartRectItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingRectItem::aboutToChangeEvent(reason, value);

	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setFontSize(fontSize() * scaleFactor);
	}

	return result;
}

void DrawingChartRectItem::changedEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(value);

	if (reason == UnitsChange || reason == PropertyChange) markDirty();
}

//==================================================================================================

void DrawingChartRectItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingRectItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("fontFamily", fontFamily());
	xmlWriter.writeAttribute("fontSize", QString::number(fontSize()));
	xmlWriter.writeAttribute("fontBold", isFontBold() ? "true" : "false");
	xmlWriter.writeAttribute("fontItalic", isFontItalic() ? "true" : "false");
	xmlWriter.writeAttribute("fontUnderline", isFontUnderline() ? "true" : "false");
	xmlWriter.writeAttribute("fontOverline", isFontOverline() ? "true" : "false");
	xmlWriter.writeAttribute("fontStrikeOut", isFontStrikeOut() ? "true" : "false");
	xmlWriter.writeAttribute("textColor", Drawing::colorToString(textColor()));
	xmlWriter.writeAttribute("caption", caption());
}

void DrawingChartRectItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingRectItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("fontFamily"))
		setFontFamily(attributes.value("fontFamily").toString());
	if (attributes.hasAttribute("fontSize"))
		setFontSize(attributes.value("fontSize").toString().toDouble());
	if (attributes.hasAttribute("fontBold"))
		setFontBold(attributes.value("fontBold").toString().toLower() == "true");
	if (attributes.hasAttribute("fontItalic"))
		setFontItalic(attributes.value("fontItalic").toString().toLower() == "true");
	if (attributes.hasAttribute("fontUnderline"))
		setFontUnderline(attributes.value("fontUnderline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontOverline"))
		setFontOverline(attributes.value("fontOverline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontStrikeOut"))
		setFontStrikeOut(attributes.value("fontStrikeOut").toString().toLower() == "true");

	if (attributes.hasAttribute("textColor"))
		setTextColor(Drawing::colorFromString(attributes.value("textColor").toString()));

	if (attributes.hasAttribute("caption"))
		setCaption(attributes.value("caption").toString());
}

//==================================================================================================

void DrawingChartRectItem::updateLabel(const QFont& font, QPaintDevice* device)
{
	QSizeF textSizeF = Drawing::textSize(caption(), font, device);
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);

	// Update mTextRect
	mTextRect = QRectF(QPointF(-textSizeF.width() / 2, -textSizeF.height() / 2), textSizeF);

	// Update mBoundingRect
	mBoundingRect = Drawing::rotateRect(QRectF(
		QPointF(mTextRect.left() * scaleFactor, mTextRect.top() * scaleFactor),
		QPointF(mTextRect.right() * scaleFactor, mTextRect.bottom() * scaleFactor)),
		rotationAngle()).translated(DrawingRectItem::boundingRect().center());
}

qreal DrawingChartRectItem::orientedTextAngle() const
{
	qreal textAngle = rotationAngle();

	if (isFlipped())
	{
		if (textAngle == 90) textAngle = 270;
		else if (textAngle == 270) textAngle = 90;
	}

	return textAngle;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingChartEllipseItem::DrawingChartEllipseItem() : DrawingEllipseItem()
{
	addProperty("Font Family", "Arial");
	addProperty("Font Size", 100.0);
	addProperty("Font Bold", false);
	addProperty("Font Italic", false);
	addProperty("Font Underline", false);
	addProperty("Font Overline", false);
	addProperty("Font Strike-Out", false);
	addProperty("Text Color", QColor(255, 0, 255));
	addProperty("Caption", "Label");

	setFlags(CanMove | CanRotate | CanFlip | CanResize | MatchUnitsWithParent);
	setPlaceType(PlaceMouseUp);

	point(0)->setPos(-400.0f, -200.0f);
	point(1)->setPos(400.0f, 200.0f);
	point(2)->setPos(400.0f, -200.0f);
	point(3)->setPos(-400.0f, 200.0f);
	point(4)->setPos(0.0f, -200.0f);
	point(5)->setPos(400.0f, 0.0f);
	point(6)->setPos(0.0f, 200.0f);
	point(7)->setPos(-400.0f, 0.0f);

	point(8)->setPos(-282.84, -141.42);
	point(9)->setPos(282.84, -141.42);
	point(10)->setPos(282.84, 141.42);
	point(11)->setPos(-282.84, 141.42);
}

DrawingChartEllipseItem::DrawingChartEllipseItem(const DrawingChartEllipseItem& item) : DrawingEllipseItem(item) { }

DrawingChartEllipseItem::~DrawingChartEllipseItem() { }

//==================================================================================================

DrawingItem* DrawingChartEllipseItem::copy() const
{
	return new DrawingChartEllipseItem(*this);
}

QString DrawingChartEllipseItem::uniqueKey() const
{
	return "ChartEllipse";
}

//==================================================================================================

void DrawingChartEllipseItem::setFont(const QFont& font)
{
	setFontFamily(font.family());
	setFontSize(font.pointSizeF());
	setFontBold(font.bold());
	setFontItalic(font.italic());
	setFontUnderline(font.underline());
	setFontOverline(font.overline());
	setFontStrikeOut(font.strikeOut());
}

void DrawingChartEllipseItem::setFontFamily(const QString& family)
{
	setPropertyValue("Font Family", family);
}

void DrawingChartEllipseItem::setFontSize(qreal size)
{
	setPropertyValue("Font Size", size);
}

void DrawingChartEllipseItem::setFontBold(bool bold)
{
	setPropertyValue("Font Bold", bold);
}

void DrawingChartEllipseItem::setFontItalic(bool italic)
{
	setPropertyValue("Font Italic", italic);
}

void DrawingChartEllipseItem::setFontUnderline(bool underline)
{
	setPropertyValue("Font Underline", underline);
}

void DrawingChartEllipseItem::setFontOverline(bool overline)
{
	setPropertyValue("Font Overline", overline);
}

void DrawingChartEllipseItem::setFontStrikeOut(bool strikeOut)
{
	setPropertyValue("Font Strike-Out", strikeOut);
}

QFont DrawingChartEllipseItem::font() const
{
	QFont font;

	font.setFamily(fontFamily());
	font.setPointSizeF(fontSize());
	font.setBold(isFontBold());
	font.setItalic(isFontItalic());
	font.setUnderline(isFontUnderline());
	font.setOverline(isFontOverline());
	font.setStrikeOut(isFontStrikeOut());

	return font;
}

QString DrawingChartEllipseItem::fontFamily() const
{
	return propertyValue("Font Family").toString();
}

qreal DrawingChartEllipseItem::fontSize() const
{
	return propertyValue("Font Size").toDouble();
}

bool DrawingChartEllipseItem::isFontBold() const
{
	return propertyValue("Font Bold").toBool();
}

bool DrawingChartEllipseItem::isFontItalic() const
{
	return propertyValue("Font Italic").toBool();
}

bool DrawingChartEllipseItem::isFontUnderline() const
{
	return propertyValue("Font Underline").toBool();
}

bool DrawingChartEllipseItem::isFontOverline() const
{
	return propertyValue("Font Overline").toBool();
}

bool DrawingChartEllipseItem::isFontStrikeOut() const
{
	return propertyValue("Font Strike-Out").toBool();
}

//==================================================================================================

void DrawingChartEllipseItem::setTextColor(const QColor& color)
{
	setPropertyValue("Text Color", color);
}

QColor DrawingChartEllipseItem::textColor() const
{
	return propertyValue("Text Color").value<QColor>();
}

//==================================================================================================

void DrawingChartEllipseItem::setCaption(const QString& caption)
{
	setPropertyValue("Caption", QVariant(caption));
}

QString DrawingChartEllipseItem::caption() const
{
	return propertyValue("Caption").toString();
}

//==================================================================================================

void DrawingChartEllipseItem::markDirty()
{
	mBoundingRect = QRectF();
}

//==================================================================================================

QRectF DrawingChartEllipseItem::boundingRect() const
{
	return DrawingEllipseItem::boundingRect().united(mBoundingRect);
}

QPainterPath DrawingChartEllipseItem::shape() const
{
	QPainterPath shape = DrawingEllipseItem::shape();

	if (brush().color().alpha() == 0)
		shape.addRect(mBoundingRect);

	return shape;
}

bool DrawingChartEllipseItem::isSuperfluous() const
{
	return (DrawingEllipseItem::isSuperfluous() && caption() == "");
}

//==================================================================================================

void DrawingChartEllipseItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QFont lFont = font();
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);
	qreal deviceFactor = 1.0;

	DrawingEllipseItem::render(painter, styleOptions);

	if (painter->paintEngine()->paintDevice())
		deviceFactor = 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX();

	lFont.setPointSizeF(lFont.pointSizeF() * 0.72 / scaleFactor);       // Scale to workspace
	lFont.setPointSizeF(lFont.pointSizeF() * deviceFactor);             // Scale to device

	if (!mBoundingRect.isValid()) updateLabel(lFont, painter->device());

	painter->setFont(lFont);
	setupPainter(painter, styleOptions, QPen(textColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), textColor());
	painter->translate(DrawingEllipseItem::boundingRect().center());
	painter->rotate(orientedTextAngle());
	painter->scale(scaleFactor, scaleFactor);
	painter->drawText(mTextRect, Qt::AlignCenter, caption());
}

//==================================================================================================

void DrawingChartEllipseItem::rotateItem(const QPointF& parentPos)
{
	DrawingEllipseItem::rotateItem(parentPos);
	markDirty();
}

void DrawingChartEllipseItem::rotateBackItem(const QPointF& parentPos)
{
	DrawingEllipseItem::rotateBackItem(parentPos);
	markDirty();
}

void DrawingChartEllipseItem::flipItem(const QPointF& parentPos)
{
	DrawingEllipseItem::flipItem(parentPos);
	markDirty();
}

void DrawingChartEllipseItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingEllipseItem::resizeItem(itemPoint, parentPos);
	markDirty();
}

//==================================================================================================

QVariant DrawingChartEllipseItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingEllipseItem::aboutToChangeEvent(reason, value);

	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setFontSize(fontSize() * scaleFactor);
	}

	return result;
}

void DrawingChartEllipseItem::changedEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(value);

	if (reason == UnitsChange || reason == PropertyChange) markDirty();
}

//==================================================================================================

void DrawingChartEllipseItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingEllipseItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("fontFamily", fontFamily());
	xmlWriter.writeAttribute("fontSize", QString::number(fontSize()));
	xmlWriter.writeAttribute("fontBold", isFontBold() ? "true" : "false");
	xmlWriter.writeAttribute("fontItalic", isFontItalic() ? "true" : "false");
	xmlWriter.writeAttribute("fontUnderline", isFontUnderline() ? "true" : "false");
	xmlWriter.writeAttribute("fontOverline", isFontOverline() ? "true" : "false");
	xmlWriter.writeAttribute("fontStrikeOut", isFontStrikeOut() ? "true" : "false");
	xmlWriter.writeAttribute("textColor", Drawing::colorToString(textColor()));
	xmlWriter.writeAttribute("caption", caption());
}

void DrawingChartEllipseItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingEllipseItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("fontFamily"))
		setFontFamily(attributes.value("fontFamily").toString());
	if (attributes.hasAttribute("fontSize"))
		setFontSize(attributes.value("fontSize").toString().toDouble());
	if (attributes.hasAttribute("fontBold"))
		setFontBold(attributes.value("fontBold").toString().toLower() == "true");
	if (attributes.hasAttribute("fontItalic"))
		setFontItalic(attributes.value("fontItalic").toString().toLower() == "true");
	if (attributes.hasAttribute("fontUnderline"))
		setFontUnderline(attributes.value("fontUnderline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontOverline"))
		setFontOverline(attributes.value("fontOverline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontStrikeOut"))
		setFontStrikeOut(attributes.value("fontStrikeOut").toString().toLower() == "true");

	if (attributes.hasAttribute("textColor"))
		setTextColor(Drawing::colorFromString(attributes.value("textColor").toString()));

	if (attributes.hasAttribute("caption"))
		setCaption(attributes.value("caption").toString());
}

//==================================================================================================

void DrawingChartEllipseItem::updateLabel(const QFont& font, QPaintDevice* device)
{
	QSizeF textSizeF = Drawing::textSize(caption(), font, device);
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);

	// Update mTextRect
	mTextRect = QRectF(QPointF(-textSizeF.width() / 2, -textSizeF.height() / 2), textSizeF);

	// Update mBoundingRect
	mBoundingRect = Drawing::rotateRect(QRectF(
		QPointF(mTextRect.left() * scaleFactor, mTextRect.top() * scaleFactor),
		QPointF(mTextRect.right() * scaleFactor, mTextRect.bottom() * scaleFactor)),
		rotationAngle()).translated(DrawingEllipseItem::boundingRect().center());
}

qreal DrawingChartEllipseItem::orientedTextAngle() const
{
	qreal textAngle = rotationAngle();

	if (isFlipped())
	{
		if (textAngle == 90) textAngle = 270;
		else if (textAngle == 270) textAngle = 90;
	}

	return textAngle;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingChartPolygonItem::DrawingChartPolygonItem() : DrawingPolygonItem()
{
	addProperty("Font Family", "Arial");
	addProperty("Font Size", 100.0);
	addProperty("Font Bold", false);
	addProperty("Font Italic", false);
	addProperty("Font Underline", false);
	addProperty("Font Overline", false);
	addProperty("Font Strike-Out", false);
	addProperty("Text Color", QColor(255, 0, 255));
	addProperty("Caption", "Label");

	setFlags(CanMove | CanRotate | CanFlip | CanResize | CanInsertRemovePoints | MatchUnitsWithParent);
	setPlaceType(PlaceMouseUp);
}

DrawingChartPolygonItem::DrawingChartPolygonItem(const DrawingChartPolygonItem& item) : DrawingPolygonItem(item) { }

DrawingChartPolygonItem::~DrawingChartPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingChartPolygonItem::copy() const
{
	return new DrawingChartPolygonItem(*this);
}

QString DrawingChartPolygonItem::uniqueKey() const
{
	return "ChartPolygon";
}

//==================================================================================================

void DrawingChartPolygonItem::setFont(const QFont& font)
{
	setFontFamily(font.family());
	setFontSize(font.pointSizeF());
	setFontBold(font.bold());
	setFontItalic(font.italic());
	setFontUnderline(font.underline());
	setFontOverline(font.overline());
	setFontStrikeOut(font.strikeOut());
}

void DrawingChartPolygonItem::setFontFamily(const QString& family)
{
	setPropertyValue("Font Family", family);
}

void DrawingChartPolygonItem::setFontSize(qreal size)
{
	setPropertyValue("Font Size", size);
}

void DrawingChartPolygonItem::setFontBold(bool bold)
{
	setPropertyValue("Font Bold", bold);
}

void DrawingChartPolygonItem::setFontItalic(bool italic)
{
	setPropertyValue("Font Italic", italic);
}

void DrawingChartPolygonItem::setFontUnderline(bool underline)
{
	setPropertyValue("Font Underline", underline);
}

void DrawingChartPolygonItem::setFontOverline(bool overline)
{
	setPropertyValue("Font Overline", overline);
}

void DrawingChartPolygonItem::setFontStrikeOut(bool strikeOut)
{
	setPropertyValue("Font Strike-Out", strikeOut);
}

QFont DrawingChartPolygonItem::font() const
{
	QFont font;

	font.setFamily(fontFamily());
	font.setPointSizeF(fontSize());
	font.setBold(isFontBold());
	font.setItalic(isFontItalic());
	font.setUnderline(isFontUnderline());
	font.setOverline(isFontOverline());
	font.setStrikeOut(isFontStrikeOut());

	return font;
}

QString DrawingChartPolygonItem::fontFamily() const
{
	return propertyValue("Font Family").toString();
}

qreal DrawingChartPolygonItem::fontSize() const
{
	return propertyValue("Font Size").toDouble();
}

bool DrawingChartPolygonItem::isFontBold() const
{
	return propertyValue("Font Bold").toBool();
}

bool DrawingChartPolygonItem::isFontItalic() const
{
	return propertyValue("Font Italic").toBool();
}

bool DrawingChartPolygonItem::isFontUnderline() const
{
	return propertyValue("Font Underline").toBool();
}

bool DrawingChartPolygonItem::isFontOverline() const
{
	return propertyValue("Font Overline").toBool();
}

bool DrawingChartPolygonItem::isFontStrikeOut() const
{
	return propertyValue("Font Strike-Out").toBool();
}

//==================================================================================================

void DrawingChartPolygonItem::setTextColor(const QColor& color)
{
	setPropertyValue("Text Color", color);
}

QColor DrawingChartPolygonItem::textColor() const
{
	return propertyValue("Text Color").value<QColor>();
}

//==================================================================================================

void DrawingChartPolygonItem::setCaption(const QString& caption)
{
	setPropertyValue("Caption", QVariant(caption));
}

QString DrawingChartPolygonItem::caption() const
{
	return propertyValue("Caption").toString();
}

//==================================================================================================

void DrawingChartPolygonItem::markDirty()
{
	mBoundingRect = QRectF();
}

//==================================================================================================

QRectF DrawingChartPolygonItem::boundingRect() const
{
	return DrawingPolygonItem::boundingRect().united(mBoundingRect);
}

QPainterPath DrawingChartPolygonItem::shape() const
{
	QPainterPath shape = DrawingPolygonItem::shape();

	if (brush().color().alpha() == 0)
		shape.addRect(mBoundingRect);

	return shape;
}

bool DrawingChartPolygonItem::isSuperfluous() const
{
	return (DrawingPolygonItem::isSuperfluous() && caption() == "");
}

//==================================================================================================

void DrawingChartPolygonItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QFont lFont = font();
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);
	qreal deviceFactor = 1.0;

	DrawingPolygonItem::render(painter, styleOptions);

	if (painter->paintEngine()->paintDevice())
		deviceFactor = 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX();

	lFont.setPointSizeF(lFont.pointSizeF() * 0.72 / scaleFactor);       // Scale to workspace
	lFont.setPointSizeF(lFont.pointSizeF() * deviceFactor);             // Scale to device

	if (!mBoundingRect.isValid()) updateLabel(lFont, painter->device());

	painter->setFont(lFont);
	setupPainter(painter, styleOptions, QPen(textColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), textColor());
	painter->translate(centerPoint());
	painter->rotate(orientedTextAngle());
	painter->scale(scaleFactor, scaleFactor);
	painter->drawText(mTextRect, Qt::AlignCenter, caption());
}

//==================================================================================================

void DrawingChartPolygonItem::rotateItem(const QPointF& parentPos)
{
	DrawingPolygonItem::rotateItem(parentPos);
	markDirty();
}

void DrawingChartPolygonItem::rotateBackItem(const QPointF& parentPos)
{
	DrawingPolygonItem::rotateBackItem(parentPos);
	markDirty();
}

void DrawingChartPolygonItem::flipItem(const QPointF& parentPos)
{
	DrawingPolygonItem::flipItem(parentPos);
	markDirty();
}

void DrawingChartPolygonItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingPolygonItem::resizeItem(itemPoint, parentPos);
	markDirty();
}

//==================================================================================================

QVariant DrawingChartPolygonItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	QVariant result = DrawingPolygonItem::aboutToChangeEvent(reason, value);

	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.toInt();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setFontSize(fontSize() * scaleFactor);
	}
	else if (reason == AddNewItemToScene) markDirty();

	return result;
}

void DrawingChartPolygonItem::changedEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(value);

	if (reason == UnitsChange || reason == PropertyChange) markDirty();
}

//==================================================================================================

void DrawingChartPolygonItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingPolygonItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("fontFamily", fontFamily());
	xmlWriter.writeAttribute("fontSize", QString::number(fontSize()));
	xmlWriter.writeAttribute("fontBold", isFontBold() ? "true" : "false");
	xmlWriter.writeAttribute("fontItalic", isFontItalic() ? "true" : "false");
	xmlWriter.writeAttribute("fontUnderline", isFontUnderline() ? "true" : "false");
	xmlWriter.writeAttribute("fontOverline", isFontOverline() ? "true" : "false");
	xmlWriter.writeAttribute("fontStrikeOut", isFontStrikeOut() ? "true" : "false");
	xmlWriter.writeAttribute("textColor", Drawing::colorToString(textColor()));
	xmlWriter.writeAttribute("caption", caption());
}

void DrawingChartPolygonItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingPolygonItem::readXmlAttributes(xmlReader, items);

	if (attributes.hasAttribute("fontFamily"))
		setFontFamily(attributes.value("fontFamily").toString());
	if (attributes.hasAttribute("fontSize"))
		setFontSize(attributes.value("fontSize").toString().toDouble());
	if (attributes.hasAttribute("fontBold"))
		setFontBold(attributes.value("fontBold").toString().toLower() == "true");
	if (attributes.hasAttribute("fontItalic"))
		setFontItalic(attributes.value("fontItalic").toString().toLower() == "true");
	if (attributes.hasAttribute("fontUnderline"))
		setFontUnderline(attributes.value("fontUnderline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontOverline"))
		setFontOverline(attributes.value("fontOverline").toString().toLower() == "true");
	if (attributes.hasAttribute("fontStrikeOut"))
		setFontStrikeOut(attributes.value("fontStrikeOut").toString().toLower() == "true");

	if (attributes.hasAttribute("textColor"))
		setTextColor(Drawing::colorFromString(attributes.value("textColor").toString()));

	if (attributes.hasAttribute("caption"))
		setCaption(attributes.value("caption").toString());
}

//==================================================================================================

void DrawingChartPolygonItem::updateLabel(const QFont& font, QPaintDevice* device)
{
	QSizeF textSizeF = Drawing::textSize(caption(), font, device);
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);

	// Update mTextRect
	mTextRect = QRectF(QPointF(-textSizeF.width() / 2, -textSizeF.height() / 2), textSizeF);

	// Update mBoundingRect
	mBoundingRect = Drawing::rotateRect(QRectF(
		QPointF(mTextRect.left() * scaleFactor, mTextRect.top() * scaleFactor),
		QPointF(mTextRect.right() * scaleFactor, mTextRect.bottom() * scaleFactor)),
		rotationAngle()).translated(centerPoint());
}

//==================================================================================================

qreal DrawingChartPolygonItem::orientedTextAngle() const
{
	qreal textAngle = rotationAngle();

	if (isFlipped())
	{
		if (textAngle == 90) textAngle = 270;
		else if (textAngle == 270) textAngle = 90;
	}

	return textAngle;
}

QPointF DrawingChartPolygonItem::centerPoint() const
{
	QPointF centerPoint;
	QList<DrawingItemPoint*> lPoints = points();

	for(auto pointIter = lPoints.begin(); pointIter != lPoints.end(); pointIter++)
		centerPoint += (*pointIter)->pos();

	if (!lPoints.isEmpty()) centerPoint /= lPoints.size();

	return centerPoint;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingChartPlusItem::DrawingChartPlusItem() : DrawingEllipseItem() { }

DrawingChartPlusItem::DrawingChartPlusItem(const DrawingChartPlusItem& item) : DrawingEllipseItem(item) { }

DrawingChartPlusItem::~DrawingChartPlusItem() { }

//==================================================================================================

DrawingItem* DrawingChartPlusItem::copy() const
{
	return new DrawingChartPlusItem(*this);
}

QString DrawingChartPlusItem::uniqueKey() const
{
	return "ChartPlus";
}

//==================================================================================================

void DrawingChartPlusItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QRectF bounds = boundingRect();
	QPointF center = bounds.center();

	DrawingEllipseItem::render(painter, styleOptions);

	painter->drawLine(QPointF(bounds.left(), center.y()), QPointF(bounds.right(), center.y()));
	painter->drawLine(QPointF(center.x(), bounds.top()), QPointF(center.x(), bounds.bottom()));
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingChartSumItem::DrawingChartSumItem() : DrawingEllipseItem() { }

DrawingChartSumItem::DrawingChartSumItem(const DrawingChartSumItem& item) : DrawingEllipseItem(item) { }

DrawingChartSumItem::~DrawingChartSumItem() { }

//==================================================================================================

DrawingItem* DrawingChartSumItem::copy() const
{
	return new DrawingChartSumItem(*this);
}

QString DrawingChartSumItem::uniqueKey() const
{
	return "ChartSum";
}

//==================================================================================================

void DrawingChartSumItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QRectF bounds = boundingRect();
	QPointF center = bounds.center();
	qreal xOffset = bounds.width() * qSqrt(2) / 4;
	qreal yOffset = bounds.height() * qSqrt(2) / 4;

	DrawingEllipseItem::render(painter, styleOptions);

	painter->drawLine(center + QPointF(-xOffset, -yOffset), center + QPointF(xOffset, yOffset));
	painter->drawLine(center + QPointF(-xOffset, yOffset), center + QPointF(xOffset, -yOffset));
}
