/* DrawingItem.h
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

#ifndef DRAWINGITEM_H
#define DRAWINGITEM_H

#include <DrawingTypes.h>

/* The DrawingItem class is the base class for all graphical items in a DrawingScene.
 *
 * It provides a light-weight foundation for writing your own custom items, similar to
 * QGraphicsItem.
 *
 * Item Coordinates
 * ================
 *
 * The position of the DrawingItem is specified in terms of its parent units.  Typically this is the
 * item's DrawingScene, but it may also be its parent DrawingItem.
 *
 * All other methods in DrawingItem refer to the item's local coordinate system.  The item's
 * position in the coordinate system of its parent maps to the origin (0,0) in local item
 * coordinates.  All transformations are applied around this origin point.  Supported
 * transformations include:
 * \li units - An item may be expressed in different units than its parent.  The appropriate scaling factor between units systems is automatically applied.
 * \li rotation - An item may be rotated around its origin.  The only supported rotation angles are 0, 90, 180, and 270.  The current rotation angle can be obtained from rotationAngle().
 * \li flipped - An item may be flipped horizontally around its origin.  The current flipping can be obtained from isFlipped().
 *
 * Some items have units that should always match the parent.  For these kinds of items, set the
 * matchUnitsWithParent property to true.  Whenever the parent's units are updated, it will
 * check its children for this flag and automatically update each child accordingly.
 *
 * Tips for Creating Custom Items
 * ==============================
 *
 * boundingRect, paint
 * copy, uniqueKey
 * register with ItemFactory
 * placeType
 * flags
 * writeXmlAttributes, readXmlAttributes, copy constructor
 * drawingItemChange (no need to overload itemChange, relevant changes are forwarded)
 *
 * Any item properties should be saved in derived class writeXmlAttributes, loaded in derived class readXmlAttributes
 * Any item children should be saved in derived class writeXmlChildElements, loaded in derived class readXmlChildElements
 */
class DrawingItem
{
	friend class DrawingScene;
	friend class DrawingView;

public:
	enum Flag { CanMove = 0x01, CanRotate = 0x02, CanFlip = 0x04, CanResize = 0x08,
		CanInsertRemovePoints = 0x10, MatchUnitsWithParent = 0x1000, UsePalette = 0x2000 };
	Q_DECLARE_FLAGS(Flags, Flag)

	enum PlaceType { PlaceMouseUp, PlaceMouseDownAndUp, PlaceTwoClick };

	enum Reason { PositionChange, UnitsChange, SelectedChange, VisibleChange, PropertyChange,
				  AddNewItemToScene, UserChange };

private:
	DrawingScene* mScene;

	QPointF mPosition;
	DrawingUnits mUnits;

	Flags mFlags;
	PlaceType mPlaceType;

	QList<DrawingItemPoint*> mPoints;
	DrawingItemPoint* mSelectedPoint;

	QHash<QString, QVariant> mProperties;

	QList<DrawingItem*> mChildren;
	DrawingItem* mParent;

	bool mSelected;
	bool mVisible;

	qreal mRotationAngle;
	bool mFlipped;

public:
	DrawingItem();
	DrawingItem(const DrawingItem& item);
	virtual ~DrawingItem();

	virtual DrawingItem* copy() const = 0;
	virtual QString uniqueKey() const = 0;

	DrawingScene* scene() const;
	DrawingView* view() const;

	// Selectors
	void setPos(const QPointF& parentPos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setUnits(DrawingUnits units);
	DrawingUnits units() const;

	void setFlags(Flags flags);
	Flags flags() const;
	bool canMove() const;
	bool canRotate() const;
	bool canFlip() const;
	bool canResize() const;
	bool canInsertRemovePoints() const;
	bool shouldMatchUnitsWithParent() const;
	bool shouldUsePalette() const;

	void setPlaceType(PlaceType type);
	PlaceType placeType() const;

	// Properties
	void addProperty(const QString& property, const QVariant& value);
	void removeProperty(const QString& property);
	void clearProperties();
	int numberOfProperties() const;
	bool containsProperty(const QString& property) const;
	QStringList propertiesList() const;
	QList<QVariant> propertyValues() const;

	void setPropertyValue(const QString& property, const QVariant& value);
	QVariant propertyValue(const QString& property) const;

	void setProperties(const QHash<QString, QVariant>& properties);
	QHash<QString, QVariant> properties() const;

	// Points
	void addPoint(DrawingItemPoint* itemPoint);
	void insertPoint(int index, DrawingItemPoint* itemPoint);
	void removePoint(DrawingItemPoint* itemPoint);
	void clearPoints();
	QList<DrawingItemPoint*> points() const;
	int numberOfPoints() const;
	bool containsPoint(DrawingItemPoint* itemPoint) const;
	DrawingItemPoint* point(int index) const;

	DrawingItemPoint* pointAt(const QPointF& itemPos) const;
	DrawingItemPoint* pointNearest(const QPointF& itemPos) const;

	bool hasSelectedPoint() const;
	DrawingItemPoint* selectedPoint() const;

	// Children
	void addChild(DrawingItem* child);
	void insertChild(int index, DrawingItem* child);
	void removeChild(DrawingItem* child);
	void clearChildren();
	QList<DrawingItem*> children() const;
	int numberOfChildren() const;
	bool containsChild(DrawingItem* child) const;
	DrawingItem* child(int index) const;

    DrawingItem* parent() const;

	// Transformations and status
	void setSelected(bool select);
	void setVisible(bool visible);
	bool isSelected() const;
	bool isVisible() const;

	void setRotationAngle(qreal angle);
	void setFlipped(bool flipped);
	qreal rotationAngle() const;
	bool isFlipped() const;

	// Coordinate mapping
	QPointF mapToParent(const QPointF& itemPos) const;
	QPointF mapFromParent(const QPointF& parentPos) const;
	QRectF mapToParent(const QRectF& itemRect) const;
	QRectF mapFromParent(const QRectF& parentRect) const;

	QPointF mapToScene(const QPointF& itemPos) const;
	QPointF mapFromScene(const QPointF& scenePos) const;
	QRectF mapToScene(const QRectF& itemRect) const;
	QRectF mapFromScene(const QRectF& sceneRect) const;

	// Description
	virtual QRectF boundingRect() const = 0;
	virtual QPainterPath shape() const;
	virtual QPointF centerPos() const;
	virtual bool isSuperfluous() const;

	// Render
	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions) = 0;

	// Transformations
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& parentPos);
	virtual void rotateItem(const QPointF& parentPos);
	virtual void rotateBackItem(const QPointF& parentPos);
	virtual void flipItem(const QPointF& parentPos);

	virtual void insertItemPoint(int index, DrawingItemPoint* itemPoint);
	virtual void removeItemPoint(DrawingItemPoint* itemPoint);
	virtual bool canInsertPoint(DrawingItemPoint* itemPoint) const;
	virtual bool canRemovePoint(DrawingItemPoint* itemPoint) const;
	virtual int insertPointIndex(DrawingItemPoint* itemPoint) const;

protected:
	virtual void contextMenuEvent(DrawingMouseEvent& event);
	virtual void mouseDoubleClickEvent(DrawingMouseEvent& event);
	virtual void mousePressEvent(DrawingMouseEvent& event);
	virtual void mouseMoveEvent(DrawingMouseEvent& event);
	virtual void mouseReleaseEvent(DrawingMouseEvent& event);

	virtual bool createEvent(DrawingView* view);
	virtual QVariant aboutToChangeEvent(Reason reason, const QVariant& value);
	virtual void changedEvent(Reason reason, const QVariant& value);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void writeXmlChildElements(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);
	virtual void readXmlChildElement(QXmlStreamReader& xmlReader, const QList<DrawingItem*>& items);

protected:
	void setupPainter(QPainter* painter, const DrawingStyleOptions& styleOptions,
		const QPen& itemPen, const QBrush& itemBrush = Qt::transparent);
	QPainterPath itemShapeFromPath(const QPainterPath& path, const QPen& pen, bool adjustOutline = true) const;
	qreal adjustOutlineForView(qreal penWidth) const;
	void adjustReferencePoint();

	DrawingItem& operator=(const DrawingItem& item);

public:
	static QList<DrawingItem*> copyItems(const QList<DrawingItem*>& items);

	static void writeItemsToXml(QXmlStreamWriter& xmlWriter, const QList<DrawingItem*>& items);
	static QList<DrawingItem*> readItemsFromXml(QXmlStreamReader& xmlReader);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItem::Flags)

#endif
