/* DrawingTextItem.h
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

#ifndef DRAWINGTEXTITEM_H
#define DRAWINGTEXTITEM_H

#include <DrawingItem.h>

class DrawingTextItem : public DrawingItem
{
protected:
	QRectF mBoundingRect;
	QRectF mTextRect;

public:
	DrawingTextItem();
	DrawingTextItem(const DrawingTextItem& item);
	~DrawingTextItem();

	virtual DrawingItem* copy() const;
	virtual QString uniqueKey() const;

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

	void setAlignmentHorizontal(Qt::Alignment alignment);
	void setAlignmentVertical(Qt::Alignment alignment);
	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignmentHorizontal() const;
	Qt::Alignment alignmentVertical() const;
	Qt::Alignment alignment() const;

	void setColor(const QColor& color);
	QColor color() const;

	void setCaption(const QString& caption);
	QString caption() const;

	void markDirty();

	// Description
	virtual QRectF boundingRect() const;
	virtual QPointF centerPos() const;
	virtual bool isSuperfluous() const;

	// Render
	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions);

	virtual void rotateItem(const QPointF& parentPos);
	virtual void rotateBackItem(const QPointF& parentPos);
	virtual void flipItem(const QPointF& parentPos);

protected:
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);
	virtual void changedEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

	void updateLabel(const QFont& font, QPaintDevice* device);
	qreal orientedTextAngle() const;
};

#endif
