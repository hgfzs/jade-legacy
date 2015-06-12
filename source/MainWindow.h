/* MainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DiagramProperties.h"

class DiagramView;
class DiagramToolBox;
class EditToolBar;
class DiagramToolBar;
class PenBrushToolBar;
class TextToolBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	enum ActionIndex { NewAction, OpenAction, SaveAction, SaveAsAction, CloseAction,
		ExportPngAction, ExportSvgAction, PrintPreviewAction, PrintAction, PrintPdfAction,
		PreferencesAction, ExitAction, AboutAction, AboutQtAction, NumberOfActions };

private:
	DiagramView* mDiagramView;
	QStackedWidget* mStackedWidget;

	QToolBar* mFileToolBar;
	EditToolBar* mEditToolBar;
	DiagramToolBar* mDiagramToolBar;
	PenBrushToolBar* mPenBrushToolBar;
	TextToolBar* mTextToolBar;

	QDockWidget* mToolBoxDock;
	DiagramToolBox* mDiagramToolBox;

	QLabel* mModeLabel;
	QLabel* mStatusLabel;
	QLabel* mNumberOfItemsLabel;
	QLabel* mMouseLabel;

	DiagramProperties mDefaultProperties;

	QString mFilePath;
	QString mFileFilter;
	QString mFileSuffix;

	QString mBaseWindowTitle;

	bool mPromptCloseUnsaved;
	bool mPromptOverwrite;

	int mNewDiagramCount;
	QDir mWorkingDir;
	QByteArray mWindowState;

public:
	MainWindow(const QString& filePath = QString());
	~MainWindow();

	void setFilePath(const QString& filePath);
	QString filePath() const;

	void setDiagramVisible(bool visible);
	bool isDiagramVisible() const;

public slots:
	bool newDiagram();
	bool openDiagram();
	bool saveDiagram();
	bool saveDiagramAs();
	bool closeDiagram();

	void exportPng();
	void exportSvg();

	void printPreview();
	void printDiagram();
	void printPdf();

	void preferences();
	void about();

	void savePreferences();
	void loadPreferences();

signals:
	void filePathChanged(const QString& filePath);
	void visibilityChanged(bool visible);

private slots:
	void updateWindowTitle(const QString& filePath);
	void updateActions(bool diagramVisible);

	void updateMode(DrawingView::Mode mode);
	void updateClean(bool clean);
	void updateNumberOfItems(int itemCount);

private:
	void showEvent(QShowEvent* event);
	void hideEvent(QHideEvent* event);
	void closeEvent(QCloseEvent* event);

	void registerItems();
	bool addLibrary(const QString& libraryPath);
	DrawingPathItem* readPathItem(QXmlStreamReader& xmlReader, QString& iconPath);
	void registerItem(DrawingItem* item, const QString& section, const QString& text,
		const QString& iconPath);

	void createActions();
	void createMenus();
	void createToolBars();
	void createDockWidget();
	void createStatusBar();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
