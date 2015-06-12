/* ExportOptionsDialog.cpp
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

// ExportOptionsDialog.cpp

#include "ExportOptionsDialog.h"
#include "DiagramView.h"
#include "EnumComboBox.h"

ExportOptionsDialog::ExportOptionsDialog(DiagramView* diagramView) : QDialog(diagramView)
{
	mDiagram = diagramView;

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(createSizeGroup());
	mainLayout->addWidget(createOptionsGroup());
	mainLayout->addWidget(new QWidget());
	mainLayout->addWidget(createButtonBox());
	setLayout(mainLayout);

	setWindowTitle("Export Options");
	resize(240, 10);
}

ExportOptionsDialog::~ExportOptionsDialog() { }

//==================================================================================================

void ExportOptionsDialog::setPrevious(int width, int height,
	DrawingStyleOptions::ColorMode renderMode, DrawingStyleOptions::RenderFlags renderFlags)
{
	if (width > 0 && height > 0)
	{
		mWidthEdit->setText(QString::number(width));
		if (mMaintainAspectRatioCheck->isChecked())
		{
			mHeightEdit->setText(QString::number(qRound(width *
				mDiagram->scene()->sceneRect().height() / mDiagram->scene()->sceneRect().width())));
		}
		else mHeightEdit->setText(QString::number(height));
	}
	else
	{
		if (mDiagram->scene()->units() == UnitsMils)
		{
			mWidthEdit->setText(QString::number(mDiagram->scene()->sceneRect().width() / 5));
			mHeightEdit->setText(QString::number(mDiagram->scene()->sceneRect().height() / 5));
		}
		else
		{
			mWidthEdit->setText(QString::number(mDiagram->scene()->sceneRect().width() * 8));
			mHeightEdit->setText(QString::number(mDiagram->scene()->sceneRect().height() * 8));
		}
	}

	mRenderModeCombo->setMode(renderMode);
	mDrawBackgroundCheck->setChecked(renderFlags & DrawingStyleOptions::DrawBackground);
	mDrawBorderCheck->setChecked(renderFlags & DrawingStyleOptions::DrawBorder);
	mDrawGridCheck->setChecked(renderFlags & DrawingStyleOptions::DrawGrid);
}

QSize ExportOptionsDialog::exportSize() const
{
	return QSize(mWidthEdit->text().toInt(), mHeightEdit->text().toInt());
}

DrawingStyleOptions::ColorMode ExportOptionsDialog::renderMode() const
{
	return mRenderModeCombo->mode();
}

DrawingStyleOptions::RenderFlags ExportOptionsDialog::renderFlags() const
{
	DrawingStyleOptions::RenderFlags flags = 0;

	if (mDrawBackgroundCheck->isChecked()) flags |= DrawingStyleOptions::DrawBackground;
	if (mDrawBorderCheck->isChecked()) flags |= DrawingStyleOptions::DrawBorder;
	if (mDrawGridCheck->isChecked()) flags |= DrawingStyleOptions::DrawGrid;

	return flags;
}

//==================================================================================================

void ExportOptionsDialog::updateWidth()
{
	if (mMaintainAspectRatioCheck->isChecked())
	{
		mWidthEdit->setText(QString::number(qRound(mHeightEdit->text().toInt() *
			mDiagram->scene()->sceneRect().width() / mDiagram->scene()->sceneRect().height())));
	}
}

void ExportOptionsDialog::updateHeight()
{
	if (mMaintainAspectRatioCheck->isChecked())
	{
		mHeightEdit->setText(QString::number(qRound(mWidthEdit->text().toInt() *
			mDiagram->scene()->sceneRect().height() / mDiagram->scene()->sceneRect().width())));
	}
}

//==================================================================================================

QGroupBox* ExportOptionsDialog::createSizeGroup()
{
	QGroupBox* sizeGroup = new QGroupBox("Size");

	mWidthEdit = new QLineEdit();
	mHeightEdit = new QLineEdit();
	mWidthEdit->setValidator(new QIntValidator(0, 1E9));
	mHeightEdit->setValidator(new QIntValidator(0, 1E9));
	connect(mWidthEdit, SIGNAL(editingFinished()), this, SLOT(updateHeight()));
	connect(mHeightEdit, SIGNAL(editingFinished()), this, SLOT(updateWidth()));

	mMaintainAspectRatioCheck = new QCheckBox("Maintain Aspect Ratio");
	mMaintainAspectRatioCheck->setChecked(true);

	QWidget* widthHeightWidget = new QWidget();
	QFormLayout* widthHeightLayout = new QFormLayout();
	widthHeightLayout->addRow("Width: ", mWidthEdit);
	widthHeightLayout->addRow("Height: ", mHeightEdit);
	widthHeightLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	widthHeightLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	widthHeightLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	widthHeightLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(100);
	widthHeightLayout->setContentsMargins(0, 0, 0, 0);
	widthHeightWidget->setLayout(widthHeightLayout);

	QWidget* maintainWidget = new QWidget();
	QVBoxLayout* maintainLayout = new QVBoxLayout();
	maintainLayout->addWidget(mMaintainAspectRatioCheck);
	maintainLayout->setContentsMargins(0, 0, 0, 0);
	maintainWidget->setLayout(maintainLayout);

	QVBoxLayout* sizeLayout = new QVBoxLayout();
	sizeLayout->addWidget(widthHeightWidget);
	sizeLayout->addWidget(maintainWidget);
	sizeLayout->setSpacing(8);
	sizeGroup->setLayout(sizeLayout);

	return sizeGroup;
}

QGroupBox* ExportOptionsDialog::createOptionsGroup()
{
	QGroupBox* optionsGroup = new QGroupBox("Render Options");

	mRenderModeCombo = new RenderModeComboBox();
	mDrawBackgroundCheck = new QCheckBox("Draw Background");
	mDrawBorderCheck = new QCheckBox("Draw Border");
	mDrawGridCheck = new QCheckBox("Draw Grid");

	QWidget* modeWidget = new QWidget();
	QFormLayout* modeLayout = new QFormLayout();
	modeLayout->addRow("Mode: ", mRenderModeCombo);
	modeLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	modeLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	modeLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	modeLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(100);
	modeLayout->setContentsMargins(0, 0, 0, 0);
	modeWidget->setLayout(modeLayout);

	QWidget* flagsWidget = new QWidget();
	QVBoxLayout* flagsLayout = new QVBoxLayout();
	flagsLayout->addWidget(mDrawBackgroundCheck);
	flagsLayout->addWidget(mDrawBorderCheck);
	flagsLayout->addWidget(mDrawGridCheck);
	flagsLayout->setContentsMargins(0, 0, 0, 0);
	flagsWidget->setLayout(flagsLayout);

	QVBoxLayout* optionsLayout = new QVBoxLayout();
	optionsLayout->addWidget(modeWidget);
	optionsLayout->addWidget(flagsWidget);
	optionsLayout->setSpacing(8);
	optionsGroup->setLayout(optionsLayout);

	return optionsGroup;
}

QDialogButtonBox* ExportOptionsDialog::createButtonBox()
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
