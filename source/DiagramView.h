/* DiagramView.h
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

#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include "DiagramProperties.h"

class DiagramScene;
class QPrinter;

class DiagramView : public DrawingView
{
	Q_OBJECT

	friend class DiagramItemToolBox;

public:
	enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction, DeleteAction,
		GroupAction, UngroupAction, SendBackwardAction, BringForwardAction, SendToBackAction, BringToFrontAction,
		InsertPointAction, RemovePointAction, ZoomInAction, ZoomOutAction, ZoomFitAction,
		PropertiesAction, NumberOfActions };

public:
	const static QVector<qreal> kZoomLevels;

private:
	DiagramScene* mScene;

	int mExportWidth;
	int mExportHeight;
	DrawingStyleOptions::ColorMode mExportMode;
	DrawingStyleOptions::RenderFlags mExportFlags;

	QPrinter* mPrinter;

	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDiagramContextMenu;

public:
	DiagramView();
	~DiagramView();

	DiagramScene* diagramScene() const;
	QPrinter* printer() const;

	void setDiagramProperties(const DiagramProperties& properties);
	DiagramProperties diagramProperties() const;

	int exportWidth() const;
	int exportHeight() const;
	DrawingStyleOptions::ColorMode exportMode() const;
	DrawingStyleOptions::RenderFlags exportFlags() const;

public slots:
	void zoomIn();
	void zoomOut();

	bool save(const QString& filePath);
	bool load(const QString& filePath);
	void clear();

	bool properties();

	void updateItemProperty(const QString& name, const QVariant& value);
	void updateDefaultItemProperties(const QHash<QString,QVariant>& properties);

	void exportPng(const QString& filePath, const QSize& size, const DrawingStyleOptions& options);
	void exportSvg(const QString& filePath, const QSize& size, const DrawingStyleOptions& options);
	void printPages(QPrinter* printer);

signals:
	void propertiesChanged();

private slots:
	void updatePrinter();

private:
	void contextMenuEvent(QContextMenuEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

	void addActions();
	void createContextMenus();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
