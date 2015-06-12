/* DrawingView.cpp
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

#include <DrawingView.h>
#include <DrawingScene.h>
#include <DrawingItem.h>

DrawingItemFactory DrawingView::itemFactory;

DrawingView::DrawingView() : QAbstractScrollArea()
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mScene = nullptr;

	mAlignment = Qt::AlignCenter;
	mViewportAnchor = AnchorUnderMouse;

	mRoundMousePositionText = true;

	mZoomLevel = 1.0;
	mMode = DefaultMode;
}

DrawingView::~DrawingView()
{
	setDefaultMode();
	setScene(nullptr);
}

//==================================================================================================

void DrawingView::setScene(DrawingScene* scene)
{
	if (mScene)
	{
		mScene->disconnect(this);
		mScene->mView = nullptr;
	}

	mScene = scene;
	if (mScene)
	{
		mScene->mView = this;

		connect(mScene, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
		connect(mScene, SIGNAL(numberOfItemsChanged(int)), this, SIGNAL(numberOfItemsChanged(int)));
	}
}

DrawingScene* DrawingView::scene() const
{
	return mScene;
}

//==================================================================================================

void DrawingView::setStyleOptions(const DrawingStyleOptions& styleOptions)
{
	mStyleOptions = styleOptions;
}

void DrawingView::setAlignment(Qt::Alignment alignment)
{
	mAlignment = alignment;
}

void DrawingView::setViewportAnchor(ViewportAnchor anchor)
{
	mViewportAnchor = anchor;
}

DrawingStyleOptions DrawingView::styleOptions() const
{
	return mStyleOptions;
}

Qt::Alignment DrawingView::alignment() const
{
	return mAlignment;
}

DrawingView::ViewportAnchor DrawingView::viewportAnchor() const
{
	return mViewportAnchor;
}

//==================================================================================================

void DrawingView::setRoundMousePositionText(bool round)
{
	mRoundMousePositionText = round;
}

bool DrawingView::shouldRoundMousePositionText() const
{
	return mRoundMousePositionText;
}

//==================================================================================================

qreal DrawingView::zoomLevel() const
{
	return mZoomLevel;
}

DrawingView::Mode DrawingView::mode() const
{
	return mMode;
}

//==================================================================================================

QPointF DrawingView::mapToScene(const QPoint& screenPos) const
{
	QPointF scenePos;
	QRectF viewRect = visibleRect();

	scenePos.setX(screenPos.x() / (qreal)maximumViewportSize().width() * viewRect.width() + viewRect.left());
	scenePos.setY(screenPos.y() / (qreal)maximumViewportSize().height() * viewRect.height() + viewRect.top());

	return scenePos;
}

QRectF DrawingView::mapToScene(const QRect& screenRect) const
{
	return QRectF(mapToScene(screenRect.topLeft()), mapToScene(screenRect.bottomRight()));
}

QPoint DrawingView::mapFromScene(const QPointF& scenePos) const
{
	QPoint screenPos;
	QRectF viewRect = visibleRect();

	screenPos.setX((int)(((scenePos.x() - viewRect.left()) / viewRect.width() *
		(qreal)maximumViewportSize().width()) + 0.5));
	screenPos.setY((int)(((scenePos.y() - viewRect.top()) / viewRect.height() *
		(qreal)maximumViewportSize().height()) + 0.5));

	return screenPos;
}

QRect DrawingView::mapFromScene(const QRectF& sceneRect) const
{
	return QRect(mapFromScene(sceneRect.topLeft()), mapFromScene(sceneRect.bottomRight()));
}

QRectF DrawingView::visibleRect() const
{
	QRectF rect;

	if (mScene)
	{
		QScrollBar* horizontalScroll = horizontalScrollBar();
		QScrollBar* verticalScroll = verticalScrollBar();

		qreal lScale = scale();
		QRectF lSceneRect = mScene->sceneRect();

		qreal width = maximumViewportSize().width() / lScale;
		qreal height = maximumViewportSize().height() / lScale;

		qreal left = horizontalScroll->value() / lScale + lSceneRect.left();
		qreal top = verticalScroll->value() / lScale + lSceneRect.top();
		if (horizontalScroll->maximum() == 0)
		{
			if (mAlignment & Qt::AlignHCenter) left += (lSceneRect.width() - width) / 2;
			else if (mAlignment & Qt::AlignRight) left += (lSceneRect.width() - width);
		}
		if (verticalScroll->maximum() == 0)
		{
			if (mAlignment & Qt::AlignVCenter) top += (lSceneRect.height() - height) / 2;
			else if (mAlignment & Qt::AlignBottom) top += (lSceneRect.height() - height);
		}

		rect = QRectF(left, top, width, height);
	}

	return rect;
}

//==================================================================================================

void DrawingView::setClean()
{
	if (mScene) mScene->setClean();
}

bool DrawingView::isClean() const
{
	return (mScene) ? mScene->isClean() : true;
}

//==================================================================================================

void DrawingView::render(QPainter* painter, const DrawingStyleOptions& styleOptions, const QRectF& rect)
{
	if (mScene)
	{
		mScene->drawBackground(painter, styleOptions, rect);
		mScene->drawItems(painter, styleOptions, rect);
		mScene->drawForeground(painter, styleOptions, rect);

		if (mMode == ZoomMode && mMouseEvent.isDragged())
		{
			mScene->drawRubberBand(painter, styleOptions,
				Drawing::rectFromPoints(mMouseEvent.scenePos(), mMouseEvent.buttonDownScenePos()));
		}
	}
}

//==================================================================================================

void DrawingView::undo()
{
	if (mScene) mScene->undo();
	setDefaultMode();
	emit mousePositionChanged("");
	updateScrollBars();
	update();
}

void DrawingView::redo()
{
	if (mScene) mScene->redo();
	setDefaultMode();
	emit mousePositionChanged("");
	updateScrollBars();
	update();
}

//==================================================================================================

void DrawingView::cut()
{
	if (mScene) mScene->cut();
	update();
}

void DrawingView::copy()
{
	if (mScene) mScene->copy();
	update();
}

void DrawingView::paste()
{
	if (mScene) mScene->paste();
	update();
}

//==================================================================================================

void DrawingView::selectAll()
{
	if (mScene) mScene->selectAll();
	update();
}

void DrawingView::deselectAll()
{
	if (mScene) mScene->deselectAll();
	update();
}

//==================================================================================================

void DrawingView::rotateSelection()
{
	if (mScene) mScene->rotateSelection();
	update();
}

void DrawingView::rotateBackSelection()
{
	if (mScene) mScene->rotateBackSelection();
	update();
}

void DrawingView::flipSelection()
{
	if (mScene) mScene->flipSelection();
	update();
}

void DrawingView::deleteSelection()
{
	if (mScene) mScene->deleteSelection();
	update();
}

//==================================================================================================

void DrawingView::group()
{
	if (mScene) mScene->group();
	update();
}

void DrawingView::ungroup()
{
	if (mScene) mScene->ungroup();
	update();
}

//==================================================================================================

void DrawingView::sendBackward()
{
	if (mScene) mScene->sendBackward();
	update();
}

void DrawingView::bringForward()
{
	if (mScene) mScene->bringForward();
	update();
}

void DrawingView::sendToBack()
{
	if (mScene) mScene->sendToBack();
	update();
}

void DrawingView::bringToFront()
{
	if (mScene) mScene->bringToFront();
	update();
}

//==================================================================================================

void DrawingView::insertItemPoint()
{
	if (mScene) mScene->insertItemPoint();
	update();
}

void DrawingView::removeItemPoint()
{
	if (mScene) mScene->removeItemPoint();
	update();
}

//==================================================================================================

void DrawingView::zoomIn()
{
	zoomToLevel(zoomLevel() * qSqrt(2), true);
}

void DrawingView::zoomOut()
{
	zoomToLevel(zoomLevel() * qSqrt(2) / 2, true);
}

void DrawingView::zoomFit(const QRectF& rect)
{
	if (mScene)
	{
		QRectF actualRect = rect;
		if (!actualRect.isValid()) actualRect = mScene->sceneRect();

		qreal scaleX = maximumViewportSize().width() / actualRect.width();
		qreal scaleY = maximumViewportSize().height() / actualRect.height();
		qreal finalScale = qMin(scaleX, scaleY);

		beginScrollUpdate();
		setScale(finalScale);
		endScrollUpdate(false);

		horizontalScrollBar()->setValue((int)(finalScale * (actualRect.left() - mScene->sceneRect().left())));
		verticalScrollBar()->setValue((int)(finalScale * (actualRect.top() - mScene->sceneRect().top())));

		update();

		emit zoomLevelChanged(mZoomLevel);
	}
}

void DrawingView::zoomToLevel(qreal level, bool adjustAnchor)
{
	if (mScene)
	{
		beginScrollUpdate();
		mZoomLevel = level;
		endScrollUpdate(adjustAnchor);
		update();

		emit zoomLevelChanged(mZoomLevel);
	}
}

//==================================================================================================

void DrawingView::centerOn(const QPointF& pos)
{
	if (mScene)
	{
		QRectF lVisibleRect = visibleRect();
		qreal lScale = scale();

		horizontalScrollBar()->setValue(
			(int)(horizontalScrollBar()->value() + (pos.x() - lVisibleRect.center().x()) * lScale));
		verticalScrollBar()->setValue(
			(int)(verticalScrollBar()->value() + (pos.y() - lVisibleRect.center().y()) * lScale));
	}
}

//==================================================================================================

void DrawingView::setDefaultMode()
{
	mMode = DefaultMode;
	setCursor(Qt::ArrowCursor);

	if (mScene)
	{
		mScene->deselectAll();
		mScene->setNewItem(nullptr);
	}

	emit modeChanged(mMode);
}

void DrawingView::setZoomMode()
{
	if (mScene)
	{
		mMode = ZoomMode;
		setCursor(Qt::CrossCursor);

		mScene->deselectAll();
		mScene->setNewItem(nullptr);

		emit modeChanged(mMode);
	}
	else setDefaultMode();
}

void DrawingView::setScrollMode()
{
	if (mScene)
	{
		mMode = ScrollMode;
		setCursor(Qt::OpenHandCursor);

		mScene->deselectAll();
		mScene->setNewItem(nullptr);

		emit modeChanged(mMode);
	}
	else setDefaultMode();
}

void DrawingView::setNewMode(DrawingItem* item)
{
	if (mScene && item)
	{
		mMode = NewMode;
		setCursor(Qt::CrossCursor);

		mScene->deselectAll();
		mScene->setNewItem(item);

		emit modeChanged(mMode);
	}
	else setDefaultMode();
}

void DrawingView::setNewMode(const QString& key)
{
	DrawingItem* item = itemFactory.create(key);

	if (item)
	{
		if (!item->createEvent(this))
		{
			delete item;
			item = nullptr;
		}

		setNewMode(item);
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingView::update()
{
	if (viewport()) viewport()->update();
}

void DrawingView::updateScrollBars()
{
	endScrollUpdate(false);
}

//==================================================================================================

void DrawingView::contextMenuEvent(QContextMenuEvent* event)
{
	if (event)
	{
		mMouseEvent.update(this, event);
		mMouseEvent.ignore();
		event->ignore();

		switch (mMode)
		{
		case ScrollMode:
		case ZoomMode:
		case NewMode:
			setDefaultMode();
			event->accept();
			break;

		case DefaultMode:
			if (mScene)
			{
				mScene->contextMenuEvent(mMouseEvent);
				if (mMouseEvent.isAccepted()) event->accept();
			}
			break;

		default: break;
		}
	}

	update();
}

void DrawingView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event && event->button() == Qt::LeftButton)
	{
		mMouseEvent.update(this, event);
		mMouseEvent.ignore();
		event->ignore();

		switch (mMode)
		{
		case ScrollMode:
		case ZoomMode:
		case NewMode:
			setDefaultMode();
			event->accept();
			break;

		case DefaultMode:
			if (mScene) mScene->mouseDoubleClickEvent(mMouseEvent);
			if (mMouseEvent.isAccepted()) event->accept();
			break;

		default: break;
		}
	}

	update();
}

void DrawingView::mousePressEvent(QMouseEvent* event)
{
	if (event)
	{
		mMouseEvent.update(this, event);
		mMouseEvent.ignore();

		if (event->button() == Qt::LeftButton)
		{
			switch (mMode)
			{
			case ScrollMode:
				setCursor(Qt::ClosedHandCursor);
				event->accept();
				break;

			case ZoomMode:
				event->accept();
				break;

			case NewMode:
			case DefaultMode:
				if (mScene) mScene->mousePressEvent(mMouseEvent);
				if (mMouseEvent.isAccepted()) event->accept();
				break;

			default: break;
			}
		}
	}

	update();
}

void DrawingView::mouseMoveEvent(QMouseEvent* event)
{
	if (event)
	{
		mMouseEvent.update(this, event);
		mMouseEvent.ignore();

		switch (mMode)
		{
		case ScrollMode:
			if (mMouseEvent.isDragged())
			{
				horizontalScrollBar()->setValue(mMouseEvent.buttonDownScrollValue().x() -
					(mMouseEvent.screenPos().x() - mMouseEvent.buttonDownScreenPos().x()));
				verticalScrollBar()->setValue(mMouseEvent.buttonDownScrollValue().y() -
					(mMouseEvent.screenPos().y() - mMouseEvent.buttonDownScreenPos().y()));
			}
			event->accept();
			break;

		case ZoomMode:
			event->accept();
			break;

		case NewMode:
		case DefaultMode:
			if (mScene) mScene->mouseMoveEvent(mMouseEvent);
			if (mMouseEvent.isAccepted()) event->accept();
			break;

		default: break;
		}

		updateMousePosition(true);
	}

	update();
}

void DrawingView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event)
	{
		mMouseEvent.update(this, event);
		mMouseEvent.ignore();

		if (event->button() == Qt::LeftButton)
		{
			switch (mMode)
			{
			case ScrollMode:
				setCursor(Qt::OpenHandCursor);
				event->accept();
				break;

			case ZoomMode:
				if (mMouseEvent.isDragged())
				{
					zoomFit(Drawing::rectFromPoints(
						mMouseEvent.scenePos(), mMouseEvent.buttonDownScenePos()));
					setDefaultMode();
				}
				event->accept();
				break;

			case NewMode:
			case DefaultMode:
				if (mScene) mScene->mouseReleaseEvent(mMouseEvent);
				if (mMouseEvent.isAccepted()) event->accept();
				break;

			default: break;
			}
		}
	}

	updateMousePosition(false);
	update();
}

void DrawingView::keyPressEvent(QKeyEvent* event)
{
	// Base implementation ignores key press events
	Q_UNUSED(event);
}

void DrawingView::keyReleaseEvent(QKeyEvent* event)
{
	// Base implementation ignores key press events
	Q_UNUSED(event);
}

void DrawingView::wheelEvent(QWheelEvent* event)
{
	if (event && (event->modifiers() & Qt::ControlModifier))
	{
		if (event->delta() > 0) zoomIn();
		else if (event->delta() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingView::paintEvent(QPaintEvent* event)
{
	if (mScene)
	{
		QImage image(viewport()->width(), viewport()->height(), QImage::Format_RGB32);
		image.fill(palette().brush(QPalette::Window).color());

		QPainter painter(&image);
		qreal scale = DrawingView::scale();
		QRectF sceneRect = mScene->sceneRect();

		painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
		painter.scale(scale, scale);
		painter.translate(-sceneRect.topLeft());

		// Adjust for alignment
		if (horizontalScrollBar()->maximum() == 0)
		{
			if (mAlignment & Qt::AlignHCenter)
				painter.translate(-(sceneRect.width() - maximumViewportSize().width() / scale) / 2, 0);
			else if (mAlignment & Qt::AlignRight)
				painter.translate(-(sceneRect.width() - maximumViewportSize().width() / scale), 0);
		}
		if (verticalScrollBar()->maximum() == 0)
		{
			if (mAlignment & Qt::AlignVCenter)
				painter.translate(0, -(sceneRect.height() - maximumViewportSize().height() / scale) / 2);
			else if (mAlignment & Qt::AlignBottom)
				painter.translate(0, -(sceneRect.height() - maximumViewportSize().height() / scale));
		}

		render(&painter, mStyleOptions, visibleRect());

		// Draw page outline
		QPen scenePen(Qt::black, 1);
		scenePen.setCosmetic(true);
		painter.setBrush(Qt::transparent);
		painter.setPen(scenePen);
		painter.drawRect(mScene->sceneRect());

		painter.end();

		QPainter widgetPainter(viewport());
		widgetPainter.drawImage(0, 0, image);
	}

	Q_UNUSED(event);
}

void DrawingView::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	endScrollUpdate(false);
}

//==================================================================================================

void DrawingView::writeXmlAttributes(QXmlStreamWriter& xmlWriter)
{
	if (mScene) mScene->writeXmlAttributes(xmlWriter);

	xmlWriter.writeAttribute("alignment", QString::number((quint32)alignment()));
	xmlWriter.writeAttribute("viewportAnchor", QString::number((quint32)viewportAnchor()));

	xmlWriter.writeAttribute("gridStyle", QString::number((quint16)mStyleOptions.gridStyle()));
	xmlWriter.writeAttribute("majorGridSpacing", QString::number(mStyleOptions.majorGridSpacing()));
	xmlWriter.writeAttribute("minorGridSpacing", QString::number(mStyleOptions.minorGridSpacing()));

	xmlWriter.writeAttribute("backgroundColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::Background)));
	xmlWriter.writeAttribute("borderColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::Border)));
	xmlWriter.writeAttribute("gridColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::Grid)));
	xmlWriter.writeAttribute("rubberBandColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::RubberBand)));
	xmlWriter.writeAttribute("resizePointColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::ResizePoint)));
	xmlWriter.writeAttribute("hotpointColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::Hotpoint)));
	xmlWriter.writeAttribute("itemColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::Item)));
	xmlWriter.writeAttribute("alternateItemColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::AlternateItem)));
	xmlWriter.writeAttribute("userColor", Drawing::brushToString(mStyleOptions.brush(DrawingStyleOptions::UserDefined)));
}

void DrawingView::writeXmlChildElements(QXmlStreamWriter& xmlWriter)
{
	if (mScene) mScene->writeXmlChildElements(xmlWriter);
}

void DrawingView::readXmlAttributes(QXmlStreamReader& xmlReader)
{
	QXmlStreamAttributes attributes = xmlReader.attributes();

	if (mScene) mScene->readXmlAttributes(xmlReader);

	if (attributes.hasAttribute("alignment"))
		setAlignment((Qt::Alignment)attributes.value("alignment").toString().toUInt());
	if (attributes.hasAttribute("viewportAnchor"))
		setViewportAnchor((ViewportAnchor)attributes.value("viewportAnchor").toString().toUInt());

	if (attributes.hasAttribute("gridStyle"))
		mStyleOptions.setGridStyle((DrawingStyleOptions::GridStyle)attributes.value("gridStyle").toString().toUShort());
	if (attributes.hasAttribute("majorGridSpacing"))
		mStyleOptions.setMajorGridSpacing(attributes.value("majorGridSpacing").toString().toInt());
	if (attributes.hasAttribute("minorGridSpacing"))
		mStyleOptions.setMinorGridSpacing(attributes.value("minorGridSpacing").toString().toInt());

	if (attributes.hasAttribute("backgroundColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::Background, Drawing::brushFromString(attributes.value("backgroundColor").toString()));
	if (attributes.hasAttribute("borderColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::Border, Drawing::brushFromString(attributes.value("borderColor").toString()));
	if (attributes.hasAttribute("gridColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::Grid, Drawing::brushFromString(attributes.value("gridColor").toString()));
	if (attributes.hasAttribute("rubberBandColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::RubberBand, Drawing::brushFromString(attributes.value("rubberBandColor").toString()));
	if (attributes.hasAttribute("resizePointColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::ResizePoint, Drawing::brushFromString(attributes.value("resizePointColor").toString()));
	if (attributes.hasAttribute("hotpointColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::Hotpoint, Drawing::brushFromString(attributes.value("hotpointColor").toString()));
	if (attributes.hasAttribute("itemColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::Item, Drawing::brushFromString(attributes.value("itemColor").toString()));
	if (attributes.hasAttribute("alternateItemColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::AlternateItem, Drawing::brushFromString(attributes.value("alternateItemColor").toString()));
	if (attributes.hasAttribute("userColor"))
		mStyleOptions.setBrush(DrawingStyleOptions::UserDefined, Drawing::brushFromString(attributes.value("userColor").toString()));
}

void DrawingView::readXmlChildElement(QXmlStreamReader& xmlReader)
{
	if (mScene) mScene->readXmlChildElement(xmlReader);
}

//==================================================================================================

void DrawingView::setScale(qreal scl)
{
	if (mScene)
	{
		qreal zoomLevel = scl;

		switch (mScene->units())
		{
		case UnitsSimpleMM:
		case UnitsMM:
			zoomLevel = scl * 0.2;
			break;
		default:
			zoomLevel = scl * 8.0;
			break;
		}

		mZoomLevel = zoomLevel;
	}
}

qreal DrawingView::scale() const
{
	qreal scale = mZoomLevel;

	if (mScene)
	{
		switch (mScene->units())
		{
		case UnitsSimpleMM:
		case UnitsMM:
			scale = mZoomLevel / 0.2;
			break;
		default:
			scale = mZoomLevel / 8.0;
			break;
		}
	}

	return scale;
}

//==================================================================================================

void DrawingView::beginScrollUpdate()
{
	mMouseScenePos = mapToScene(mapFromGlobal(QCursor::pos()));
	mViewCenter = visibleRect().center();
}

void DrawingView::endScrollUpdate(bool adjustAnchor)
{
	if (mScene)
	{
		QScrollBar* horizontalScroll = horizontalScrollBar();
		QScrollBar* verticalScroll = verticalScrollBar();
		qreal scale = DrawingView::scale();
		QRectF sceneRect = mScene->sceneRect();

		int innerWidth = maximumViewportSize().width();
		int innerHeight = maximumViewportSize().height();
		int widgetWidth = (int)((scale * sceneRect.width()) + 0.5);
		int widgetHeight = (int)((scale * sceneRect.height()) + 0.5);
		QPoint mousePos = mapFromGlobal(QCursor::pos());

		// Adjust the horizontal scroll bar to reflect the new size of viewport()
		if (widgetWidth - innerWidth > 0)
		{
			horizontalScroll->setRange(0, widgetWidth - innerWidth + 1);
			horizontalScroll->setSingleStep(innerWidth / 40);
			horizontalScroll->setPageStep(innerWidth);
		}
		else horizontalScroll->setRange(0, 0);

		// Adjust the vertical scroll bar to reflect the new size of viewport()
		if (widgetHeight - innerHeight > 0)
		{
			verticalScroll->setRange(0, widgetHeight - innerHeight + 1);
			verticalScroll->setSingleStep(innerHeight / 40);
			verticalScroll->setPageStep(innerHeight);
		}
		else verticalScroll->setRange(0, 0);

		if (adjustAnchor)
		{
			switch (mViewportAnchor)
			{
			case AnchorViewCenter:
				centerOn(mViewCenter);
				break;

			case AnchorUnderMouse:
				// Try to keep oldMousePos mapping to the same screen position
				if (rect().contains(mousePos))
				{
					QPointF scenePos = mapToScene(mousePos);
					horizontalScroll->setValue((int)(horizontalScroll->value() +
						(mMouseScenePos.x() - scenePos.x()) * scale));
					verticalScroll->setValue((int)(verticalScroll->value() +
						(mMouseScenePos.y() - scenePos.y()) * scale));
				}
				else centerOn(mViewCenter);
				break;

			default:
				break;
			}
		}
	}

	emit mousePositionChanged("");
}

void DrawingView::updateMousePosition(bool showScroll)
{
	QPointF buttonDownPos, scenePos, difference;
	DrawingUnits units = UnitsMils;

	if (mScene && mRoundMousePositionText)
	{
		buttonDownPos = mScene->roundToGrid(mMouseEvent.buttonDownScenePos());
		scenePos = mScene->roundToGrid(mMouseEvent.scenePos());
		difference = scenePos - buttonDownPos;
		units = mScene->units();
	}
	else
	{
		buttonDownPos = mMouseEvent.buttonDownScenePos();
		scenePos = mMouseEvent.scenePos();
		difference = scenePos - buttonDownPos;
		units = mScene->units();
	}

	if (showScroll && mMouseEvent.isDragged())
	{
		if (mMode != ScrollMode)
		{
			emit mousePositionChanged(
				Drawing::pointToString(buttonDownPos, units) + " - " + Drawing::pointToString(scenePos, units) +
				"; " + QString(QChar(0x394)) + " = " + Drawing::pointToString(difference, units));
		}
	}
	else emit mousePositionChanged(Drawing::pointToString(scenePos, units));
}
