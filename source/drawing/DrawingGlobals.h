/* DrawingGlobals.h
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

#ifndef DRAWINGGLOBALS_H
#define DRAWINGGLOBALS_H

#include <QtCore>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtGui>
#else
#include <QtWidgets>
#endif

//#define DEBUG_DRAW_ITEM_SHAPE
#undef DEBUG_DRAW_ITEM_SHAPE

class DrawingView;
class DrawingScene;
class DrawingItem;
class DrawingItemPoint;

enum DrawingUnits { UnitsMils, UnitsSimpleMM, UnitsMM };
enum DrawingItemPlaceMode { DoNotPlace, PlaceStrict, PlaceLoose };

//==================================================================================================

namespace Drawing
{

QRectF rectFromPoints(const QPointF& point0, const QPointF& point1);
QRectF adjustRectForMinimumSize(const QRectF& rect);
QRectF rotateRect(const QRectF& rect, qreal rotationAngle);

qreal magnitude(const QPointF &vec);
qreal magnitude(const QPoint &vec);
qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line);

QSizeF textSize(const QString& text, const QFont& font, QPaintDevice* device);

qreal unitsScale(DrawingUnits units, DrawingUnits newUnits);

QString colorToString(const QColor& color);
QColor colorFromString(const QString& string, bool* ok = nullptr);

QString penToString(const QPen& pen);
QString brushToString(const QBrush& brush);
QString fontToString(const QFont& font);
QString rectToString(const QRectF& rect);
QString pathToString(const QPainterPath& path);
QPen penFromString(const QString& string, bool* ok = nullptr);
QBrush brushFromString(const QString& string, bool* ok = nullptr);
QFont fontFromString(const QString& string, bool* ok = nullptr);
QRectF rectFromString(const QString& string, bool* ok = nullptr);
QPainterPath pathFromString(const QString& string, bool* ok = nullptr);

QString pointToString(const QPointF& point, DrawingUnits units);

}

#endif
