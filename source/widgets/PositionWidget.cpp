/* PositionWidget.cpp
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

#include "PositionWidget.h"
#include "UnitsValueEdit.h"

PositionWidget::PositionWidget(DrawingUnits units) : QWidget()
{
	mXEdit = new UnitsValueEdit(units, UnitsValueEdit::AllValuesAllowed, true);
	mYEdit = new UnitsValueEdit(units, UnitsValueEdit::AllValuesAllowed, true);
	connect(mXEdit, SIGNAL(editingFinished()), this, SIGNAL(positionChanged()));
	connect(mYEdit, SIGNAL(editingFinished()), this, SIGNAL(positionChanged()));

	QFontMetrics fontMetrics(mXEdit->font());
	mXEdit->setMinimumWidth(fontMetrics.width("XXXX.X mils") + 8);
	mYEdit->setMinimumWidth(fontMetrics.width("XXXX.X mils") + 8);

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mXEdit);
	hLayout->addWidget(mYEdit);
	hLayout->setSpacing(2);
	hLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(hLayout);
}

PositionWidget::~PositionWidget() { }

//==================================================================================================

void PositionWidget::setPos(const QPointF& position)
{
	mXEdit->setValue(position.x());
	mYEdit->setValue(position.y());
}

QPointF PositionWidget::pos() const
{
	return QPointF(mXEdit->value(), mYEdit->value());
}
