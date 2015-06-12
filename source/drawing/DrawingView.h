/* DrawingView.h
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

#ifndef DRAWINGVIEW_H
#define DRAWINGVIEW_H

#include <DrawingItemFactory.h>

class DrawingView : public QAbstractScrollArea
{
	Q_OBJECT

public:
	enum Mode { DefaultMode, ScrollMode, ZoomMode, NewMode };
	enum ViewportAnchor { NoAnchor, AnchorViewCenter, AnchorUnderMouse };

	static DrawingItemFactory itemFactory;

private:
	DrawingScene* mScene;

	DrawingStyleOptions mStyleOptions;
	Qt::Alignment mAlignment;
	ViewportAnchor mViewportAnchor;

	bool mRoundMousePositionText;

	qreal mZoomLevel;
	Mode mMode;

	QPointF mViewCenter;
	QPointF mMouseScenePos;
	DrawingMouseEvent mMouseEvent;

public:
	DrawingView();
	virtual ~DrawingView();

	void setScene(DrawingScene* scene);
	DrawingScene* scene() const;

	void setStyleOptions(const DrawingStyleOptions& styleOptions);
	void setAlignment(Qt::Alignment alignment);
	void setViewportAnchor(ViewportAnchor anchor);
	DrawingStyleOptions styleOptions() const;
	Qt::Alignment alignment() const;
	ViewportAnchor viewportAnchor() const;

	void setRoundMousePositionText(bool round);
	bool shouldRoundMousePositionText() const;

	qreal zoomLevel() const;
	Mode mode() const;

	QPointF mapToScene(const QPoint& screenPos) const;
	QRectF mapToScene(const QRect& screenRect) const;
	QPoint mapFromScene(const QPointF& scenePos) const;
	QRect mapFromScene(const QRectF& sceneRect) const;
	QRectF visibleRect() const;

	void setClean();
	bool isClean() const;

	// Render
	virtual void render(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect);

public slots:
	virtual void undo();
	virtual void redo();

	virtual void cut();
	virtual void copy();
	virtual void paste();

	virtual void selectAll();
	virtual void deselectAll();

	virtual void rotateSelection();
	virtual void rotateBackSelection();
	virtual void flipSelection();
	virtual void deleteSelection();

	virtual void group();
	virtual void ungroup();

	virtual void sendBackward();
	virtual void bringForward();
	virtual void sendToBack();
	virtual void bringToFront();

	virtual void insertItemPoint();
	virtual void removeItemPoint();

	virtual void zoomIn();
	virtual void zoomOut();
	virtual void zoomFit(const QRectF& rect = QRectF());
	virtual void zoomToLevel(qreal level, bool adjustAnchor = false);

	virtual void centerOn(const QPointF& scenePos);

	virtual void setDefaultMode();
	virtual void setZoomMode();
	virtual void setScrollMode();
	virtual void setNewMode(DrawingItem* item);
	virtual void setNewMode(const QString& key);

	virtual void update();
	virtual void updateScrollBars();

signals:
	void modeChanged(DrawingView::Mode mode);
	void zoomLevelChanged(qreal level);
	void cleanChanged(bool clean);
	void numberOfItemsChanged(int itemCount);
	void mousePositionChanged(const QString& text);

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

	virtual void writeXmlAttributes(QXmlStreamWriter& xmlWriter);
	virtual void writeXmlChildElements(QXmlStreamWriter& xmlWriter);
	virtual void readXmlAttributes(QXmlStreamReader& xmlReader);
	virtual void readXmlChildElement(QXmlStreamReader& xmlReader);

private:
	void setScale(qreal scale);
	qreal scale() const;

	void beginScrollUpdate();
	void endScrollUpdate(bool adjustAnchor);
	void updateMousePosition(bool showScroll);
};

#endif
