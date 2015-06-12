/* UnitsValueEdit.cpp
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

#include "UnitsValueEdit.h"

UnitsValueEdit::UnitsValueEdit(DrawingUnits units, Range range, bool expand) : QLineEdit()
{
	setRange(range);
	setValue(0.0, units);
	setExpanding(expand);
	setValidator(new UnitsValueValidator(this));
	connect(this, SIGNAL(editingFinished()), this, SLOT(updateForEditingFinished()));
}

UnitsValueEdit::~UnitsValueEdit() { }

//==================================================================================================

void UnitsValueEdit::setValue(qreal value, DrawingUnits units)
{
	mUnits = units;
	setValue(value);
}

void UnitsValueEdit::setValue(qreal value)
{
	setText(valueToText(value, mUnits));
}

void UnitsValueEdit::setUnits(DrawingUnits units)
{
	qreal scaleFactor = Drawing::unitsScale(mUnits, units);

	mUnits = units;
	setValue(value() * scaleFactor);
}

qreal UnitsValueEdit::value(DrawingUnits valueUnits) const
{
	qreal scaleFactor = Drawing::unitsScale(valueUnits, mUnits);
	return value() * scaleFactor;
}

qreal UnitsValueEdit::value() const
{
	qreal value	= 0;
	DrawingUnits units = UnitsMils;
	return textToValue(text(), value, units) ? value : 0.0;
}

DrawingUnits UnitsValueEdit::units() const
{
	return mUnits;
}

//==================================================================================================

void UnitsValueEdit::setRange(Range range)
{
	mRange = range;
}

UnitsValueEdit::Range UnitsValueEdit::range() const
{
	return mRange;
}

//==================================================================================================

void UnitsValueEdit::setExpanding(bool expand)
{
	setSizePolicy((expand) ? QSizePolicy::MinimumExpanding : QSizePolicy::Minimum, sizePolicy().verticalPolicy());
}

QSize UnitsValueEdit::sizeHint() const
{
	QSize sizeHint = QLineEdit::sizeHint();

	QFontMetrics fontMetrics(font());
	sizeHint.setWidth(fontMetrics.width("XXXX.XX mils") + 8);

	return sizeHint;
}

//==================================================================================================

bool UnitsValueEdit::textToValue(const QString& text, qreal& value, DrawingUnits& units) const
{
	bool ok = false;
	QString trimText = text.trimmed();

	if (trimText.endsWith("mm", Qt::CaseInsensitive))
	{
		units = UnitsSimpleMM;
		value = trimText.left(trimText.size() - 2).toDouble(&ok);
	}
	else if (trimText.endsWith("mils", Qt::CaseInsensitive))
	{
		units = UnitsMils;
		value = trimText.left(trimText.size() - 4).toDouble(&ok);
	}
	else
	{
		units = mUnits;
		value = trimText.toDouble(&ok);
	}

	return ok;
}

QString UnitsValueEdit::valueToText(qreal value, DrawingUnits units) const
{
	QString text = QString::number(value) + " ";

	if (units == UnitsMils) text += "mils";
	else text += "mm";

	return text;
}

//==================================================================================================

void UnitsValueEdit::updateForEditingFinished()
{
	qreal newValue = 0.0;
	DrawingUnits newUnits = mUnits;

	if (textToValue(text(), newValue, newUnits))
	{
		if (newUnits == mUnits) setValue(newValue);
		else
		{
			qreal scaleFactor = Drawing::unitsScale(newUnits, mUnits);
			setValue(newValue * scaleFactor);
		}
	}
}

//==================================================================================================

QValidator::State UnitsValueEdit::validate(QString& input, int& pos) const
{
	QValidator::State state = QValidator::Intermediate;

	QString trimInput = input.trimmed();
	qreal newValue = 0.0;
	DrawingUnits newUnits = mUnits;

	if (textToValue(input, newValue, newUnits))
	{
		switch (mRange)
		{
		case PositivesOnly:
			state = (newValue > 0) ? QValidator::Acceptable : QValidator::Invalid;
			break;
		case NonNegativesOnly:
			state = (newValue >= 0) ? QValidator::Acceptable : QValidator::Invalid;
			break;
		default:
			state = QValidator::Acceptable;
			break;
		}
	}
	else if (trimInput.isEmpty() || (mRange == AllValuesAllowed && trimInput == "-") || trimInput == "+")
		state = QValidator::Intermediate;
	else if (trimInput.startsWith('-') && mRange != AllValuesAllowed)
		state = QValidator::Invalid;

	Q_UNUSED(pos);
	return state;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

UnitsValueValidator::UnitsValueValidator(UnitsValueEdit* edit) : QValidator(edit)
{
	mEdit = edit;
}

QValidator::State UnitsValueValidator::validate(QString& input, int& pos) const
{
	return mEdit->validate(input, pos);
}
