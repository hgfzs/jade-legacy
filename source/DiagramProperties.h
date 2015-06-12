/* DiagramProperties.h
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

#ifndef DIAGRAMPROPERTIES_H
#define DIAGRAMPROPERTIES_H

#include <Drawing>

class DiagramProperties
{
private:
	DrawingUnits mUnits;
	QRectF mSceneRect;
	QRectF mContentsRect;

	QColor mBackgroundColor;

	qreal mGridSize;
	DrawingStyleOptions::GridStyle mGridStyle;
	int mGridMajorSpacing;
	int mGridMinorSpacing;
	QColor mGridColor;

	qreal mBorderWidth;
	QColor mBorderColor;

public:
	DiagramProperties(DrawingUnits units = UnitsMils);
	DiagramProperties(const DiagramProperties& other);
	~DiagramProperties();

	DiagramProperties& operator=(const DiagramProperties& other);

	void setUnits(DrawingUnits units);
	void setSceneRect(const QRectF& rect);
	void setContentsRect(const QRectF& rect);
	DrawingUnits units() const;
	QRectF sceneRect() const;
	QRectF contentsRect() const;

	void setBackgroundColor(const QColor& color);
	QColor backgroundColor() const;

	void setGridSize(qreal size);
	void setGridStyle(DrawingStyleOptions::GridStyle style, int majorSpacing, int minorSpacing);
	void setGridColor(const QColor& color);
	qreal gridSize() const;
	DrawingStyleOptions::GridStyle gridStyle() const;
	int gridMajorSpacing() const;
	int gridMinorSpacing() const;
	QColor gridColor() const;

	void setBorderWidth(qreal width);
	void setBorderColor(const QColor& color);
	qreal borderWidth() const;
	QColor borderColor() const;

	void save(QSettings& settings);
	void load(QSettings& settings);
};

//==================================================================================================

class DiagramView;

class DiagramPropertiesCommand : public DrawingUndoCommand
{
private:
	DiagramView* mView;
	DiagramProperties mProperties, mOriginalProperties;

public:
	DiagramPropertiesCommand(DiagramView* view, const DiagramProperties& newProperties, QUndoCommand* parent = nullptr);
	~DiagramPropertiesCommand();

	void redo();
	void undo();
};

//==================================================================================================

class DiagramItemPropertiesCommand : public DrawingUndoCommand
{
private:
	DrawingItem* mItem;
	QHash<QString,QVariant> mOriginalProperties;
	QHash<QString,QVariant> mNewProperties;

public:
	DiagramItemPropertiesCommand(DrawingItem* item, const QHash<QString,
		QVariant>& newProperties, QUndoCommand* parent = nullptr);
	~DiagramItemPropertiesCommand();

	void redo();
	void undo();
};

#endif
