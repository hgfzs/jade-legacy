/* DiagramScene.h
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

#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "DiagramProperties.h"

class DiagramView;

class DiagramScene : public DrawingScene
{
	Q_OBJECT

	friend class DiagramView;

private:


public:
	DiagramScene();
	~DiagramScene();

	DiagramView* diagramView() const;

	bool singleItemProperties(DrawingItem* item);
	bool multipleItemProperties(const QList<DrawingItem*>& items);

private:
	void updateProperties(const DiagramProperties& properties, QUndoCommand* command = nullptr);

	void updateItemProperties(DrawingItem* item,
		const QHash<QString,QVariant>& properties, QUndoCommand* command = nullptr);
	void updateItemProperties(const QList<DrawingItem*> items,
		const QHash<QString,QVariant>& properties, QUndoCommand* command = nullptr);
};

#endif
