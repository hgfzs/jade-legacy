/* PositionWidget.h
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

#ifndef POSITIONWIDGET_H
#define POSITIONWIDGET_H

#include <Drawing>

class UnitsValueEdit;

class PositionWidget : public QWidget
{
	Q_OBJECT

private:
	UnitsValueEdit* mXEdit;
	UnitsValueEdit* mYEdit;

public:
	PositionWidget(DrawingUnits units);
	~PositionWidget();

	void setPos(const QPointF& position);
	QPointF pos() const;

signals:
	void positionChanged();
};

#endif
