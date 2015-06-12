/* DiagramMultipleItemPropertiesWidget.h
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

#ifndef DIAGRAMMULTIPLEITEMPROPERTIESWIDGET_H
#define DIAGRAMMULTIPLEITEMPROPERTIESWIDGET_H

#include <Drawing>

class AlignmentHorizontalComboBox;
class AlignmentVerticalComboBox;
class ArrowStyleComboBox;
class ColorPushButton;
class PenStyleComboBox;
class PositionWidget;
class UnitsValueEdit;

class DiagramMultipleItemPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QList<DrawingItem*> mItems;

	PositionWidget* mRectCornerRadiusWidget;

	PenStyleComboBox* mPenStyleCombo;
	UnitsValueEdit* mPenWidthEdit;
	ColorPushButton* mPenColorButton;
	ColorPushButton* mBrushColorButton;

	QFontComboBox* mFontFamilyCombo;
	UnitsValueEdit* mFontSizeEdit;
	QWidget* mFontStyleWidget;
	QToolButton* mFontBoldButton;
	QToolButton* mFontItalicButton;
	QToolButton* mFontUnderlineButton;
	QToolButton* mFontOverlineButton;
	QToolButton* mFontStrikeOutButton;
	AlignmentHorizontalComboBox* mTextAlignmentHorizontalCombo;
	AlignmentVerticalComboBox* mTextAlignmentVerticalCombo;
	ColorPushButton* mTextColorButton;

	ArrowStyleComboBox* mStartArrowStyleCombo;
	UnitsValueEdit* mStartArrowSizeEdit;

	ArrowStyleComboBox* mEndArrowStyleCombo;
	UnitsValueEdit* mEndArrowSizeEdit;

public:
	DiagramMultipleItemPropertiesWidget(const QList<DrawingItem*>& items);
	~DiagramMultipleItemPropertiesWidget();

	QHash<QString,QVariant> properties() const;
	bool shouldExec() const;

private:
	QGroupBox* createRectGroup();
	QGroupBox* createPenBrushGroup();
	QGroupBox* createTextGroup();
	QGroupBox* createStartPointGroup();
	QGroupBox* createEndPointGroup();

	QWidget* createFontStyleWidget(bool& propertiesMatch);
	QWidget* createTextAlignmentWidget(bool& propertiesMatch);

	bool allItemsContainAtLeastOneProperty(const QStringList& propertyNames);
	bool allItemsContainProperty(const QString& propertyName, bool& propertiesMatch, QVariant& propertyValue);
	void addWidget(QFormLayout* layout, const QString& text, QWidget* widget, bool checked);

	int labelWidth() const;
};

//==================================================================================================

class DiagramMultipleItemPropertiesDialog : public QDialog
{
	Q_OBJECT

private:
	DiagramMultipleItemPropertiesWidget* mPropertiesWidget;

public:
	DiagramMultipleItemPropertiesDialog(const QList<DrawingItem*>& items, QWidget* parent = nullptr);
	~DiagramMultipleItemPropertiesDialog();

	QHash<QString,QVariant> properties() const;
	bool shouldExec() const;

private:
	QDialogButtonBox* createButtonBox();
};

#endif
