/* DiagramItemToolBox.h
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

#ifndef DIAGRAMITEMTOOLBOX_H
#define DIAGRAMITEMTOOLBOX_H

#include <Drawing>

class DiagramView;

class DiagramView;

class DiagramToolBox : public QWidget
{
	Q_OBJECT

public:
	enum ActionIndex { SelectModeAction, ScrollModeAction, ZoomModeAction, PlaceActionStart };

private:
	DiagramView* mDiagramView;

	QActionGroup* mDiagramModeActionGroup;

	QDialogButtonBox* mButtonBox;
	QTreeWidget* mTreeWidget;

	QHash<QTreeWidgetItem*,QAction*> mItemActions;

public:
	DiagramToolBox(DiagramView* diagramView);
	~DiagramToolBox();

	void addItem(DrawingItem* item, const QString& section, const QString& text,
		const QString& iconPath = "");
	QSize sizeHint() const;

private slots:
	void setDiagramModeFromAction(QAction* action);
	void triggerAction(QTreeWidgetItem* item, int column);
	void updateDiagramMode(DrawingView::Mode mode);

private:
	void showEvent(QShowEvent* event);

	void createActions();
	QDialogButtonBox* createButtonBox();
	QTreeWidget* createTreeWidget();

	QAction* createAction(const QString& text, const QString& iconPath = "",
		const QString& shortcut = "", const QVariant& data = QVariant());
	QToolButton* createButton(QAction* action);
};

#endif
