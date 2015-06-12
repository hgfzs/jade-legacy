/* DrawingTypes.h
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

#ifndef DRAWINGTYPES_H
#define DRAWINGTYPES_H

#include <DrawingGlobals.h>

class DrawingArrow
{
public:
	enum Style { None, Normal, Triangle, TriangleFilled,
		Circle, CircleFilled, Diamond, DiamondFilled,
		Harpoon, HarpoonMirrored, Concave, ConcaveFilled, Reverse, XArrow };

private:
	Style mStyle;
	qreal mSize;

public:
	DrawingArrow(Style style = None, qreal size = 100.0);
	DrawingArrow(const DrawingArrow& arrow);
	virtual ~DrawingArrow();

	DrawingArrow& operator=(const DrawingArrow& arrow);

	void setArrow(Style style, qreal size);
	void setStyle(Style style);
	void setSize(qreal size);
	Style style() const;
	qreal size() const;

	QString toString() const;

	void render(QPainter* painter, const QPointF& position, qreal direction);

	void drawNormal(QPainter* painter, const QPointF& position, qreal direction);
	void drawTriangle(QPainter* painter, const QPointF& position, qreal direction);
	void drawCircle(QPainter* painter, const QPointF& position, qreal direction);
	void drawDiamond(QPainter* painter, const QPointF& position, qreal direction);
	void drawHarpoon(QPainter* painter, const QPointF& position, qreal direction);
	void drawConcave(QPainter* painter, const QPointF& position, qreal direction);
	void drawReverse(QPainter* painter, const QPointF& position, qreal direction);
	void drawX(QPainter* painter, const QPointF& position, qreal direction);

	bool operator==(const DrawingArrow& arrow) const;
	bool operator!=(const DrawingArrow& arrow) const;

public:
	static DrawingArrow fromString(const QString& string, bool* ok = nullptr);
};

Q_DECLARE_METATYPE(DrawingArrow)

//==================================================================================================

class DrawingMouseEvent : public QEvent
{
private:
	Qt::MouseButton mButton;
	Qt::MouseButtons mButtons;
	Qt::KeyboardModifiers mModifiers;

	QPointF mScenePos;
	QPointF mPreviousScenePos;
	QPointF mButtonDownScenePos;

	QPoint mScreenPos;
	QPoint mPreviousScreenPos;
	QPoint mButtonDownScreenPos;

	QPoint mButtonDownScrollValue;
	bool mDragged;

public:
	DrawingMouseEvent();
	~DrawingMouseEvent();

	void setType(QEvent::Type type);
	void update(DrawingView* view, QMouseEvent* event);
	void update(DrawingView* view, QContextMenuEvent* event);

	Qt::MouseButton button() const;
	Qt::MouseButtons buttons() const;
	Qt::KeyboardModifiers modifiers() const;

	QPointF scenePos() const;
	QPointF previousScenePos() const;
	QPointF buttonDownScenePos() const;

	QPoint screenPos() const;
	QPoint previousScreenPos() const;
	QPoint buttonDownScreenPos() const;

	QPoint buttonDownScrollValue() const;
	bool isDragged() const;
};

//==================================================================================================

class DrawingStyleOptions
{
public:
	enum ColorMode { Normal, BlackOnWhite, WhiteOnBlack };
	enum BrushType { Background, Border, Grid, RubberBand, ResizePoint, Hotpoint, Item, AlternateItem, UserDefined };
	enum GridStyle { GridNone, GridDotted, GridLined, GridGraphPaper };

	enum RenderFlag { DrawBackground = 0x01, DrawBorder = 0x02, DrawGrid = 0x04 };
	Q_DECLARE_FLAGS(RenderFlags, RenderFlag)

private:
	ColorMode mColorMode;
	QList<QBrush> mBrushes;

	RenderFlags mRenderFlags;

	GridStyle mGridStyle;
	int mGridSpacingMajor;
	int mGridSpacingMinor;

public:
	DrawingStyleOptions();
	DrawingStyleOptions(const DrawingStyleOptions& other);
	~DrawingStyleOptions();

	DrawingStyleOptions& operator=(const DrawingStyleOptions& other);

	void setColorMode(ColorMode mode);
	ColorMode colorMode() const;

	void setBrush(BrushType index, const QBrush& brush);
	QBrush brush(BrushType index) const;
	QBrush outputBrush(BrushType index) const;

	void setRenderFlags(RenderFlags flags);
	RenderFlags renderFlags() const;
	bool shouldDrawBackground() const;
	bool shouldDrawBorder() const;
	bool shouldDrawGrid() const;

	void setGridStyle(GridStyle style);
	void setGridSpacing(int major, int minor);
	void setMajorGridSpacing(int major);
	void setMinorGridSpacing(int minor);
	GridStyle gridStyle() const;
	int majorGridSpacing() const;
	int minorGridSpacing() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingStyleOptions::RenderFlags)

#endif
