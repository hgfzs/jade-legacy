/* DiagramSingleItemPropertiesWidget.h
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

#ifndef DIAGRAMSINGLEITEMPROPERTIESWIDGET_H
#define DIAGRAMSINGLEITEMPROPERTIESWIDGET_H

#include <Drawing>

class AlignmentHorizontalComboBox;
class AlignmentVerticalComboBox;
class ArrowStyleComboBox;
class ColorPushButton;
class ImageWidget;
class PenStyleComboBox;
class PointsListWidget;
class PositionWidget;
class UnitsValueEdit;

class DiagramSingleItemPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	DrawingItem* mItem;
	QPointF mPositionPrev;
	QPointF mStartPointPrev;
	QPointF mEndPointPrev;

	PositionWidget* mPositionWidget;

	PositionWidget* mRectTopLeftWidget;
	PositionWidget* mRectBottomRightWidget;
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
	QTextEdit* mCaptionEdit;

	PositionWidget* mStartPointPositionWidget;
	ArrowStyleComboBox* mStartArrowStyleCombo;
	UnitsValueEdit* mStartArrowSizeEdit;

	PositionWidget* mEndPointPositionWidget;
	ArrowStyleComboBox* mEndArrowStyleCombo;
	UnitsValueEdit* mEndArrowSizeEdit;

	PointsListWidget* mControlPointsWidget;
	PointsListWidget* mPolyPointsWidget;
	ImageWidget* mImageWidget;

public:
	DiagramSingleItemPropertiesWidget(DrawingItem* item);
	~DiagramSingleItemPropertiesWidget();

	QPointF position() const;
	QHash<DrawingItemPoint*, QPointF> pointPositions() const;
	QHash<QString,QVariant> properties() const;
	bool shouldExec() const;

private slots:
	void updateForPositionChange();
	void updateForStartPointPositionChange();
	void updateForEndPointPositionChange();

private:
	QGroupBox* createPositionGroup();
	QGroupBox* createRectGroup();
	QGroupBox* createPenBrushGroup();
	QGroupBox* createTextGroup();
	QGroupBox* createStartPointGroup();
	QGroupBox* createEndPointGroup();
	QGroupBox* createControlPointsGroup();
	QGroupBox* createPolyPointsGroup();
	QGroupBox* createImageGroup();

	QWidget* createFontStyleWidget();
	QWidget* createTextAlignmentWidget();
	int labelWidth() const;
};

//==================================================================================================

class DiagramSingleItemPropertiesDialog : public QDialog
{
	Q_OBJECT

private:
	DiagramSingleItemPropertiesWidget* mPropertiesWidget;

public:
	DiagramSingleItemPropertiesDialog(DrawingItem* item, QWidget* parent = nullptr);
	~DiagramSingleItemPropertiesDialog();

	QPointF position() const;
	QHash<DrawingItemPoint*, QPointF> pointPositions() const;
	QHash<QString,QVariant> properties() const;
	bool shouldExec() const;

private:
	QDialogButtonBox* createButtonBox();
};

#endif
