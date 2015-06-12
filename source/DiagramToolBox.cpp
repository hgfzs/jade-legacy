/* DiagramToolBox.cpp
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

#include "DiagramToolBox.h"
#include "DiagramView.h"

DiagramToolBox::DiagramToolBox(DiagramView* diagramView) : QWidget()
{
	mDiagramView = diagramView;

	createActions();

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(createButtonBox());
	vLayout->addWidget(createTreeWidget(), 100);
	setLayout(vLayout);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	connect(mDiagramView, SIGNAL(modeChanged(DrawingView::Mode)),
		this, SLOT(updateDiagramMode(DrawingView::Mode)));
}

DiagramToolBox::~DiagramToolBox() { }

//==================================================================================================

void DiagramToolBox::addItem(DrawingItem* item, const QString& section, const QString& text,
	const QString& iconPath)
{
	if (item)
	{
		QTreeWidgetItem* newItem = nullptr;
		QTreeWidgetItem* sectionItem = nullptr;

		for(int i = 0; !sectionItem && i < mTreeWidget->topLevelItemCount(); i++)
		{
			if (mTreeWidget->topLevelItem(i)->text(0) == section)
				sectionItem = mTreeWidget->topLevelItem(i);
		}

		if (!sectionItem)
		{
			sectionItem = new QTreeWidgetItem();
			sectionItem->setText(0, section);
			sectionItem->setBackground(0, palette().brush(QPalette::Button));
			mTreeWidget->addTopLevelItem(sectionItem);
		}

		newItem = new QTreeWidgetItem(sectionItem);
		newItem->setText(0, text);
		if (!iconPath.isEmpty()) newItem->setIcon(0, QIcon(iconPath));

		QAction* newAction = createAction(text, iconPath, "", item->uniqueKey());
		mItemActions[newItem] = newAction;
	}
}

QSize DiagramToolBox::sizeHint() const
{
	return QSize(140, 400);
}

//==================================================================================================

void DiagramToolBox::setDiagramModeFromAction(QAction* action)
{
	if (action->text() == "Select Mode") mDiagramView->setDefaultMode();
	else if (action->text() == "Scroll Mode") mDiagramView->setScrollMode();
	else if (action->text() == "Zoom Mode") mDiagramView->setZoomMode();
	else
	{
		DrawingItem* newItem = DrawingView::itemFactory.create(action->data().toString());
		if (newItem) mDiagramView->setNewMode(newItem);
		else mDiagramView->setDefaultMode();
	}
}

void DiagramToolBox::triggerAction(QTreeWidgetItem* item, int column)
{
	Q_UNUSED(column);
	if (mItemActions.contains(item)) mItemActions[item]->trigger();
}

void DiagramToolBox::updateDiagramMode(DrawingView::Mode mode)
{
	if (mode == DrawingView::DefaultMode) actions()[SelectModeAction]->setChecked(true);
}

//==================================================================================================

void DiagramToolBox::showEvent(QShowEvent* event)
{
	Q_UNUSED(event);
	mTreeWidget->expandItem(mTreeWidget->topLevelItem(0));
}

//==================================================================================================

void DiagramToolBox::createActions()
{
	mDiagramModeActionGroup = new QActionGroup(this);
	connect(mDiagramModeActionGroup, SIGNAL(triggered(QAction*)),
		this, SLOT(setDiagramModeFromAction(QAction*)));

	addAction(createAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape"));
	addAction(createAction("Scroll Mode", ":/icons/oxygen/transform-move.png"));
	addAction(createAction("Zoom Mode", ":/icons/oxygen/page-zoom.png"));

	actions()[SelectModeAction]->setChecked(true);
}

QDialogButtonBox* DiagramToolBox::createButtonBox()
{
	QList<QAction*> actions = DiagramToolBox::actions();

	mButtonBox = new QDialogButtonBox();
	mButtonBox->setCenterButtons(true);

	mButtonBox->addButton(createButton(actions[SelectModeAction]), QDialogButtonBox::ActionRole);
	mButtonBox->addButton(createButton(actions[ScrollModeAction]), QDialogButtonBox::ActionRole);
	mButtonBox->addButton(createButton(actions[ZoomModeAction]), QDialogButtonBox::ActionRole);
	mButtonBox->addButton(createButton(
		mDiagramView->actions()[DiagramView::PropertiesAction]), QDialogButtonBox::ActionRole);

	return mButtonBox;
}

QTreeWidget* DiagramToolBox::createTreeWidget()
{
	mTreeWidget = new QTreeWidget();
	mTreeWidget->setHeaderHidden(true);
	mTreeWidget->setIndentation(6);
	mTreeWidget->expandAll();
	connect(mTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
		this, SLOT(triggerAction(QTreeWidgetItem*,int)));

	return mTreeWidget;
}

//==================================================================================================

QAction* DiagramToolBox::createAction(const QString& text,
	const QString& iconPath, const QString& shortcut, const QVariant& data)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));
	action->setData(data);

	action->setCheckable(true);
	action->setActionGroup(mDiagramModeActionGroup);

	return action;
}

QToolButton* DiagramToolBox::createButton(QAction* action)
{
	int size = 1.8 * QFontMetrics(font()).height();

	QToolButton* newButton = new QToolButton();
	newButton->setDefaultAction(action);
	newButton->setToolTip(action->text());
	newButton->setIconSize(QSize(size, size));

	return newButton;
}
