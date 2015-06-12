/* DrawingGlobals.cpp
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

#include <DrawingGlobals.h>

namespace Drawing
{

QRectF rectFromPoints(const QPointF& point0, const QPointF& point1)
{
	return QRectF(qMin(point0.x(), point1.x()), qMin(point0.y(), point1.y()),
			qAbs(point0.x() - point1.x()), qAbs(point0.y() - point1.y()));
}

QRectF adjustRectForMinimumSize(const QRectF& rect)
{
	QRectF result = rect;
	if (result.width() == 0) result.setWidth(0.001);
	if (result.height() == 0) result.setHeight(0.001);
	return result;
}

QRectF rotateRect(const QRectF& rect, qreal rotationAngle)
{
	qreal l = rect.left();
	qreal t = rect.top();
	qreal r = rect.right();
	qreal b = rect.bottom();
	qreal deltaX, deltaY;

	while (rotationAngle >= 360.0) rotationAngle -= 360.0;
	while (rotationAngle < 0.0) rotationAngle += 360.0;

	if (rotationAngle == 90)
	{
		deltaX = l; deltaY = t;
		l = -deltaY; t = deltaX;
		deltaX = r; deltaY = b;
		r = -deltaY; b = deltaX;
	}
	else if (rotationAngle == 180)
	{
		l *= -1; t *= -1; r *= -1; b *= -1;
	}
	else if (rotationAngle == 270)
	{
		deltaX = l; deltaY = t;
		l = deltaY; t = -deltaX;
		deltaX = r; deltaY = b;
		r = deltaY; b = -deltaX;
	}

	return QRectF(qMin(l, r), qMin(t, b), qAbs(r - l), qAbs(b - t));
}

//==================================================================================================

qreal magnitude(const QPointF &vec)
{
	return qSqrt(vec.x() * vec.x() + vec.y() * vec.y());
}

qreal magnitude(const QPoint &vec)
{
	return qSqrt(vec.x() * vec.x() + vec.y() * vec.y());
}

qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line)
{
	qreal distance = 1E10;
	qreal dotAbBc, dotBaAc, crossABC, distanceAB, distanceAC, distanceBC;

	// Let A = line point 0, B = line point 1, and C = point
	dotAbBc = (line.x2() - line.x1()) * (point.x() - line.x2()) +
			  (line.y2() - line.y1()) * (point.y() - line.y2());
	dotBaAc = (line.x1() - line.x2()) * (point.x() - line.x1()) +
			  (line.y1() - line.y2()) * (point.y() - line.y1());
	crossABC = (line.x2() - line.x1()) * (point.y() - line.y1()) -
			   (line.y2() - line.y1()) * (point.x() - line.x1());
	distanceAB = qSqrt( (line.x2() - line.x1()) * (line.x2() - line.x1()) +
						(line.y2() - line.y1()) * (line.y2() - line.y1()) );
	distanceAC = qSqrt( (point.x() - line.x1()) * (point.x() - line.x1()) +
						(point.y() - line.y1()) * (point.y() - line.y1()) );
	distanceBC = qSqrt( (point.x() - line.x2()) * (point.x() - line.x2()) +
						(point.y() - line.y2()) * (point.y() - line.y2()) );

	if (distanceAB != 0)
	{
		if (dotAbBc > 0) distance = distanceBC;
		else if (dotBaAc > 0) distance = distanceAC;
		else distance = qAbs(crossABC) / distanceAB;
	}

	return distance;
}

//==================================================================================================

QSizeF textSize(const QString& text, const QFont& font, QPaintDevice* device)
{
	QFontMetricsF fontMetrics(font, device);
	QStringList lines = text.split("\n");
	QSizeF textSize(0, (lines.size() - 1) * fontMetrics.leading());
	QString line;
	QRectF tempRect;

	while (!lines.isEmpty())
	{
		line = lines.takeFirst();

		if (line.isEmpty()) tempRect = QRectF(0, 0, 0, fontMetrics.height());
		else tempRect = fontMetrics.boundingRect(line);

		textSize.setWidth(qMax(textSize.width(), tempRect.width()));
		textSize.setHeight(textSize.height() + tempRect.height());
	}

	return textSize;
}

//==================================================================================================

qreal unitsScale(DrawingUnits units, DrawingUnits newUnits)
{
	qreal result = 1.0;

	switch (units)
	{
	case UnitsSimpleMM:
		switch (newUnits)
		{
		case UnitsSimpleMM:     break;
		case UnitsMM:           result = 25.4 / 25.0; break;
		default:                result = 1000.0 / 25.0; break;
		};
		break;

	case UnitsMM:
		switch (newUnits)
		{
		case UnitsSimpleMM:     result = 25.0 / 25.4; break;
		case UnitsMM:           break;
		default:                result = 1000.0 / 25.4; break;
		};
		break;

	default:
		switch (newUnits)
		{
		case UnitsSimpleMM:     result = 25.0 / 1000.0; break;
		case UnitsMM:           result = 25.4 / 1000.0; break;
		default:                break;
		};
		break;
	}

	return result;
}

//==================================================================================================

QString colorToString(const QColor& color)
{
	QString value = QString::number(color.rgba(), 16);
	value = "#" + QString(8 - value.size(), '0') + value;
	return value;
}

QColor colorFromString(const QString& string, bool* ok)
{
	QColor color(0, 0, 0);
	bool conversionOk = true;

	if (string.startsWith("#"))
	{
		bool rgbaOk = true;
		QRgb rgba = string.right(string.size() - 1).toUInt(&rgbaOk, 16);

		if (rgbaOk) color.setRgba(rgba);
		conversionOk = (conversionOk && rgbaOk);
	}
	else conversionOk = false;

	if (ok) *ok = conversionOk;
	return color;
}

//==================================================================================================

QString penToString(const QPen& pen)
{
	return (Drawing::brushToString(pen.brush()) + "," + QString::number(pen.widthF()) + "," +
		QString::number((quint32)pen.style()) + "," +
		QString::number((quint32)pen.capStyle()) + "," + QString::number((quint32)pen.joinStyle()));
}

QString brushToString(const QBrush& brush)
{
	QString value = QString::number(brush.color().rgba(), 16);
	value = "#" + QString(8 - value.size(), '0') + value;
	return value;
}

QString fontToString(const QFont& font)
{
	return font.family() + "," + QString::number(font.pointSizeF()) + "," +
		((font.bold()) ? "true" : "false") + ","  +
		((font.italic()) ? "true" : "false") + ","  +
		((font.underline()) ? "true" : "false") + ","  +
		((font.overline()) ? "true" : "false") + ","  +
		((font.strikeOut()) ? "true" : "false") + ","  +
		QString::number((quint32)font.capitalization());
}

QString rectToString(const QRectF& rect)
{
	return QString::number(rect.left()) + " " +
		QString::number(rect.top()) + " " +
		QString::number(rect.width()) + " " +
		QString::number(rect.height());
}

QString pathToString(const QPainterPath& path)
{
	QString value;
	QList<QPointF> curvePoints;
	QPainterPath::Element element;

	for(int i = 0; i < path.elementCount(); i++)
	{
		element = path.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			value += "M " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;

		case QPainterPath::LineToElement:
			value += "L " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;

		case QPainterPath::CurveToElement:
			curvePoints.append(QPointF(element.x, element.y));
			break;

		case QPainterPath::CurveToDataElement:
			if (i + 1 >= path.elementCount() || path.elementAt(i+1).type != QPainterPath::CurveToDataElement)
			{
				if (curvePoints.size() == 1)
				{
					value += "S " + QString::number(curvePoints[0].x()) + " " + QString::number(curvePoints[0].y()) + " ";
					value += QString::number(element.x) + " " + QString::number(element.y) + " ";
				}
				else if (curvePoints.size() == 2)
				{
					value += "C " + QString::number(curvePoints[0].x()) + " " + QString::number(curvePoints[0].y()) + " ";
					value += QString::number(curvePoints[1].x()) + " " + QString::number(curvePoints[1].y()) + " ";
					value += QString::number(element.x) + " " + QString::number(element.y) + " ";
				}

				curvePoints.clear();
			}
			else curvePoints.append(QPointF(element.x, element.y));
			break;

		default:
			break;
		}
	}

	return value.trimmed();
}

QPen penFromString(const QString& string, bool* ok)
{
	QPen pen;
	bool conversionOk = true;
	QStringList data = string.split(",", QString::SkipEmptyParts);

	if (data.isEmpty()) conversionOk = false;
	else
	{
		if (data.size() >= 1)
		{
			bool brushOk = true;
			QBrush brush = Drawing::brushFromString(data[0], &brushOk);

			if (brushOk) pen.setBrush(brush);

			conversionOk = (conversionOk && brushOk);
		}
		if (data.size() >= 2)
		{
			bool widthOk = true;
			qreal widthValue = data[1].toDouble(&widthOk);

			if (widthOk) pen.setWidthF(widthValue);

			conversionOk = (conversionOk && widthOk);
		}
		if (data.size() >= 3)
		{
			bool styleOk = true;
			Qt::PenStyle styleValue = (Qt::PenStyle)data[2].toUInt(&styleOk);

			if (styleOk) pen.setStyle(styleValue);

			conversionOk = (conversionOk && styleOk);
		}
		if (data.size() >= 4)
		{
			bool capStyleOk = true;
			Qt::PenCapStyle capStyleValue = (Qt::PenCapStyle)data[3].toUInt(&capStyleOk);

			if (capStyleOk) pen.setCapStyle(capStyleValue);

			conversionOk = (conversionOk && capStyleOk);
		}
		if (data.size() >= 5)
		{
			bool joinStyleOk = true;
			Qt::PenJoinStyle joinStyleValue = (Qt::PenJoinStyle)data[4].toUInt(&joinStyleOk);

			if (joinStyleOk) pen.setJoinStyle(joinStyleValue);

			conversionOk = (conversionOk && joinStyleOk);
		}
	}

	if (ok) *ok = conversionOk;
	return pen;
}

QBrush brushFromString(const QString& string, bool* ok)
{
	QBrush brush = Qt::black;
	bool conversionOk = true;

	if (string.startsWith("#"))
	{
		bool rgbaOk = true;
		QRgb rgba = string.right(string.size() - 1).toUInt(&rgbaOk, 16);

		if (rgbaOk)
		{
			QColor color;
			color.setRgba(rgba);
			brush.setColor(color);
		}

		conversionOk = (conversionOk && rgbaOk);
	}
	else conversionOk = false;

	if (ok) *ok = conversionOk;
	return brush;
}

QFont fontFromString(const QString& string, bool* ok)
{
	QFont font;
	bool conversionOk = true;
	QStringList data = string.split(",", QString::SkipEmptyParts);

	if (data.isEmpty()) conversionOk = false;
	else
	{
		if (data.size() >= 1) font.setFamily(data[0]);
		if (data.size() >= 2)
		{
			bool sizeOk = true;
			qreal sizeValue = data[1].toDouble(&sizeOk);

			if (sizeOk) font.setPointSizeF(sizeValue);

			conversionOk = (conversionOk && sizeOk);
		}
		if (data.size() >= 3) font.setBold(data[2].toLower() == "true");
		if (data.size() >= 4) font.setItalic(data[3].toLower() == "true");
		if (data.size() >= 5) font.setUnderline(data[4].toLower() == "true");
		if (data.size() >= 6) font.setOverline(data[5].toLower() == "true");
		if (data.size() >= 7) font.setStrikeOut(data[6].toLower() == "true");
		if (data.size() >= 8)
		{
			bool capitalizationOk = true;
			QFont::Capitalization capitialization = (QFont::Capitalization)data[7].toUInt(&capitalizationOk);

			if (capitalizationOk) font.setCapitalization(capitialization);

			conversionOk = (conversionOk && capitalizationOk);
		}
	}

	if (ok) *ok = conversionOk;
	return font;
}

QRectF rectFromString(const QString& string, bool* ok)
{
	QRectF rect;
	bool conversionOk = false;

	QStringList data = string.split(" ", QString::SkipEmptyParts);
	if (data.size() >= 4)
	{
		bool leftOk, topOk, widthOk, heightOk;
		rect.setLeft(data[0].toDouble(&leftOk));
		rect.setTop(data[1].toDouble(&topOk));
		rect.setWidth(data[2].toDouble(&widthOk));
		rect.setHeight(data[3].toDouble(&heightOk));
		conversionOk = (leftOk && topOk && widthOk && heightOk);
	}

	if (ok) *ok = conversionOk;
	return rect;
}

QPainterPath pathFromString(const QString& string, bool* ok)
{
	QPainterPath path;
	bool conversionOk = true;

	QStringList data = string.split(" ", QString::SkipEmptyParts);
	qreal positionX, positionY;
	qreal curvePosition0X, curvePosition0Y;
	qreal curvePosition1X, curvePosition1Y;

	if (data.isEmpty()) conversionOk = false;
	else
	{
		int index = 0;
		while (conversionOk && index < data.size())
		{
			if (data[index] == "M" && index+2 < data.size())
			{
				positionX = data[index+1].toDouble(&conversionOk);
				if (conversionOk) positionY = data[index+2].toDouble(&conversionOk);

				if (conversionOk) path.moveTo(positionX, positionY);
				index += 3;
			}
			else if (data[index] == "L" && index+2 < data.size())
			{
				positionX = data[index+1].toDouble(&conversionOk);
				if (conversionOk) positionY = data[index+2].toDouble(&conversionOk);

				if (conversionOk) path.lineTo(positionX, positionY);
				index += 3;
			}
			else if (data[index] == "C" && index+6 < data.size())
			{
				curvePosition0X = data[index+1].toDouble(&conversionOk);
				if (conversionOk) curvePosition0Y = data[index+2].toDouble(&conversionOk);
				if (conversionOk) curvePosition1X = data[index+3].toDouble(&conversionOk);
				if (conversionOk) curvePosition1Y = data[index+4].toDouble(&conversionOk);
				if (conversionOk) positionX = data[index+5].toDouble(&conversionOk);
				if (conversionOk) positionY = data[index+6].toDouble(&conversionOk);

				if (conversionOk) path.cubicTo(curvePosition0X, curvePosition0Y,
					curvePosition1X, curvePosition1Y, positionX, positionY);
				index += 7;
			}
			else if (data[index] == "S" && index+4 < data.size())
			{
				curvePosition0X = data[index+1].toDouble(&conversionOk);
				if (conversionOk) curvePosition0Y = data[index+2].toDouble(&conversionOk);
				if (conversionOk) positionX = data[index+3].toDouble(&conversionOk);
				if (conversionOk) positionY = data[index+4].toDouble(&conversionOk);

				if (conversionOk) path.quadTo(curvePosition0X, curvePosition0Y, positionX, positionY);
				index += 5;
			}
			else conversionOk = false;
		}
	}

	if (ok) *ok = conversionOk;
	return path;
}

//==================================================================================================

QString pointToString(const QPointF& point, DrawingUnits units)
{
	QString result;

	switch (units)
	{
	case UnitsSimpleMM:
	case UnitsMM:
		result = "(" + QString::number(point.x(), 'f', 1) + "," + QString::number(point.y(), 'f', 1) + ")";
		break;
	default:
		result = "(" + QString::number(point.x(), 'f', 0) + "," + QString::number(point.y(), 'f', 0) + ")";
		break;
	}

	return result;
}

}
