/* EnumComboBox.cpp
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

#include "EnumComboBox.h"

EnumComboBox::EnumComboBox() : QComboBox()
{
	connect(this, SIGNAL(activated(int)), this, SLOT(indexChange(int)));
}

EnumComboBox::~EnumComboBox() { }

//==================================================================================================

void EnumComboBox::setValue(int value)
{
	for(int i = 0; i < count(); i++)
	{
		if (itemData(i).toInt() == value)
			setCurrentIndex(i);
	}
}

int EnumComboBox::value() const
{
	return currentData().toInt();
}

//==================================================================================================

void EnumComboBox::addValue(int value, const QString& text, const QIcon& icon)
{
	addItem(icon, text, QVariant(value));
}

void EnumComboBox::clearValues()
{
	clear();
}

//==================================================================================================

void EnumComboBox::indexChange(int index)
{
	emit valueChanged(itemData(index).toInt());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

UnitsComboBox::UnitsComboBox()
{
	addValue(UnitsMils, "Mils");
	addValue(UnitsSimpleMM, "Millimeters");
	setUnits(UnitsMils);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

UnitsComboBox::~UnitsComboBox() { }

void UnitsComboBox::setUnits(DrawingUnits units)
{
	setValue((int)units);
}

DrawingUnits UnitsComboBox::units() const
{
	return (DrawingUnits)value();
}

void UnitsComboBox::valueChange(int value)
{
	emit unitsChanged((DrawingUnits)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

RenderModeComboBox::RenderModeComboBox()
{
	addValue(DrawingStyleOptions::Normal, "Normal");
	addValue(DrawingStyleOptions::BlackOnWhite, "Black on White");
	addValue(DrawingStyleOptions::WhiteOnBlack, "White on Black");
	setMode(DrawingStyleOptions::Normal);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

RenderModeComboBox::~RenderModeComboBox() { }

void RenderModeComboBox::setMode(DrawingStyleOptions::ColorMode mode)
{
	setValue((int)mode);
}

DrawingStyleOptions::ColorMode RenderModeComboBox::mode() const
{
	return (DrawingStyleOptions::ColorMode)value();
}

void RenderModeComboBox::valueChange(int value)
{
	emit modeChanged((DrawingStyleOptions::ColorMode)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

GridStyleComboBox::GridStyleComboBox()
{
	addValue(DrawingStyleOptions::GridNone, "None");
	addValue(DrawingStyleOptions::GridDotted, "Dotted");
	addValue(DrawingStyleOptions::GridLined, "Lined");
	addValue(DrawingStyleOptions::GridGraphPaper, "Graph Paper");
	setStyle(DrawingStyleOptions::GridGraphPaper);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

GridStyleComboBox::~GridStyleComboBox() { }

void GridStyleComboBox::setStyle(DrawingStyleOptions::GridStyle style)
{
	setValue((int)style);
}

DrawingStyleOptions::GridStyle GridStyleComboBox::style() const
{
	return (DrawingStyleOptions::GridStyle)value();
}

void GridStyleComboBox::valueChange(int value)
{
	emit styleChanged((DrawingStyleOptions::GridStyle)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ArrowStyleComboBox::ArrowStyleComboBox(bool arrowsRight) : EnumComboBox()
{
	addValue(DrawingArrow::None, "None", generateIcon(":/icons/arrow/arrow_none.png", arrowsRight));
	addValue(DrawingArrow::Normal, "Normal", generateIcon(":/icons/arrow/arrow_normal.png", arrowsRight));
	addValue(DrawingArrow::Reverse, "Reverse", generateIcon(":/icons/arrow/arrow_reverse.png", arrowsRight));
	addValue(DrawingArrow::Triangle, "Triangle", generateIcon(":/icons/arrow/arrow_triangle.png", arrowsRight));
	addValue(DrawingArrow::TriangleFilled, "Triangle Filled", generateIcon(":/icons/arrow/arrow_triangle_filled.png", arrowsRight));
	addValue(DrawingArrow::Concave, "Concave", generateIcon(":/icons/arrow/arrow_concave.png", arrowsRight));
	addValue(DrawingArrow::ConcaveFilled, "Concave Filled", generateIcon(":/icons/arrow/arrow_concave_filled.png", arrowsRight));
	addValue(DrawingArrow::Circle, "Circle", generateIcon(":/icons/arrow/arrow_circle.png", arrowsRight));
	addValue(DrawingArrow::CircleFilled, "Circle Filled", generateIcon(":/icons/arrow/arrow_circle_filled.png", arrowsRight));
	addValue(DrawingArrow::Diamond, "Diamond", generateIcon(":/icons/arrow/arrow_diamond.png", arrowsRight));
	addValue(DrawingArrow::DiamondFilled, "Diamond Filled", generateIcon(":/icons/arrow/arrow_diamond_filled.png", arrowsRight));
	addValue(DrawingArrow::Harpoon, "Harpoon", generateIcon(":/icons/arrow/arrow_harpoon.png", arrowsRight));
	addValue(DrawingArrow::HarpoonMirrored, "Harpoon Mirrored", generateIcon(":/icons/arrow/arrow_harpoon_mirrored.png", arrowsRight));
	addValue(DrawingArrow::XArrow, "X", generateIcon(":/icons/arrow/arrow_x.png", arrowsRight));
	setStyle(DrawingArrow::None);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

ArrowStyleComboBox::~ArrowStyleComboBox() { }

void ArrowStyleComboBox::setStyle(DrawingArrow::Style style)
{
	setValue((int)style);
}

DrawingArrow::Style ArrowStyleComboBox::style() const
{
	return (DrawingArrow::Style)value();
}

void ArrowStyleComboBox::valueChange(int value)
{
	emit styleChanged((DrawingArrow::Style)value);
}

QIcon ArrowStyleComboBox::generateIcon(const QString& path, bool arrowsRight)
{
	QIcon icon;

	QPixmap pixmap(path);

	if (arrowsRight)
		icon = QIcon(pixmap);
	else
		icon = QIcon(QPixmap::fromImage(pixmap.toImage().mirrored(true, false)));

	return icon;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PenStyleComboBox::PenStyleComboBox() : EnumComboBox()
{
	addValue(Qt::NoPen, "No Pen", QIcon(":/icons/penstyle/pen_none.png"));
	addValue(Qt::SolidLine, "Solid Line", QIcon(":/icons/penstyle/pen_solid.png"));
	addValue(Qt::DashLine, "Dashed Line", QIcon(":/icons/penstyle/pen_dashed.png"));
	addValue(Qt::DotLine, "Dotted Line", QIcon(":/icons/penstyle/pen_dotted.png"));
	addValue(Qt::DashDotLine, "Dash-Dotted Line", QIcon(":/icons/penstyle/pen_dash_dotted.png"));
	addValue(Qt::DashDotDotLine, "Dash-Dot-Dotted Line", QIcon(":/icons/penstyle/pen_dash_dot_dotted.png"));
	setStyle(Qt::SolidLine);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

PenStyleComboBox::~PenStyleComboBox() { }

void PenStyleComboBox::setStyle(Qt::PenStyle style)
{
	setValue((int)style);
}

Qt::PenStyle PenStyleComboBox::style() const
{
	return (Qt::PenStyle)value();
}

void PenStyleComboBox::valueChange(int value)
{
	emit styleChanged((Qt::PenStyle)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

AlignmentHorizontalComboBox::AlignmentHorizontalComboBox() : EnumComboBox()
{
	addValue(Qt::AlignLeft, "Left", QIcon(":/icons/oxygen/align-horizontal-left.png"));
	addValue(Qt::AlignHCenter, "Center", QIcon(":/icons/oxygen/align-horizontal-center.png"));
	addValue(Qt::AlignRight, "Right", QIcon(":/icons/oxygen/align-horizontal-right.png"));
	setAlignment(Qt::AlignHCenter);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

AlignmentHorizontalComboBox::~AlignmentHorizontalComboBox() { }

void AlignmentHorizontalComboBox::setAlignment(Qt::Alignment alignment)
{
	setValue((int)alignment);
}

Qt::Alignment AlignmentHorizontalComboBox::alignment() const
{
	return (Qt::Alignment)value();
}

void AlignmentHorizontalComboBox::valueChange(int value)
{
	emit alignmentChanged((Qt::Alignment)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

AlignmentVerticalComboBox::AlignmentVerticalComboBox() : EnumComboBox()
{
	addValue(Qt::AlignTop, "Top", QIcon(":/icons/oxygen/align-vertical-top.png"));
	addValue(Qt::AlignVCenter, "Center", QIcon(":/icons/oxygen/align-vertical-center.png"));
	addValue(Qt::AlignBottom, "Bottom", QIcon(":/icons/oxygen/align-vertical-bottom.png"));
	setAlignment(Qt::AlignVCenter);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

AlignmentVerticalComboBox::~AlignmentVerticalComboBox() { }

void AlignmentVerticalComboBox::setAlignment(Qt::Alignment alignment)
{
	setValue((int)alignment);
}

Qt::Alignment AlignmentVerticalComboBox::alignment() const
{
	return (Qt::Alignment)value();
}

void AlignmentVerticalComboBox::valueChange(int value)
{
	emit alignmentChanged((Qt::Alignment)value);
}
