/* DrawingScene.cpp
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

#include <DrawingScene.h>
#include <DrawingView.h>
#include <DrawingItem.h>
#include <DrawingItemGroup.h>
#include <DrawingItemPoint.h>
#include <DrawingUndo.h>

DrawingScene::DrawingScene() : QObject()
{
	mView = nullptr;

	mUnits = UnitsMils;
	mSceneRect.setRect(-500, -500, 11000, 8500);
	mContentsRect.setRect(0, 0, 10000, 7500);
	mBorderWidth = 24;
	mGrid = 100;

	mSelectionMode = Qt::ContainsItemBoundingRect;
	mForcingItemsInside = true;

	mNewItem = nullptr;

	mMouseState = MouseReady;
	mMouseDownItem = nullptr;
	mNewClickCount = 0;
	mConsecutivePastes = 0;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(updateSelectionCenter()));
}

DrawingScene::~DrawingScene()
{
	clearItems();
	mNewItem = nullptr;
	mMouseDownItem = nullptr;
	mView = nullptr;
}

DrawingView* DrawingScene::view() const
{
	return mView;
}

//==================================================================================================

void DrawingScene::setUnits(DrawingUnits units)
{
	mUnits = units;
}

void DrawingScene::updateUnits(DrawingUnits units)
{
	qreal scaleFactor = Drawing::unitsScale(mUnits, units);

	mUnits = units;

	if (scaleFactor != 1.0)
	{
		mSceneRect.setRect(mSceneRect.left() * scaleFactor, mSceneRect.top() * scaleFactor,
			mSceneRect.width() * scaleFactor, mSceneRect.height() * scaleFactor);
		mContentsRect.setRect(mContentsRect.left() * scaleFactor, mContentsRect.top() * scaleFactor,
			mContentsRect.width() * scaleFactor, mContentsRect.height() * scaleFactor);
		mBorderWidth *= scaleFactor;
		mGrid *= scaleFactor;

		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			(*itemIter)->setPos((*itemIter)->pos() * scaleFactor);
			if ((*itemIter)->shouldMatchUnitsWithParent()) (*itemIter)->setUnits(mUnits);
		}
	}
}

DrawingUnits DrawingScene::units() const
{
	return mUnits;
}

//==================================================================================================

void DrawingScene::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

void DrawingScene::setContentsRect(const QRectF& rect)
{
	mContentsRect = rect;
}

void DrawingScene::setBorderWidth(qreal borderWidth)
{
	mBorderWidth = borderWidth;
}

QRectF DrawingScene::sceneRect() const
{
	return mSceneRect;
}

QRectF DrawingScene::contentsRect() const
{
	return mContentsRect;
}

qreal DrawingScene::borderWidth() const
{
	return mBorderWidth;
}

//==================================================================================================

void DrawingScene::setGrid(qreal grid)
{
	mGrid = grid;
}

qreal DrawingScene::grid() const
{
	return mGrid;
}

qreal DrawingScene::roundToGrid(qreal value) const
{
	qreal temp;

	temp = value - (fmod(value, mGrid));
	if (fmod(value, mGrid) >= mGrid/2) temp += mGrid;

	return temp;
}

QPointF DrawingScene::roundToGrid(const QPointF& scenePos) const
{
	return QPointF(roundToGrid(scenePos.x()), roundToGrid(scenePos.y()));
}

//==================================================================================================

void DrawingScene::setSelectionMode(Qt::ItemSelectionMode mode)
{
	mSelectionMode = mode;
}

void DrawingScene::setForcingItemsInside(bool inside)
{
	mForcingItemsInside = inside;
}

Qt::ItemSelectionMode DrawingScene::selectionMode() const
{
	return mSelectionMode;
}

bool DrawingScene::isForcingItemsInside() const
{
	return mForcingItemsInside;
}

//==================================================================================================

void DrawingScene::addItem(DrawingItem* item)
{
	if (item && item->parent() == nullptr && !containsItem(item))
	{
		mItems.append(item);
		item->mScene = this;
	}
}

void DrawingScene::insertItem(int index, DrawingItem* item)
{
	if (item && item->parent() == nullptr && !containsItem(item))
	{
		mItems.insert(index, item);
		item->mScene = this;
	}
}

void DrawingScene::removeItem(DrawingItem* item)
{
	if (item && containsItem(item))
	{
		deselectItem(item);
		mItems.removeAll(item);
		item->mScene = nullptr;
	}
}

void DrawingScene::clearItems()
{
	DrawingItem* item;

	while (!mItems.isEmpty())
	{
		item = mItems.first();
		removeItem(item);
		delete item;
	}

	mMouseDownItem = nullptr;
	mUndoStack.clear();
}

QList<DrawingItem*> DrawingScene::items() const
{
	return mItems;
}

int DrawingScene::numberOfItems() const
{
	return mItems.size();
}

int DrawingScene::itemIndex(DrawingItem* item) const
{
	int index = -1;
	if (item) index = mItems.indexOf(item);
	return index;
}

bool DrawingScene::containsItem(DrawingItem* item) const
{
	bool result = false;
	if (item) result = mItems.contains(item);
	return result;
}

void DrawingScene::reorderItems(const QList<DrawingItem*>& items)
{
	mItems = items;
}

//==================================================================================================

QList<DrawingItem*> DrawingScene::items(const QRectF& sceneRect) const
{
	QList<DrawingItem*> itemsInRect;
	QRectF itemRect;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesRect(*itemIter, sceneRect, mSelectionMode))
			itemsInRect.append(*itemIter);
	}

	return itemsInRect;
}

QList<DrawingItem*> DrawingScene::childItems(const QRectF& sceneRect) const
{
	QList<DrawingItem*> itemsInRect;
	QList<DrawingItem*> children;
	QRectF itemRect;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		children = (*itemIter)->children();
		for(auto childIter = mItems.begin(); childIter != mItems.end(); childIter++)
		{
			if (itemMatchesRect(*childIter, sceneRect, mSelectionMode))
				itemsInRect.append(*childIter);
		}
	}

	return itemsInRect;
}

DrawingItem* DrawingScene::itemAt(const QPointF& scenePos) const
{
	DrawingItem* item = nullptr;

	// Favor selected items
	auto itemIter = mSelectedItems.end();
	while (item == nullptr && itemIter != mSelectedItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	// Search all items
	itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	return item;
}

DrawingItem* DrawingScene::childAt(const QPointF& scenePos) const
{
	DrawingItem* item = nullptr;
	QList<DrawingItem*> children;

	// Favor selected items
	auto itemIter = mSelectedItems.end();
	while (item == nullptr && itemIter != mSelectedItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	// Search all items
	itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;

		children = (*itemIter)->children();
		for(auto childIter = children.begin(); item == nullptr && childIter != children.end(); childIter++)
		{
			if (itemMatchesPoint(*childIter, scenePos)) item = *childIter;
		}
	}

	return item;
}

QRectF DrawingScene::itemsRect() const
{
	QRectF rect;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (!rect.isValid()) rect = (*itemIter)->boundingRect().translated((*itemIter)->pos());
		else rect = rect.united((*itemIter)->boundingRect().translated((*itemIter)->pos()));
	}

	return rect;
}

QRectF DrawingScene::itemsShapeRect() const
{
	QRectF rect;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (!rect.isValid())
			rect = (*itemIter)->shape().boundingRect().translated((*itemIter)->pos());
		else
			rect = rect.united((*itemIter)->shape().boundingRect().translated((*itemIter)->pos()));
	}

	return rect;
}

//==================================================================================================

void DrawingScene::setNewItem(DrawingItem* item)
{
	if (mNewItem) mNewItem->mScene = nullptr;
	delete mNewItem;

	mNewItem = item;
	mNewClickCount = 0;
	if (mNewItem)
	{
		mNewItem->mScene = this;
		if (mNewItem->shouldMatchUnitsWithParent()) mNewItem->setUnits(units());
	}
}

DrawingItem* DrawingScene::newItem() const
{
	return mNewItem;
}

//==================================================================================================

void DrawingScene::selectItem(DrawingItem* item)
{
	if (item && !item->isSelected())
	{
		// Ensure that none of the child items are already selected
		for(int i = 0; i < item->numberOfChildren(); i++)
			deselectItem(item->child(i));

		item->setSelected(true);
		mSelectedItems.append(item);
	}
}

void DrawingScene::deselectItem(DrawingItem* item)
{
	if (item && item->isSelected())
	{
		mSelectedItems.removeAll(item);
		item->setSelected(false);
	}
}

void DrawingScene::selectItems(const QRectF& sceneRect)
{
	QList<DrawingItem*> lItems = items(sceneRect);

	for(auto itemIter = lItems.begin();	itemIter != lItems.end(); itemIter++) selectItem(*itemIter);
}

QList<DrawingItem*> DrawingScene::selectedItems() const
{
	return mSelectedItems;
}

int DrawingScene::numberOfSelectedItems() const
{
	return mSelectedItems.size();
}

//==================================================================================================

void DrawingScene::setUndoLimit(int undoLimit)
{
	mUndoStack.setUndoLimit(undoLimit);
}

void DrawingScene::setClean()
{
	mUndoStack.setClean();
}

void DrawingScene::pushUndo(QUndoCommand* command)
{
	mUndoStack.push(command);
}

int DrawingScene::undoLimit() const
{
	return mUndoStack.undoLimit();
}

bool DrawingScene::isClean() const
{
	return mUndoStack.isClean();
}

bool DrawingScene::canUndo() const
{
	return mUndoStack.canUndo();
}

bool DrawingScene::canRedo() const
{
	return mUndoStack.canRedo();
}

//==================================================================================================

DrawingItem* DrawingScene::mouseDownItem() const
{
	return mMouseDownItem;
}

DrawingMouseEvent DrawingScene::lastMouseEvent() const
{
	return mLastMouseEvent;
}

//==================================================================================================

void DrawingScene::undo()
{
	mUndoStack.undo();
	emit numberOfItemsChanged(numberOfItems());
}

void DrawingScene::redo()
{
	mUndoStack.redo();
	emit numberOfItemsChanged(numberOfItems());
}

//==================================================================================================

void DrawingScene::cut()
{
	copy();
	deleteSelection();
}

void DrawingScene::copy()
{
	QClipboard* clipboard = QApplication::clipboard();
	if (clipboard && !mSelectedItems.isEmpty())
	{
		QString xmlItems;
		QXmlStreamWriter xmlWriter(&xmlItems);

		xmlWriter.setAutoFormatting(true);
		xmlWriter.setAutoFormattingIndent(-1);

		xmlWriter.writeStartElement("items");
		DrawingItem::writeItemsToXml(xmlWriter, mSelectedItems);
		xmlWriter.writeEndElement();

		clipboard->setText(xmlItems);
	}
}

void DrawingScene::paste()
{
	QClipboard* clipboard = QApplication::clipboard();
	QList<DrawingItem*> newItems;

	if (clipboard)
	{
		QString xmlItems = clipboard->text();
		QXmlStreamReader xmlReader(xmlItems);

		if (xmlReader.readNextStartElement() && xmlReader.name() == "items")
			newItems = DrawingItem::readItemsFromXml(xmlReader);
	}

	if (!newItems.isEmpty())
	{
		// Offset items based on mConsecutivePastes
		bool shouldAddItems = true;
		qreal offset = 2 * mConsecutivePastes * grid();
		QPointF deltaPosition = QPointF(offset, offset) +
			roundToGrid(mLastMouseEvent.buttonDownScenePos() - newItems.first()->pos());

		for(auto itemIter = newItems.begin(); itemIter != newItems.end(); itemIter++)
		{
			if ((*itemIter)->shouldMatchUnitsWithParent())
				(*itemIter)->setUnits(units());

			(*itemIter)->setPos((*itemIter)->pos() + deltaPosition);
			(*itemIter)->setSelected(false);
		}

		if (mForcingItemsInside) shouldAddItems = keepItemsInside(newItems);

		if (shouldAddItems)
		{
			// Add new items to scene
			addItems(newItems, DoNotPlace);

			// Ensure new items are selected
			deselectAll();
			for(auto itemIter = newItems.begin(); itemIter != newItems.end(); itemIter++)
				selectItem(*itemIter);
			emit selectionChanged();

			mConsecutivePastes++;
		}
		else
		{
			while (!newItems.isEmpty()) delete newItems.takeFirst();
		}
	}
}

//==================================================================================================

void DrawingScene::selectAll()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) selectItem(*itemIter);
	emit selectionChanged();
}

void DrawingScene::deselectAll()
{
	while (!mSelectedItems.empty()) deselectItem(mSelectedItems.last());
	emit selectionChanged();
}

//==================================================================================================

void DrawingScene::rotateSelection()
{
	if (mNewItem)
	{
		if (mNewItem->placeType() == DrawingItem::PlaceMouseUp)
			mNewItem->rotateItem(mNewItem->pos());
	}
	else
	{
		if (mSelectedItems.size() > 0 && tryRotate(mSelectedItems, mSelectionCenter))
			rotateItems(mSelectedItems, mSelectionCenter);
	}
}

void DrawingScene::rotateBackSelection()
{
	if (mNewItem)
	{
		if (mNewItem->placeType() == DrawingItem::PlaceMouseUp)
			mNewItem->rotateBackItem(mNewItem->pos());
	}
	else
	{
		if (mSelectedItems.size() > 0 && tryRotateBack(mSelectedItems, mSelectionCenter))
			rotateBackItems(mSelectedItems, mSelectionCenter);
	}
}

void DrawingScene::flipSelection()
{
	if (mNewItem)
	{
		if (mNewItem->placeType() == DrawingItem::PlaceMouseUp)
			mNewItem->flipItem(mNewItem->pos());
	}
	else
	{
		if (mSelectedItems.size() > 0 && tryFlip(mSelectedItems, mSelectionCenter))
			flipItems(mSelectedItems, mSelectionCenter);
	}
}

void DrawingScene::deleteSelection()
{
	if (numberOfSelectedItems() > 0)
	{
		QList<DrawingItem*> items;

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			if ((*itemIter)->parent() == nullptr) items.append(*itemIter);
		}

		removeItems(items);
	}
}

//==================================================================================================

void DrawingScene::sendBackward()
{
	QList<DrawingItem*> lItems = mSelectedItems;
	QList<DrawingItem*> lItemsOrdered = mItems;

	DrawingItem* item;
	qint32 index;

	while (!lItems.empty())
	{
		item = lItems.takeLast();
		index = lItemsOrdered.indexOf(item);
		if (index > 0)
		{
			lItemsOrdered.removeAll(item);
			lItemsOrdered.insert(index - 1, item);
		}
	}

	mUndoStack.push(new DrawingReorderItemsCommand(this, lItemsOrdered));
}

void DrawingScene::bringForward()
{
	QList<DrawingItem*> lItems = mSelectedItems;
	QList<DrawingItem*> lItemsOrdered = mItems;

	DrawingItem* item;
	qint32 index;

	while (!lItems.empty())
	{
		item = lItems.takeLast();
		index = lItemsOrdered.indexOf(item);
		if (index >= 0)
		{
			lItemsOrdered.removeAll(item);
			lItemsOrdered.insert(index + 1, item);
		}
	}

	mUndoStack.push(new DrawingReorderItemsCommand(this, lItemsOrdered));
}

void DrawingScene::sendToBack()
{
	QList<DrawingItem*> lItems = mSelectedItems;
	QList<DrawingItem*> lItemsOrdered = mItems;

	DrawingItem* item;

	while (!lItems.empty())
	{
		item = lItems.takeLast();
		if (lItemsOrdered.contains(item))
		{
			lItemsOrdered.removeAll(item);
			lItemsOrdered.prepend(item);
		}
	}

	mUndoStack.push(new DrawingReorderItemsCommand(this, lItemsOrdered));
}

void DrawingScene::bringToFront()
{
	QList<DrawingItem*> lItems = mSelectedItems;
	QList<DrawingItem*> lItemsOrdered = mItems;

	DrawingItem* item;

	while (!lItems.empty())
	{
		item = lItems.takeLast();
		if (lItemsOrdered.contains(item))
		{
			lItemsOrdered.removeAll(item);
			lItemsOrdered.append(item);
		}
	}

	mUndoStack.push(new DrawingReorderItemsCommand(this, lItemsOrdered));
}

//==================================================================================================

void DrawingScene::insertItemPoint()
{
	DrawingItem* item = nullptr;
	if (mSelectedItems.size() == 1) item = mSelectedItems.first();

	if (item && item->canInsertRemovePoints())
	{
		DrawingItemPoint* itemPoint = new DrawingItemPoint();
		itemPoint->setPos(
			item->mapFromScene(roundToGrid(mLastMouseEvent.buttonDownScenePos())));

		if (item->canInsertPoint(itemPoint) && item->insertPointIndex(itemPoint) >= 0)
		{
			mUndoStack.push(new DrawingItemInsertPointCommand(
				item, itemPoint, item->insertPointIndex(itemPoint)));
		}
	}
}

void DrawingScene::removeItemPoint()
{
	DrawingItem* item = nullptr;
	if (mSelectedItems.size() == 1) item = mSelectedItems.first();

	if (item && item->canInsertRemovePoints())
	{
		DrawingItemPoint* itemPoint = item->pointNearest(
			item->mapFromScene(roundToGrid(mLastMouseEvent.buttonDownScenePos())));

		if (item->canRemovePoint(itemPoint))
			mUndoStack.push(new DrawingItemRemovePointCommand(item, itemPoint));
	}
}

//==================================================================================================

void DrawingScene::group()
{
	QList<DrawingItem*> lSelectedItems = selectedItems();
	if (lSelectedItems.size() > 1)
	{
		QUndoCommand* command = new QUndoCommand("Group Items");

		QList<DrawingItem*> items = DrawingItem::copyItems(lSelectedItems);
		DrawingItemGroup* itemGroup = new DrawingItemGroup();

		itemGroup->setPos(items.first()->pos());
		itemGroup->setUnits(units());
		for(auto iter = items.begin(); iter != items.end(); iter++)
		{
			(*iter)->setSelected(false);
			(*iter)->setPos((*iter)->pos() - itemGroup->pos());
		}
		itemGroup->setItems(items);

		removeItems(lSelectedItems, command);
		addItems(itemGroup, DoNotPlace, command);

		mUndoStack.push(command);
		selectItem(itemGroup);

		emit numberOfItemsChanged(numberOfItems());
	}
}

void DrawingScene::ungroup()
{
	QList<DrawingItem*> lSelectedItems = selectedItems();
	if (lSelectedItems.size() == 1)
	{
		DrawingItemGroup* itemGroup = dynamic_cast<DrawingItemGroup*>(lSelectedItems.first());
		if (itemGroup)
		{
			QUndoCommand* command = new QUndoCommand("Ungroup Items");

			QList<DrawingItem*> items = DrawingItem::copyItems(itemGroup->items());
			for(auto iter = items.begin(); iter != items.end(); iter++)
				(*iter)->setPos((*iter)->pos() + itemGroup->pos());

			removeItems(itemGroup, command);
			addItems(items, DoNotPlace, command);

			mUndoStack.push(command);
			for(auto iter = items.begin(); iter != items.end(); iter++)
			{
				if ((*iter)->shouldMatchUnitsWithParent()) (*iter)->setUnits(units());
				selectItem(*iter);
			}

			emit numberOfItemsChanged(numberOfItems());
		}
	}
}

//==================================================================================================

void DrawingScene::updateSelectionCenter()
{
	int count = 0;

	mSelectionCenter = QPointF(0.0, 0.0);
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
	{
		mSelectionCenter += (*itemIter)->mapToScene((*itemIter)->centerPos());
		count++;
	}

	if (count > 0) mSelectionCenter /= count;

	mSelectionCenter = roundToGrid(mSelectionCenter);
}

//==================================================================================================

bool DrawingScene::itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const
{
	bool match = false;

	if (item)
	{
		// Check item shape
		match = item->shape().contains(item->mapFromScene(scenePos));

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
				match = (*pointIter)->sceneRect().contains(scenePos);
		}
	}

	return match;
}

bool DrawingScene::itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item)
	{
		// Check item boundingRect or shape
		switch (mode)
		{
		case Qt::IntersectsItemShape:
			match = item->shape().intersects(item->mapFromScene(rect));
			break;
		case Qt::ContainsItemShape:
			match = rect.contains(Drawing::adjustRectForMinimumSize(item->mapToScene(item->shape().boundingRect())));
			break;
		case Qt::IntersectsItemBoundingRect:
			match = rect.intersects(Drawing::adjustRectForMinimumSize(item->mapToScene(item->boundingRect())));
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = rect.contains(Drawing::adjustRectForMinimumSize(item->mapToScene(item->boundingRect())));
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = rect.intersects((*pointIter)->sceneRect());
				else
					match = rect.contains((*pointIter)->sceneRect());
			}
		}
	}

	return match;
}

//==================================================================================================

void DrawingScene::contextMenuEvent(DrawingMouseEvent& event)
{
	mLastMouseEvent = event;

	if (!mNewItem)
	{
		if (event.modifiers() & Qt::ShiftModifier) mMouseDownItem = childAt(event.scenePos());
		else mMouseDownItem = itemAt(event.scenePos());

		if (mMouseDownItem)
			mMouseDownItemPos = mMouseDownItem->mapFromScene(event.buttonDownScenePos());

		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->contextMenuEvent(event);
	}
}

void DrawingScene::mouseDoubleClickEvent(DrawingMouseEvent& event)
{
	mLastMouseEvent = event;

	if (!mNewItem)
	{
		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->mouseDoubleClickEvent(event);
	}
}

void DrawingScene::mousePressEvent(DrawingMouseEvent& event)
{
	mLastMouseEvent = event;

	if (!mNewItem)
	{
		mMouseState = MouseSelect;

		if (event.modifiers() & Qt::ShiftModifier) mMouseDownItem = childAt(event.scenePos());
		else mMouseDownItem = itemAt(event.scenePos());

		if (mMouseDownItem)
			mMouseDownItemPos = mMouseDownItem->mapFromScene(event.buttonDownScenePos());

		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->mousePressEvent(event);
	}
	else newMousePressEvent(event);
}

void DrawingScene::mouseMoveEvent(DrawingMouseEvent& event)
{
	mLastMouseEvent = event;

	if (!mNewItem)
	{
		QPointF deltaPos;
		bool canMove = true;

		if (event.buttons() & Qt::LeftButton)
		{
			switch (mMouseState)
			{
			case MouseSelect:
				if (event.isDragged())
				{
					if (mMouseDownItem && mMouseDownItem->isSelected())
					{
						bool resizeItem = (mSelectedItems.size() == 1 &&
										   mSelectedItems.first()->hasSelectedPoint() &&
										   mSelectedItems.first()->selectedPoint()->isControlPoint());
						mMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
					}
					else mMouseState = MouseRubberBand;
				}
				break;

			case MouseMoveItems:
				for(auto itemIter = mSelectedItems.begin();	canMove && itemIter != mSelectedItems.end(); itemIter++)
					canMove = (*itemIter)->canMove();

				if (canMove)
				{
					deltaPos = tryMove(mSelectedItems, roundToGrid(event.scenePos()) - roundToGrid(mMouseDownItem->mapToScene(mMouseDownItemPos)));
					moveItems(mSelectedItems, deltaPos, DoNotPlace);
					emit selectionChanged();
				}
				break;

			case MouseResizeItem:
				if (mMouseDownItem->canResize())
				{
					resizeItem(mMouseDownItem->selectedPoint(),
						tryResize(mMouseDownItem->selectedPoint(), roundToGrid(event.scenePos())), DoNotPlace, true);
					emit selectionChanged();
				}
				break;

			default:
				break;
			}

			if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
				mMouseDownItem->mouseMoveEvent(event);
		}
	}
	else newMouseMoveEvent(event);
}

void DrawingScene::mouseReleaseEvent(DrawingMouseEvent& event)
{
	mLastMouseEvent = event;

	if (!mNewItem)
	{
		bool controlDown = ((event.modifiers() & Qt::ControlModifier) != 0);
		bool canMove = true;
		QPointF deltaPos;
		DrawingItemPlaceMode placeMode = (event.modifiers() & Qt::ShiftModifier) ? PlaceStrict : PlaceLoose;

		switch (mMouseState)
		{
		case MouseSelect:
			if (!controlDown) deselectAll();
			if (mMouseDownItem)
			{
				if (controlDown && mMouseDownItem->isSelected())
					deselectItem(mMouseDownItem);
				else selectItem(mMouseDownItem);

				emit selectionChanged();
			}
			break;

		case MouseMoveItems:
			for(auto itemIter = mSelectedItems.begin(); canMove && itemIter != mSelectedItems.end(); itemIter++)
				canMove = (*itemIter)->canMove();

			if (canMove)
			{
				deltaPos = tryMove(mSelectedItems, roundToGrid(event.scenePos()) - roundToGrid(mMouseDownItem->mapToScene(mMouseDownItemPos)));
				moveItems(mSelectedItems, deltaPos, placeMode);
				emit selectionChanged();
			}
			break;

		case MouseResizeItem:
			if (mMouseDownItem->canResize())
			{
				resizeItem(mMouseDownItem->selectedPoint(),
					tryResize(mMouseDownItem->selectedPoint(), roundToGrid(event.scenePos())), placeMode, true);
				emit selectionChanged();
			}
			break;

		case MouseRubberBand:
			if (!controlDown) deselectAll();
			selectItems(Drawing::rectFromPoints(event.scenePos(), event.buttonDownScenePos()));
			emit selectionChanged();
			break;

		default:
			break;
		}

		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->mouseReleaseEvent(event);

		mMouseState = MouseReady;
		mConsecutivePastes = 0;
	}
	else newMouseReleaseEvent(event);
}

//==================================================================================================

void DrawingScene::newMousePressEvent(DrawingMouseEvent& event)
{
	Q_UNUSED(event);
}

void DrawingScene::newMouseMoveEvent(DrawingMouseEvent& event)
{
	if (mNewItem)
	{
		QPointF scenePos = roundToGrid(event.scenePos());
		DrawingItemPoint* itemPoint = mNewItem->point(1);

		switch (mNewItem->placeType())
		{
		case DrawingItem::PlaceMouseDownAndUp:
			if (event.isDragged())
			{
				mNewItem->setPos(roundToGrid(event.buttonDownScenePos()));
				if (itemPoint && itemPoint->isControlPoint())
					mNewItem->resizeItem(itemPoint, scenePos);
			}
			else
				mNewItem->setPos(scenePos);
			break;

		case DrawingItem::PlaceTwoClick:
			if (mNewClickCount % 2 == 1)
			{
				if (itemPoint && itemPoint->isControlPoint())
					mNewItem->resizeItem(itemPoint, scenePos);
			}
			else
				mNewItem->setPos(scenePos);
			break;

		default:    // DrawingItem::PlaceMouseUp
			mNewItem->setPos(scenePos);
			break;
		}
	}
}

void DrawingScene::newMouseReleaseEvent(DrawingMouseEvent& event)
{
	if (mNewItem)
	{
		bool itemAdded = false;
		bool adjustPoints = false;
		DrawingItemPlaceMode placeMode = (event.modifiers() & Qt::ShiftModifier) ? PlaceStrict : PlaceLoose;

		QRectF newItemRect = Drawing::adjustRectForMinimumSize(mNewItem->mapToScene(mNewItem->boundingRect()));
		if (!mNewItem->isSuperfluous() && (!mForcingItemsInside || contentsRect().contains(newItemRect)))
		{
			itemAdded = (mNewItem->placeType() != DrawingItem::PlaceTwoClick ||
					mNewClickCount % 2 == 1);
			adjustPoints = (itemAdded && mNewItem->placeType() != DrawingItem::PlaceMouseUp);
			mNewClickCount++;
		}
		else adjustPoints = (mNewItem->placeType() != DrawingItem::PlaceMouseUp);

		if (itemAdded)
		{
			DrawingItem* newItem = mNewItem;
			mNewItem = nullptr;
			setNewItem(newItem->copy());

			newItem->aboutToChangeEvent(DrawingItem::AddNewItemToScene, QVariant());
			addItems(newItem, placeMode);
			newItem->changedEvent(DrawingItem::AddNewItemToScene, QVariant());
		}

		if (adjustPoints && mNewItem)
		{
			if (mNewItem->point(0)) mNewItem->point(0)->setPos(0.0, 0.0);
			if (mNewItem->point(1)) mNewItem->point(1)->setPos(0.0, 0.0);
		}
		if (itemAdded) mNewItem->setPos(roundToGrid(event.scenePos()));
	}
}

//==================================================================================================

void DrawingScene::addItems(const QList<DrawingItem*>& items, DrawingItemPlaceMode placeMode, QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, items, command);

	addCommand->redo();
	placeItems(items, placeMode, addCommand);
	addCommand->undo();

	if (!command)
	{
		mUndoStack.push(addCommand);
		emit numberOfItemsChanged(numberOfItems());
	}
}

void DrawingScene::addItems(DrawingItem* item, DrawingItemPlaceMode placeMode, QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, item, command);

	addCommand->redo();
	placeItems(item, placeMode, addCommand);
	addCommand->undo();

	if (!command)
	{
		mUndoStack.push(addCommand);
		emit numberOfItemsChanged(numberOfItems());
	}
}

void DrawingScene::removeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

void DrawingScene::removeItems(DrawingItem* item, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, item, command);

	removeCommand->redo();
	unplaceItems(item, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

//==================================================================================================

void DrawingScene::moveItems(const QList<DrawingItem*>& items, const QPointF& deltaPos,
	DrawingItemPlaceMode placeMode, QUndoCommand* command)
{
	DrawingMoveItemsDeltaCommand* moveCommand =
		new DrawingMoveItemsDeltaCommand(items, deltaPos, (placeMode != DoNotPlace), command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	placeItems(items, placeMode, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingScene::moveItems(DrawingItem* item, const QPointF& deltaPos,
	DrawingItemPlaceMode placeMode, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		moveItems(items, deltaPos, placeMode, command);
	}
}

void DrawingScene::resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos,
	DrawingItemPlaceMode placeMode, bool disconnect, QUndoCommand* command)
{
	if (itemPoint)
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(itemPoint, parentPos, (placeMode != DoNotPlace), command);

		resizeCommand->redo();
		if (disconnect) disconnectAll(itemPoint, resizeCommand);
		tryToMaintainConnections(itemPoint->item(), true, !itemPoint->isFree(), itemPoint, resizeCommand);
		placeItems(itemPoint->item(), placeMode, resizeCommand);
		resizeCommand->undo();

		if (!command) mUndoStack.push(resizeCommand);
	}
}

//==================================================================================================

void DrawingScene::rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingScene::rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingScene::flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingFlipItemsCommand* flipCommand =
		new DrawingFlipItemsCommand(items, scenePos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

//==================================================================================================

void DrawingScene::connectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand = new DrawingItemPointConnectCommand(point0, point1, command);
	QPointF point0Pos = point0->item()->mapToScene(point0->pos());
	QPointF point1Pos = point1->item()->mapToScene(point1->pos());

	if (point0Pos != point1Pos)
	{
		if (point1->item()->canResize() && point1->isControlPoint())
			resizeItem(point1, point0Pos, DoNotPlace, true, connectCommand);
		else if (point0->item()->canResize() && point0->isControlPoint())
			resizeItem(point0, point1Pos, DoNotPlace, true, connectCommand);
	}

	if (!command) mUndoStack.push(connectCommand);
}

void DrawingScene::disconnectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(point0, point1, command);

	if (!command) mUndoStack.push(disconnectCommand);
}

//==================================================================================================

void DrawingScene::placeItems(const QList<DrawingItem*>& items, DrawingItemPlaceMode placeMode,	QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;

	if (placeMode != DoNotPlace)
	{
		for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		{
			for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
			{
				if (!items.contains(*otherItemIter) && (mNewItem == nullptr || (*otherItemIter) != mNewItem))
				{
					itemPoints = (*itemIter)->points();
					otherItemPoints = (*otherItemIter)->points();

					for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
					{
						for(auto otherItemPointIter = otherItemPoints.begin(); otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
						{
							if ((*itemPointIter)->shouldConnect(*otherItemPointIter, placeMode))
								connectItemPoints(*itemPointIter, *otherItemPointIter, command);
						}
					}
				}
			}
		}
	}
}

void DrawingScene::placeItems(DrawingItem* item, DrawingItemPlaceMode placeMode, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		placeItems(items, placeMode, command);
	}
}

void DrawingScene::unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingItem* item;
	DrawingItemPoint* itemPoint;
	QList<DrawingItemPoint*> itemPoints, targetPoints;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			targetPoints = itemPoint->targets();
			for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
			{
				if (!items.contains((*targetPointIter)->item()))
					disconnectItemPoints(itemPoint, *targetPointIter, command);
			}
		}
	}
}

void DrawingScene::unplaceItems(DrawingItem* item, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		unplaceItems(items, command);
	}
}

//==================================================================================================

void DrawingScene::tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
	bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, targetPoints;
	DrawingItem* item;
	DrawingItem* targetItem;
	DrawingItemPoint* itemPoint;
	DrawingItemPoint* targetItemPoint;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			if (itemPoint != pointToSkip && (checkControlPoints || !itemPoint->isControlPoint()))
			{
				targetPoints = itemPoint->targets();
				for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
				{
					targetItemPoint = *targetPointIter;
					targetItem = targetItemPoint->item();

					if (item->mapToScene(itemPoint->pos()) != targetItem->mapToScene(targetItemPoint->pos()))
					{
						// Try to maintain the connection by resizing targetPoint if possible
						if (allowResize && targetItemPoint->isFree() &&	!itemPoint->shouldDisconnect(targetItemPoint))
							resizeItem(targetItemPoint, item->mapToScene(itemPoint->pos()), DoNotPlace, false, command);
						else
							disconnectItemPoints(itemPoint, targetItemPoint, command);
					}
				}
			}
		}
	}
}

void DrawingScene::tryToMaintainConnections(DrawingItem* item, bool allowResize,
	bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		tryToMaintainConnections(items, allowResize, checkControlPoints, pointToSkip, command);
	}
}

void DrawingScene::disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command)
{
	QList<DrawingItemPoint*> targetPoints;

	if (itemPoint)
	{
		targetPoints = itemPoint->targets();
		for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
			disconnectItemPoints(itemPoint, *targetPointIter, command);
	}
}

//==================================================================================================

QPointF DrawingScene::tryMove(const QList<DrawingItem*>& items, const QPointF& deltaPos)
{
	QPointF actualDeltaPos = deltaPos;

	if (mForcingItemsInside)
	{
		QRectF rect;
		qreal grid = DrawingScene::grid();

		for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		{
			(*itemIter)->setPos((*itemIter)->pos() + deltaPos);

			rect = (*itemIter)->mapToScene((*itemIter)->boundingRect());
			if (rect.left() < mContentsRect.left())
			{
				actualDeltaPos.setX(qMax(actualDeltaPos.x(),
					deltaPos.x() + mContentsRect.left() - qFloor(rect.left() / grid) * grid));
			}
			else if (rect.right() > mContentsRect.right())
			{
				actualDeltaPos.setX(qMin(actualDeltaPos.x(),
					deltaPos.x() + mContentsRect.right() - qCeil(rect.right() / grid) * grid));
			}

			if (rect.top() < mContentsRect.top())
			{
				actualDeltaPos.setY(qMax(actualDeltaPos.y(),
					deltaPos.y() + mContentsRect.top() - qFloor(rect.top() / grid) * grid));
			}
			else if (rect.bottom() > mContentsRect.bottom())
			{
				actualDeltaPos.setY(qMin(actualDeltaPos.y(),
					deltaPos.y() + mContentsRect.bottom() - qCeil(rect.bottom() / grid) * grid));
			}

			(*itemIter)->setPos((*itemIter)->pos() - deltaPos);
		}
	}

	return actualDeltaPos;
}

QPointF DrawingScene::tryResize(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	QPointF actualScenePos = scenePos;

	Q_UNUSED(itemPoint);

	if (mForcingItemsInside)
	{
		if (actualScenePos.x() < mContentsRect.left()) actualScenePos.setX(roundToGrid(mContentsRect.left()));
		else if (actualScenePos.x() > mContentsRect.right()) actualScenePos.setX(roundToGrid(mContentsRect.right()));

		if (actualScenePos.y() < mContentsRect.top()) actualScenePos.setY(roundToGrid(mContentsRect.top()));
		else if (actualScenePos.y() > mContentsRect.bottom()) actualScenePos.setY(roundToGrid(mContentsRect.bottom()));
	}

	return actualScenePos;
}

bool DrawingScene::tryRotate(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	bool canRotate = false;
	QRectF sceneRect = DrawingScene::contentsRect();
	QRectF itemRect;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateItem(scenePos);

	canRotate = true;
	for(auto itemIter = items.begin(); canRotate && itemIter != items.end(); itemIter++)
	{
		itemRect = Drawing::adjustRectForMinimumSize((*itemIter)->mapToScene((*itemIter)->boundingRect()));
		canRotate = ((*itemIter)->canRotate() && (!mForcingItemsInside || sceneRect.contains(itemRect)));
	}

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateBackItem(scenePos);

	return canRotate;
}

bool DrawingScene::tryRotateBack(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	bool canRotate = false;
	QRectF sceneRect = DrawingScene::contentsRect();
	QRectF itemRect;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateBackItem(scenePos);

	canRotate = true;
	for(auto itemIter = items.begin(); canRotate && itemIter != items.end(); itemIter++)
	{
		itemRect = Drawing::adjustRectForMinimumSize((*itemIter)->mapToScene((*itemIter)->boundingRect()));
		canRotate = ((*itemIter)->canRotate() && (!mForcingItemsInside || sceneRect.contains(itemRect)));
	}

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateItem(scenePos);

	return canRotate;
}

bool DrawingScene::tryFlip(const QList<DrawingItem*>& items, const QPointF& scenePos)
{
	bool canFlip = false;
	QRectF sceneRect = DrawingScene::contentsRect();
	QRectF itemRect;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipItem(scenePos);

	canFlip = true;
	for(auto itemIter = items.begin(); canFlip && itemIter != items.end(); itemIter++)
	{
		itemRect = Drawing::adjustRectForMinimumSize((*itemIter)->mapToScene((*itemIter)->boundingRect()));
		canFlip = ((*itemIter)->canFlip() && (!mForcingItemsInside || sceneRect.contains(itemRect)));
	}

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipItem(scenePos);

	return canFlip;
}

bool DrawingScene::keepItemsInside(const QList<DrawingItem*>& items)
{
	bool success = false;

	// Ensure that new items are placed inside scene boundaries
	QRectF sceneRect = DrawingScene::contentsRect();
	QRectF itemsRect;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		if (!itemsRect.isValid())
			itemsRect = (*itemIter)->boundingRect().translated((*itemIter)->pos());
		else
		{
			itemsRect = itemsRect.united(
				(*itemIter)->boundingRect().translated((*itemIter)->pos()));
		}
	}

	success = (itemsRect.width() <= sceneRect.width() && itemsRect.height() <= sceneRect.height());
	if (success)
	{
		qreal offset;

		offset = itemsRect.right() - sceneRect.right();
		if (offset > 0)
		{
			for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
				(*itemIter)->setPos((*itemIter)->x() - offset, (*itemIter)->y());
		}
		offset = itemsRect.left() - sceneRect.left();
		if (offset < 0)
		{
			for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
				(*itemIter)->setPos((*itemIter)->x() - offset, (*itemIter)->y());
		}

		offset = itemsRect.bottom() - sceneRect.bottom();
		if (offset > 0)
		{
			for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
				(*itemIter)->setPos((*itemIter)->x(), (*itemIter)->y() - offset);
		}
		offset = itemsRect.top() - sceneRect.top();
		if (offset < 0)
		{
			for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
				(*itemIter)->setPos((*itemIter)->x(), (*itemIter)->y() - offset);
		}
	}

	return success;
}

//==================================================================================================

void DrawingScene::drawBackground(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	painter->setRenderHints((QPainter::RenderHints)0);

	if (styleOptions.shouldDrawBackground())
	{
		painter->setBrush(styleOptions.outputBrush(DrawingStyleOptions::Background));
		painter->setPen(Qt::NoPen);
		painter->drawRect(mSceneRect);
	}

	if (styleOptions.shouldDrawGrid() && mGrid > 0) drawGrid(painter, styleOptions, mSceneRect);

	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	if (styleOptions.shouldDrawBorder()) drawBorder(painter, styleOptions, mContentsRect);

	Q_UNUSED(rect);
}

void DrawingScene::drawItems(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			qreal scaleFactor = Drawing::unitsScale(units(), (*itemIter)->units());

			painter->save();
			painter->translate((*itemIter)->pos());
			painter->scale(scaleFactor, scaleFactor);
			(*itemIter)->render(painter, styleOptions);
			painter->restore();
		}
	}

	Q_UNUSED(rect);
}

void DrawingScene::drawForeground(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	QList<DrawingItem*> items;
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;
	DrawingItemPlaceMode placeMode = (mLastMouseEvent.modifiers() & Qt::ShiftModifier) ? PlaceStrict : PlaceLoose;

	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	// Draw new item
	if (mNewItem)
	{
		qreal itemScale = Drawing::unitsScale(units(), mNewItem->units());

		painter->save();
		painter->translate(mNewItem->pos());
		painter->scale(itemScale, itemScale);
		mNewItem->render(painter, styleOptions);
		painter->restore();
	}

	// Draw item points
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
	{
		itemPoints = (*itemIter)->points();
		for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
			drawItemPoint(painter, styleOptions, *pointIter);
	}

	// Draw hotpoints
	items = mSelectedItems;
	if (mNewItem) items.prepend(mNewItem);

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		itemPoints = (*itemIter)->points();

		for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
		{
			for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
			{
				if ((*itemIter) != (*otherItemIter))
				{
					otherItemPoints = (*otherItemIter)->points();

					for(auto otherItemPointIter = otherItemPoints.begin();
						otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
					{
						if ((*pointIter)->shouldConnect(*otherItemPointIter, placeMode))
							drawHotpoint(painter, styleOptions, *pointIter);
					}
				}
			}
		}
	}

	// Draw rubber band
	if (mMouseState == MouseRubberBand)
	{
		drawRubberBand(painter, styleOptions,
			Drawing::rectFromPoints(mLastMouseEvent.scenePos(), mLastMouseEvent.buttonDownScenePos()));
	}

	Q_UNUSED(rect);
}

//==================================================================================================

void DrawingScene::drawBorder(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	painter->setBrush(Qt::transparent);
	painter->setPen(QPen(styleOptions.outputBrush(DrawingStyleOptions::Border), mBorderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawRect(rect);
}

void DrawingScene::drawGrid(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	qreal grid = DrawingScene::grid();
    qreal spacing;

	QPen gridPen(styleOptions.outputBrush(DrawingStyleOptions::Grid), 1);
	gridPen.setCosmetic(true);

	painter->setPen(gridPen);

	if (styleOptions.gridStyle() == DrawingStyleOptions::GridDotted)
	{
		if (styleOptions.majorGridSpacing() > 0)
		{
			spacing = grid * styleOptions.majorGridSpacing();
			for(qreal y = ceil(rect.top() / spacing) * spacing; y < rect.bottom(); y += spacing)
			{
				for(qreal x = ceil(rect.left() / spacing) * spacing; x < rect.right(); x += spacing)
					painter->drawPoint(QPointF(x, y));
			}
		}
	}

	if (styleOptions.gridStyle() == DrawingStyleOptions::GridGraphPaper)
	{
		gridPen.setStyle(Qt::DotLine);
		painter->setPen(gridPen);
		if (styleOptions.minorGridSpacing() > 0)
		{
			spacing = grid * styleOptions.minorGridSpacing();
			for(qreal y = ceil(rect.top() / spacing) * spacing; y < rect.bottom(); y += spacing)
				painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
			for(qreal x = ceil(rect.left() / spacing) * spacing; x < rect.right(); x += spacing)
				painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
		}

	}

	if (styleOptions.gridStyle() == DrawingStyleOptions::GridLined || styleOptions.gridStyle() == DrawingStyleOptions::GridGraphPaper)
	{
		gridPen.setStyle(Qt::SolidLine);
		painter->setPen(gridPen);
		if (styleOptions.majorGridSpacing() > 0)
		{
			spacing = grid * styleOptions.majorGridSpacing();
			for(qreal y = ceil(rect.top() / spacing) * spacing;	y < rect.bottom(); y += spacing)
				painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
			for(qreal x = ceil(rect.left() / spacing) * spacing; x < rect.right(); x += spacing)
				painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
		}
	}
}

//==================================================================================================

void DrawingScene::drawItemPoint(QPainter* painter, const DrawingStyleOptions& styleOptions, DrawingItemPoint* point)
{
	if (mView)
	{
		QPoint offset = QPoint(point->size(), point->size());
		QPoint centerPoint = mView->mapFromScene(point->item()->mapToScene(point->pos()));
		QPoint topLeft = centerPoint - offset;
		QPoint bottomRight = centerPoint + offset - QPoint(1, 1);

		QColor color = styleOptions.outputBrush(DrawingStyleOptions::Background).color();
		color.setRed(255 - color.red());
		color.setGreen(255 - color.green());
		color.setBlue(255 - color.blue());

		painter->save();

		painter->resetTransform();
		painter->setRenderHints(QPainter::Antialiasing, false);
		painter->setPen(QPen(color, 1));
		painter->setBrush(styleOptions.outputBrush(DrawingStyleOptions::ResizePoint));

		if (point->isControlPoint())
			painter->drawRect(QRect(topLeft, bottomRight));

		if (point->isConnectionPoint())
		{
			for(int x = 0; x <= 2 * point->size(); x++)
			{
				painter->drawPoint(topLeft.x() + x, bottomRight.y() + 1 - x);
				painter->drawPoint(topLeft.x() + x, topLeft.y() + x);
			}
		}

		painter->restore();
	}
}

void DrawingScene::drawHotpoint(QPainter* painter, const DrawingStyleOptions& styleOptions, DrawingItemPoint* point)
{
	if (mView)
	{
		QPoint offset = QPoint(2 * point->size(), 2 * point->size());
		QPoint centerPoint = mView->mapFromScene(point->item()->mapToScene(point->pos()));
		QPoint topLeft = centerPoint - offset;
		QPoint bottomRight = centerPoint + offset - QPoint(1, 1);

		painter->save();

		painter->resetTransform();
		painter->setRenderHints(QPainter::Antialiasing, false);
		painter->setBrush(styleOptions.outputBrush(DrawingStyleOptions::Hotpoint));
		painter->setPen(QPen(painter->brush(), 1));

		painter->drawEllipse(QRect(topLeft, bottomRight));

		painter->restore();
	}
}

void DrawingScene::drawRubberBand(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rubberBandRect)
{
	QPen rubberBandPen(styleOptions.outputBrush(DrawingStyleOptions::RubberBand), 1);
	rubberBandPen.setCosmetic(true);

	painter->setBrush(rubberBandPen.brush());
	painter->setPen(rubberBandPen);
	painter->drawRect(rubberBandRect);
}

//==================================================================================================

void DrawingScene::writeXmlAttributes(QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeAttribute("sceneRect", Drawing::rectToString(mSceneRect));
	xmlWriter.writeAttribute("contentsRect", Drawing::rectToString(mContentsRect));
	xmlWriter.writeAttribute("height", QString::number(mContentsRect.height()));
	xmlWriter.writeAttribute("units", QString::number((quint16)units()));

	xmlWriter.writeAttribute("borderWidth", QString::number(borderWidth()));
	xmlWriter.writeAttribute("grid", QString::number(grid()));

	xmlWriter.writeAttribute("selectionMode", QString::number((quint32)selectionMode()));
	xmlWriter.writeAttribute("forceItemsInside", (isForcingItemsInside()) ? "true" : "false");
}

void DrawingScene::writeXmlChildElements(QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("items");
	DrawingItem::writeItemsToXml(xmlWriter, items());
	xmlWriter.writeEndElement();
}

void DrawingScene::readXmlAttributes(QXmlStreamReader& xmlReader)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	if (attributes.hasAttribute("sceneRect"))
		setSceneRect(Drawing::rectFromString(attributes.value("sceneRect").toString()));
	if (attributes.hasAttribute("contentsRect"))
		setContentsRect(Drawing::rectFromString(attributes.value("contentsRect").toString()));

	if (attributes.hasAttribute("units"))
		setUnits((DrawingUnits)attributes.value("units").toString().toUShort());

	if (attributes.hasAttribute("borderWidth"))
		setBorderWidth(attributes.value("borderWidth").toString().toDouble());

	if (attributes.hasAttribute("grid"))
		setGrid(attributes.value("grid").toString().toDouble());

	if (attributes.hasAttribute("selectionMode"))
		setSelectionMode((Qt::ItemSelectionMode)attributes.value("selectionMode").toString().toUInt());

	if (attributes.hasAttribute("forceItemsInside"))
		setForcingItemsInside(attributes.value("forceItemsInside").toString().toLower() == "true");
}

void DrawingScene::readXmlChildElement(QXmlStreamReader& xmlReader)
{
	if (xmlReader.name() == "items")
	{
		QList<DrawingItem*> items = DrawingItem::readItemsFromXml(xmlReader);
		while (!items.isEmpty()) addItem(items.takeFirst());
	}
	else xmlReader.skipCurrentElement();
}
