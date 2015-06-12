/* EnumComboBox.h
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

#ifndef ENUMCOMBOBOX_H
#define ENUMCOMBOBOX_H

#include <Drawing>

class EnumComboBox : public QComboBox
{
	Q_OBJECT

private:
	QMenu mMenu;

public:
	EnumComboBox();
	virtual ~EnumComboBox();

	void setValue(int value);
	int value() const;

	void addValue(int value, const QString& text, const QIcon& icon = QIcon());
	void clearValues();

signals:
	void valueChanged(int value);

private slots:
	void indexChange(int index);
};

//==================================================================================================

class UnitsComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	UnitsComboBox();
	~UnitsComboBox();

	void setUnits(DrawingUnits units);
	DrawingUnits units() const;

signals:
	void unitsChanged(DrawingUnits units);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class RenderModeComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	RenderModeComboBox();
	~RenderModeComboBox();

	void setMode(DrawingStyleOptions::ColorMode mode);
	DrawingStyleOptions::ColorMode mode() const;

signals:
	void modeChanged(DrawingStyleOptions::ColorMode mode);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class GridStyleComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	GridStyleComboBox();
	~GridStyleComboBox();

	void setStyle(DrawingStyleOptions::GridStyle style);
	DrawingStyleOptions::GridStyle style() const;

signals:
	void styleChanged(DrawingStyleOptions::GridStyle value);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class ArrowStyleComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	ArrowStyleComboBox(bool arrowsRight = false);
	~ArrowStyleComboBox();

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

class PenStyleComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	PenStyleComboBox();
	~PenStyleComboBox();

	void setStyle(Qt::PenStyle style);
	Qt::PenStyle style() const;

signals:
	void styleChanged(Qt::PenStyle value);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class AlignmentHorizontalComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	AlignmentHorizontalComboBox();
	~AlignmentHorizontalComboBox();

	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const;

signals:
	void alignmentChanged(Qt::Alignment alignment);

private slots:
	void valueChange(int value);
};

//==================================================================================================

class AlignmentVerticalComboBox : public EnumComboBox
{
	Q_OBJECT

public:
	AlignmentVerticalComboBox();
	~AlignmentVerticalComboBox();

	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const;

signals:
	void alignmentChanged(Qt::Alignment alignment);

private slots:
	void valueChange(int value);
};

#endif
