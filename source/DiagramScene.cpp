/* DiagramScene.cpp
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

#include "DiagramScene.h"
#include "DiagramView.h"
#include "DiagramSingleItemPropertiesWidget.h"
#include "DiagramMultipleItemPropertiesWidget.h"

DiagramScene::DiagramScene() : DrawingScene() { }

DiagramScene::~DiagramScene() { }

//==================================================================================================

DiagramView* DiagramScene::diagramView() const
{
	DrawingView* view = DrawingScene::view();
	return (view) ? qobject_cast<DiagramView*>(view) : nullptr;
}

//==================================================================================================

bool DiagramScene::singleItemProperties(DrawingItem* item)
{
	bool dialogAccepted = false;
	DiagramSingleItemPropertiesDialog* propertiesDialog = new DiagramSingleItemPropertiesDialog(item, view());

	if (propertiesDialog->shouldExec())
	{
		dialogAccepted = (propertiesDialog->exec() == QDialog::Accepted);
		if (dialogAccepted)
		{
			QUndoCommand* command = new QUndoCommand("Change Item Properties");

			// First, update item properties
			updateItemProperties(item, propertiesDialog->properties(), command);

			// Move item, if necessary
			if (propertiesDialog->position() != item->pos())
				moveItems(item, propertiesDialog->position() - item->pos(), PlaceStrict, command);

			// Resize item as necessary
			QHash<DrawingItemPoint*, QPointF> pointPositions = propertiesDialog->pointPositions();
			for(auto hashIter = pointPositions.begin(); hashIter != pointPositions.end(); hashIter++)
				resizeItem(hashIter.key(), hashIter.value(), PlaceStrict, true, command);

			pushUndo(command);
		}
	}

	return dialogAccepted;
}

bool DiagramScene::multipleItemProperties(const QList<DrawingItem*>& items)
{
	bool dialogAccepted = false;
	DiagramMultipleItemPropertiesDialog* propertiesDialog = new DiagramMultipleItemPropertiesDialog(items, view());

	if (propertiesDialog->shouldExec())
	{
		dialogAccepted = (propertiesDialog->exec() == QDialog::Accepted);
		if (dialogAccepted) updateItemProperties(items, propertiesDialog->properties());
	}

	return dialogAccepted;
}

//==================================================================================================

void DiagramScene::updateProperties(const DiagramProperties& properties, QUndoCommand* command)
{
	DiagramPropertiesCommand* propertiesCommand = new DiagramPropertiesCommand(diagramView(), properties, command);

	if (isForcingItemsInside())
	{
		QList<DrawingItem*> itemsToDelete;
		QList<DrawingItem*> sceneItems = items();
		QRectF itemRect;

		propertiesCommand->redo();

		// Prepare to delete any items that now fall outside the diagram boundaries
		for(auto iter = sceneItems.begin(); iter != sceneItems.end(); iter++)
		{
			itemRect = Drawing::adjustRectForMinimumSize((*iter)->mapToScene((*iter)->boundingRect()));
			if (!contentsRect().contains(itemRect)) itemsToDelete.append(*iter);
		}

		if (!itemsToDelete.isEmpty()) removeItems(itemsToDelete, propertiesCommand);

		propertiesCommand->undo();
	}

	if (!command) pushUndo(propertiesCommand);
}

//==================================================================================================

void DiagramScene::updateItemProperties(DrawingItem* item,
	const QHash<QString,QVariant>& properties, QUndoCommand* command)
{
	QHash<QString, QVariant> itemProperties = item->properties();

	for(auto hashIter = properties.begin(); hashIter != properties.end(); hashIter++)
	{
		if (itemProperties.contains(hashIter.key()))
			itemProperties[hashIter.key()] = hashIter.value();
	}

	DiagramItemPropertiesCommand* propertiesCommand = new DiagramItemPropertiesCommand(item, itemProperties, command);
	if (!command) pushUndo(propertiesCommand);
}

void DiagramScene::updateItemProperties(const QList<DrawingItem*> items,
	const QHash<QString,QVariant>& properties, QUndoCommand* command)
{
	QUndoCommand* propertiesCommand = new QUndoCommand("Change Item Properties", command);

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		updateItemProperties(*itemIter, properties, propertiesCommand);

	if (!command) pushUndo(propertiesCommand);
}
