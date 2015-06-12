/* DiagramMultipleItemPropertiesWidget.cpp
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

#include "DiagramMultipleItemPropertiesWidget.h"
#include "ColorButton.h"
#include "EnumComboBox.h"
#include "PositionWidget.h"
#include "UnitsValueEdit.h"

DiagramMultipleItemPropertiesWidget::DiagramMultipleItemPropertiesWidget(const QList<DrawingItem*>& items) : QWidget()
{
	mItems = items;

	mRectCornerRadiusWidget = nullptr;
	mPenStyleCombo = nullptr;
	mPenWidthEdit = nullptr;
	mPenColorButton = nullptr;
	mBrushColorButton = nullptr;
	mFontFamilyCombo = nullptr;
	mFontSizeEdit = nullptr;
	mFontStyleWidget = nullptr;
	mFontBoldButton = nullptr;
	mFontItalicButton = nullptr;
	mFontUnderlineButton = nullptr;
	mFontOverlineButton = nullptr;
	mFontStrikeOutButton = nullptr;
	mTextAlignmentHorizontalCombo = nullptr;
	mTextAlignmentVerticalCombo = nullptr;
	mTextColorButton = nullptr;
	mStartArrowStyleCombo = nullptr;
	mStartArrowSizeEdit = nullptr;
	mEndArrowStyleCombo = nullptr;
	mEndArrowSizeEdit = nullptr;

	QGroupBox* rectGroup = createRectGroup();
	QGroupBox* penBrushGroup = createPenBrushGroup();
	QGroupBox* textGroup = createTextGroup();
	QGroupBox* startPointGroup = createStartPointGroup();
	QGroupBox* endPointGroup = createEndPointGroup();

	QVBoxLayout* vLayout = new QVBoxLayout();
	if (rectGroup) vLayout->addWidget(rectGroup);
	if (penBrushGroup) vLayout->addWidget(penBrushGroup);
	if (textGroup) vLayout->addWidget(textGroup);
	if (startPointGroup) vLayout->addWidget(startPointGroup);
	if (endPointGroup) vLayout->addWidget(endPointGroup);
	vLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(vLayout);
}

DiagramMultipleItemPropertiesWidget::~DiagramMultipleItemPropertiesWidget() { }

//==================================================================================================

QHash<QString,QVariant> DiagramMultipleItemPropertiesWidget::properties() const
{
	QHash<QString,QVariant> properties;

	if (mRectCornerRadiusWidget && mRectCornerRadiusWidget->isEnabled())
	{
		properties["Corner X-Radius"] = QVariant(mRectCornerRadiusWidget->pos().x());
		properties["Corner Y-Radius"] = QVariant(mRectCornerRadiusWidget->pos().y());
	}

	if (mPenStyleCombo && mPenStyleCombo->isEnabled())
		properties["Pen Style"] = QVariant((unsigned int)mPenStyleCombo->style());
	if (mPenWidthEdit && mPenWidthEdit->isEnabled())
		properties["Pen Width"] = QVariant(mPenWidthEdit->value());
	if (mPenColorButton && mPenColorButton->isEnabled())
		properties["Pen Color"] = QVariant(mPenColorButton->color());
	if (mBrushColorButton && mBrushColorButton->isEnabled())
		properties["Brush Color"] = QVariant(mBrushColorButton->color());

	if (mFontFamilyCombo && mFontFamilyCombo->isEnabled())
		properties["Font Family"] = QVariant(mFontFamilyCombo->currentFont().family());
	if (mFontSizeEdit && mFontSizeEdit->isEnabled())
		properties["Font Size"] = QVariant(mFontSizeEdit->value());
	if (mFontBoldButton && mFontBoldButton->isEnabled())
		properties["Font Bold"] = QVariant(mFontBoldButton->isChecked());
	if (mFontItalicButton && mFontItalicButton->isEnabled())
		properties["Font Italic"] = QVariant(mFontItalicButton->isChecked());
	if (mFontUnderlineButton && mFontUnderlineButton->isEnabled())
		properties["Font Underline"] = QVariant(mFontUnderlineButton->isChecked());
	if (mFontOverlineButton && mFontOverlineButton->isEnabled())
		properties["Font Overline"] = QVariant(mFontOverlineButton->isChecked());
	if (mFontStrikeOutButton && mFontStrikeOutButton->isEnabled())
		properties["Font Strike-Out"] = QVariant(mFontStrikeOutButton->isChecked());
	if (mTextAlignmentHorizontalCombo && mTextAlignmentHorizontalCombo->isEnabled())
		properties["Text Horizontal Alignment"] = QVariant((unsigned int)mTextAlignmentHorizontalCombo->alignment());
	if (mTextAlignmentVerticalCombo && mTextAlignmentVerticalCombo->isEnabled())
		properties["Text Vertical Alignment"] = QVariant((unsigned int)mTextAlignmentVerticalCombo->alignment());
	if (mTextColorButton && mTextColorButton->isEnabled())
		properties["Text Color"] = QVariant(mTextColorButton->color());

	if (mStartArrowStyleCombo && mStartArrowStyleCombo->isEnabled())
		properties["Start Arrow Style"] = QVariant((unsigned int)mStartArrowStyleCombo->style());
	if (mStartArrowSizeEdit && mStartArrowSizeEdit->isEnabled())
		properties["Start Arrow Size"] = QVariant(mStartArrowSizeEdit->value());

	if (mEndArrowStyleCombo && mEndArrowStyleCombo->isEnabled())
		properties["End Arrow Style"] = QVariant((unsigned int)mEndArrowStyleCombo->style());
	if (mEndArrowSizeEdit && mEndArrowSizeEdit->isEnabled())
		properties["End Arrow Size"] = QVariant(mEndArrowSizeEdit->value());

	return properties;
}

bool DiagramMultipleItemPropertiesWidget::shouldExec() const
{
	return (layout()->count() > 0);
}

//==================================================================================================

QGroupBox* DiagramMultipleItemPropertiesWidget::createRectGroup()
{
	QGroupBox* rectGroup = nullptr;
	bool xRadiusMatch, yRadiusMatch;
	QVariant xRadiusValue, yRadiusValue;

	if (allItemsContainProperty("Corner X-Radius", xRadiusMatch, xRadiusValue) &&
		allItemsContainProperty("Corner Y-Radius", yRadiusMatch, yRadiusValue))
	{
		QFormLayout* rectLayout = new QFormLayout();

		mRectCornerRadiusWidget = new PositionWidget(mItems.first()->units());
		if (xRadiusValue.isValid() && yRadiusValue.isValid())
			mRectCornerRadiusWidget->setPos(QPointF(xRadiusValue.toDouble(), yRadiusValue.toDouble()));
		addWidget(rectLayout, "Corner Radius: ", mRectCornerRadiusWidget, xRadiusMatch && yRadiusMatch);

		rectGroup = new QGroupBox("Rect");
		rectLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		rectLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		rectLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		rectLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		rectGroup->setLayout(rectLayout);
	}

	return rectGroup;
}

QGroupBox* DiagramMultipleItemPropertiesWidget::createPenBrushGroup()
{
	QGroupBox* penBrushGroup = nullptr;

	if (allItemsContainAtLeastOneProperty(QStringList() << "Pen Style" << "Pen Width" << "Pen Color" << "Brush Color"))
	{
		QFormLayout* penBrushLayout = new QFormLayout();
		bool propertiesMatch;
		QVariant propertyValue;

		if (allItemsContainProperty("Pen Style", propertiesMatch, propertyValue))
		{
			mPenStyleCombo = new PenStyleComboBox();
			if (propertyValue.isValid()) mPenStyleCombo->setStyle((Qt::PenStyle)propertyValue.toUInt());
			addWidget(penBrushLayout, "Pen Style: ", mPenStyleCombo, propertiesMatch);
		}

		if (allItemsContainProperty("Pen Width", propertiesMatch, propertyValue))
		{
			mPenWidthEdit = new UnitsValueEdit(mItems.first()->units(), UnitsValueEdit::NonNegativesOnly, true);
			if (propertyValue.isValid()) mPenWidthEdit->setValue(propertyValue.toDouble());
			addWidget(penBrushLayout, "Pen Width: ", mPenWidthEdit, propertiesMatch);
		}

		if (allItemsContainProperty("Pen Color", propertiesMatch, propertyValue))
		{
			mPenColorButton = new ColorPushButton();
			if (propertyValue.isValid()) mPenColorButton->setColor(propertyValue.value<QColor>());
			addWidget(penBrushLayout, "Pen Color: ", mPenColorButton, propertiesMatch);
		}

		if (allItemsContainProperty("Brush Color", propertiesMatch, propertyValue))
		{
			mBrushColorButton = new ColorPushButton();
			if (propertyValue.isValid()) mBrushColorButton->setColor(propertyValue.value<QColor>());
			addWidget(penBrushLayout, "Brush Color: ", mBrushColorButton, propertiesMatch);
		}

		penBrushGroup = new QGroupBox("Pen and Brush");
		penBrushLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		penBrushLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		penBrushLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		penBrushLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		penBrushGroup->setLayout(penBrushLayout);
	}

	return penBrushGroup;
}

QGroupBox* DiagramMultipleItemPropertiesWidget::createTextGroup()
{
	QGroupBox* textGroup = nullptr;

	if (allItemsContainAtLeastOneProperty(QStringList() << "Font Family" << "Font Size" << "Font Bold" <<
		"Font Italic" << "Font Underline" << "Font Overline" << "Font Strike-Out" <<
		"Text Horizontal Alignment" << "Text Vertical Alignment" << "Text Color"))
	{
		QFormLayout* textLayout = new QFormLayout();
		QWidget* fontStyleWidget;
		QWidget* textAlignmentWidget;
		bool propertiesMatch;
		QVariant propertyValue;

		if (allItemsContainProperty("Font Family", propertiesMatch, propertyValue))
		{
			mFontFamilyCombo = new QFontComboBox();
			if (propertyValue.isValid()) mFontFamilyCombo->setCurrentFont(QFont(propertyValue.toString()));
			addWidget(textLayout, "Font Family: ", mFontFamilyCombo, propertiesMatch);
		}

		if (allItemsContainProperty("Font Size", propertiesMatch, propertyValue))
		{
			mFontSizeEdit = new UnitsValueEdit(mItems.first()->units(), UnitsValueEdit::PositivesOnly, true);
			if (propertyValue.isValid()) mFontSizeEdit->setValue(propertyValue.toDouble());
			addWidget(textLayout, "Font Size: ", mFontSizeEdit, propertiesMatch);
		}

		fontStyleWidget = createFontStyleWidget(propertiesMatch);
		if (fontStyleWidget) addWidget(textLayout, "Font Style: ", fontStyleWidget, propertiesMatch);

		textAlignmentWidget = createTextAlignmentWidget(propertiesMatch);
		if (textAlignmentWidget) addWidget(textLayout, "Text Alignment: ", textAlignmentWidget, propertiesMatch);

		if (allItemsContainProperty("Text Color", propertiesMatch, propertyValue))
		{
			mTextColorButton = new ColorPushButton();
			if (propertyValue.isValid()) mTextColorButton->setColor(propertyValue.value<QColor>());
			addWidget(textLayout, "Text Color: ", mTextColorButton, propertiesMatch);
		}

		textGroup = new QGroupBox("Text");
		textLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		textLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		textLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		textLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		textGroup->setLayout(textLayout);
	}

	return textGroup;
}

QGroupBox* DiagramMultipleItemPropertiesWidget::createStartPointGroup()
{
	QGroupBox* startPointGroup = nullptr;

	if (allItemsContainAtLeastOneProperty(QStringList() << "Start Arrow Style" << "Start Arrow Size"))
	{
		QFormLayout* startPointLayout = new QFormLayout();
		bool propertiesMatch;
		QVariant propertyValue;

		if (allItemsContainProperty("Start Arrow Style", propertiesMatch, propertyValue))
		{
			mStartArrowStyleCombo = new ArrowStyleComboBox();
			if (propertyValue.isValid()) mStartArrowStyleCombo->setStyle((DrawingArrow::Style)propertyValue.toUInt());
			addWidget(startPointLayout, "Arrow Style: ", mStartArrowStyleCombo, propertiesMatch);
		}

		if (allItemsContainProperty("Start Arrow Size", propertiesMatch, propertyValue))
		{
			mStartArrowSizeEdit = new UnitsValueEdit(mItems.first()->units(), UnitsValueEdit::NonNegativesOnly, true);
			if (propertyValue.isValid()) mStartArrowSizeEdit->setValue(propertyValue.toDouble());
			addWidget(startPointLayout, "Arrow Size: ", mStartArrowSizeEdit, propertiesMatch);
		}

		startPointGroup = new QGroupBox("Start Point");
		startPointLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		startPointLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		startPointLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		startPointLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		startPointGroup->setLayout(startPointLayout);
	}

	return startPointGroup;
}

QGroupBox* DiagramMultipleItemPropertiesWidget::createEndPointGroup()
{
	QGroupBox* endPointGroup = nullptr;

	if (allItemsContainAtLeastOneProperty(QStringList() << "End Arrow Style" << "End Arrow Size"))
	{
		QFormLayout* endPointLayout = new QFormLayout();
		bool propertiesMatch;
		QVariant propertyValue;

		if (allItemsContainProperty("End Arrow Style", propertiesMatch, propertyValue))
		{
			mEndArrowStyleCombo = new ArrowStyleComboBox();
			if (propertyValue.isValid()) mEndArrowStyleCombo->setStyle((DrawingArrow::Style)propertyValue.toUInt());
			addWidget(endPointLayout, "Arrow Style: ", mEndArrowStyleCombo, propertiesMatch);
		}

		if (allItemsContainProperty("End Arrow Size", propertiesMatch, propertyValue))
		{
			mEndArrowSizeEdit = new UnitsValueEdit(mItems.first()->units(), UnitsValueEdit::NonNegativesOnly, true);
			if (propertyValue.isValid()) mEndArrowSizeEdit->setValue(propertyValue.toDouble());
			addWidget(endPointLayout, "Arrow Size: ", mEndArrowSizeEdit, propertiesMatch);
		}

		endPointGroup = new QGroupBox("End Point");
		endPointLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		endPointLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		endPointLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		endPointLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		endPointGroup->setLayout(endPointLayout);
	}

	return endPointGroup;
}

//==================================================================================================

QWidget* DiagramMultipleItemPropertiesWidget::createFontStyleWidget(bool& propertiesMatch)
{
	QWidget* fontStyleWidget = nullptr;

	propertiesMatch = false;

	if (allItemsContainAtLeastOneProperty(QStringList() << "Font Bold" <<
		"Font Italic" << "Font Underline" << "Font Overline" << "Font Strike-Out"))
	{
		QHBoxLayout* fontStyleLayout = new QHBoxLayout();
		bool fontStyleMatch;
		QVariant propertyValue;

		propertiesMatch = true;

		if (allItemsContainProperty("Font Bold", fontStyleMatch, propertyValue))
		{
			mFontBoldButton = new QToolButton();
			mFontBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
			mFontBoldButton->setToolTip("Font Bold");
			mFontBoldButton->setCheckable(true);
			if (propertyValue.isValid()) mFontBoldButton->setChecked(propertyValue.toBool());
			fontStyleLayout->addWidget(mFontBoldButton);
			propertiesMatch = (propertiesMatch && fontStyleMatch);
		}

		if (allItemsContainProperty("Font Italic", fontStyleMatch, propertyValue))
		{
			mFontItalicButton = new QToolButton();
			mFontItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
			mFontItalicButton->setToolTip("Font Italic");
			mFontItalicButton->setCheckable(true);
			if (propertyValue.isValid()) mFontItalicButton->setChecked(propertyValue.toBool());
			fontStyleLayout->addWidget(mFontItalicButton);
			propertiesMatch = (propertiesMatch && fontStyleMatch);
		}

		if (allItemsContainProperty("Font Underline", fontStyleMatch, propertyValue))
		{
			mFontUnderlineButton = new QToolButton();
			mFontUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
			mFontUnderlineButton->setToolTip("Font Underline");
			mFontUnderlineButton->setCheckable(true);
			if (propertyValue.isValid()) mFontUnderlineButton->setChecked(propertyValue.toBool());
			fontStyleLayout->addWidget(mFontUnderlineButton);
			propertiesMatch = (propertiesMatch && fontStyleMatch);
		}

		if (allItemsContainProperty("Font Overline", fontStyleMatch, propertyValue))
		{
			mFontOverlineButton = new QToolButton();
			mFontOverlineButton->setIcon(QIcon(":/icons/oxygen/format-text-overline.png"));
			mFontOverlineButton->setToolTip("Font Overline");
			mFontOverlineButton->setCheckable(true);
			if (propertyValue.isValid()) mFontOverlineButton->setChecked(propertyValue.toBool());
			fontStyleLayout->addWidget(mFontOverlineButton);
			propertiesMatch = (propertiesMatch && fontStyleMatch);
		}

		if (allItemsContainProperty("Font Strike-Out", fontStyleMatch, propertyValue))
		{
			mFontStrikeOutButton = new QToolButton();
			mFontStrikeOutButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
			mFontStrikeOutButton->setToolTip("Font Strike-Out");
			mFontStrikeOutButton->setCheckable(true);
			if (propertyValue.isValid()) mFontStrikeOutButton->setChecked(propertyValue.toBool());
			fontStyleLayout->addWidget(mFontStrikeOutButton);
			propertiesMatch = (propertiesMatch && fontStyleMatch);
		}

		fontStyleWidget = new QWidget();
		fontStyleLayout->addWidget(new QWidget(), 100);
		fontStyleLayout->setSpacing(1);
		fontStyleLayout->setContentsMargins(0, 0, 0, 0);
		fontStyleWidget->setLayout(fontStyleLayout);
	}

	return fontStyleWidget;
}

QWidget* DiagramMultipleItemPropertiesWidget::createTextAlignmentWidget(bool& propertiesMatch)
{
	QWidget* textAlignmentWidget = nullptr;

	propertiesMatch = false;

	if (allItemsContainAtLeastOneProperty(QStringList() << "Text Horizontal Alignment" <<
		"Text Vertical Alignment"))
	{
		QHBoxLayout* textAlignmentLayout = new QHBoxLayout();
		bool alignmentMatch;
		QVariant propertyValue;

		propertiesMatch = true;

		if (allItemsContainProperty("Text Horizontal Alignment", alignmentMatch, propertyValue))
		{
			mTextAlignmentHorizontalCombo = new AlignmentHorizontalComboBox();
			if (propertyValue.isValid()) mTextAlignmentHorizontalCombo->setAlignment((Qt::Alignment)propertyValue.toUInt());
			textAlignmentLayout->addWidget(mTextAlignmentHorizontalCombo);
			propertiesMatch = (propertiesMatch && alignmentMatch);
		}

		if (allItemsContainProperty("Text Vertical Alignment", alignmentMatch, propertyValue))
		{
			mTextAlignmentVerticalCombo = new AlignmentVerticalComboBox();
			if (propertyValue.isValid()) mTextAlignmentVerticalCombo->setAlignment((Qt::Alignment)propertyValue.toUInt());
			textAlignmentLayout->addWidget(mTextAlignmentVerticalCombo);
			propertiesMatch = (propertiesMatch && alignmentMatch);
		}

		textAlignmentWidget = new QWidget();
		textAlignmentLayout->setSpacing(1);
		textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
		textAlignmentWidget->setLayout(textAlignmentLayout);
	}

	return textAlignmentWidget;
}

//==================================================================================================

bool DiagramMultipleItemPropertiesWidget::allItemsContainAtLeastOneProperty(const QStringList& propertyNames)
{
	bool valid = false;
	bool propertiesMatch;
	QVariant propertyValue;

	for(auto nameIter = propertyNames.begin(); !valid && nameIter != propertyNames.end(); nameIter++)
		valid = allItemsContainProperty(*nameIter, propertiesMatch, propertyValue);

	return valid;
}

bool DiagramMultipleItemPropertiesWidget::allItemsContainProperty(const QString& propertyName, bool& propertiesMatch, QVariant& propertyValue)
{
	bool allItemsHaveProperty = true;

	propertiesMatch = true;
	propertyValue = QVariant();

	for(auto itemIter = mItems.begin(); allItemsHaveProperty && itemIter != mItems.end(); itemIter++)
	{
		allItemsHaveProperty = (*itemIter)->containsProperty(propertyName);
		if (allItemsHaveProperty)
		{
			if (!propertyValue.isValid()) propertyValue = (*itemIter)->propertyValue(propertyName);
			else propertiesMatch = (propertiesMatch && propertyValue == (*itemIter)->propertyValue(propertyName));
		}
	}

	return allItemsHaveProperty;
}

void DiagramMultipleItemPropertiesWidget::addWidget(QFormLayout* layout, const QString& text, QWidget* widget, bool checked)
{
	QCheckBox* checkBox = new QCheckBox(text);
	checkBox->setChecked(checked);
	widget->setEnabled(checked);
	connect(checkBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));

	layout->addRow(checkBox, widget);
}

//==================================================================================================

int DiagramMultipleItemPropertiesWidget::labelWidth() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.width("Corner Radius: ") + 24;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramMultipleItemPropertiesDialog::DiagramMultipleItemPropertiesDialog(const QList<DrawingItem*>& items,
	QWidget* parent) : QDialog(parent)
{
	mPropertiesWidget = new DiagramMultipleItemPropertiesWidget(items);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mPropertiesWidget, 100);
	vLayout->addWidget(createButtonBox());
	setLayout(vLayout);

	setWindowTitle("Multiple Item Properties");
	resize(320, 10);
}

DiagramMultipleItemPropertiesDialog::~DiagramMultipleItemPropertiesDialog() { }

//==================================================================================================

QHash<QString,QVariant> DiagramMultipleItemPropertiesDialog::properties() const
{
	return mPropertiesWidget->properties();
}

bool DiagramMultipleItemPropertiesDialog::shouldExec() const
{
	return mPropertiesWidget->shouldExec();
}

//==================================================================================================

QDialogButtonBox* DiagramMultipleItemPropertiesDialog::createButtonBox()
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
