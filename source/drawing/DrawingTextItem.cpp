/* DrawingTextItem.cpp
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

#include <DrawingTextItem.h>
#include <DrawingItemPoint.h>

DrawingTextItem::DrawingTextItem() : DrawingItem()
{
	addProperty("Font Family", "Arial");
	addProperty("Font Size", 100.0);
	addProperty("Font Bold", false);
	addProperty("Font Italic", false);
	addProperty("Font Underline", false);
	addProperty("Font Overline", false);
	addProperty("Font Strike-Out", false);
	addProperty("Text Horizontal Alignment", Qt::AlignHCenter);
	addProperty("Text Vertical Alignment", Qt::AlignVCenter);
	addProperty("Text Color", QColor(255, 0, 255));
	addProperty("Caption", "Label");

	setFlags(CanMove | CanRotate | CanFlip | MatchUnitsWithParent);
	setPlaceType(PlaceMouseUp);

	addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::Control, 0));
}

DrawingTextItem::DrawingTextItem(const DrawingTextItem& item) : DrawingItem(item) { }

DrawingTextItem::~DrawingTextItem() { }

//==================================================================================================

DrawingItem* DrawingTextItem::copy() const
{
	return new DrawingTextItem(*this);
}

QString DrawingTextItem::uniqueKey() const
{
	return "text";
}

//==================================================================================================

void DrawingTextItem::setFont(const QFont& font)
{
	setFontFamily(font.family());
	setFontSize(font.pointSizeF());
	setFontBold(font.bold());
	setFontItalic(font.italic());
	setFontUnderline(font.underline());
	setFontOverline(font.overline());
	setFontStrikeOut(font.strikeOut());
}

void DrawingTextItem::setFontFamily(const QString& family)
{
	setPropertyValue("Font Family", family);
}

void DrawingTextItem::setFontSize(qreal size)
{
	setPropertyValue("Font Size", size);
}

void DrawingTextItem::setFontBold(bool bold)
{
	setPropertyValue("Font Bold", bold);
}

void DrawingTextItem::setFontItalic(bool italic)
{
	setPropertyValue("Font Italic", italic);
}

void DrawingTextItem::setFontUnderline(bool underline)
{
	setPropertyValue("Font Underline", underline);
}

void DrawingTextItem::setFontOverline(bool overline)
{
	setPropertyValue("Font Overline", overline);
}

void DrawingTextItem::setFontStrikeOut(bool strikeOut)
{
	setPropertyValue("Font Strike-Out", strikeOut);
}

QFont DrawingTextItem::font() const
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

QString DrawingTextItem::fontFamily() const
{
	return propertyValue("Font Family").toString();
}

qreal DrawingTextItem::fontSize() const
{
	return propertyValue("Font Size").toDouble();
}

bool DrawingTextItem::isFontBold() const
{
	return propertyValue("Font Bold").toBool();
}

bool DrawingTextItem::isFontItalic() const
{
	return propertyValue("Font Italic").toBool();
}

bool DrawingTextItem::isFontUnderline() const
{
	return propertyValue("Font Underline").toBool();
}

bool DrawingTextItem::isFontOverline() const
{
	return propertyValue("Font Overline").toBool();
}

bool DrawingTextItem::isFontStrikeOut() const
{
	return propertyValue("Font Strike-Out").toBool();
}

//==================================================================================================

void DrawingTextItem::setAlignmentHorizontal(Qt::Alignment alignment)
{
	setPropertyValue("Text Horizontal Alignment", QVariant((quint32)(alignment & Qt::AlignHorizontal_Mask)));
}

void DrawingTextItem::setAlignmentVertical(Qt::Alignment alignment)
{
	setPropertyValue("Text Vertical Alignment", QVariant((quint32)(alignment & Qt::AlignVertical_Mask)));
}

void DrawingTextItem::setAlignment(Qt::Alignment alignment)
{
	setAlignmentHorizontal(alignment);
	setAlignmentVertical(alignment);
}

Qt::Alignment DrawingTextItem::alignmentHorizontal() const
{
	return (Qt::Alignment)propertyValue("Text Horizontal Alignment").toUInt();
}

Qt::Alignment DrawingTextItem::alignmentVertical() const
{
	return (Qt::Alignment)propertyValue("Text Vertical Alignment").toUInt();
}

Qt::Alignment DrawingTextItem::alignment() const
{
	return (alignmentHorizontal() | alignmentVertical());
}

//==================================================================================================

void DrawingTextItem::setColor(const QColor& color)
{
	setPropertyValue("Text Color", color);
}

QColor DrawingTextItem::color() const
{
	return propertyValue("Text Color").value<QColor>();
}

//==================================================================================================

void DrawingTextItem::setCaption(const QString& caption)
{
	setPropertyValue("Caption", QVariant(caption));
}

QString DrawingTextItem::caption() const
{
	return propertyValue("Caption").toString();
}

//==================================================================================================

void DrawingTextItem::markDirty()
{
	mBoundingRect = QRectF();
}

//==================================================================================================

QRectF DrawingTextItem::boundingRect() const
{
	return mBoundingRect;
}

QPointF DrawingTextItem::centerPos() const
{
	return QPointF(0.0, 0.0);
}

bool DrawingTextItem::isSuperfluous() const
{
	return (caption() == "");
}

//==================================================================================================

void DrawingTextItem::render(QPainter* painter, const DrawingStyleOptions& styleOptions)
{
	QFont lFont = font();
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);
	qreal deviceFactor = 1.0;

	if (painter->paintEngine()->paintDevice())
		deviceFactor = 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX();
	lFont.setPointSizeF(lFont.pointSizeF() * 0.72 / scaleFactor);       // Scale to workspace
	lFont.setPointSizeF(lFont.pointSizeF() * deviceFactor);             // Scale to device

	if (!mBoundingRect.isValid()) updateLabel(lFont, painter->device());

#ifdef DEBUG_DRAW_ITEM_SHAPE
	painter->setBrush(Qt::magenta);
	painter->setPen(QPen(Qt::magenta, 1));
	painter->drawPath(shape());
#endif

	setupPainter(painter, styleOptions, QPen(color(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), color());
	painter->setFont(lFont);

	painter->rotate(orientedTextAngle());
	painter->scale(scaleFactor, scaleFactor);
	painter->drawText(mTextRect, alignment(), caption());
}

//==================================================================================================

void DrawingTextItem::rotateItem(const QPointF& parentPos)
{
	DrawingItem::rotateItem(parentPos);
	markDirty();
}

void DrawingTextItem::rotateBackItem(const QPointF& parentPos)
{
	DrawingItem::rotateBackItem(parentPos);
	markDirty();
}

void DrawingTextItem::flipItem(const QPointF& parentPos)
{
	DrawingItem::flipItem(parentPos);
	Qt::Alignment align = alignmentHorizontal();

	if (align & Qt::AlignLeft) align = Qt::AlignRight;
	else if (align & Qt::AlignRight) align = Qt::AlignLeft;

	setAlignmentHorizontal(align);
	markDirty();
}

//==================================================================================================

QVariant DrawingTextItem::aboutToChangeEvent(Reason reason, const QVariant& value)
{
	if (reason == UnitsChange)
	{
		DrawingUnits newUnits = (DrawingUnits)value.value<quint16>();
		qreal scaleFactor = Drawing::unitsScale(units(), newUnits);

		setFontSize(fontSize() * scaleFactor);
	}

	return value;
}

void DrawingTextItem::changedEvent(Reason reason, const QVariant& value)
{
	Q_UNUSED(value);

	if (reason == UnitsChange || reason == PropertyChange) markDirty();
}

//==================================================================================================

void DrawingTextItem::writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items)
{
	DrawingItem::writeXmlAttributes(xmlWriter, items);

	xmlWriter.writeAttribute("fontFamily", fontFamily());
	xmlWriter.writeAttribute("fontSize", QString::number(fontSize()));
	xmlWriter.writeAttribute("fontBold", isFontBold() ? "true" : "false");
	xmlWriter.writeAttribute("fontItalic", isFontItalic() ? "true" : "false");
	xmlWriter.writeAttribute("fontUnderline", isFontUnderline() ? "true" : "false");
	xmlWriter.writeAttribute("fontOverline", isFontOverline() ? "true" : "false");
	xmlWriter.writeAttribute("fontStrikeOut", isFontStrikeOut() ? "true" : "false");
	xmlWriter.writeAttribute("textAlignment", QString::number((quint32)alignment()));
	xmlWriter.writeAttribute("textColor", Drawing::colorToString(color()));
	xmlWriter.writeAttribute("caption", caption());
}

void DrawingTextItem::readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	DrawingItem::readXmlAttributes(xmlReader, items);

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

	if (attributes.hasAttribute("textAlignment"))
		setAlignment((Qt::Alignment)attributes.value("textAlignment").toString().toUInt());

	if (attributes.hasAttribute("textColor"))
		setColor(Drawing::colorFromString(attributes.value("textColor").toString()));

	if (attributes.hasAttribute("caption"))
		setCaption(attributes.value("caption").toString());
}

//==================================================================================================

void DrawingTextItem::updateLabel(const QFont& font, QPaintDevice* device)
{
	Qt::Alignment align = alignment();
	qreal scaleFactor = 1.0 / Drawing::unitsScale(units(), UnitsMils);

	// Update mTextRect
	mTextRect = QRectF(QPointF(0.0, 0.0), Drawing::textSize(caption(), font, device));

	if (align & Qt::AlignLeft) mTextRect.translate(0.0, 0.0);
	else if (align & Qt::AlignRight) mTextRect.translate(-mTextRect.width(), 0.0);
	else mTextRect.translate(-mTextRect.width() / 2, 0.0);

	if (align & Qt::AlignBottom) mTextRect.translate(0.0, -mTextRect.height());
	else if (align & Qt::AlignTop) mTextRect.translate(0.0, 0.0);
	else mTextRect.translate(0.0, -mTextRect.height() / 2);

	// Update mBoundingRect
	mBoundingRect = Drawing::rotateRect(QRectF(
		QPointF(mTextRect.left() * scaleFactor, mTextRect.top() * scaleFactor),
		QPointF(mTextRect.right() * scaleFactor, mTextRect.bottom() * scaleFactor)),
		rotationAngle());
}

qreal DrawingTextItem::orientedTextAngle() const
{
	//bool rotated = (rotationAngle() == 90.0 || rotationAngle() == 270.0);
	qreal textAngle = rotationAngle();

	if (isFlipped())
	{
		if (textAngle == 90) textAngle = 270;
		else if (textAngle == 270) textAngle = 90;
	}

	return textAngle;
}
