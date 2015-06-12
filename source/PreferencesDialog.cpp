/* PreferencesDialog.cpp
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

#include "PreferencesDialog.h"
#include "DiagramPropertiesWidget.h"

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
	QFontMetrics fontMetrics(font());
	int width = fontMetrics.width("Diagram Defaults") + 8;
	int height = fontMetrics.height();

	listWidget = new QListWidget();
	stackedWidget = new QStackedWidget();

	listWidget->setIconSize(QSize(1.6 * height, 1.6 * height));
	listWidget->setGridSize(QSize(width, 3.2 * height));
	listWidget->setViewMode(QListView::IconMode);
	listWidget->setMovement(QListView::Static);
	listWidget->setFixedWidth(width + 1);
	listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(listWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

	setupGeneralFrame();
	setupDiagramDefaultsFrame();

	QWidget *widget = new QWidget();
	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(listWidget);
	hLayout->addWidget(stackedWidget, 100);
	hLayout->setSpacing(16);
	widget->setLayout(hLayout);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(widget, 100);
	vLayout->addWidget(createButtonBox());
	setLayout(vLayout);

	setWindowTitle("Preferences");
	resize(500, 500);
}

PreferencesDialog::~PreferencesDialog() { }

//==================================================================================================

void PreferencesDialog::setPrompts(bool closingUnsaved, bool overwrite)
{
	promptCloseUnsavedCheck->setChecked(closingUnsaved);
	promptOverwriteCheck->setChecked(overwrite);
}

bool PreferencesDialog::shouldPromptOnClosingUnsaved() const
{
	return promptCloseUnsavedCheck->isChecked();
}

bool PreferencesDialog::shouldPromptOnOverwrite() const
{
	return promptOverwriteCheck->isChecked();
}

//==================================================================================================

void PreferencesDialog::setDiagramProperties(const DiagramProperties& properties)
{
	diagramPropertiesWidget->setProperties(properties);
}

DiagramProperties PreferencesDialog::diagramProperties() const
{
	return diagramPropertiesWidget->properties();
}

//==================================================================================================

void PreferencesDialog::setupGeneralFrame()
{
	promptOverwriteCheck = new QCheckBox("Prompt when overwriting existing files");
	promptCloseUnsavedCheck = new QCheckBox("Prompt when closing unsaved files");

	QGroupBox* promptGroup = new QGroupBox("Warnings");
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(promptOverwriteCheck);
	vLayout->addWidget(promptCloseUnsavedCheck);
	promptGroup->setLayout(vLayout);

	QFrame* generalFrame = new QFrame();
	vLayout = new QVBoxLayout();
	vLayout->addWidget(promptGroup);
	vLayout->addWidget(new QWidget(), 100);
	vLayout->setContentsMargins(0, 0, 0, 0);
	generalFrame->setLayout(vLayout);

	QListWidgetItem *item = new QListWidgetItem("General");
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	item->setTextAlignment(Qt::AlignCenter);
	item->setIcon(QIcon(":/icons/oxygen/configure.png"));
	listWidget->addItem(item);
	stackedWidget->addWidget(generalFrame);
}

void PreferencesDialog::setupDiagramDefaultsFrame()
{
	diagramPropertiesWidget = new DiagramPropertiesWidget();

	QFrame* diagramDefaultsFrame = new QFrame();
	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->addWidget(diagramPropertiesWidget);
	vLayout->addWidget(new QWidget(), 100);
	vLayout->setContentsMargins(0, 0, 0, 0);
	diagramPropertiesWidget->layout()->setContentsMargins(0, 0, 0, 0);
	diagramDefaultsFrame->setLayout(vLayout);

	QListWidgetItem *item = new QListWidgetItem("Diagram Defaults");
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	item->setTextAlignment(Qt::AlignCenter);
	item->setIcon(QIcon(":/icons/diagram.png"));
	listWidget->addItem(item);
	stackedWidget->addWidget(diagramDefaultsFrame);
}

//==================================================================================================

QDialogButtonBox* PreferencesDialog::createButtonBox()
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
