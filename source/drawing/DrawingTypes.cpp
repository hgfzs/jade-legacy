/* DrawingTypes.cpp
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

#include <DrawingTypes.h>
#include <DrawingView.h>

DrawingArrow::DrawingArrow(Style style, qreal size)
{
	mStyle = style;
	mSize = size;
}

DrawingArrow::DrawingArrow(const DrawingArrow& arrow)
{
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
}

DrawingArrow::~DrawingArrow() { }

DrawingArrow& DrawingArrow::operator=(const DrawingArrow& arrow)
{
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
	return *this;
}

//==================================================================================================

void DrawingArrow::setArrow(Style style, qreal size)
{
	mStyle = style;
	mSize = size;
}

void DrawingArrow::setStyle(Style style)
{
	mStyle = style;
}

void DrawingArrow::setSize(qreal size)
{
	mSize = size;
}

DrawingArrow::Style DrawingArrow::style() const
{
	return mStyle;
}

qreal DrawingArrow::size() const
{
	return mSize;
}

//==================================================================================================

QString DrawingArrow::toString() const
{
	return QString::number((quint32)mStyle) + "," + QString::number(mSize);
}

//==================================================================================================

void DrawingArrow::render(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		direction *= 3.141592654 / 180.0;

		switch (mStyle)
		{
		case Normal: drawNormal(painter, position, direction); break;

		case TriangleFilled:
		case Triangle: drawTriangle(painter, position, direction); break;

		case DiamondFilled:
		case Diamond: drawDiamond(painter, position, direction); break;

		case CircleFilled:
		case Circle: drawCircle(painter, position, direction); break;

		case Harpoon:
		case HarpoonMirrored: drawHarpoon(painter, position, direction); break;

		case Concave:
		case ConcaveFilled: drawConcave(painter, position, direction); break;

		case Reverse: drawReverse(painter, position, direction); break;

		case XArrow: drawX(painter, position, direction); break;

		default:
			break;
		}
	}
}

//==================================================================================================

void DrawingArrow::drawNormal(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		qreal angle = 3.14159 / 6;
		qreal sqrt2 = qSqrt(2);
		QPointF tipPoint1(position.x() + mSize / sqrt2 * cos(direction - angle),
						  position.y() + mSize / sqrt2 * sin(direction - angle));
		QPointF tipPoint2(position.x() + mSize / sqrt2 * cos(direction + angle),
						  position.y() + mSize / sqrt2 * sin(direction + angle));

		painter->drawLine(position, tipPoint1);
		painter->drawLine(position, tipPoint2);
	}
}

void DrawingArrow::drawTriangle(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		QPolygonF polygon;
		qreal angle = 3.14159 / 6;
		qreal sqrt2 = qSqrt(2);

		QBrush originalBrush = painter->brush();
		if (mStyle == TriangleFilled) painter->setBrush(painter->pen().brush());

		polygon.append(position);
		polygon.append(QPointF(position.x() + mSize / sqrt2 * cos(direction - angle),
							   position.y() + mSize / sqrt2 * sin(direction - angle)));
		polygon.append(QPointF(position.x() + mSize / sqrt2 * cos(direction + angle),
							   position.y() + mSize / sqrt2 * sin(direction + angle)));
		polygon.append(polygon[0]);
		painter->drawPolygon(polygon);

		if (mStyle == TriangleFilled) painter->setBrush(originalBrush);
	}
}

void DrawingArrow::drawCircle(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		QBrush originalBrush = painter->brush();
		if (mStyle == CircleFilled) painter->setBrush(painter->pen().brush());

		Q_UNUSED(direction);
		painter->drawEllipse(QRectF(position.x() - mSize / 2, position.y() - mSize / 2, mSize, mSize));

		if (mStyle == CircleFilled) painter->setBrush(originalBrush);
	}
}

void DrawingArrow::drawDiamond(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		QPolygonF polygon;
		qreal pi = 3.141592654;

		QBrush originalBrush = painter->brush();
		if (mStyle == DiamondFilled) painter->setBrush(painter->pen().brush());

		polygon.append(QPointF(position.x() + mSize / 2 * qCos(direction),
							   position.y() + mSize / 2 * qSin(direction)));
		polygon.append(QPointF(position.x() + mSize / 2 * qCos(direction - pi / 2),
							   position.y() + mSize / 2 * qSin(direction - pi / 2)));
		polygon.append(QPointF(position.x() + mSize / 2 * qCos(direction - pi),
							   position.y() + mSize / 2 * qSin(direction - pi)));
		polygon.append(QPointF(position.x() + mSize / 2 * qCos(direction + pi / 2),
							   position.y() + mSize / 2 * qSin(direction + pi / 2)));
		polygon.append(polygon[0]);
		painter->drawPolygon(polygon);

		if (mStyle == DiamondFilled) painter->setBrush(originalBrush);
	}
}

void DrawingArrow::drawHarpoon(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		qreal angle = 3.14159 / 6;
		qreal sqrt2 = qSqrt(2);
		QPointF tipPoint1(position.x() + mSize / sqrt2 * cos(direction - angle),
						  position.y() + mSize / sqrt2 * sin(direction - angle));
		QPointF tipPoint2(position.x() + mSize / sqrt2 * cos(direction + angle),
						  position.y() + mSize / sqrt2 * sin(direction + angle));

		if (mStyle == HarpoonMirrored) painter->drawLine(position, tipPoint1);
		else painter->drawLine(position, tipPoint2);
	}
}

void DrawingArrow::drawConcave(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		QPolygonF polygon;
		qreal angle = 3.14159 / 6;
		qreal sqrt2 = qSqrt(2);

		QBrush originalBrush = painter->brush();
		if (mStyle == ConcaveFilled) painter->setBrush(painter->pen().brush());

		polygon.append(position);
		polygon.append(QPointF(position.x() + mSize / sqrt2 * cos(direction - angle),
							   position.y() + mSize / sqrt2 * sin(direction - angle)));
		polygon.append(QPointF(position.x() + mSize / sqrt2 / 2 * cos(direction),
							   position.y() + mSize / sqrt2 / 2 * sin(direction)));
		polygon.append(QPointF(position.x() + mSize / sqrt2 * cos(direction + angle),
							   position.y() + mSize / sqrt2 * sin(direction + angle)));
		polygon.append(polygon[0]);
		painter->drawPolygon(polygon);

		if (mStyle == ConcaveFilled) painter->setBrush(originalBrush);
	}
}

void DrawingArrow::drawReverse(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		qreal angle = 3.14159 / 6;
		qreal sqrt2 = qSqrt(2);
		QPointF headPoint(position.x() + mSize / sqrt2 * cos(direction),
						  position.y() + mSize / sqrt2 * sin(direction));
		QPointF tipPoint1(headPoint.x() - mSize / sqrt2 * cos(direction - angle),
						  headPoint.y() - mSize / sqrt2 * sin(direction - angle));
		QPointF tipPoint2(headPoint.x() - mSize / sqrt2 * cos(direction + angle),
						  headPoint.y() - mSize / sqrt2 * sin(direction + angle));

		painter->drawLine(headPoint, tipPoint1);
		painter->drawLine(headPoint, tipPoint2);
	}
}

void DrawingArrow::drawX(QPainter* painter, const QPointF& position, qreal direction)
{
	if (painter)
	{
		qreal pi = 3.141592654;
		qreal sqrt2 = qSqrt(2);
		QPointF tipPoint1(position.x() + mSize / sqrt2 * cos(direction + pi / 4), position.y() + mSize / sqrt2 * sin(direction + pi / 4));
		QPointF tipPoint2(position.x() + mSize / sqrt2 * cos(direction + 5 * pi / 4), position.y() + mSize / sqrt2 * sin(direction + 5 * pi / 4));
		QPointF tipPoint3(position.x() + mSize / sqrt2 * cos(direction + 3 * pi / 4), position.y() + mSize / sqrt2 * sin(direction + 3 * pi / 4));
		QPointF tipPoint4(position.x() + mSize / sqrt2 * cos(direction + 7 * pi / 4), position.y() + mSize / sqrt2 * sin(direction + 7 * pi / 4));

		painter->drawLine(tipPoint1, tipPoint2);
		painter->drawLine(tipPoint3, tipPoint4);
	}
}

//==================================================================================================

bool DrawingArrow::operator==(const DrawingArrow& arrow) const
{
	return (mSize == arrow.mSize && mStyle == arrow.mStyle);
}

bool DrawingArrow::operator!=(const DrawingArrow& arrow) const
{
	return (mSize != arrow.mSize || mStyle != arrow.mStyle);
}

//==================================================================================================

DrawingArrow DrawingArrow::fromString(const QString& string, bool* ok)
{
	DrawingArrow arrow;
	bool conversionOk = true;
	QStringList data = string.split(",", QString::SkipEmptyParts);

	if (data.isEmpty()) conversionOk = false;
	else
	{
		if (data.size() >= 1)
		{
			bool styleOk = true;
			quint32 styleValue = data[0].toInt(&styleOk);

			if (styleOk) arrow.setStyle((DrawingArrow::Style)styleValue);

			conversionOk = (conversionOk && styleOk);
		}
		if (data.size() >= 2)
		{
			bool sizeOk = true;
			qreal sizeValue = data[1].toDouble(&sizeOk);

			if (sizeOk) arrow.setSize(sizeValue);

			conversionOk = (conversionOk && sizeOk);
		}
	}

	if (ok) *ok = conversionOk;
	return arrow;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingMouseEvent::DrawingMouseEvent() : QEvent(None)
{
	mButton = Qt::NoButton;
	mButtons = Qt::NoButton;
	mModifiers = Qt::NoModifier;
	mDragged = false;
}

DrawingMouseEvent::~DrawingMouseEvent() { }

//==================================================================================================

void DrawingMouseEvent::setType(QEvent::Type type)
{
	t = type;
}

void DrawingMouseEvent::update(DrawingView* view, QMouseEvent* event)
{
	if (view && event)
	{
		mButton = event->button();
		mButtons = event->buttons();
		mModifiers = event->modifiers();

		mPreviousScenePos = mScenePos;
		mPreviousScreenPos = mScreenPos;

		mScreenPos = event->pos();
		mScenePos = view->mapToScene(mScreenPos);

		if (event->type() == QEvent::MouseButtonPress ||
			event->type() == QEvent::MouseButtonDblClick)
		{
			mButtonDownScreenPos = event->pos();
			mButtonDownScenePos = view->mapToScene(mButtonDownScreenPos);

			mButtonDownScrollValue.setX(view->horizontalScrollBar()->value());
			mButtonDownScrollValue.setY(view->verticalScrollBar()->value());
		}

		if (event->type() == QEvent::MouseMove && (mButtons & Qt::LeftButton))
			mDragged = (mDragged || Drawing::magnitude(mScreenPos - mButtonDownScreenPos) > 4);
		else if (event->type() != QEvent::MouseButtonRelease &&
			event->type() != QEvent::MouseButtonDblClick)
			mDragged = false;
	}
}

void DrawingMouseEvent::update(DrawingView* view, QContextMenuEvent* event)
{
	if (view && event)
	{
		mButton = Qt::RightButton;
		mButtons = Qt::RightButton;
		mModifiers = event->modifiers();

		mPreviousScenePos = mScenePos;
		mPreviousScreenPos = mScreenPos;

		mScreenPos = event->pos();
		mScenePos = view->mapToScene(mScreenPos);

		mButtonDownScreenPos = event->pos();
		mButtonDownScenePos = view->mapToScene(mButtonDownScreenPos);

		mButtonDownScrollValue.setX(view->horizontalScrollBar()->value());
		mButtonDownScrollValue.setY(view->verticalScrollBar()->value());
		mDragged = false;
	}
}

//==================================================================================================

Qt::MouseButton DrawingMouseEvent::button() const
{
	return mButton;
}

Qt::MouseButtons DrawingMouseEvent::buttons() const
{
	return mButtons;
}

Qt::KeyboardModifiers DrawingMouseEvent::modifiers() const
{
	return mModifiers;
}

//==================================================================================================

QPointF DrawingMouseEvent::scenePos() const
{
	return mScenePos;
}

QPointF DrawingMouseEvent::previousScenePos() const
{
	return mPreviousScenePos;
}

QPointF DrawingMouseEvent::buttonDownScenePos() const
{
	return mButtonDownScenePos;
}

//==================================================================================================

QPoint DrawingMouseEvent::screenPos() const
{
	return mScreenPos;
}

QPoint DrawingMouseEvent::previousScreenPos() const
{
	return mPreviousScreenPos;
}

QPoint DrawingMouseEvent::buttonDownScreenPos() const
{
	return mButtonDownScreenPos;
}

//==================================================================================================

QPoint DrawingMouseEvent::buttonDownScrollValue() const
{
	return mButtonDownScrollValue;
}

bool DrawingMouseEvent::isDragged() const
{
	return mDragged;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingStyleOptions::DrawingStyleOptions()
{
	mColorMode = Normal;

	mBrushes.append(Qt::white);                             // Background
	mBrushes.append(QBrush(QColor(0, 0, 0)));				// Border
	mBrushes.append(QBrush(QColor(0, 128, 128)));           // Grid
	mBrushes.append(QBrush(QColor(144, 160, 172, 128)));    // RubberBand
	mBrushes.append(QBrush(QColor(0, 224, 0)));             // ResizePoint
	mBrushes.append(QBrush(QColor(255, 128, 0, 128)));      // Hotpoint
	mBrushes.append(QBrush(QColor(255, 0, 0)));             // Item
	mBrushes.append(QBrush(QColor(255, 255, 0)));           // AlternateItem
	mBrushes.append(QBrush(QColor(0, 0, 0)));				// UserDefined

	mRenderFlags = DrawBackground | DrawBorder | DrawGrid;

	mGridStyle = GridGraphPaper;
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;
}

DrawingStyleOptions::DrawingStyleOptions(const DrawingStyleOptions& other)
{
	mColorMode = other.mColorMode;
	mBrushes = other.mBrushes;
	mRenderFlags = other.mRenderFlags;
	mGridStyle = other.mGridStyle;
	mGridSpacingMajor = other.mGridSpacingMajor;
	mGridSpacingMinor = other.mGridSpacingMinor;
}

DrawingStyleOptions::~DrawingStyleOptions() { }

//==================================================================================================

DrawingStyleOptions& DrawingStyleOptions::operator=(const DrawingStyleOptions& other)
{
	mColorMode = other.mColorMode;
	mBrushes = other.mBrushes;
	mRenderFlags = other.mRenderFlags;
	mGridStyle = other.mGridStyle;
	mGridSpacingMajor = other.mGridSpacingMajor;
	mGridSpacingMinor = other.mGridSpacingMinor;
	return *this;
}

//==================================================================================================

void DrawingStyleOptions::setColorMode(ColorMode mode)
{
	mColorMode = mode;
}

DrawingStyleOptions::ColorMode DrawingStyleOptions::colorMode() const
{
	return mColorMode;
}

//==================================================================================================

void DrawingStyleOptions::setBrush(BrushType index, const QBrush& brush)
{
	 mBrushes.replace(index, brush);
}

QBrush DrawingStyleOptions::brush(BrushType index) const
{
	return mBrushes[index];
}

QBrush DrawingStyleOptions::outputBrush(BrushType index) const
{
	QBrush outputBrush;

	switch (mColorMode)
	{
	case BlackOnWhite:
		outputBrush = (index == Background) ? Qt::white : Qt::black;
		break;
	case WhiteOnBlack:
		outputBrush = (index == Background) ? Qt::black : Qt::white;
		break;
	default:
		outputBrush = brush(index);
		break;
	}

	return outputBrush;
}

//==================================================================================================

void DrawingStyleOptions::setRenderFlags(RenderFlags flags)
{
	mRenderFlags = flags;
}

DrawingStyleOptions::RenderFlags DrawingStyleOptions::renderFlags() const
{
	return mRenderFlags;
}

bool DrawingStyleOptions::shouldDrawBackground() const
{
	return (mRenderFlags & DrawBackground);
}

bool DrawingStyleOptions::shouldDrawBorder() const
{
	return (mRenderFlags & DrawBorder);
}

bool DrawingStyleOptions::shouldDrawGrid() const
{
	return (mRenderFlags & DrawGrid);
}

//==================================================================================================

void DrawingStyleOptions::setGridStyle(GridStyle style)
{
	mGridStyle = style;
}

void DrawingStyleOptions::setGridSpacing(int major, int minor)
{
	mGridSpacingMajor = major;
	mGridSpacingMinor = minor;
}

void DrawingStyleOptions::setMajorGridSpacing(int major)
{
	mGridSpacingMajor = major;
}

void DrawingStyleOptions::setMinorGridSpacing(int minor)
{
	mGridSpacingMinor = minor;
}

DrawingStyleOptions::GridStyle DrawingStyleOptions::gridStyle() const
{
	return mGridStyle;
}

int DrawingStyleOptions::majorGridSpacing() const
{
	return mGridSpacingMajor;
}

int DrawingStyleOptions::minorGridSpacing() const
{
	return mGridSpacingMinor;
}
