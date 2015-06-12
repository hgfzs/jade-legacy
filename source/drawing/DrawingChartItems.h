/* DrawingChartItems.h
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

#ifndef DRAWINGCHARTITEMS_H
#define DRAWINGCHARTITEMS_H

#include <DrawingRectItems.h>
#include <DrawingPolyItems.h>

class DrawingChartRectItem : public DrawingRectItem
{
protected:
	QRectF mBoundingRect;
	QRectF mTextRect;

public:
	DrawingChartRectItem();
	DrawingChartRectItem(const DrawingChartRectItem& item);
	~DrawingChartRectItem();

	DrawingItem* copy() const;
	QString uniqueKey() const;

	// Selectors
	void setFont(const QFont& font);
	void setFontFamily(const QString& family);
	void setFontSize(qreal size);
	void setFontBold(bool bold);
	void setFontItalic(bool italic);
	void setFontUnderline(bool underline);
	void setFontOverline(bool overline);
	void setFontStrikeOut(bool strikeOut);
	QFont font() const;
	QString fontFamily() const;
	qreal fontSize() const;
	bool isFontBold() const;
	bool isFontItalic() const;
	bool isFontUnderline() const;
	bool isFontOverline() const;
	bool isFontStrikeOut() const;

	void setTextColor(const QColor& color);
	QColor textColor() const;

	void setCaption(const QString& caption);
	QString caption() const;

	void markDirty();

	// Description
	QRectF boundingRect() const;
	QPainterPath shape() const;
	bool isSuperfluous() const;

	// Render
	void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	void rotateItem(const QPointF& parentPos);
	void rotateBackItem(const QPointF& parentPos);
	void flipItem(const QPointF& parentPos);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos);

protected:
	QVariant aboutToChangeEvent(Reason reason, const QVariant& value);
	void changedEvent(Reason reason, const QVariant& value);

	void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	void updateLabel(const QFont& font, QPaintDevice* device);
	qreal orientedTextAngle() const;
};

//==================================================================================================

class DrawingChartEllipseItem : public DrawingEllipseItem
{
protected:
	QRectF mBoundingRect;
	QRectF mTextRect;

public:
	DrawingChartEllipseItem();
	DrawingChartEllipseItem(const DrawingChartEllipseItem& item);
	~DrawingChartEllipseItem();

	DrawingItem* copy() const;
	QString uniqueKey() const;

	// Selectors
	void setFont(const QFont& font);
	void setFontFamily(const QString& family);
	void setFontSize(qreal size);
	void setFontBold(bool bold);
	void setFontItalic(bool italic);
	void setFontUnderline(bool underline);
	void setFontOverline(bool overline);
	void setFontStrikeOut(bool strikeOut);
	QFont font() const;
	QString fontFamily() const;
	qreal fontSize() const;
	bool isFontBold() const;
	bool isFontItalic() const;
	bool isFontUnderline() const;
	bool isFontOverline() const;
	bool isFontStrikeOut() const;

	void setTextColor(const QColor& color);
	QColor textColor() const;

	void setCaption(const QString& caption);
	QString caption() const;

	void markDirty();

	// Description
	QRectF boundingRect() const;
	QPainterPath shape() const;
	bool isSuperfluous() const;

	// Render
	void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	void rotateItem(const QPointF& parentPos);
	void rotateBackItem(const QPointF& parentPos);
	void flipItem(const QPointF& parentPos);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos);

protected:
	QVariant aboutToChangeEvent(Reason reason, const QVariant& value);
	void changedEvent(Reason reason, const QVariant& value);

	void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	void updateLabel(const QFont& font, QPaintDevice* device);
	qreal orientedTextAngle() const;
};

//==================================================================================================

class DrawingChartPolygonItem : public DrawingPolygonItem
{
protected:
	QRectF mBoundingRect;
	QRectF mTextRect;

public:
	DrawingChartPolygonItem();
	DrawingChartPolygonItem(const DrawingChartPolygonItem& item);
	~DrawingChartPolygonItem();

	DrawingItem* copy() const;
	QString uniqueKey() const;

	// Selectors
	void setFont(const QFont& font);
	void setFontFamily(const QString& family);
	void setFontSize(qreal size);
	void setFontBold(bool bold);
	void setFontItalic(bool italic);
	void setFontUnderline(bool underline);
	void setFontOverline(bool overline);
	void setFontStrikeOut(bool strikeOut);
	QFont font() const;
	QString fontFamily() const;
	qreal fontSize() const;
	bool isFontBold() const;
	bool isFontItalic() const;
	bool isFontUnderline() const;
	bool isFontOverline() const;
	bool isFontStrikeOut() const;

	void setTextColor(const QColor& color);
	QColor textColor() const;

	void setCaption(const QString& caption);
	QString caption() const;

	void markDirty();

	// Description
	QRectF boundingRect() const;
	QPainterPath shape() const;
	bool isSuperfluous() const;

	// Render
	void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	void rotateItem(const QPointF& parentPos);
	void rotateBackItem(const QPointF& parentPos);
	void flipItem(const QPointF& parentPos);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos);

protected:
	QVariant aboutToChangeEvent(Reason reason, const QVariant& value);
	void changedEvent(Reason reason, const QVariant& value);

	void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	void updateLabel(const QFont& font, QPaintDevice* device);

	qreal orientedTextAngle() const;
	QPointF centerPoint() const;
};

//==================================================================================================

class DrawingChartPlusItem : public DrawingEllipseItem
{
public:
	DrawingChartPlusItem();
	DrawingChartPlusItem(const DrawingChartPlusItem& item);
	~DrawingChartPlusItem();

	DrawingItem* copy() const;
	QString uniqueKey() const;

	// Render
	void render(QPainter* painter, const DrawingStyleOptions& styleOptions);
};

//==================================================================================================

class DrawingChartSumItem : public DrawingEllipseItem
{
public:
	DrawingChartSumItem();
	DrawingChartSumItem(const DrawingChartSumItem& item);
	~DrawingChartSumItem();

	DrawingItem* copy() const;
	QString uniqueKey() const;

	// Render
	void render(QPainter* painter, const DrawingStyleOptions& styleOptions);
};

#endif
