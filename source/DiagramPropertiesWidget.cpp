/* DiagramPropertiesWidget.cpp
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

#include "DiagramPropertiesWidget.h"
#include "ColorButton.h"
#include "EnumComboBox.h"
#include "UnitsValueEdit.h"

DiagramPropertiesWidget::DiagramPropertiesWidget() : QWidget()
{
	QWidget* diagramWidget = new QWidget();
	QVBoxLayout* diagramLayout = new QVBoxLayout();
	diagramLayout->addWidget(createDiagramGroup());
	diagramLayout->addWidget(createGridGroup());
	diagramLayout->addWidget(createBorderGroup());
	diagramLayout->addWidget(new QWidget(), 100);
	diagramWidget->setLayout(diagramLayout);

	QWidget* pageWidget = new QWidget();
	QVBoxLayout* pageLayout = new QVBoxLayout();
	pageLayout->addWidget(createPaperFormatGroup());
	pageLayout->addWidget(createMarginsGroup());
	pageLayout->addWidget(new QWidget(), 100);
	pageWidget->setLayout(pageLayout);

	QTabWidget* tabWidget = new QTabWidget();
	QVBoxLayout* vLayout = new QVBoxLayout();
	tabWidget->addTab(diagramWidget, "Diagram");
	tabWidget->addTab(pageWidget, "Page");
	vLayout->addWidget(tabWidget);
	vLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(vLayout);
}

DiagramPropertiesWidget::~DiagramPropertiesWidget() { }

//==================================================================================================

void DiagramPropertiesWidget::setProperties(const DiagramProperties& properties)
{
	mUnitsCombo->setUnits(properties.units());
	updateForUnitsChange(mUnitsCombo->units());

	mBackgroundColorButton->setColor(properties.backgroundColor());

	mGridEdit->setValue(properties.gridSize());
	mGridStyleCombo->setStyle(properties.gridStyle());
	mGridSpacingMajorSpin->setValue(properties.gridMajorSpacing());
	mGridSpacingMinorSpin->setValue(properties.gridMinorSpacing());
	mGridColorButton->setColor(properties.gridColor());
	updateForGridStyleChange(mGridStyleCombo->style());

	mBorderWidthEdit->setValue(properties.borderWidth());
	mBorderColorButton->setColor(properties.borderColor());

	mWidthEdit->setValue(properties.sceneRect().width());
	mHeightEdit->setValue(properties.sceneRect().height());
	mMarginLeftEdit->setValue(properties.contentsRect().left() - properties.sceneRect().left());
	mMarginTopEdit->setValue(properties.contentsRect().top() - properties.sceneRect().top());
	mMarginRightEdit->setValue(properties.sceneRect().right() - properties.contentsRect().right());
	mMarginBottomEdit->setValue(properties.sceneRect().bottom() - properties.contentsRect().bottom());
	updateForWidthHeightOrMarginChange();
}

DiagramProperties DiagramPropertiesWidget::properties() const
{
	DiagramProperties properties;

	properties.setUnits(mUnitsCombo->units());

	properties.setBackgroundColor(mBackgroundColorButton->color());

	properties.setGridSize(mGridEdit->value());
	properties.setGridStyle(mGridStyleCombo->style(),
		mGridSpacingMajorSpin->value(), mGridSpacingMinorSpin->value());
	properties.setGridColor(mGridColorButton->color());

	properties.setBorderWidth(mBorderWidthEdit->value());
	properties.setBorderColor(mBorderColorButton->color());

	properties.setSceneRect(QRectF(-mMarginLeftEdit->value(), -mMarginTopEdit->value(),
		mWidthEdit->value(), mHeightEdit->value()));
	properties.setContentsRect(properties.sceneRect().adjusted(mMarginLeftEdit->value(),
		mMarginTopEdit->value(), -mMarginRightEdit->value(), -mMarginBottomEdit->value()));

	return properties;
}

//==================================================================================================

void DiagramPropertiesWidget::updateForUnitsChange(DrawingUnits units)
{
	mGridEdit->setUnits(units);
	mBorderWidthEdit->setUnits(units);
	mWidthEdit->setUnits(units);
	mHeightEdit->setUnits(units);

	mMarginLeftEdit->setUnits(units);
	mMarginTopEdit->setUnits(units);
	mMarginRightEdit->setUnits(units);
	mMarginBottomEdit->setUnits(units);

	updateWidthHeightEdit();
}

void DiagramPropertiesWidget::updateForGridStyleChange(DrawingStyleOptions::GridStyle style)
{
	mGridSpacingMajorSpin->setEnabled(style != DrawingStyleOptions::GridNone);
	mGridSpacingMinorSpin->setEnabled(style == DrawingStyleOptions::GridGraphPaper);
}

void DiagramPropertiesWidget::updateForPageSizeOrOrientationChange(int index)
{
	if (mPageSizeCombo->currentIndex() != mPageSizeCombo->count() - 1)
	{
		mWidthEdit->setValue(pageSize(mUnitsCombo->units(), mOrientationCombo->currentIndex() == 0).width());
		mHeightEdit->setValue(pageSize(mUnitsCombo->units(), mOrientationCombo->currentIndex() == 0).height());
	}

	updateWidthHeightEdit();

	Q_UNUSED(index);
}

void DiagramPropertiesWidget::updateForWidthHeightOrMarginChange()
{
	setPageSize(QSizeF(mWidthEdit->value(), mHeightEdit->value()), mUnitsCombo->units());
	mOrientationCombo->setCurrentIndex((mWidthEdit->value() > mHeightEdit->value()) ? 1 : 0);

	updateWidthHeightEdit();
}

void DiagramPropertiesWidget::updateWidthHeightEdit()
{
	qreal width = mWidthEdit->value() - mMarginRightEdit->value() - mMarginLeftEdit->value();
	qreal height = mHeightEdit->value() - mMarginBottomEdit->value() - mMarginTopEdit->value();
	mWidthLabel->setText(mWidthEdit->valueToText(width, mWidthEdit->units()));
	mHeightLabel->setText(mHeightEdit->valueToText(height, mWidthEdit->units()));
}

//==================================================================================================

QGroupBox* DiagramPropertiesWidget::createDiagramGroup()
{
	QGroupBox* diagramGroup = new QGroupBox("Diagram");
	QFormLayout* diagramLayout = new QFormLayout();

	mUnitsCombo = new UnitsComboBox();
	connect(mUnitsCombo, SIGNAL(unitsChanged(DrawingUnits)), this, SLOT(updateForUnitsChange(DrawingUnits)));
	diagramLayout->addRow("Units: ", mUnitsCombo);

	mWidthLabel = new QLineEdit();
	mWidthLabel->setEnabled(false);
	diagramLayout->addRow("Width: ", mWidthLabel);

	mHeightLabel = new QLineEdit();
	mHeightLabel->setEnabled(false);
	diagramLayout->addRow("Height: ", mHeightLabel);

	mBackgroundColorButton = new ColorPushButton();
	diagramLayout->addRow("Background Color: ", mBackgroundColorButton);

	diagramLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	diagramLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	diagramLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	diagramLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	diagramGroup->setLayout(diagramLayout);

	return diagramGroup;
}

QGroupBox* DiagramPropertiesWidget::createGridGroup()
{
	QGroupBox* gridGroup = new QGroupBox("Grid");
	QFormLayout* gridLayout = new QFormLayout();

	mGridEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	gridLayout->addRow("Grid: ", mGridEdit);

	mGridStyleCombo = new GridStyleComboBox();
	gridLayout->addRow("Style: ", mGridStyleCombo);
	connect(mGridStyleCombo, SIGNAL(styleChanged(DrawingStyleOptions::GridStyle)), this, SLOT(updateForGridStyleChange(DrawingStyleOptions::GridStyle)));

	mGridSpacingMajorSpin = new QSpinBox();
	mGridSpacingMajorSpin->setMinimum(0);
	gridLayout->addRow("Major Spacing: ", mGridSpacingMajorSpin);

	mGridSpacingMinorSpin = new QSpinBox();
	mGridSpacingMinorSpin->setMinimum(0);
	gridLayout->addRow("Minor Spacing: ", mGridSpacingMinorSpin);

	mGridColorButton = new ColorPushButton();
	gridLayout->addRow("Color: ", mGridColorButton);

	gridLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	gridLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	gridLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	gridLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	gridGroup->setLayout(gridLayout);

	return gridGroup;
}

QGroupBox* DiagramPropertiesWidget::createBorderGroup()
{
	QGroupBox* borderGroup = new QGroupBox("Border");
	QFormLayout* borderLayout = new QFormLayout();

	mBorderWidthEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	borderLayout->addRow("Width: ", mBorderWidthEdit);

	mBorderColorButton = new ColorPushButton();
	borderLayout->addRow("Color: ", mBorderColorButton);

	borderLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	borderLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	borderLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	borderLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	borderGroup->setLayout(borderLayout);

	return borderGroup;
}

//==================================================================================================

QGroupBox* DiagramPropertiesWidget::createPaperFormatGroup()
{
	QGroupBox* paperFormatGroup = new QGroupBox("Paper Format");
	QFormLayout* paperFormatLayout = new QFormLayout();

	mPageSizeCombo = new QComboBox();
	paperFormatLayout->addRow("Format: ", mPageSizeCombo);
	connect(mPageSizeCombo, SIGNAL(activated(int)), this, SLOT(updateForPageSizeOrOrientationChange(int)));
	addSupportedPageSizes(true);

	mWidthEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::PositivesOnly, true);
	paperFormatLayout->addRow("Width: ", mWidthEdit);
	connect(mWidthEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	mHeightEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::PositivesOnly, true);
	paperFormatLayout->addRow("Height: ", mHeightEdit);
	connect(mHeightEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	mOrientationCombo = new QComboBox();
	mOrientationCombo->addItems(QStringList() << "Portrait" << "Landscape");
	paperFormatLayout->addRow("Orientation: ", mOrientationCombo);
	connect(mOrientationCombo, SIGNAL(activated(int)), this, SLOT(updateForPageSizeOrOrientationChange(int)));

	paperFormatLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	paperFormatLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	paperFormatLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	paperFormatLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	paperFormatGroup->setLayout(paperFormatLayout);

	return paperFormatGroup;
}

QGroupBox* DiagramPropertiesWidget::createMarginsGroup()
{
	QGroupBox* marginsGroup = new QGroupBox("Margins");
	QFormLayout* marginsLayout = new QFormLayout();

	mMarginLeftEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	marginsLayout->addRow("Left: ", mMarginLeftEdit);
	connect(mMarginLeftEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	mMarginTopEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	marginsLayout->addRow("Top: ", mMarginTopEdit);
	connect(mMarginTopEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	mMarginRightEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	marginsLayout->addRow("Right: ", mMarginRightEdit);
	connect(mMarginRightEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	mMarginBottomEdit = new UnitsValueEdit(mUnitsCombo->units(), UnitsValueEdit::NonNegativesOnly, true);
	marginsLayout->addRow("Bottom: ", mMarginBottomEdit);
	connect(mMarginBottomEdit, SIGNAL(editingFinished()), this, SLOT(updateForWidthHeightOrMarginChange()));

	marginsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	marginsLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	marginsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	marginsLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	marginsGroup->setLayout(marginsLayout);

	return marginsGroup;
}

//==================================================================================================

int DiagramPropertiesWidget::labelWidth() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.width("Background Color: ") + 8;
}

//==================================================================================================

void DiagramPropertiesWidget::addSupportedPageSizes(bool showDimensions)
{
	QString unitsStr, dimensionsStr;

	mPageSizeInfo.append(PageSizeInfo(QPrinter::A4, "A4", 210, 297, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B5, "B5", 176, 250, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Letter, "Letter", 8.5, 11, QPrinter::Inch));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Legal, "Legal", 8.5, 14, QPrinter::Inch));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Executive, "Executive", 7.5, 10, QPrinter::Inch));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Ledger, "Ledger", 11, 17, QPrinter::Inch));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A0, "A0", 841, 1189, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A1, "A1", 594, 841, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A2, "A2", 420, 594, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A3, "A3", 297, 420, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A5, "A5", 148, 210, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A6, "A6", 105, 148, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A7, "A7", 74, 105, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A8, "A8", 52, 74, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::A9, "A9", 37, 52, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B0, "B0", 1000, 1414, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B1, "B1", 707, 1000, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B2, "B2", 500, 707, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B3, "B3", 353, 500, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B4, "B4", 250, 353, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B6, "B6", 125, 176, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B7, "B7", 88, 125, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B8, "B8", 62, 88, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B9, "B9", 33, 62, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::B10, "B10", 31, 44, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::C5E, "C5E", 163, 229, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Comm10E, "Comm10E", 105, 241, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::DLE, "DLE", 110, 220, QPrinter::Millimeter));
	mPageSizeInfo.append(PageSizeInfo(QPrinter::Folio, "Folio", 210, 330, QPrinter::Millimeter));

	for(auto pageIter = mPageSizeInfo.begin(); pageIter != mPageSizeInfo.end(); pageIter++)
	{
		if (showDimensions)
		{
			unitsStr = ((*pageIter).units == QPrinter::Millimeter) ? " mm" : "\"";
			dimensionsStr = "(" + QString::number((*pageIter).portraitWidth) + unitsStr + " x " +
				QString::number((*pageIter).portraitHeight) + unitsStr + ")";
			mPageSizeCombo->addItem((*pageIter).name + " " + dimensionsStr);
		}
		else mPageSizeCombo->addItem((*pageIter).name);
	}

	mPageSizeCombo->addItem("Custom");
}

void DiagramPropertiesWidget::setPageSize(const QSizeF& size, DrawingUnits units)
{
	int selectIndex = mPageSizeCombo->count() - 1;
	qreal width = qMin(size.width(), size.height());
	qreal height = qMax(size.width(), size.height());
	qreal scaleFactor;

	for(int i = 0; selectIndex == mPageSizeCombo->count() - 1 && i < mPageSizeInfo.length(); i++)
	{
		if (mPageSizeInfo[i].units == QPrinter::Inch)
		{
			if (units == UnitsMils) scaleFactor = 1000;
			else scaleFactor = Drawing::unitsScale(units, UnitsMils) * 1000;
		}
		else
		{
			if (units == UnitsMils) scaleFactor = Drawing::unitsScale(units, UnitsSimpleMM);
			else scaleFactor = 1;
		}

		if (mPageSizeInfo[i].portraitWidth * scaleFactor == width && mPageSizeInfo[i].portraitHeight * scaleFactor == height)
			selectIndex = i;
	}

	mPageSizeCombo->setCurrentIndex(selectIndex);
}

QSizeF DiagramPropertiesWidget::pageSize(DrawingUnits units, bool portrait)
{
	QSizeF pageSize;

	if (mPageSizeCombo->currentIndex() < mPageSizeCombo->count() - 1)
	{
		qreal pageWidth = mPageSizeInfo[mPageSizeCombo->currentIndex()].portraitWidth;
		qreal pageHeight = mPageSizeInfo[mPageSizeCombo->currentIndex()].portraitHeight;
		qreal scaleFactor = 1;

		if (mPageSizeInfo[mPageSizeCombo->currentIndex()].units == QPrinter::Inch)
		{
			if (units == UnitsMils) scaleFactor = 1000;
			else scaleFactor = Drawing::unitsScale(UnitsMils, units) * 1000;
		}
		else
		{
			if (units == UnitsMils) scaleFactor = Drawing::unitsScale(UnitsSimpleMM, units);
			else scaleFactor = 1;
		}

		pageWidth *= scaleFactor;
		pageHeight *= scaleFactor;

		if (portrait) pageSize = QSizeF(pageWidth, pageHeight);
		else pageSize = QSizeF(pageHeight, pageWidth);
	}

	return pageSize;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramPropertiesDialog::DiagramPropertiesDialog(QWidget* parent) : QDialog(parent)
{
	mPropertiesWidget = new DiagramPropertiesWidget();

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mPropertiesWidget, 100);
	vLayout->addWidget(createButtonBox());
	setLayout(vLayout);

	setWindowTitle("Diagram Properties");
	resize(280, 10);
}

DiagramPropertiesDialog::~DiagramPropertiesDialog() { }

//==================================================================================================

void DiagramPropertiesDialog::setProperties(const DiagramProperties& properties)
{
	mPropertiesWidget->setProperties(properties);
}

DiagramProperties DiagramPropertiesDialog::properties() const
{
	return mPropertiesWidget->properties();
}

//==================================================================================================

QDialogButtonBox* DiagramPropertiesDialog::createButtonBox()
{
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->setCenterButtons(true);

	QPushButton* okButton = buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
	QPushButton* cancelButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	okButton->setMinimumSize(72, 28);
	cancelButton->setMinimumSize(72, 28);

	return buttonBox;
}
