/* DrawingUndo.cpp
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

#include <DrawingUndo.h>
#include <DrawingScene.h>
#include <DrawingItem.h>
#include <DrawingItemPoint.h>

DrawingUndoCommand::DrawingUndoCommand(const QString& title, QUndoCommand* parent) :
	QUndoCommand(title, parent) { }

DrawingUndoCommand::DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent) :
	QUndoCommand(command.text(), parent)
{
	QList<QUndoCommand*> otherChildren;

	for(int i = 0; i < command.childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command.child(i)));

	for(auto otherChildIter = otherChildren.begin(); otherChildIter != otherChildren.end(); otherChildIter++)
	{
		switch ((*otherChildIter)->id())
		{
		case ItemResizeType:
			new DrawingResizeItemCommand(*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
			break;
		case PointConnectType:
			new DrawingItemPointConnectCommand(*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
			break;
		case PointDisconnectType:
			new DrawingItemPointDisconnectCommand(*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
			break;
		default:
			break;
		}
	}
}

DrawingUndoCommand::~DrawingUndoCommand() { }

void DrawingUndoCommand::mergeChildren(const QUndoCommand* command)
{
	bool mergeSuccess;
	QList<QUndoCommand*> children, otherChildren;

	for(int i = 0; i < childCount(); i++)
		children.append(const_cast<QUndoCommand*>(child(i)));
	for(int i = 0; i < command->childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command->child(i)));

	for(auto otherChildIter = otherChildren.begin(); otherChildIter != otherChildren.end(); otherChildIter++)
	{
		mergeSuccess = false;
		for(auto childIter = children.begin(); childIter != children.end(); childIter++)
			mergeSuccess = ((*childIter)->mergeWith(*otherChildIter) && mergeSuccess);

		if (!mergeSuccess)
		{
			switch ((*otherChildIter)->id())
			{
			case ItemResizeType:
				new DrawingResizeItemCommand(*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
				break;
			case PointConnectType:
				new DrawingItemPointConnectCommand(*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
				break;
			case PointDisconnectType:
				new DrawingItemPointDisconnectCommand(*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
				break;
			default:
				break;
			}
		}
	}
}

//==================================================================================================

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingScene* scene, DrawingItem* item,
	QUndoCommand* parent) : DrawingUndoCommand("Add Item", parent)
{
	mScene = scene;
	mItems.append(item);
	mUndone = true;
}

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingScene* scene, const QList<DrawingItem*>& items,
	QUndoCommand* parent) : DrawingUndoCommand("Add Items", parent)
{
	mScene = scene;
	mItems = items;
	mUndone = true;
}

DrawingAddItemsCommand::~DrawingAddItemsCommand()
{
	if (mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

int DrawingAddItemsCommand::id() const
{
	return AddItemsType;
}

void DrawingAddItemsCommand::redo()
{
	mUndone = false;
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mScene->addItem(*itemIter);
	DrawingUndoCommand::redo();
}

void DrawingAddItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mScene->removeItem(*itemIter);
	mUndone = true;
}

//==================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingScene* scene, DrawingItem* item,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Item", parent)
{
	mScene = scene;
	if (item) mItems.append(item);
	mUndone = true;
}

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingScene* scene, const QList<DrawingItem*>& items,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Items", parent)
{
	mScene = scene;
	mItems = items;
	mUndone = true;
}

DrawingRemoveItemsCommand::~DrawingRemoveItemsCommand()
{
	if (!mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

int DrawingRemoveItemsCommand::id() const
{
	return RemoveItemsType;
}

void DrawingRemoveItemsCommand::redo()
{
	mUndone = false;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		mItemIndex[*itemIter] = mScene->itemIndex(*itemIter);
		mScene->removeItem(*itemIter);
	}

	DrawingUndoCommand::redo();
}

void DrawingRemoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mScene->insertItem(mItemIndex[*itemIter], *itemIter);

	mUndone = true;
}

//==================================================================================================

DrawingMoveItemsDeltaCommand::DrawingMoveItemsDeltaCommand(DrawingItem* item,
	const QPointF& deltaPosition, bool final, QUndoCommand* parent) : DrawingUndoCommand("Move Item", parent)
{
	mItems.append(item);
	mDeltaPosition = deltaPosition;
	mFinalMove = final;
}

DrawingMoveItemsDeltaCommand::DrawingMoveItemsDeltaCommand(const QList<DrawingItem*>& items,
	const QPointF& deltaPosition, bool final, QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mItems = items;
	mDeltaPosition = deltaPosition;
	mFinalMove = final;
}

DrawingMoveItemsDeltaCommand::~DrawingMoveItemsDeltaCommand() { }

int DrawingMoveItemsDeltaCommand::id() const
{
	return MoveItemsDeltaType;
}

bool DrawingMoveItemsDeltaCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == MoveItemsDeltaType)
	{
		const DrawingMoveItemsDeltaCommand* moveCommand =
			static_cast<const DrawingMoveItemsDeltaCommand*>(command);

		if (mItems == moveCommand->mItems && !mFinalMove)
		{
			mDeltaPosition += moveCommand->mDeltaPosition;
			mFinalMove = moveCommand->mFinalMove;
			mergeChildren(moveCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingMoveItemsDeltaCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->setPos((*itemIter)->pos() + mDeltaPosition);

	DrawingUndoCommand::redo();
}

void DrawingMoveItemsDeltaCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->setPos((*itemIter)->pos() - mDeltaPosition);
}

//==================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingItemPoint* point, const QPointF& scenePos,
	bool final, QUndoCommand* parent) :	DrawingUndoCommand("Resize Item", parent)
{
	mPoint = point;
	mScenePos = scenePos;
	mOriginalScenePos = mPoint->item()->mapToScene(mPoint->pos());
	mFinalMove = final;
}

DrawingResizeItemCommand::DrawingResizeItemCommand(const DrawingResizeItemCommand& command,
	QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint = command.mPoint;
	mScenePos = command.mScenePos;
	mOriginalScenePos = command.mOriginalScenePos;
	mFinalMove = command.mFinalMove;
}

DrawingResizeItemCommand::~DrawingResizeItemCommand() { }

int DrawingResizeItemCommand::id() const
{
	return ItemResizeType;
}

bool DrawingResizeItemCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == ItemResizeType)
	{
		const DrawingResizeItemCommand* resizeCommand =
			static_cast<const DrawingResizeItemCommand*>(command);

		if (resizeCommand && mPoint == resizeCommand->mPoint && !mFinalMove)
		{
			mScenePos = resizeCommand->mScenePos;
			mFinalMove = resizeCommand->mFinalMove;
			mergeChildren(resizeCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingResizeItemCommand::redo()
{
	if (mPoint && mPoint->item())
	{
		mPoint->item()->resizeItem(mPoint,
			mPoint->item()->mapToParent(mPoint->item()->mapFromScene(mScenePos)));
	}

	DrawingUndoCommand::redo();
}

void DrawingResizeItemCommand::undo()
{
	DrawingUndoCommand::undo();

	if (mPoint && mPoint->item())
	{
		mPoint->item()->resizeItem(mPoint,
			mPoint->item()->mapToParent(mPoint->item()->mapFromScene(mOriginalScenePos)));
	}
}

//==================================================================================================

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(DrawingItemPoint* point1,
	DrawingItemPoint* point2, QUndoCommand* parent) : DrawingUndoCommand("Connect Points", parent)
{
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(
	const DrawingItemPointConnectCommand& command, QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointConnectCommand::~DrawingItemPointConnectCommand()
{
	mPoint1 = nullptr;
	mPoint2 = nullptr;
}

int DrawingItemPointConnectCommand::id() const
{
	return PointConnectType;
}

void DrawingItemPointConnectCommand::redo()
{
	mPoint1->addTarget(mPoint2);
	mPoint2->addTarget(mPoint1);
	DrawingUndoCommand::redo();
}

void DrawingItemPointConnectCommand::undo()
{
	DrawingUndoCommand::undo();
	mPoint1->removeTarget(mPoint2);
	mPoint2->removeTarget(mPoint1);
}

//==================================================================================================

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(DrawingItemPoint* point1,
	DrawingItemPoint* point2, QUndoCommand* parent) : DrawingUndoCommand("Disconnect Points", parent)
{
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(
	const DrawingItemPointDisconnectCommand& command, QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointDisconnectCommand::~DrawingItemPointDisconnectCommand()
{
	mPoint1 = nullptr;
	mPoint2 = nullptr;
}

int DrawingItemPointDisconnectCommand::id() const
{
	return PointDisconnectType;
}

void DrawingItemPointDisconnectCommand::redo()
{
	mPoint1->removeTarget(mPoint2);
	mPoint2->removeTarget(mPoint1);
	DrawingUndoCommand::redo();
}

void DrawingItemPointDisconnectCommand::undo()
{
	DrawingUndoCommand::undo();
	mPoint1->addTarget(mPoint2);
	mPoint2->addTarget(mPoint1);
}

//==================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingRotateItemsCommand::DrawingRotateItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateItemsCommand::~DrawingRotateItemsCommand() { }

int DrawingRotateItemsCommand::id() const
{
	return RotateItemsType;
}

void DrawingRotateItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
	DrawingUndoCommand::redo();
}

void DrawingRotateItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateBackItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
}

//==================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Back Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Back Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateBackItemsCommand::~DrawingRotateBackItemsCommand() { }

int DrawingRotateBackItemsCommand::id() const
{
	return RotateBackItemsType;
}

void DrawingRotateBackItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateBackItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
	DrawingUndoCommand::redo();
}

void DrawingRotateBackItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
}

//==================================================================================================

DrawingFlipItemsCommand::DrawingFlipItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Flip Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingFlipItemsCommand::DrawingFlipItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Flip Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingFlipItemsCommand::~DrawingFlipItemsCommand() { }

int DrawingFlipItemsCommand::id() const
{
	return FlipItemsType;
}

void DrawingFlipItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->flipItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->flipItem((*itemIter)->mapToParent((*itemIter)->mapFromScene(mScenePos)));
}

//==================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingScene* scene,
	const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent) : DrawingUndoCommand("Reorder Items", parent)
{
	mScene = scene;
	mNewItemOrder = newItemOrder;
	mOriginalItemOrder = mScene->items();
}

DrawingReorderItemsCommand::~DrawingReorderItemsCommand() { }

int DrawingReorderItemsCommand::id() const
{
	return ReorderItemsType;
}

void DrawingReorderItemsCommand::redo()
{
	mScene->reorderItems(mNewItemOrder);
	DrawingUndoCommand::redo();
}

void DrawingReorderItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	mScene->reorderItems(mOriginalItemOrder);
}

//==================================================================================================

DrawingItemInsertPointCommand::DrawingItemInsertPointCommand(DrawingItem* item, DrawingItemPoint* point,
	int pointIndex, QUndoCommand* parent) : DrawingUndoCommand("Insert Point", parent)
{
	mItem = item;
	mPoint = point;
	mPointIndex = pointIndex;
	mUndone = true;
}

DrawingItemInsertPointCommand::~DrawingItemInsertPointCommand()
{
	if (mUndone) delete mPoint;
}

int DrawingItemInsertPointCommand::id() const
{
	return InsertItemPointType;
}

void DrawingItemInsertPointCommand::redo()
{
	mItem->insertItemPoint(mPointIndex, mPoint);
	mUndone = false;

	DrawingUndoCommand::redo();
}

void DrawingItemInsertPointCommand::undo()
{
	DrawingUndoCommand::undo();

	mPointIndex = mItem->points().indexOf(mPoint);
	mItem->removeItemPoint(mPoint);
	mUndone = true;
}

//==================================================================================================

DrawingItemRemovePointCommand::DrawingItemRemovePointCommand(DrawingItem* item, DrawingItemPoint* point,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Point", parent)
{
	mItem = item;
	mPoint = point;
	mPointIndex = mItem->points().indexOf(mPoint);
	mUndone = true;
}

DrawingItemRemovePointCommand::~DrawingItemRemovePointCommand()
{
	if (!mUndone) delete mPoint;
}

int DrawingItemRemovePointCommand::id() const
{
	return RemoveItemPointType;
}

void DrawingItemRemovePointCommand::redo()
{
	mPointIndex = mItem->points().indexOf(mPoint);
	mItem->removeItemPoint(mPoint);
	mUndone = false;

	DrawingUndoCommand::redo();
}

void DrawingItemRemovePointCommand::undo()
{
	DrawingUndoCommand::undo();

	mItem->insertItemPoint(mPointIndex, mPoint);
	mUndone = true;
}
