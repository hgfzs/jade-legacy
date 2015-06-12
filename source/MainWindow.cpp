/* MainWindow.cpp
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

#include "MainWindow.h"
#include "DiagramView.h"
#include "AboutDialog.h"
#include "DiagramToolBox.h"
#include "DiagramToolBar.h"
#include "ExportOptionsDialog.h"
#include "PreferencesDialog.h"
#include <QtPrintSupport>

//#define RELEASE_BUILD
#undef RELEASE_BUILD

MainWindow::MainWindow(const QString& filePath) : QMainWindow()
{
	mDiagramView = new DiagramView();
	mStackedWidget = new QStackedWidget();
	mStackedWidget->addWidget(new QWidget());
	mStackedWidget->addWidget(mDiagramView);
	setCentralWidget(mStackedWidget);

	mBaseWindowTitle = "Jade";
	mFileFilter = "Jade Diagrams (*.jdm);;All Files (*)";
	mFileSuffix = "jdm";
	mPromptCloseUnsaved = true;
	mPromptOverwrite = true;

	mNewDiagramCount = 0;
#ifndef WIN32
	mWorkingDir = QDir::home();
#endif

	createActions();
	createMenus();
	createToolBars();
	createDockWidget();
	createStatusBar();
	registerItems();

	setWindowIcon(QIcon(":/icons/diagram.png"));
	resize(1024, 768);

	connect(this, SIGNAL(filePathChanged(const QString&)), this, SLOT(updateWindowTitle(const QString&)));
	connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(updateActions(bool)));

	loadPreferences();

	if (!filePath.isEmpty() && mDiagramView->load(filePath))
	{
		setFilePath(filePath);
		setDiagramVisible(true);
	}
	else newDiagram();
}

MainWindow::~MainWindow() { }

//==================================================================================================

void MainWindow::setFilePath(const QString& path)
{
	mFilePath = path;
	emit filePathChanged(mFilePath);
}

QString MainWindow::filePath() const
{
	return mFilePath;
}

//==================================================================================================

void MainWindow::setDiagramVisible(bool visible)
{
	mStackedWidget->setCurrentIndex(visible ? 1 : 0);
	emit visibilityChanged(visible);
}

bool MainWindow::isDiagramVisible() const
{
	return (mStackedWidget->currentIndex() == 1);
}

//==================================================================================================

bool MainWindow::newDiagram()
{
	bool diagramCreated = false;

	if (closeDiagram())
	{
		diagramCreated = true;
		mNewDiagramCount++;

		setFilePath("Untitled " + QString::number(mNewDiagramCount));
		setDiagramVisible(true);
		mDiagramView->zoomFit();
	}

	return diagramCreated;
}

bool MainWindow::openDiagram()
{
	bool drawingOpened = false;

	if (mDiagramView)
	{
		QString filePath = mWorkingDir.path();
		QFileDialog::Options options = (mPromptOverwrite) ? 0 : QFileDialog::DontConfirmOverwrite;

		filePath = QFileDialog::getOpenFileName(this, "Open File", filePath, mFileFilter, nullptr, options);
		if (!filePath.isEmpty())
		{
			QFileInfo fileInfo(filePath);
			mWorkingDir = fileInfo.dir();

			drawingOpened = mDiagramView->load(filePath);
			if (drawingOpened)
			{
				setFilePath(filePath);
				setDiagramVisible(true);
				mDiagramView->zoomFit();
			}
			else
			{
				QMessageBox::critical(this, "Error Reading File",
					"File could not be read. Please ensure that this file is a valid Jade diagram.");

				setDiagramVisible(false);
				setFilePath("");
				mDiagramView->clear();
			}
		}
	}

	return drawingOpened;
}

bool MainWindow::saveDiagram()
{
	bool drawingSaved = false;

	if (mDiagramView && isDiagramVisible())
	{
		if (!mFilePath.startsWith("Untitled"))
		{
			drawingSaved = mDiagramView->save(mFilePath);
			if (!drawingSaved)
			{
				QMessageBox::critical(this, "Error Saving File",
					"Unable to open " + mFilePath + "for saving.  File not saved!");
			}
		}

		else drawingSaved = saveDiagramAs();
	}

	return drawingSaved;
}

bool MainWindow::saveDiagramAs()
{
	bool drawingSaved = false;

	if (mDiagramView && isDiagramVisible())
	{
		QString filePath = (mFilePath.startsWith("Untitled")) ? mWorkingDir.path() : mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? 0 : QFileDialog::DontConfirmOverwrite;

		filePath = QFileDialog::getSaveFileName(this, "Save File", filePath, mFileFilter, nullptr, options);
		if (!filePath.isEmpty())
		{
			QFileInfo fileInfo(filePath);
			mWorkingDir = fileInfo.dir();

			if (!filePath.endsWith("." + mFileSuffix, Qt::CaseInsensitive))
				filePath += "." + mFileSuffix;

			drawingSaved = mDiagramView->save(filePath);
			if (drawingSaved) setFilePath(filePath);
		}
	}

	return drawingSaved;
}

bool MainWindow::closeDiagram()
{
	bool drawingClosed = true;

	if (mDiagramView && isDiagramVisible())
	{
		QMessageBox::StandardButton button = QMessageBox::Yes;

		if (mPromptCloseUnsaved && !mDiagramView->isClean())
		{
			QFileInfo fileInfo(mFilePath);

			button = QMessageBox::question(this, "Save Changes",
				"Save changes to " + fileInfo.fileName() + " before closing?",
				QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

			if (button == QMessageBox::Yes)
			{
				if (mFilePath.startsWith("Untitled"))
				{
					if (!saveDiagramAs()) button = QMessageBox::Cancel;
				}
				else saveDiagram();
			}
		}

		drawingClosed = (button != QMessageBox::Cancel);
		if (drawingClosed)
		{
			setDiagramVisible(false);
			setFilePath("");
			mDiagramView->clear();
		}
	}

	return drawingClosed;
}

//==================================================================================================

void MainWindow::exportPng()
{
	if (isDiagramVisible())
	{
		QString filePath;
		QFileDialog::Options options = (mPromptOverwrite) ? 0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".png";

		filePath = QFileDialog::getSaveFileName(this, "Export PNG", filePath, "Portable Network Graphics (*.png);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			ExportOptionsDialog exportDialog(mDiagramView);
			exportDialog.setPrevious(mDiagramView->exportWidth(), mDiagramView->exportHeight(),
				mDiagramView->exportMode(), mDiagramView->exportFlags());

			if (exportDialog.exec() == QDialog::Accepted)
			{
				DrawingStyleOptions exportOptions = mDiagramView->styleOptions();
				exportOptions.setColorMode(exportDialog.renderMode());
				exportOptions.setRenderFlags(exportDialog.renderFlags());

				if (!filePath.endsWith(".png", Qt::CaseInsensitive)) filePath += ".png";

				mDiagramView->deselectAll();
				mDiagramView->exportPng(filePath, exportDialog.exportSize(), exportOptions);
			}
		}
	}
}

void MainWindow::exportSvg()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? 0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".svg";

		filePath = QFileDialog::getSaveFileName(this, "Export SVG", filePath, "Scalable Vector Graphics (*.svg);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			ExportOptionsDialog exportDialog(mDiagramView);
			exportDialog.setPrevious(mDiagramView->exportWidth(), mDiagramView->exportHeight(),
				mDiagramView->exportMode(), mDiagramView->exportFlags());

			if (exportDialog.exec() == QDialog::Accepted)
			{
				DrawingStyleOptions exportOptions = mDiagramView->styleOptions();
				exportOptions.setColorMode(exportDialog.renderMode());
				exportOptions.setRenderFlags(exportDialog.renderFlags());

				if (!filePath.endsWith(".svg", Qt::CaseInsensitive)) filePath += ".svg";

				mDiagramView->deselectAll();
				mDiagramView->exportSvg(filePath, exportDialog.exportSize(), exportOptions);
			}
		}
	}
}

//==================================================================================================

void MainWindow::printPreview()
{
	if (isDiagramVisible())
	{
		QPrintPreviewDialog printPreviewDialog(mDiagramView->printer(), this);
		connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), mDiagramView, SLOT(printPages(QPrinter*)));

		mDiagramView->deselectAll();
		printPreviewDialog.exec();
	}
}

void MainWindow::printDiagram()
{
	if (isDiagramVisible())
	{
		QPrintDialog printDialog(mDiagramView->printer(), this);
		printDialog.setEnabledOptions(QAbstractPrintDialog::PrintShowPageSize);

		mDiagramView->deselectAll();

		if (printDialog.exec() == QDialog::Accepted)
		{
			for(int i = 0; i < mDiagramView->printer()->numCopies(); i++)
				mDiagramView->printPages(mDiagramView->printer());
		}
	}
}

void MainWindow::printPdf()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? 0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".pdf";

		filePath = QFileDialog::getSaveFileName(this, "Print to PDF", filePath, "Portable Document Format (*.pdf);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) filePath += ".pdf";

			mDiagramView->deselectAll();

			mDiagramView->printer()->setOutputFileName(filePath);
			mDiagramView->printPages(mDiagramView->printer());
			mDiagramView->printer()->setOutputFileName("");
		}
	}
}

//==================================================================================================

void MainWindow::preferences()
{
	PreferencesDialog dialog(this);
	dialog.setPrompts(mPromptCloseUnsaved, mPromptOverwrite);
	dialog.setDiagramProperties(mDefaultProperties);

	if (dialog.exec() == QDialog::Accepted)
	{
		mPromptCloseUnsaved = dialog.shouldPromptOnClosingUnsaved();
		mPromptOverwrite = dialog.shouldPromptOnOverwrite();
		mDefaultProperties = dialog.diagramProperties();
	}
}

void MainWindow::about()
{
	AboutDialog dialog(this);
	dialog.exec();
}

//==================================================================================================

void MainWindow::savePreferences()
{
#ifdef RELEASE_BUILD
#ifdef WIN32
	QString configPath("config.ini");
#else
	QString configPath(QDir::home().absoluteFilePath(".jade/config.ini"));
#endif
#else
	QString configPath("config.ini");
#endif

	QSettings settings(configPath, QSettings::IniFormat);

	settings.beginGroup("Window");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.endGroup();

	settings.beginGroup("Prompts");
	settings.setValue("promptOnClosingUnsaved", mPromptCloseUnsaved);
	settings.setValue("promptOnOverwrite", mPromptOverwrite);
	settings.endGroup();

	settings.beginGroup("DiagramDefaults");
	mDefaultProperties.save(settings);
	settings.endGroup();

	settings.beginGroup("ItemDefaults");
	mPenBrushToolBar->save(settings);
	mTextToolBar->save(settings);
	settings.endGroup();

	settings.beginGroup("Recent");
	settings.setValue("workingDir", mWorkingDir.absolutePath());
	settings.endGroup();
}

void MainWindow::loadPreferences()
{
#ifdef RELEASE_BUILD
#ifdef WIN32
	QString configPath("config.ini");
#else
	QString configPath(QDir::home().absoluteFilePath(".jade/config.ini"));
#endif
#else
	QString configPath("config.ini");
#endif

	QSettings settings(configPath, QSettings::IniFormat);

	if (!settings.allKeys().isEmpty())
	{
		settings.beginGroup("Window");
		restoreGeometry(settings.value("geometry", QVariant()).toByteArray());
		restoreState(settings.value("state", QVariant()).toByteArray());
		settings.endGroup();

		settings.beginGroup("Prompts");
		mPromptCloseUnsaved = settings.value("promptOnClosingUnsaved", QVariant(true)).toBool();
		mPromptOverwrite = settings.value("promptOnOverwrite", QVariant(true)).toBool();
		settings.endGroup();

		settings.beginGroup("DiagramDefaults");
		mDefaultProperties.load(settings);
		settings.endGroup();

		settings.beginGroup("ItemDefaults");
		mPenBrushToolBar->load(settings);
		mTextToolBar->load(settings);
		settings.endGroup();

		settings.beginGroup("Recent");
		if (settings.contains("workingDir"))
		{
			QDir newDir(settings.value("workingDir").toString());
			if (newDir.exists()) mWorkingDir = newDir;
		}
		settings.endGroup();
	}
}

//==================================================================================================

void MainWindow::updateWindowTitle(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();

	setWindowTitle(fileName.isEmpty() ? mBaseWindowTitle : fileName + " - " + mBaseWindowTitle);
}

void MainWindow::updateActions(bool diagramVisible)
{
	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> diagramActions = mDiagramView->actions();

	while (!diagramActions.isEmpty())
		diagramActions.takeFirst()->setEnabled(diagramVisible);
	actions[ExportPngAction]->setEnabled(diagramVisible);
	actions[ExportSvgAction]->setEnabled(diagramVisible);
	actions[PrintPreviewAction]->setEnabled(diagramVisible);
	actions[PrintAction]->setEnabled(diagramVisible);
	actions[PrintPdfAction]->setEnabled(diagramVisible);
	mDiagramToolBox->setEnabled(diagramVisible);
	mEditToolBar->setEnabled(diagramVisible);
	mDiagramToolBar->setEnabled(diagramVisible);
	mPenBrushToolBar->setEnabled(diagramVisible);
	mTextToolBar->setEnabled(diagramVisible);

	updateWindowTitle(diagramVisible ? mFilePath : "");
}

//==================================================================================================

void MainWindow::updateMode(DrawingView::Mode mode)
{
	QString modeText = "Select Mode";

	switch (mode)
	{
	case DrawingView::ScrollMode: modeText = "Scroll Mode"; break;
	case DrawingView::ZoomMode: modeText = "Zoom Mode"; break;
	case DrawingView::NewMode: modeText = "Place Mode"; break;
	default: break;
	}

	mModeLabel->setText(modeText);
}

void MainWindow::updateClean(bool clean)
{
	mStatusLabel->setText((clean) ? "" : "Modified");
}

void MainWindow::updateNumberOfItems(int itemCount)
{
	mNumberOfItemsLabel->setText(QString::number(itemCount));
}

//==================================================================================================

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	if (!event->spontaneous()) mDiagramView->zoomFit();
	else if (!mWindowState.isEmpty()) restoreState(mWindowState);
}

void MainWindow::hideEvent(QHideEvent* event)
{
	QMainWindow::hideEvent(event);
	if (event->spontaneous()) mWindowState = saveState();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (closeDiagram())
	{
		savePreferences();
		event->accept();
	}
	else event->ignore();
}

//==================================================================================================

void MainWindow::registerItems()
{
	DiagramView::itemFactory.registerItem(new DrawingItemGroup());

	registerItem(new DrawingLineItem(), "Basic Items", "Line", ":/icons/oxygen/draw-line.png");
	registerItem(new DrawingArcItem(), "Basic Items", "Arc", ":/icons/oxygen/draw-arc.png");
	registerItem(new DrawingPolylineItem(), "Basic Items", "Polyline", ":/icons/oxygen/draw-polyline.png");
	registerItem(new DrawingCurveItem(), "Basic Items", "Curve", ":/icons/oxygen/draw-curve.png");
	registerItem(new DrawingRectItem(), "Basic Items", "Rect", ":/icons/oxygen/draw-rectangle.png");
	registerItem(new DrawingEllipseItem(), "Basic Items", "Ellipse", ":/icons/oxygen/draw-ellipse.png");
	registerItem(new DrawingPolygonItem(), "Basic Items", "Polygon", ":/icons/oxygen/draw-polygon.png");
	registerItem(new DrawingTextItem(), "Basic Items", "Text", ":/icons/oxygen/draw-text.png");
	registerItem(new DrawingPixmapItem(), "Basic Items", "Image", ":/icons/oxygen/draw-image.png");
	registerItem(new DrawingChartRectItem(), "Chart Items", "Chart Rect", ":/icons/items/chartbox.png");
	registerItem(new DrawingChartEllipseItem(), "Chart Items", "Chart Ellipse", ":/icons/items/chartellipse.png");
	registerItem(new DrawingChartPolygonItem(), "Chart Items", "Chart Polygon", ":/icons/items/chartpolygon.png");
	registerItem(new DrawingChartSumItem(), "Chart Items", "Chart Sum", ":/icons/items/chartsum.png");
	registerItem(new DrawingChartPlusItem(), "Chart Items", "Chart Plus", ":/icons/items/chartplus.png");

	// Register libraries
	addLibrary(":/lib/electric.jlb");
	addLibrary(":/lib/logic.jlb");
}

bool MainWindow::addLibrary(const QString& libraryPath)
{
	bool success = false;

	QFile libFile(libraryPath);
	if (libFile.open(QIODevice::ReadOnly))
	{
		QXmlStreamReader xmlReader(&libFile);
		QString libraryName;

		xmlReader.readNextStartElement();
		if (xmlReader.name() == "library")
		{
			DrawingPathItem* pathItem;
			QString pathItemIcon;

			QXmlStreamAttributes attributes = xmlReader.attributes();

			if (attributes.hasAttribute("name")) libraryName = attributes.value("name").toString();

			while (xmlReader.readNextStartElement())
			{
				if (xmlReader.name() == "item")
				{
					pathItem = readPathItem(xmlReader, pathItemIcon);

					if (pathItem->uniqueKey() != "path")
						registerItem(pathItem, libraryName, pathItem->uniqueKey(), pathItemIcon);
					else
						delete pathItem;
				}
				else xmlReader.skipCurrentElement();
			}
		}

		success = (!xmlReader.hasError() && !libraryName.isEmpty());
	}

	return success;
}

DrawingPathItem* MainWindow::readPathItem(QXmlStreamReader& xmlReader, QString& iconPath)
{
	DrawingPathItem* pathItem = new DrawingPathItem();

	QPainterPath initialPath;
	QPointF connectionPoint;

	QXmlStreamAttributes attributes = xmlReader.attributes();
	if (attributes.hasAttribute("name"))
		pathItem->setUniqueKey(attributes.value("name").toString());
	if (attributes.hasAttribute("icon"))
		iconPath = attributes.value("icon").toString();

	while (xmlReader.readNextStartElement())
	{
		if (xmlReader.name() == "units")
		{
			attributes = xmlReader.attributes();
			if (attributes.hasAttribute("value"))
				pathItem->setUnits((DrawingUnits)attributes.value("value").toString().toInt());
		}
		else if (xmlReader.name() == "path")
		{
			attributes = xmlReader.attributes();

			initialPath = QPainterPath();
			if (attributes.hasAttribute("d"))
				initialPath = Drawing::pathFromString(attributes.value("d").toString());

			if (!initialPath.isEmpty()) pathItem->setInitialPath(initialPath);

			xmlReader.skipCurrentElement();
		}
		else if (xmlReader.name() == "connectionPoint")
		{
			attributes = xmlReader.attributes();

			connectionPoint.setX((attributes.hasAttribute("x")) ?
				attributes.value("x").toString().toDouble() : 0.0);
			connectionPoint.setY((attributes.hasAttribute("y")) ?
				attributes.value("y").toString().toDouble() : 0.0);
			pathItem->addConnectionPoint(connectionPoint);

			xmlReader.skipCurrentElement();
		}
		else xmlReader.skipCurrentElement();
	}

	return pathItem;
}

void MainWindow::registerItem(DrawingItem* item, const QString& section, const QString& text, const QString& iconPath)
{
	DrawingView::itemFactory.registerItem(item);
	mDiagramToolBox->addItem(item, section, text, iconPath);
}

//==================================================================================================

void MainWindow::createActions()
{
	addAction("New...", this, SLOT(newDiagram()), ":/icons/oxygen/document-new.png", "Ctrl+N");
	addAction("Open...", this, SLOT(openDiagram()), ":/icons/oxygen/document-open.png", "Ctrl+O");
	addAction("Save", this, SLOT(saveDiagram()), ":/icons/oxygen/document-save.png", "Ctrl+S");
	addAction("Save As...", this, SLOT(saveDiagramAs()), ":/icons/oxygen/document-save-as.png", "Ctrl+Shift+S");
	addAction("Close", this, SLOT(closeDiagram()), ":/icons/oxygen/document-close.png", "Ctrl+W");
	addAction("Export PNG...", this, SLOT(exportPng()), ":/icons/oxygen/image-x-generic.png");
	addAction("Export SVG...", this, SLOT(exportSvg()), ":/icons/oxygen/image-svg+xml.png");
	addAction("Print Preview...", this, SLOT(printPreview()), ":/icons/oxygen/document-preview.png");
	addAction("Print...", this, SLOT(printDiagram()), ":/icons/oxygen/document-print.png", "Ctrl+P");
	addAction("Print to PDF...", this, SLOT(printPdf()), ":/icons/oxygen/application-pdf.png");
	addAction("Preferences...", this, SLOT(preferences()), ":/icons/oxygen/configure.png");
	addAction("Exit", this, SLOT(close()), ":/icons/oxygen/application-exit.png");

	addAction("About...", this, SLOT(about()), ":/icons/oxygen/help-about.png");
	addAction("About Qt...", qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
	QList<QAction*> lActions = actions();
	QList<QAction*> diagramActions = mDiagramView->actions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(lActions[NewAction]);
	menu->addAction(lActions[OpenAction]);
	menu->addSeparator();
	menu->addAction(lActions[SaveAction]);
	menu->addAction(lActions[SaveAsAction]);
	menu->addSeparator();
	menu->addAction(lActions[CloseAction]);
	menu->addSeparator();
	menu->addAction(lActions[ExportPngAction]);
	menu->addAction(lActions[ExportSvgAction]);
	menu->addSeparator();
	menu->addAction(lActions[PrintPreviewAction]);
	menu->addAction(lActions[PrintAction]);
	menu->addAction(lActions[PrintPdfAction]);
	menu->addSeparator();
	menu->addAction(lActions[PreferencesAction]);
	menu->addSeparator();
	menu->addAction(lActions[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(diagramActions[DiagramView::UndoAction]);
	menu->addAction(diagramActions[DiagramView::RedoAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramView::CutAction]);
	menu->addAction(diagramActions[DiagramView::CopyAction]);
	menu->addAction(diagramActions[DiagramView::PasteAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramView::SelectAllAction]);
	menu->addAction(diagramActions[DiagramView::SelectNoneAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(diagramActions[DiagramView::RotateAction]);
	menu->addAction(diagramActions[DiagramView::RotateBackAction]);
	menu->addAction(diagramActions[DiagramView::FlipAction]);
	menu->addAction(diagramActions[DiagramView::DeleteAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramView::GroupAction]);
	menu->addAction(diagramActions[DiagramView::UngroupAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramView::BringForwardAction]);
	menu->addAction(diagramActions[DiagramView::SendBackwardAction]);
	menu->addAction(diagramActions[DiagramView::BringToFrontAction]);
	menu->addAction(diagramActions[DiagramView::SendToBackAction]);

	menu = menuBar()->addMenu("Diagram");
	menu->addAction(diagramActions[DiagramView::PropertiesAction]);
	menu->addSeparator();
	menu->addAction(diagramActions[DiagramView::ZoomInAction]);
	menu->addAction(diagramActions[DiagramView::ZoomOutAction]);
	menu->addAction(diagramActions[DiagramView::ZoomFitAction]);

	menu = menuBar()->addMenu("Help");
	menu->addAction(lActions[AboutAction]);
	menu->addAction(lActions[AboutQtAction]);
}

void MainWindow::createToolBars()
{
	QList<QAction*> lActions = actions();
    int size = QFontMetrics(font()).height();

	mFileToolBar = new QToolBar("File");
	mFileToolBar->setObjectName("FileToolBar");
    mFileToolBar->setIconSize(QSize(size, size));
	mFileToolBar->addAction(lActions[NewAction]);
	mFileToolBar->addAction(lActions[OpenAction]);
	mFileToolBar->addAction(lActions[SaveAction]);
	mFileToolBar->addAction(lActions[CloseAction]);
	mFileToolBar->addAction(lActions[PrintPreviewAction]);
	mFileToolBar->addAction(lActions[PrintAction]);
	addToolBar(mFileToolBar);

	mEditToolBar = new EditToolBar(mDiagramView);
    mEditToolBar->setIconSize(QSize(size, size));
	addToolBar(mEditToolBar);

	mDiagramToolBar = new DiagramToolBar(mDiagramView);
    mDiagramToolBar->setIconSize(QSize(size, size));
	addToolBar(mDiagramToolBar);
	connect(mDiagramToolBar, SIGNAL(zoomChanged(qreal)), mDiagramView, SLOT(zoomToLevel(qreal)));

	addToolBarBreak();

	mPenBrushToolBar = new PenBrushToolBar(mDiagramView);
    mPenBrushToolBar->setIconSize(QSize(size, size));
	addToolBar(mPenBrushToolBar);

	mTextToolBar = new TextToolBar(mDiagramView);
    mTextToolBar->setIconSize(QSize(size, size));
	addToolBar(mTextToolBar);
}

void MainWindow::createDockWidget()
{
	mDiagramToolBox = new DiagramToolBox(mDiagramView);

	mToolBoxDock = new QDockWidget("Tools");
	mToolBoxDock->setObjectName("Tools");
	mToolBoxDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mToolBoxDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mToolBoxDock->setWidget(mDiagramToolBox);
	addDockWidget(Qt::LeftDockWidgetArea, mToolBoxDock, Qt::Vertical);
}

void MainWindow::createStatusBar()
{
	mModeLabel = new QLabel("Select Mode");
	mStatusLabel = new QLabel("");
	mNumberOfItemsLabel = new QLabel("0");
	mMouseLabel = new QLabel("");

	mModeLabel->setMinimumWidth(120);
	mStatusLabel->setMinimumWidth(120);
	mNumberOfItemsLabel->setMinimumWidth(80);
	mMouseLabel->setMinimumWidth(120);

	statusBar()->addWidget(mModeLabel);
	statusBar()->addWidget(mStatusLabel);
	statusBar()->addWidget(mNumberOfItemsLabel);
	statusBar()->addWidget(mMouseLabel);

	connect(mDiagramView, SIGNAL(modeChanged(DrawingView::Mode)), this, SLOT(updateMode(DrawingView::Mode)));
	connect(mDiagramView, SIGNAL(cleanChanged(bool)), this, SLOT(updateClean(bool)));
	connect(mDiagramView, SIGNAL(numberOfItemsChanged(int)), this, SLOT(updateNumberOfItems(int)));
	connect(mDiagramView, SIGNAL(mousePositionChanged(const QString&)), mMouseLabel, SLOT(setText(const QString&)));
}

QAction* MainWindow::addAction(const QString& text, QObject* slotObj,
	const char* slotFunction, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QMainWindow::addAction(action);
	return action;
}
