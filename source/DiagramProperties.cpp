/* DiagramProperties.cpp
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

#include "DiagramProperties.h"
#include "DiagramView.h"

DiagramProperties::DiagramProperties(DrawingUnits units)
{
	if (units == UnitsMils)
	{
		mUnits = UnitsMils;
		mSceneRect = QRectF(-500, -500, 11000, 8500);
		mContentsRect = QRectF(0, 0, 10000, 7500);
		mBorderWidth = 24.0;
		mGridSize = 50.0;
	}
	else
	{
		mUnits = units;
		mSceneRect = QRectF(-12.5, -12.5, 262.5, 200);
		mContentsRect = QRectF(0, 0, 250, 187.5);
		mBorderWidth = 0.6;
		mGridSize = 1.25;
	}

	mBackgroundColor = QColor(255, 255, 255);

	mGridStyle = DrawingStyleOptions::GridGraphPaper;
	mGridMajorSpacing = 8;
	mGridMinorSpacing = 2;
	mGridColor = QColor(0, 128, 128);

	mBorderColor = QColor(0, 0, 0);
}

DiagramProperties::DiagramProperties(const DiagramProperties& other)
{
	mUnits = other.mUnits;
	mSceneRect = other.mSceneRect;
	mContentsRect = other.mContentsRect;

	mBackgroundColor = other.mBackgroundColor;

	mGridSize = other.mGridSize;
	mGridStyle = other.mGridStyle;
	mGridMajorSpacing = other.mGridMajorSpacing;
	mGridMinorSpacing = other.mGridMinorSpacing;
	mGridColor = other.mGridColor;

	mBorderWidth = other.mBorderWidth;
	mBorderColor = other.mBorderColor;
}

DiagramProperties::~DiagramProperties() { }

//==================================================================================================

DiagramProperties& DiagramProperties::operator=(const DiagramProperties& other)
{
	mUnits = other.mUnits;
	mSceneRect = other.mSceneRect;
	mContentsRect = other.mContentsRect;

	mBackgroundColor = other.mBackgroundColor;

	mGridSize = other.mGridSize;
	mGridStyle = other.mGridStyle;
	mGridMajorSpacing = other.mGridMajorSpacing;
	mGridMinorSpacing = other.mGridMinorSpacing;
	mGridColor = other.mGridColor;

	mBorderWidth = other.mBorderWidth;
	mBorderColor = other.mBorderColor;

	return *this;
}

//==================================================================================================

void DiagramProperties::setUnits(DrawingUnits units)
{
	mUnits = units;
}

void DiagramProperties::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

void DiagramProperties::setContentsRect(const QRectF& rect)
{
	mContentsRect = rect;
}

DrawingUnits DiagramProperties::units() const
{
	return mUnits;
}

QRectF DiagramProperties::sceneRect() const
{
	return mSceneRect;
}

QRectF DiagramProperties::contentsRect() const
{
	return mContentsRect;
}

//==================================================================================================

void DiagramProperties::setBackgroundColor(const QColor& color)
{
	mBackgroundColor = color;
}

QColor DiagramProperties::backgroundColor() const
{
	return mBackgroundColor;
}

//==================================================================================================

void DiagramProperties::setGridSize(qreal size)
{
	mGridSize = size;
}

void DiagramProperties::setGridStyle(DrawingStyleOptions::GridStyle style, int majorSpacing, int minorSpacing)
{
	mGridStyle = style;
	mGridMajorSpacing = majorSpacing;
	mGridMinorSpacing = minorSpacing;
}

void DiagramProperties::setGridColor(const QColor& color)
{
	mGridColor = color;
}

qreal DiagramProperties::gridSize() const
{
	return mGridSize;
}

DrawingStyleOptions::GridStyle DiagramProperties::gridStyle() const
{
	return mGridStyle;
}

int DiagramProperties::gridMajorSpacing() const
{
	return mGridMajorSpacing;
}

int DiagramProperties::gridMinorSpacing() const
{
	return mGridMinorSpacing;
}

QColor DiagramProperties::gridColor() const
{
	return mGridColor;
}

//==================================================================================================

void DiagramProperties::setBorderWidth(qreal width)
{
	mBorderWidth = width;
}

void DiagramProperties::setBorderColor(const QColor& color)
{
	mBorderColor = color;
}

qreal DiagramProperties::borderWidth() const
{
	return mBorderWidth;
}

QColor DiagramProperties::borderColor() const
{
	return mBorderColor;
}

//==================================================================================================

void DiagramProperties::save(QSettings& settings)
{
	settings.setValue("units", (int)mUnits);
	settings.setValue("sceneRect", mSceneRect);
	settings.setValue("contentsRect", mContentsRect);
	settings.setValue("backgroundColor", mBackgroundColor);
	settings.setValue("borderWidth", mBorderWidth);
	settings.setValue("borderColor", mBorderColor);
	settings.setValue("grid", mGridSize);
	settings.setValue("gridStyle", (int)mGridStyle);
	settings.setValue("gridSpacingMajor", mGridMajorSpacing);
	settings.setValue("gridSpacingMinor", mGridMinorSpacing);
	settings.setValue("gridColor", mGridColor);
}

void DiagramProperties::load(QSettings& settings)
{
	mUnits = (DrawingUnits)settings.value("units", QVariant(UnitsMils)).toInt();
	mSceneRect = settings.value("sceneRect", QVariant(QRectF(-500, -500, 11000, 8500))).toRectF();
	mContentsRect = settings.value("contentsRect", QVariant(QRectF(0, 0, 10000, 7500))).toRectF();
	mBackgroundColor = settings.value("backgroundColor", QVariant(QColor(255, 255, 255, 255))).value<QColor>();
	mBorderWidth = settings.value("borderWidth", QVariant(24.0)).toDouble();
	mBorderColor = settings.value("borderColor", QVariant(QColor(0, 0, 0))).value<QColor>();
	mGridSize = settings.value("grid", QVariant(100.0)).toDouble();
	mGridStyle = (DrawingStyleOptions::GridStyle)settings.value("gridStyle", QVariant(DrawingStyleOptions::GridGraphPaper)).toInt();
	mGridMajorSpacing = settings.value("gridSpacingMajor", QVariant(8)).toInt();
	mGridMinorSpacing = settings.value("gridSpacingMinor", QVariant(2)).toInt();
	mGridColor = settings.value("gridColor", QVariant(QColor(0, 128, 128, 255))).value<QColor>();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramPropertiesCommand::DiagramPropertiesCommand(DiagramView* view, const DiagramProperties& newProperties,
	QUndoCommand* parent) : DrawingUndoCommand("Change Diagram Properties", parent)
{
	mView = view;
	mOriginalProperties = mView->diagramProperties();
	mProperties = newProperties;
}

DiagramPropertiesCommand::~DiagramPropertiesCommand() { }

void DiagramPropertiesCommand::redo()
{
	mView->setDiagramProperties(mProperties);
	QUndoCommand::redo();
}

void DiagramPropertiesCommand::undo()
{
	QUndoCommand::undo();
	mView->setDiagramProperties(mOriginalProperties);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramItemPropertiesCommand::DiagramItemPropertiesCommand(DrawingItem* item,
	const QHash<QString,QVariant>& newProperties, QUndoCommand* parent) :
	DrawingUndoCommand("Change Item Properties", parent)
{
	mItem = item;
	mOriginalProperties = item->properties();
	mNewProperties = newProperties;
}

DiagramItemPropertiesCommand::~DiagramItemPropertiesCommand() { }

void DiagramItemPropertiesCommand::redo()
{
	mItem->setProperties(mNewProperties);
	DrawingUndoCommand::redo();
}

void DiagramItemPropertiesCommand::undo()
{
	DrawingUndoCommand::undo();
	mItem->setProperties(mOriginalProperties);
}

