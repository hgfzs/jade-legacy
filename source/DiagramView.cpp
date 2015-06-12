/* DiagramView.cpp
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

#include "DiagramView.h"
#include "DiagramScene.h"
#include "DiagramPropertiesWidget.h"
#include <QtPrintSupport>
#include <QtSvg>

const QVector<qreal> DiagramView::kZoomLevels = QVector<qreal>() << 0.1 << 0.25 << 0.33 << 0.5 <<
	0.67 << 0.75 << 1.0 << 1.5 << 2.0 << 3.0 << 4.0 << 6.0 << 8.0 << 10.0 << 12.0 << 16.0;

DiagramView::DiagramView() : DrawingView()
{
	mScene = new DiagramScene();
	setScene(mScene);

	mExportWidth = 0;
	mExportHeight = 0;
	mExportMode = DrawingStyleOptions::Normal;
	mExportFlags = DrawingStyleOptions::DrawBackground;

	mPrinter = new QPrinter();
	mPrinter->setResolution(600);

	addActions();
	createContextMenus();

	connect(this, SIGNAL(propertiesChanged()), this, SLOT(updatePrinter()));
}

DiagramView::~DiagramView() { }

//==================================================================================================

DiagramScene* DiagramView::diagramScene() const
{
	return mScene;
}

QPrinter* DiagramView::printer() const
{
	return mPrinter;
}

//==================================================================================================

void DiagramView::setDiagramProperties(const DiagramProperties& properties)
{
	mScene->updateUnits(properties.units());
	mScene->setSceneRect(properties.sceneRect());
	mScene->setContentsRect(properties.contentsRect());
	mScene->setGrid(properties.gridSize());
	mScene->setBorderWidth(properties.borderWidth());

	DrawingStyleOptions styleOptions = DiagramView::styleOptions();
	styleOptions.setBrush(DrawingStyleOptions::Background, QBrush(properties.backgroundColor()));
	styleOptions.setGridStyle(properties.gridStyle());
	styleOptions.setGridSpacing(properties.gridMajorSpacing(), properties.gridMinorSpacing());
	styleOptions.setBrush(DrawingStyleOptions::Grid, QBrush(properties.gridColor()));
	styleOptions.setBrush(DrawingStyleOptions::Border, QBrush(properties.borderColor()));
	setStyleOptions(styleOptions);

	mExportWidth = 0;
	mExportHeight = 0;

	emit propertiesChanged();
}

DiagramProperties DiagramView::diagramProperties() const
{
	DiagramProperties properties;

	properties.setUnits(mScene->units());
	properties.setSceneRect(mScene->sceneRect());
	properties.setContentsRect(mScene->contentsRect());
	properties.setGridSize(mScene->grid());
	properties.setBorderWidth(mScene->borderWidth());

	DrawingStyleOptions styleOptions = DiagramView::styleOptions();
	properties.setBackgroundColor(styleOptions.brush(DrawingStyleOptions::Background).color());
	properties.setGridStyle(styleOptions.gridStyle(), styleOptions.majorGridSpacing(), styleOptions.minorGridSpacing());
	properties.setGridColor(styleOptions.brush(DrawingStyleOptions::Grid).color());
	properties.setBorderColor(styleOptions.brush(DrawingStyleOptions::Border).color());

	return properties;
}

//==================================================================================================

int DiagramView::exportWidth() const
{
	return mExportWidth;
}

int DiagramView::exportHeight() const
{
	return mExportHeight;
}

DrawingStyleOptions::ColorMode DiagramView::exportMode() const
{
	return mExportMode;
}

DrawingStyleOptions::RenderFlags DiagramView::exportFlags() const
{
	return mExportFlags;
}

//==================================================================================================

void DiagramView::zoomIn()
{
	int zoomLevelIndex = 0;

	while (zoomLevelIndex < kZoomLevels.size() && zoomLevel() >= kZoomLevels[zoomLevelIndex])
		zoomLevelIndex++;
	if (zoomLevelIndex >= kZoomLevels.size()) zoomLevelIndex = kZoomLevels.size() - 1;

	zoomToLevel(kZoomLevels[zoomLevelIndex], true);
}

void DiagramView::zoomOut()
{
	int zoomLevelIndex = kZoomLevels.size() - 1;

	while (zoomLevelIndex >= 0 && zoomLevel() <= kZoomLevels[zoomLevelIndex])
		zoomLevelIndex--;
	if (zoomLevelIndex < 0) zoomLevelIndex = 0;

	zoomToLevel(kZoomLevels[zoomLevelIndex], true);
}

//==================================================================================================

bool DiagramView::save(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::WriteOnly);
	if (!fileError)
	{
		QXmlStreamWriter xmlWriter(&dataFile);
		xmlWriter.setAutoFormatting(true);
		xmlWriter.setAutoFormattingIndent(-1);

		xmlWriter.writeStartDocument();

		xmlWriter.writeStartElement("diagram");
		writeXmlAttributes(xmlWriter);
		writeXmlChildElements(xmlWriter);
		xmlWriter.writeEndElement();

		xmlWriter.writeEndDocument();
		dataFile.close();

		setClean();
		update();
	}

	return (!fileError);
}

bool DiagramView::load(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::ReadOnly);
	if (!fileError)
	{
		QXmlStreamReader xmlReader(&dataFile);

		clear();

		xmlReader.readNextStartElement();
		if (xmlReader.name() == "diagram")
		{
			readXmlAttributes(xmlReader);

			while (xmlReader.readNextStartElement())
				readXmlChildElement(xmlReader);
		}
		else fileError = true;

		dataFile.close();

		setClean();
		update();
	}

	return (!fileError);
}

void DiagramView::clear()
{
	setDefaultMode();
	mScene->clearItems();
}

//==================================================================================================

bool DiagramView::properties()
{
	bool dialogAccepted = false;

	if (mode() == DefaultMode)
	{
		if (mScene->numberOfSelectedItems() == 1)
			dialogAccepted = mScene->singleItemProperties(mScene->selectedItems().first());
		else if (mScene->numberOfSelectedItems() > 1)
			dialogAccepted = mScene->multipleItemProperties(mScene->selectedItems());
		else
		{
			DiagramPropertiesDialog* propertiesDialog = new DiagramPropertiesDialog(this);
			propertiesDialog->setProperties(diagramProperties());

			if (propertiesDialog->exec() == QDialog::Accepted)
			{
				mScene->updateProperties(propertiesDialog->properties());

				updateScrollBars();
				emit mousePositionChanged("");
				emit numberOfItemsChanged(mScene->numberOfItems());
				dialogAccepted = true;
			}
		}

		update();
	}

	return dialogAccepted;
}

//==================================================================================================

void DiagramView::updateItemProperty(const QString& name, const QVariant& value)
{
	QList<DrawingItem*> items = mScene->selectedItems();
	if (!items.isEmpty())
	{
		QHash<QString,QVariant> properties;
		properties[name] = value;
		mScene->updateItemProperties(items, properties);
		update();
	}
}

void DiagramView::updateDefaultItemProperties(const QHash<QString,QVariant>& properties)
{
	QHash<QString,QVariant> itemProperties = itemFactory.defaultProperties();

	for(auto keyIter = properties.begin(); keyIter != properties.end(); keyIter++)
		itemProperties[keyIter.key()] = keyIter.value();

	itemFactory.setDefaultProperties(itemProperties);
}

//==================================================================================================

void DiagramView::exportPng(const QString& filePath, const QSize& size, const DrawingStyleOptions& options)
{
	QImage pngImage(size, QImage::Format_ARGB32);

	QPainter painter;
	QRectF visibleRect = mScene->sceneRect();

	painter.begin(&pngImage);
	painter.scale(pngImage.width() / visibleRect.width(), pngImage.height() / visibleRect.height());
	painter.translate(-visibleRect.left(), -visibleRect.top());
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
	render(&painter, options, visibleRect);
	painter.end();

	pngImage.save(filePath, "PNG");

	mExportWidth = size.width();
	mExportHeight = size.height();
	mExportMode = options.colorMode();
	mExportFlags = options.renderFlags();
}

void DiagramView::exportSvg(const QString& filePath, const QSize& size, const DrawingStyleOptions& options)
{
	QSvgGenerator svgImage;

	QPainter painter;
	QRectF visibleRect = mScene->sceneRect();

	svgImage.setFileName(filePath);
	svgImage.setSize(size);
	svgImage.setViewBox(QRect(QPoint(0, 0), size));

	painter.begin(&svgImage);
	painter.scale(svgImage.size().width() / visibleRect.width(), svgImage.size().height() / visibleRect.height());
	painter.translate(-visibleRect.left(), -visibleRect.top());
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
	render(&painter, options, visibleRect);
	painter.end();

	mExportWidth = size.width();
	mExportHeight = size.height();
	mExportMode = options.colorMode();
	mExportFlags = options.renderFlags();
}

void DiagramView::printPages(QPrinter* printer)
{
	if (printer)
	{
		QPainter painter;
		QRectF visibleRect = mScene->sceneRect();
		qreal pageAspect, scale;

		DrawingStyleOptions printOptions = styleOptions();
		printOptions.setRenderFlags(DrawingStyleOptions::DrawBorder);

		pageAspect = printer->pageRect().width() / (qreal)printer->pageRect().height();
		scale = qMin(printer->pageRect().width() / visibleRect.width(),
			printer->pageRect().height() / visibleRect.height());

		if (visibleRect.height() * pageAspect > visibleRect.width())
		{
			visibleRect.adjust(-(visibleRect.height() * pageAspect - visibleRect.width()) / 2, 0,
				(visibleRect.height() * pageAspect - visibleRect.width()) / 2, 0);
		}
		else if (visibleRect.width() / pageAspect > visibleRect.height())
		{
			visibleRect.adjust(0, -(visibleRect.width() / pageAspect - visibleRect.height()) / 2,
				0, (visibleRect.width() / pageAspect - visibleRect.height()) / 2);
		}

		painter.begin(printer);
		painter.scale(scale, scale);
		painter.translate(-visibleRect.left(), -visibleRect.top());
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

		render(&painter, printOptions, visibleRect);

		painter.end();
	}
}

//==================================================================================================

void DiagramView::updatePrinter()
{
	QRectF sceneRect = mScene->sceneRect();
	QRectF contentsRect = mScene->contentsRect();

	if (mScene->units() == UnitsMils)
	{
		mPrinter->setPaperSize(QSizeF(sceneRect.width() / 1000.0, sceneRect.height() / 1000), QPrinter::Inch);
		mPrinter->setPageMargins((contentsRect.left() - sceneRect.left()) / 1000,
			(contentsRect.top() - sceneRect.top()) / 1000,
			(sceneRect.right() - contentsRect.right()) / 1000,
			(sceneRect.bottom() - contentsRect.bottom()) / 1000, QPrinter::Inch);
	}
	else
	{
		mPrinter->setPaperSize(sceneRect.size(), QPrinter::Millimeter);
		mPrinter->setPageMargins((contentsRect.left() - sceneRect.left()),
			(contentsRect.top() - sceneRect.top()),
			(sceneRect.right() - contentsRect.right()),
			(sceneRect.bottom() - contentsRect.bottom()), QPrinter::Millimeter);
	}
}

//==================================================================================================

void DiagramView::contextMenuEvent(QContextMenuEvent* event)
{
	DrawingView::contextMenuEvent(event);

	if (!event->isAccepted() && mScene)
	{
		DrawingItem* mouseDownItem = (mScene) ? mScene->mouseDownItem() : nullptr;
		DrawingMouseEvent mouseEvent;
		if (mScene) mouseEvent = mScene->lastMouseEvent();

		if (mouseDownItem && mouseDownItem->isSelected() && mScene->numberOfSelectedItems() == 1)
		{
			if (mouseDownItem->canInsertRemovePoints())
				mSinglePolyItemContextMenu.popup(mapToGlobal(mouseEvent.screenPos()) + QPoint(2, 2));
			else
				mSingleItemContextMenu.popup(mapToGlobal(mouseEvent.screenPos()) + QPoint(2, 2));
		}
		else if (mouseDownItem && mouseDownItem->isSelected())
		{
			mMultipleItemContextMenu.popup(mapToGlobal(mouseEvent.screenPos()) + QPoint(2, 2));
		}
		else
		{
			if (mouseDownItem == nullptr) deselectAll();
			mDiagramContextMenu.popup(mapToGlobal(mouseEvent.screenPos()) + QPoint(2, 2));
		}

		event->accept();
	}

	update();
}

void DiagramView::mouseDoubleClickEvent(QMouseEvent* event)
{
	DrawingView::mouseDoubleClickEvent(event);

	if (!event->isAccepted())
	{
		properties();
		event->accept();
	}

	update();
}

//==================================================================================================

void DiagramView::addActions()
{
	addAction("Undo", this, SLOT(undo()), ":/icons/oxygen/edit-undo.png", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), ":/icons/oxygen/edit-redo.png", "Ctrl+Shift+Z");
	addAction("Cut", this, SLOT(cut()), ":/icons/oxygen/edit-cut.png", "Ctrl+X");
	addAction("Copy", this, SLOT(copy()), ":/icons/oxygen/edit-copy.png", "Ctrl+C");
	addAction("Paste", this, SLOT(paste()), ":/icons/oxygen/edit-paste.png", "Ctrl+V");
	addAction("Select All", this, SLOT(selectAll()), ":/icons/oxygen/edit-select-all.png", "Ctrl+A");
	addAction("Select None", this, SLOT(deselectAll()), "", "Ctrl+Shift+A");

	addAction("Rotate", this, SLOT(rotateSelection()), ":/icons/oxygen/object-rotate-right.png", "R");
	addAction("Rotate Back", this, SLOT(rotateBackSelection()), ":/icons/oxygen/object-rotate-left.png", "Shift+R");
	addAction("Flip", this, SLOT(flipSelection()), ":/icons/oxygen/object-flip-horizontal.png", "F");
	addAction("Delete", this, SLOT(deleteSelection()), ":/icons/oxygen/edit-delete.png", "Delete");

	addAction("Group", this, SLOT(group()), ":/icons/oxygen/merge.png", "Ctrl+G");
	addAction("Ungroup", this, SLOT(ungroup()), ":/icons/oxygen/split.png", "Ctrl+Shift+G");

	addAction("Send Backward", this, SLOT(sendBackward()), ":/icons/oxygen/go-down.png");
	addAction("Bring Forward", this, SLOT(bringForward()), ":/icons/oxygen/go-up.png");
	addAction("Send to Back", this, SLOT(sendToBack()), ":/icons/oxygen/go-bottom.png");
	addAction("Bring to Front", this, SLOT(bringToFront()), ":/icons/oxygen/go-top.png");

	addAction("Insert Point", this, SLOT(insertItemPoint()), "");
	addAction("Remove Point", this, SLOT(removeItemPoint()), "");

	addAction("Zoom In", this, SLOT(zoomIn()), ":/icons/oxygen/zoom-in.png", ".");
	addAction("Zoom Out", this, SLOT(zoomOut()), ":/icons/oxygen/zoom-out.png", ",");
	addAction("Zoom Fit", this, SLOT(zoomFit()), ":/icons/oxygen/zoom-fit-best.png", "/");

	addAction("Properties...", this, SLOT(properties()), ":/icons/oxygen/games-config-board.png");
}

void DiagramView::createContextMenus()
{
	QList<QAction*> actions = DiagramView::actions();

	mSingleItemContextMenu.addAction(actions[PropertiesAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[RotateAction]);
	mSingleItemContextMenu.addAction(actions[RotateBackAction]);
	mSingleItemContextMenu.addAction(actions[FlipAction]);
	mSingleItemContextMenu.addAction(actions[DeleteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[CutAction]);
	mSingleItemContextMenu.addAction(actions[CopyAction]);
	mSingleItemContextMenu.addAction(actions[PasteAction]);

	mSinglePolyItemContextMenu.addAction(actions[PropertiesAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[InsertPointAction]);
	mSinglePolyItemContextMenu.addAction(actions[RemovePointAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[RotateAction]);
	mSinglePolyItemContextMenu.addAction(actions[RotateBackAction]);
	mSinglePolyItemContextMenu.addAction(actions[FlipAction]);
	mSinglePolyItemContextMenu.addAction(actions[DeleteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[CutAction]);
	mSinglePolyItemContextMenu.addAction(actions[CopyAction]);
	mSinglePolyItemContextMenu.addAction(actions[PasteAction]);

	mMultipleItemContextMenu.addAction(actions[PropertiesAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[RotateAction]);
	mMultipleItemContextMenu.addAction(actions[RotateBackAction]);
	mMultipleItemContextMenu.addAction(actions[FlipAction]);
	mMultipleItemContextMenu.addAction(actions[DeleteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[GroupAction]);
	mMultipleItemContextMenu.addAction(actions[UngroupAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[CutAction]);
	mMultipleItemContextMenu.addAction(actions[CopyAction]);
	mMultipleItemContextMenu.addAction(actions[PasteAction]);

	mDiagramContextMenu.addAction(actions[PropertiesAction]);
	mDiagramContextMenu.addSeparator();
	mDiagramContextMenu.addAction(actions[UndoAction]);
	mDiagramContextMenu.addAction(actions[RedoAction]);
	mDiagramContextMenu.addSeparator();
	mDiagramContextMenu.addAction(actions[CutAction]);
	mDiagramContextMenu.addAction(actions[CopyAction]);
	mDiagramContextMenu.addAction(actions[PasteAction]);
	mDiagramContextMenu.addSeparator();
	mDiagramContextMenu.addAction(actions[ZoomInAction]);
	mDiagramContextMenu.addAction(actions[ZoomOutAction]);
	mDiagramContextMenu.addAction(actions[ZoomFitAction]);
}

QAction* DiagramView::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	if (slotObj) connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);
	return action;
}
