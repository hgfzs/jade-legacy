/* EnumToolButton.h
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

#ifndef ENUMTOOLBUTTON_H
#define ENUMTOOLBUTTON_H

#include <Drawing>

class EnumToolButton : public QToolButton
{
	Q_OBJECT

private:
	QMenu mMenu;

public:
	EnumToolButton();
	virtual ~EnumToolButton();

	void setValue(int value);
	int value() const;

	void addValue(int value, const QString& text, const QIcon& icon = QIcon());
	void clearValues();

signals:
	void valueChanged(int value);

private slots:
	void buttonToggle();
	void menuTrigger(QAction* action);
};

//==================================================================================================

class PenStyleToolButton : public EnumToolButton
{
	Q_OBJECT

public:
	PenStyleToolButton();
	~PenStyleToolButton();

	void setStyle(Qt::PenStyle style);
	Qt::PenStyle style() const;

signals:
	void styleChanged(Qt::PenStyle value);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class ArrowStyleToolButton : public EnumToolButton
{
	Q_OBJECT

public:
	ArrowStyleToolButton(bool arrowsRight = false);
	~ArrowStyleToolButton();

	void setStyle(DrawingArrow::Style style);
	DrawingArrow::Style style() const;

signals:
	void styleChanged(DrawingArrow::Style style);

private slots:
	void valueChange(int value);

private:
	QIcon generateIcon(const QString& path, bool arrowsRight);
};

//==================================================================================================

class AlignmentHorizontalToolButton : public EnumToolButton
{
	Q_OBJECT

public:
	AlignmentHorizontalToolButton();
	~AlignmentHorizontalToolButton();

	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const;

signals:
	void alignmentChanged(Qt::Alignment alignment);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class AlignmentVerticalToolButton : public EnumToolButton
{
	Q_OBJECT

public:
	AlignmentVerticalToolButton();
	~AlignmentVerticalToolButton();

	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const;

signals:
	void alignmentChanged(Qt::Alignment alignment);

private slots:
	void valueChange(int value);
};

#endif
