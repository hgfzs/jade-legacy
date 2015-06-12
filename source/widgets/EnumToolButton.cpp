/* EnumToolButton.cpp
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

#include "EnumToolButton.h"

EnumToolButton::EnumToolButton() : QToolButton()
{
	setPopupMode(MenuButtonPopup);
	setMenu(&mMenu);
	connect(&mMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuTrigger(QAction*)));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(buttonToggle()));
}

EnumToolButton::~EnumToolButton() { }

//==================================================================================================

void EnumToolButton::setValue(int value)
{
	QList<QAction*> actions = mMenu.actions();
	for(int i = 0; i < actions.size(); i++)
	{
		if (actions[i]->data().toInt() == value)
			setDefaultAction(actions[i]);
	}
}

int EnumToolButton::value() const
{
	return defaultAction()->data().toInt();
}

//==================================================================================================

void EnumToolButton::addValue(int value, const QString& text, const QIcon& icon)
{
	QAction* action = mMenu.addAction(icon, text);
	action->setData(value);
	if (mMenu.actions().size() == 1) setDefaultAction(action);
}

void EnumToolButton::clearValues()
{
	mMenu.clear();
}

//==================================================================================================

void EnumToolButton::buttonToggle()
{
	emit valueChanged(value());
}

void EnumToolButton::menuTrigger(QAction* action)
{
	setDefaultAction(action);
	emit valueChanged(action->data().toInt());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PenStyleToolButton::PenStyleToolButton() : EnumToolButton()
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

PenStyleToolButton::~PenStyleToolButton() { }

void PenStyleToolButton::setStyle(Qt::PenStyle style)
{
	setValue((int)style);
}

Qt::PenStyle PenStyleToolButton::style() const
{
	return (Qt::PenStyle)value();
}

void PenStyleToolButton::valueChange(int value)
{
	emit styleChanged((Qt::PenStyle)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ArrowStyleToolButton::ArrowStyleToolButton(bool arrowsRight) : EnumToolButton()
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

ArrowStyleToolButton::~ArrowStyleToolButton() { }

void ArrowStyleToolButton::setStyle(DrawingArrow::Style style)
{
	setValue((int)style);
}

DrawingArrow::Style ArrowStyleToolButton::style() const
{
	return (DrawingArrow::Style)value();
}

void ArrowStyleToolButton::valueChange(int value)
{
	emit styleChanged((DrawingArrow::Style)value);
}

QIcon ArrowStyleToolButton::generateIcon(const QString& path, bool arrowsRight)
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

AlignmentHorizontalToolButton::AlignmentHorizontalToolButton() : EnumToolButton()
{
	addValue(Qt::AlignLeft, "Align Left", QIcon(":/icons/oxygen/align-horizontal-left.png"));
	addValue(Qt::AlignHCenter, "Align Center", QIcon(":/icons/oxygen/align-horizontal-center.png"));
	addValue(Qt::AlignRight, "Align Right", QIcon(":/icons/oxygen/align-horizontal-right.png"));
	setAlignment(Qt::AlignHCenter);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

AlignmentHorizontalToolButton::~AlignmentHorizontalToolButton() { }

void AlignmentHorizontalToolButton::setAlignment(Qt::Alignment alignment)
{
	setValue((int)alignment);
}

Qt::Alignment AlignmentHorizontalToolButton::alignment() const
{
	return (Qt::Alignment)value();
}

void AlignmentHorizontalToolButton::valueChange(int value)
{
	emit alignmentChanged((Qt::Alignment)value);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

AlignmentVerticalToolButton::AlignmentVerticalToolButton() : EnumToolButton()
{
	addValue(Qt::AlignTop, "Align Top", QIcon(":/icons/oxygen/align-vertical-top.png"));
	addValue(Qt::AlignVCenter, "Align Center", QIcon(":/icons/oxygen/align-vertical-center.png"));
	addValue(Qt::AlignBottom, "Align Bottom", QIcon(":/icons/oxygen/align-vertical-bottom.png"));
	setAlignment(Qt::AlignVCenter);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

AlignmentVerticalToolButton::~AlignmentVerticalToolButton() { }

void AlignmentVerticalToolButton::setAlignment(Qt::Alignment alignment)
{
	setValue((int)alignment);
}

Qt::Alignment AlignmentVerticalToolButton::alignment() const
{
	return (Qt::Alignment)value();
}

void AlignmentVerticalToolButton::valueChange(int value)
{
	emit alignmentChanged((Qt::Alignment)value);
}
