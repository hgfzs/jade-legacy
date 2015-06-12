/* DrawingScene.h
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

#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <DrawingTypes.h>

/* The DrawingScene class provides a surface for managing a large number of 2D graphical
 * items.
 *
 * Adding New Items to the Scene
 * =============================
 *
 * When a valid DrawingItem is set on the scene using setNewItem(...), the DrawingScene enters a
 * mode that allows the user to add new items by clicking the mouse.  The behavior is dependent on
 * the PlaceType of the DrawingItem:
 *
 * \li DrawingItem::PlaceMouseUp: The new item is "attached" to the mouse cursor.  Each time the
 *     mouse button is released, a copy of the item is added to the scene, placed at the current
 *     location of the cursor.
 * \li DrawingItem::PlaceMouseDownAndUp: A mouse press event sets the location of the item's first
 *     DrawingItemPoint within the scene.  A mouse release event sets the location of the item's
 *     second DrawingItemPoint and causes the copy of the item to be added to the scene.
 *     Subsequent mouse press/release events may be used to add additional items to the scene.
 * \li DrawingItem::PlaceTwoClick:  Two mouse release events are needed to add the item to the
 *     scene.  The first mouse release event sets the location of the item's first
 *     DrawingItemPoint within the scene.  The second mouse release event sets the location of the
 *     item's second DrawingItemPoint and causes the copy of the item to be added to the scene.
 *     Subsequent pairs of mouse release events may be used to add additional items to the scene.
 *
 * Note that PlaceMouseDownAndUp and PlaceTwoClick are only useful for items whose geometry can be
 * described adequately by the first two DrawingItemPoints of the item.  Examples of this are lines
 * and rectangles.  More complex items should typically use PlaceMouseUp as the PlaceType; users
 * can customize the shape of the item later after it has been added to the scene.
 *
 * The user can exit the new item mode by right-clicking in the scene.  The new item mode can be
 * exited programmatically by calling setNewItem with an invalid DrawingItem (nullptr) as the
 * argument.
 */
class DrawingScene : public QObject
{
	Q_OBJECT

	friend class DrawingView;

public:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	DrawingView* mView;

	DrawingUnits mUnits;
	QRectF mSceneRect;
	QRectF mContentsRect;
	qreal mBorderWidth;
	qreal mGrid;

	Qt::ItemSelectionMode mSelectionMode;
	bool mForcingItemsInside;

	QList<DrawingItem*> mItems;
	QList<DrawingItem*> mSelectedItems;
	QPointF mSelectionCenter;
	DrawingItem* mNewItem;

	QUndoStack mUndoStack;

	DrawingMouseEvent mLastMouseEvent;
	MouseState mMouseState;
	DrawingItem* mMouseDownItem;
	QPointF mMouseDownItemPos;
	int mNewClickCount;
	int mConsecutivePastes;

public:
	DrawingScene();
	virtual ~DrawingScene();

	DrawingView* view() const;

	// Basic settings
	void setUnits(DrawingUnits units);
	void updateUnits(DrawingUnits units);
	DrawingUnits units() const;

	void setSceneRect(const QRectF& rect);
	void setContentsRect(const QRectF& rect);
	void setBorderWidth(qreal borderWidth);
	QRectF sceneRect() const;
	QRectF contentsRect() const;
	qreal borderWidth() const;

	void setGrid(qreal grid);
	qreal grid() const;
	qreal roundToGrid(qreal value) const;
	QPointF roundToGrid(const QPointF& scenePos) const;

	void setSelectionMode(Qt::ItemSelectionMode mode);
	void setForcingItemsInside(bool inside);
	Qt::ItemSelectionMode selectionMode() const;
	bool isForcingItemsInside() const;

	// Items
	void addItem(DrawingItem* item);
	void insertItem(int index, DrawingItem* item);
	void removeItem(DrawingItem* item);
	void clearItems();
	QList<DrawingItem*> items() const;
	int numberOfItems() const;
	int itemIndex(DrawingItem* item) const;
	bool containsItem(DrawingItem* item) const;
	void reorderItems(const QList<DrawingItem*>& items);

	QList<DrawingItem*> items(const QRectF& sceneRect) const;
	QList<DrawingItem*> childItems(const QRectF& sceneRect) const;
	DrawingItem* itemAt(const QPointF& scenePos) const;
	DrawingItem* childAt(const QPointF& scenePos) const;
	QRectF itemsRect() const;
	QRectF itemsShapeRect() const;

	void setNewItem(DrawingItem* item);
	DrawingItem* newItem() const;

	// Selection
	void selectItem(DrawingItem* item);
	void deselectItem(DrawingItem* item);
	void selectItems(const QRectF& sceneRect);
	QList<DrawingItem*> selectedItems() const;
	int numberOfSelectedItems() const;

	// Undo
	void setUndoLimit(int undoLimit);
	void setClean();
	void pushUndo(QUndoCommand* command);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;

	// Events
	DrawingItem* mouseDownItem() const;
	DrawingMouseEvent lastMouseEvent() const;

public slots:
	void undo();
	void redo();

	void cut();
	void copy();
	void paste();

	void selectAll();
	void deselectAll();

	void rotateSelection();
	void rotateBackSelection();
	void flipSelection();
	void deleteSelection();

	void sendBackward();
	void bringForward();
	void sendToBack();
	void bringToFront();

	void insertItemPoint();
	void removeItemPoint();

	void group();
	void ungroup();

signals:
	void cleanChanged(bool clean);
	void canRedoChanged(bool canRedo);
	void canUndoChanged(bool canUndo);

	void numberOfItemsChanged(int itemCount);
	void selectionChanged();

protected slots:
	void updateSelectionCenter();

protected:
	bool itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;

	virtual void contextMenuEvent(DrawingMouseEvent& event);
	virtual void mouseDoubleClickEvent(DrawingMouseEvent& event);
	virtual void mousePressEvent(DrawingMouseEvent& event);
	virtual void mouseMoveEvent(DrawingMouseEvent& event);
	virtual void mouseReleaseEvent(DrawingMouseEvent& event);

	virtual void newMousePressEvent(DrawingMouseEvent& event);
	virtual void newMouseMoveEvent(DrawingMouseEvent& event);
	virtual void newMouseReleaseEvent(DrawingMouseEvent& event);

	void addItems(const QList<DrawingItem*>& items, DrawingItemPlaceMode placeMode, QUndoCommand* command = nullptr);
	void addItems(DrawingItem* item, DrawingItemPlaceMode placeMode, QUndoCommand* command = nullptr);
	void removeItems(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void removeItems(DrawingItem* item, QUndoCommand* command = nullptr);

	void moveItems(const QList<DrawingItem*>& items, const QPointF& deltaPos,
		DrawingItemPlaceMode placeMode, QUndoCommand* command = nullptr);
	void moveItems(DrawingItem* item, const QPointF& deltaPos,
		DrawingItemPlaceMode placeMode, QUndoCommand* command = nullptr);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos,
		DrawingItemPlaceMode placeMode, bool disconnect, QUndoCommand* command = nullptr);

	void rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);

	void connectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);
	void disconnectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);

	void placeItems(const QList<DrawingItem*>& items, DrawingItemPlaceMode placeMode, QUndoCommand* command);
	void placeItems(DrawingItem* item, DrawingItemPlaceMode placeMode, QUndoCommand* command);
	void unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void unplaceItems(DrawingItem* item, QUndoCommand* command);

	void tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void tryToMaintainConnections(DrawingItem* item, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command);

	QPointF tryMove(const QList<DrawingItem*>& items, const QPointF& deltaPos);
	QPointF tryResize(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	bool tryRotate(const QList<DrawingItem*>& items, const QPointF& scenePos);
	bool tryRotateBack(const QList<DrawingItem*>& items, const QPointF& scenePos);
	bool tryFlip(const QList<DrawingItem*>& items, const QPointF& scenePos);
	bool keepItemsInside(const QList<DrawingItem*>& items);

	virtual void drawBackground(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);
	virtual void drawItems(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);
	virtual void drawForeground(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);

	virtual void drawBorder(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);
	virtual void drawGrid(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);

	virtual void drawItemPoint(QPainter* painter, const DrawingStyleOptions& styleOptions, DrawingItemPoint* point);
	virtual void drawHotpoint(QPainter* painter, const DrawingStyleOptions& styleOptions, DrawingItemPoint* point);
	virtual void drawRubberBand(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rubberBandRect);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter);
	virtual void writeXmlChildElements(QXmlStreamWriter& xmlWriter);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader);
	virtual void readXmlChildElement(QXmlStreamReader& xmlReader);
};

#endif
