/* UnitsValueEdit.h
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

#ifndef UNITSVALUEEDIT_H
#define UNITSVALUEEDIT_H

#include <Drawing>

class UnitsValueEdit : public QLineEdit
{
	Q_OBJECT

	friend class UnitsValueValidator;

public:
	enum Range { AllValuesAllowed, PositivesOnly, NonNegativesOnly };

private:
	DrawingUnits mUnits;
	Range mRange;

public:
	UnitsValueEdit(DrawingUnits units = UnitsMils, Range range = AllValuesAllowed, bool expand = false);
	~UnitsValueEdit();

	void setValue(qreal value, DrawingUnits units);
	void setValue(qreal value);
	qreal value(DrawingUnits valueUnits) const;
	qreal value() const;
	DrawingUnits units() const;

	void setRange(Range range);
	Range range() const;

	void setExpanding(bool expand);
	QSize sizeHint() const;

	bool textToValue(const QString& text, qreal& value, DrawingUnits& units) const;
	QString valueToText(qreal value, DrawingUnits units) const;

public slots:
	void setUnits(DrawingUnits units);

signals:
	void valueChanged(qreal value, DrawingUnits units);

private slots:
	void updateForEditingFinished();

private:
	QValidator::State validate(QString& input, int& pos) const;
}
;

//==================================================================================================

class UnitsValueValidator : public QValidator
{
private:
	UnitsValueEdit* mEdit;

public:
	UnitsValueValidator(UnitsValueEdit* edit);
	QValidator::State validate(QString& input, int& pos) const;
};

#endif
