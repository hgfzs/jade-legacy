/* DiagramPropertiesWidget.h
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

#ifndef DIAGRAMPROPERTIESWIDGET_H
#define DIAGRAMPROPERTIESWIDGET_H

#include "DiagramProperties.h"
#include <QtPrintSupport>

class ColorPushButton;
class GridStyleComboBox;
class UnitsComboBox;
class UnitsValueEdit;

struct PageSizeInfo
{
	QPrinter::PaperSize paperSize;
	QString name;
	qreal portraitWidth;
	qreal portraitHeight;
	QPrinter::Unit units;

	PageSizeInfo(QPrinter::PaperSize ps, const QString& nm, qreal w, qreal h, QPrinter::Unit u)
	{ paperSize = ps; name = nm; portraitWidth = w; portraitHeight = h; units = u; }
};

class DiagramPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	UnitsComboBox* mUnitsCombo;
	QLineEdit* mWidthLabel;
	QLineEdit* mHeightLabel;
	ColorPushButton* mBackgroundColorButton;

	UnitsValueEdit* mGridEdit;
	GridStyleComboBox* mGridStyleCombo;
	QSpinBox* mGridSpacingMajorSpin;
	QSpinBox* mGridSpacingMinorSpin;
	ColorPushButton* mGridColorButton;

	UnitsValueEdit* mBorderWidthEdit;
	ColorPushButton* mBorderColorButton;

	QComboBox* mPageSizeCombo;
	UnitsValueEdit* mWidthEdit;
	UnitsValueEdit* mHeightEdit;
	QComboBox* mOrientationCombo;

	UnitsValueEdit* mMarginLeftEdit;
	UnitsValueEdit* mMarginTopEdit;
	UnitsValueEdit* mMarginRightEdit;
	UnitsValueEdit* mMarginBottomEdit;

	QList<PageSizeInfo> mPageSizeInfo;

public:
	DiagramPropertiesWidget();
	~DiagramPropertiesWidget();

	void setProperties(const DiagramProperties& properties);
	DiagramProperties properties() const;

private slots:
	void updateForUnitsChange(DrawingUnits units);
	void updateForGridStyleChange(DrawingStyleOptions::GridStyle style);
	void updateForPageSizeOrOrientationChange(int index);
	void updateForWidthHeightOrMarginChange();

	void updateWidthHeightEdit();

private:
	QGroupBox* createDiagramGroup();
	QGroupBox* createGridGroup();
	QGroupBox* createBorderGroup();

	QGroupBox* createPaperFormatGroup();
	QGroupBox* createMarginsGroup();

	int labelWidth() const;

	void addSupportedPageSizes(bool showDimensions = true);
	void setPageSize(const QSizeF& size, DrawingUnits units);
	QSizeF pageSize(DrawingUnits units, bool portrait = true);
};

//==================================================================================================

class DiagramPropertiesDialog : public QDialog
{
	Q_OBJECT

private:
	DiagramPropertiesWidget* mPropertiesWidget;

public:
	DiagramPropertiesDialog(QWidget* parent = nullptr);
	~DiagramPropertiesDialog();

	void setProperties(const DiagramProperties& properties);
	DiagramProperties properties() const;

private:
	QDialogButtonBox* createButtonBox();
};

#endif
