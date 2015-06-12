/* DiagramSingleItemPropertiesWidget.cpp
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

#include "DiagramSingleItemPropertiesWidget.h"
#include "ColorButton.h"
#include "EnumComboBox.h"
#include "ImageWidget.h"
#include "PointsListWidget.h"
#include "PositionWidget.h"
#include "UnitsValueEdit.h"

DiagramSingleItemPropertiesWidget::DiagramSingleItemPropertiesWidget(DrawingItem* item) : QWidget()
{
	mItem = item;

	mPositionWidget = nullptr;
	mRectTopLeftWidget = nullptr;
	mRectBottomRightWidget = nullptr;
	mRectCornerRadiusWidget = nullptr;
	mPenStyleCombo = nullptr;
	mPenWidthEdit = nullptr;
	mPenColorButton = nullptr;
	mBrushColorButton = nullptr;
	mFontFamilyCombo = nullptr;
	mFontSizeEdit = nullptr;
	mFontStyleWidget = nullptr;
	mFontBoldButton = nullptr;
	mFontItalicButton = nullptr;
	mFontUnderlineButton = nullptr;
	mFontOverlineButton = nullptr;
	mFontStrikeOutButton = nullptr;
	mTextAlignmentHorizontalCombo = nullptr;
	mTextAlignmentVerticalCombo = nullptr;
	mTextColorButton = nullptr;
	mCaptionEdit = nullptr;
	mStartPointPositionWidget = nullptr;
	mStartArrowStyleCombo = nullptr;
	mStartArrowSizeEdit = nullptr;
	mEndPointPositionWidget = nullptr;
	mEndArrowStyleCombo = nullptr;
	mEndArrowSizeEdit = nullptr;
	mControlPointsWidget = nullptr;
	mPolyPointsWidget = nullptr;
	mImageWidget = nullptr;

	QGroupBox* positionGroup = createPositionGroup();
	QGroupBox* rectGroup = createRectGroup();
	QGroupBox* penBrushGroup = createPenBrushGroup();
	QGroupBox* textGroup = createTextGroup();
	QGroupBox* startPointGroup = createStartPointGroup();
	QGroupBox* endPointGroup = createEndPointGroup();
	QGroupBox* controlPointsGroup = createControlPointsGroup();
	QGroupBox* polyPointsGroup = createPolyPointsGroup();
	QGroupBox* imageGroup = createImageGroup();

	QVBoxLayout* vLayout = new QVBoxLayout();
	if (positionGroup) vLayout->addWidget(positionGroup);
	if (rectGroup) vLayout->addWidget(rectGroup);
	if (penBrushGroup) vLayout->addWidget(penBrushGroup);
	if (textGroup) vLayout->addWidget(textGroup);
	if (startPointGroup) vLayout->addWidget(startPointGroup);
	if (endPointGroup) vLayout->addWidget(endPointGroup);
	if (controlPointsGroup) vLayout->addWidget(controlPointsGroup);
	if (polyPointsGroup) vLayout->addWidget(polyPointsGroup);
	if (imageGroup) vLayout->addWidget(imageGroup);
	vLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(vLayout);
}

DiagramSingleItemPropertiesWidget::~DiagramSingleItemPropertiesWidget() { }

//==================================================================================================

QPointF DiagramSingleItemPropertiesWidget::position() const
{
	return (mPositionWidget) ? mItem->mapToParent(mItem->mapFromScene(mPositionWidget->pos())) : mItem->pos();
}

QHash<DrawingItemPoint*, QPointF> DiagramSingleItemPropertiesWidget::pointPositions() const
{
	QHash<DrawingItemPoint*, QPointF> pointPositions;

	DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItem);
	DrawingRectResizeItem* rectItem = dynamic_cast<DrawingRectResizeItem*>(mItem);
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);
	DrawingPolyItem* polyItem = dynamic_cast<DrawingPolyItem*>(mItem);

	if (rectItem)
	{
		DrawingItemPoint* topLeftPoint = rectItem->point(0);
		DrawingItemPoint* bottomRightPoint = rectItem->point(1);

		QPointF newTopLeftPosition = rectItem->mapFromScene(mRectTopLeftWidget->pos());
		QPointF newBottomRightPosition = rectItem->mapFromScene(mRectBottomRightWidget->pos());;

		if (newTopLeftPosition != topLeftPoint->pos())
			pointPositions[topLeftPoint] = rectItem->mapToParent(newTopLeftPosition);
		if (newBottomRightPosition != bottomRightPoint->pos())
			pointPositions[bottomRightPoint] = rectItem->mapToParent(newBottomRightPosition);
	}
	else if (twoPointItem)
	{
		DrawingItemPoint* startPoint = twoPointItem->startPoint();
		DrawingItemPoint* endPoint = twoPointItem->endPoint();

		QPointF newStartPointPosition = twoPointItem->mapFromScene(mStartPointPositionWidget->pos());
		QPointF newEndPointPosition = twoPointItem->mapFromScene(mEndPointPositionWidget->pos());

		if (newStartPointPosition != startPoint->pos())
			pointPositions[startPoint] = twoPointItem->mapToParent(newStartPointPosition);
		if (newEndPointPosition != endPoint->pos())
			pointPositions[endPoint] = twoPointItem->mapToParent(newEndPointPosition);

		if (curveItem)
		{
			DrawingItemPoint* point;
			QPointF newPointPosition;

			for(int i = 0; i < mControlPointsWidget->numberOfPoints(); i++)
			{
				point = curveItem->point(i+2);
				newPointPosition = curveItem->mapFromScene(mControlPointsWidget->point(i));
				if (point && newPointPosition != point->pos())
					pointPositions[point] = curveItem->mapToParent(newPointPosition);
			}
		}
	}
	else if (polyItem)
	{
		DrawingItemPoint* point;
		QPointF newPointPosition;

		for(int i = 0; i < mPolyPointsWidget->numberOfPoints(); i++)
		{
			point = polyItem->point(i);
			newPointPosition = polyItem->mapFromScene(mPolyPointsWidget->point(i));
			if (point && newPointPosition != point->pos())
				pointPositions[point] = polyItem->mapToParent(newPointPosition);
		}
	}

	return pointPositions;
}

QHash<QString,QVariant> DiagramSingleItemPropertiesWidget::properties() const
{
	QHash<QString,QVariant> properties;

	if (mRectCornerRadiusWidget)
	{
		properties["Corner X-Radius"] = QVariant(mRectCornerRadiusWidget->pos().x());
		properties["Corner Y-Radius"] = QVariant(mRectCornerRadiusWidget->pos().y());
	}

	if (mPenStyleCombo) properties["Pen Style"] = QVariant((unsigned int)mPenStyleCombo->style());
	if (mPenWidthEdit) properties["Pen Width"] = QVariant(mPenWidthEdit->value());
	if (mPenColorButton) properties["Pen Color"] = QVariant(mPenColorButton->color());
	if (mBrushColorButton) properties["Brush Color"] = QVariant(mBrushColorButton->color());

	if (mFontFamilyCombo) properties["Font Family"] = QVariant(mFontFamilyCombo->currentFont().family());
	if (mFontSizeEdit) properties["Font Size"] = QVariant(mFontSizeEdit->value());
	if (mFontBoldButton) properties["Font Bold"] = QVariant(mFontBoldButton->isChecked());
	if (mFontItalicButton) properties["Font Italic"] = QVariant(mFontItalicButton->isChecked());
	if (mFontUnderlineButton) properties["Font Underline"] = QVariant(mFontUnderlineButton->isChecked());
	if (mFontOverlineButton) properties["Font Overline"] = QVariant(mFontOverlineButton->isChecked());
	if (mFontStrikeOutButton) properties["Font Strike-Out"] = QVariant(mFontStrikeOutButton->isChecked());
	if (mTextAlignmentHorizontalCombo) properties["Text Horizontal Alignment"] = QVariant((unsigned int)mTextAlignmentHorizontalCombo->alignment());
	if (mTextAlignmentVerticalCombo) properties["Text Vertical Alignment"] = QVariant((unsigned int)mTextAlignmentVerticalCombo->alignment());
	if (mTextColorButton) properties["Text Color"] = QVariant(mTextColorButton->color());
	if (mCaptionEdit) properties["Caption"] = QVariant(mCaptionEdit->toPlainText());

	if (mStartArrowStyleCombo) properties["Start Arrow Style"] = QVariant((unsigned int)mStartArrowStyleCombo->style());
	if (mStartArrowSizeEdit) properties["Start Arrow Size"] = QVariant(mStartArrowSizeEdit->value());

	if (mEndArrowStyleCombo) properties["End Arrow Style"] = QVariant((unsigned int)mEndArrowStyleCombo->style());
	if (mEndArrowSizeEdit) properties["End Arrow Size"] = QVariant(mEndArrowSizeEdit->value());

	if (mImageWidget) properties["Image"] = QVariant(mImageWidget->pixmap());

	return properties;
}

bool DiagramSingleItemPropertiesWidget::shouldExec() const
{
	return (layout()->count() > 0);
}

//==================================================================================================

void DiagramSingleItemPropertiesWidget::updateForPositionChange()
{
	QPointF oldPos = mItem->pos();
	QPointF newPos = mPositionWidget->pos();
	DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItem);
	DrawingRectResizeItem* rectItem = dynamic_cast<DrawingRectResizeItem*>(mItem);
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);
	DrawingPolyItem* polyItem = dynamic_cast<DrawingPolyItem*>(mItem);

	if (rectItem)
	{
		mItem->setPos(mPositionPrev);

		QPointF topLeftPos = mItem->mapFromScene(mRectTopLeftWidget->pos());
		QPointF bottomRightPos = mItem->mapFromScene(mRectBottomRightWidget->pos());

		mItem->setPos(newPos);

		mRectTopLeftWidget->setPos(mItem->mapToScene(topLeftPos));
		mRectBottomRightWidget->setPos(mItem->mapToScene(bottomRightPos));
	}

	if (twoPointItem)
	{
		mItem->setPos(mPositionPrev);

		QPointF startPointPos = mItem->mapFromScene(mStartPointPositionWidget->pos());
		QPointF endPointPos = mItem->mapFromScene(mEndPointPositionWidget->pos());

		mItem->setPos(newPos);

		mStartPointPositionWidget->setPos(mItem->mapToScene(startPointPos));
		mEndPointPositionWidget->setPos(mItem->mapToScene(endPointPos));
	}

	if (curveItem)
	{
		QList<QPointF> controlPoints;

		mItem->setPos(mPositionPrev);

		for(int i = 0; i < mControlPointsWidget->numberOfPoints(); i++)
			controlPoints.append(mItem->mapFromScene(mControlPointsWidget->point(i)));

		mItem->setPos(newPos);

		for(int i = 0; i < mControlPointsWidget->numberOfPoints(); i++)
			mControlPointsWidget->setPoint(i, mItem->mapToScene(controlPoints[i]));
	}

	if (polyItem)
	{
		QList<QPointF> polyPoints;

		mItem->setPos(mPositionPrev);

		for(int i = 0; i < mPolyPointsWidget->numberOfPoints(); i++)
			polyPoints.append(mItem->mapFromScene(mPolyPointsWidget->point(i)));

		mItem->setPos(newPos);

		for(int i = 0; i < mPolyPointsWidget->numberOfPoints(); i++)
			mPolyPointsWidget->setPoint(i, mItem->mapToScene(polyPoints[i]));
	}

	mItem->setPos(oldPos);
	mPositionPrev = newPos;
}

void DiagramSingleItemPropertiesWidget::updateForStartPointPositionChange()
{
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);

	if (curveItem)
	{
		QPointF newPos = mStartPointPositionWidget->pos();
		QPointF deltaPos = mControlPointsWidget->point(0) - mStartPointPrev;

		mControlPointsWidget->setPoint(0, newPos + deltaPos);

		mStartPointPrev = newPos;
	}
}

void DiagramSingleItemPropertiesWidget::updateForEndPointPositionChange()
{
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);

	if (curveItem)
	{
		QPointF newPos = mEndPointPositionWidget->pos();
		QPointF deltaPos = mControlPointsWidget->point(1) - mEndPointPrev;

		mControlPointsWidget->setPoint(1, newPos + deltaPos);

		mEndPointPrev = newPos;
	}
}

//==================================================================================================

QGroupBox* DiagramSingleItemPropertiesWidget::createPositionGroup()
{
	QGroupBox* positionGroup = new QGroupBox("Position");
	QFormLayout* positionLayout = new QFormLayout();

	mPositionWidget = new PositionWidget(mItem->scene()->units());
	mPositionWidget->setPos(mItem->mapToScene(mItem->mapFromParent(mItem->pos())));
	positionLayout->addRow("Position: ", mPositionWidget);

	mPositionPrev = mItem->pos();
	connect(mPositionWidget, SIGNAL(positionChanged()), this, SLOT(updateForPositionChange()));

	positionLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	positionLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	positionLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	positionLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	positionGroup->setLayout(positionLayout);

	return positionGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createRectGroup()
{
	QGroupBox* rectGroup = nullptr;
	DrawingRectResizeItem* rectItem = dynamic_cast<DrawingRectResizeItem*>(mItem);

	if (rectItem)
	{
		QFormLayout* rectLayout = new QFormLayout();

		mRectTopLeftWidget = new PositionWidget(mItem->scene()->units());
		mRectTopLeftWidget->setPos(mItem->mapToScene(mItem->point(0)->pos()));
		rectLayout->addRow("Top Left: ", mRectTopLeftWidget);

		mRectBottomRightWidget = new PositionWidget(mItem->scene()->units());
		mRectBottomRightWidget->setPos(mItem->mapToScene(mItem->point(1)->pos()));
		rectLayout->addRow("Bottom Right: ", mRectBottomRightWidget);

		if (mItem->containsProperty("Corner X-Radius") && mItem->containsProperty("Corner Y-Radius"))
		{
			mRectCornerRadiusWidget = new PositionWidget(mItem->units());
			mRectCornerRadiusWidget->setPos(QPointF(
				mItem->propertyValue("Corner X-Radius").toDouble(), mItem->propertyValue("Corner Y-Radius").toDouble()));
			rectLayout->addRow("Corner Radius: ", mRectCornerRadiusWidget);
		}

		rectGroup = new QGroupBox("Rect");
		rectLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		rectLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		rectLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		rectLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		rectGroup->setLayout(rectLayout);
	}

	return rectGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createPenBrushGroup()
{
	QGroupBox* penBrushGroup = nullptr;

	if (mItem->containsProperty("Pen Style") || mItem->containsProperty("Pen Width") ||
		mItem->containsProperty("Pen Color") || mItem->containsProperty("Brush Color"))
	{
		QFormLayout* penBrushLayout = new QFormLayout();

		if (mItem->containsProperty("Pen Style"))
		{
			mPenStyleCombo = new PenStyleComboBox();
			mPenStyleCombo->setStyle((Qt::PenStyle)mItem->propertyValue("Pen Style").toUInt());
			penBrushLayout->addRow("Pen Style: ", mPenStyleCombo);
		}

		if (mItem->containsProperty("Pen Width"))
		{
			mPenWidthEdit = new UnitsValueEdit(mItem->units(), UnitsValueEdit::NonNegativesOnly, true);
			mPenWidthEdit->setValue(mItem->propertyValue("Pen Width").toDouble());
			penBrushLayout->addRow("Pen Width: ", mPenWidthEdit);
		}

		if (mItem->containsProperty("Pen Color"))
		{
			mPenColorButton = new ColorPushButton();
			mPenColorButton->setColor(mItem->propertyValue("Pen Color").value<QColor>());
			penBrushLayout->addRow("Pen Color: ", mPenColorButton);
		}

		if (mItem->containsProperty("Brush Color"))
		{
			mBrushColorButton = new ColorPushButton();
			mBrushColorButton->setColor(mItem->propertyValue("Brush Color").value<QColor>());
			penBrushLayout->addRow("Brush Color: ", mBrushColorButton);
		}

		penBrushGroup = new QGroupBox("Pen and Brush");
		penBrushLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		penBrushLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		penBrushLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		penBrushLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		penBrushGroup->setLayout(penBrushLayout);
	}

	return penBrushGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createTextGroup()
{
	QGroupBox* textGroup = nullptr;

	if (mItem->containsProperty("Font Family") || mItem->containsProperty("Font Size") ||
		mItem->containsProperty("Font Bold") || mItem->containsProperty("Font Italic") ||
		mItem->containsProperty("Font Underline") || mItem->containsProperty("Font Overline") ||
		mItem->containsProperty("Font Strike-Out") ||
		mItem->containsProperty("Text Horizontal Alignment") || mItem->containsProperty("Text Vertical Alignment") ||
		mItem->containsProperty("Text Color") || mItem->containsProperty("Caption"))
	{
		QFormLayout* textLayout = new QFormLayout();
		QWidget* fontStyleWidget;
		QWidget* textAlignmentWidget;

		if (mItem->containsProperty("Font Family"))
		{
			mFontFamilyCombo = new QFontComboBox();
			mFontFamilyCombo->setCurrentFont(QFont(mItem->propertyValue("Font Family").toString()));
			textLayout->addRow("Font Family: ", mFontFamilyCombo);
		}

		if (mItem->containsProperty("Font Size"))
		{
			mFontSizeEdit = new UnitsValueEdit(mItem->units(), UnitsValueEdit::PositivesOnly, true);
			mFontSizeEdit->setValue(mItem->propertyValue("Font Size").toDouble());
			textLayout->addRow("Font Size: ", mFontSizeEdit);
		}

		fontStyleWidget = createFontStyleWidget();
		if (fontStyleWidget) textLayout->addRow("Font Style: ", fontStyleWidget);

		textAlignmentWidget = createTextAlignmentWidget();
		if (textAlignmentWidget) textLayout->addRow("Text Alignment: ", textAlignmentWidget);

		if (mItem->containsProperty("Text Color"))
		{
			mTextColorButton = new ColorPushButton();
			mTextColorButton->setColor(mItem->propertyValue("Text Color").value<QColor>());
			textLayout->addRow("Text Color: ", mTextColorButton);
		}

		if (mItem->containsProperty("Caption"))
		{
			mCaptionEdit = new QTextEdit();
			mCaptionEdit->setPlainText(mItem->propertyValue("Caption").toString());
			textLayout->addRow("Caption: ", mCaptionEdit);
		}

		textGroup = new QGroupBox("Text");
		textLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		textLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		textLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		textLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		textGroup->setLayout(textLayout);
	}
	else
	{
	}

	return textGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createStartPointGroup()
{
	QGroupBox* startPointGroup = nullptr;
	DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItem);

	if (twoPointItem || mItem->containsProperty("Start Arrow Style") || mItem->containsProperty("Start Arrow Size"))
	{
		QFormLayout* startPointLayout = new QFormLayout();

		if (twoPointItem)
		{
			mStartPointPositionWidget = new PositionWidget(mItem->scene()->units());
			mStartPointPositionWidget->setPos(mItem->mapToScene(twoPointItem->startPoint()->pos()));
			startPointLayout->addRow("Position: ", mStartPointPositionWidget);

			mStartPointPrev = mStartPointPositionWidget->pos();
			connect(mStartPointPositionWidget, SIGNAL(positionChanged()), this, SLOT(updateForStartPointPositionChange()));
		}

		if (mItem->containsProperty("Start Arrow Style"))
		{
			mStartArrowStyleCombo = new ArrowStyleComboBox();
			mStartArrowStyleCombo->setStyle((DrawingArrow::Style)mItem->propertyValue("Start Arrow Style").toUInt());
			startPointLayout->addRow("Arrow Style: ", mStartArrowStyleCombo);
		}

		if (mItem->containsProperty("Start Arrow Size"))
		{
			mStartArrowSizeEdit = new UnitsValueEdit(mItem->units(), UnitsValueEdit::NonNegativesOnly, true);
			mStartArrowSizeEdit->setValue(mItem->propertyValue("Start Arrow Size").toDouble());
			startPointLayout->addRow("Arrow Size: ", mStartArrowSizeEdit);
		}

		startPointGroup = new QGroupBox("Start Point");
		startPointLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		startPointLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		startPointLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		startPointLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		startPointGroup->setLayout(startPointLayout);
	}

	return startPointGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createEndPointGroup()
{
	QGroupBox* endPointGroup = nullptr;
	DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItem);

	if (twoPointItem || mItem->containsProperty("End Arrow Style") || mItem->containsProperty("End Arrow Size"))
	{
		QFormLayout* endPointLayout = new QFormLayout();

		if (twoPointItem)
		{
			mEndPointPositionWidget = new PositionWidget(mItem->scene()->units());
			mEndPointPositionWidget->setPos(mItem->mapToScene(twoPointItem->endPoint()->pos()));
			endPointLayout->addRow("Position: ", mEndPointPositionWidget);

			mEndPointPrev = mEndPointPositionWidget->pos();
			connect(mEndPointPositionWidget, SIGNAL(positionChanged()), this, SLOT(updateForEndPointPositionChange()));
		}

		if (mItem->containsProperty("End Arrow Style"))
		{
			mEndArrowStyleCombo = new ArrowStyleComboBox();
			mEndArrowStyleCombo->setStyle((DrawingArrow::Style)mItem->propertyValue("End Arrow Style").toUInt());
			endPointLayout->addRow("Arrow Style: ", mEndArrowStyleCombo);
		}

		if (mItem->containsProperty("End Arrow Size"))
		{
			mEndArrowSizeEdit = new UnitsValueEdit(mItem->units(), UnitsValueEdit::NonNegativesOnly, true);
			mEndArrowSizeEdit->setValue(mItem->propertyValue("End Arrow Size").toDouble());
			endPointLayout->addRow("Arrow Size: ", mEndArrowSizeEdit);
		}

		endPointGroup = new QGroupBox("End Point");
		endPointLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		endPointLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		endPointLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
		endPointLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
		endPointGroup->setLayout(endPointLayout);
	}

	return endPointGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createControlPointsGroup()
{
	QGroupBox* controlPointsGroup = nullptr;
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);

	if (curveItem)
	{
		QVBoxLayout* controlPointsLayout = new QVBoxLayout();

		mControlPointsWidget = new PointsListWidget(2, mItem->scene()->units());
		mControlPointsWidget->setLabel(0, "Start Control:");
		mControlPointsWidget->setPoint(0, mItem->mapToScene(mItem->point(2)->pos()));
		mControlPointsWidget->setLabel(1, "End Control:");
		mControlPointsWidget->setPoint(1, mItem->mapToScene(mItem->point(3)->pos()));
		mControlPointsWidget->setLabelWidth(labelWidth());

		controlPointsGroup = new QGroupBox("Control Points");
		controlPointsLayout->addWidget(mControlPointsWidget);
		controlPointsLayout->setContentsMargins(0, 0, 0, 0);
		controlPointsGroup->setLayout(controlPointsLayout);
	}

	return controlPointsGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createPolyPointsGroup()
{
	QGroupBox* polyPointsGroup = nullptr;
	DrawingPolyItem* polyItem = dynamic_cast<DrawingPolyItem*>(mItem);

	if (polyItem)
	{
		QVBoxLayout* polyPointsLayout = new QVBoxLayout();

		mPolyPointsWidget = new PointsListWidget(mItem->numberOfPoints(), mItem->scene()->units());
		for(int i = 0; i < mItem->numberOfPoints(); i++)
			mPolyPointsWidget->setPoint(i, mItem->mapToScene(mItem->point(i)->pos()));
		mPolyPointsWidget->setMinimumHeight(100);
		mPolyPointsWidget->setLabelWidth(labelWidth());

		polyPointsGroup = new QGroupBox("Points");
		polyPointsLayout->addWidget(mPolyPointsWidget);
		polyPointsLayout->setContentsMargins(0, 0, 0, 0);
		polyPointsGroup->setLayout(polyPointsLayout);
	}

	return polyPointsGroup;
}

QGroupBox* DiagramSingleItemPropertiesWidget::createImageGroup()
{
	QGroupBox* imageGroup = nullptr;
	DrawingPixmapItem* pixmapItem = dynamic_cast<DrawingPixmapItem*>(mItem);

	if (pixmapItem)
	{
		QVBoxLayout* imageLayout = new QVBoxLayout();

		mImageWidget = new ImageWidget();
		mImageWidget->setPixmap(pixmapItem->pixmap());

		imageGroup = new QGroupBox("Image");
		imageLayout->addWidget(mImageWidget);
		imageLayout->setContentsMargins(0, 0, 0, 0);
		imageGroup->setLayout(imageLayout);
	}

	return imageGroup;
}

//==================================================================================================

QWidget* DiagramSingleItemPropertiesWidget::createFontStyleWidget()
{
	QWidget* fontStyleWidget = nullptr;

	if (mItem->containsProperty("Font Bold") || mItem->containsProperty("Font Italic") ||
		mItem->containsProperty("Font Underline") || mItem->containsProperty("Font Overline") ||
		mItem->containsProperty("Font Strike-Out"))
	{
		QHBoxLayout* fontStyleLayout = new QHBoxLayout();

		if (mItem->containsProperty("Font Bold"))
		{
			mFontBoldButton = new QToolButton();
			mFontBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
			mFontBoldButton->setToolTip("Font Bold");
			mFontBoldButton->setCheckable(true);
			mFontBoldButton->setChecked(mItem->propertyValue("Font Bold").toBool());
			fontStyleLayout->addWidget(mFontBoldButton);
		}

		if (mItem->containsProperty("Font Italic"))
		{
			mFontItalicButton = new QToolButton();
			mFontItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
			mFontItalicButton->setToolTip("Font Italic");
			mFontItalicButton->setCheckable(true);
			mFontItalicButton->setChecked(mItem->propertyValue("Font Italic").toBool());
			fontStyleLayout->addWidget(mFontItalicButton);
		}

		if (mItem->containsProperty("Font Underline"))
		{
			mFontUnderlineButton = new QToolButton();
			mFontUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
			mFontUnderlineButton->setToolTip("Font Underline");
			mFontUnderlineButton->setCheckable(true);
			mFontUnderlineButton->setChecked(mItem->propertyValue("Font Underline").toBool());
			fontStyleLayout->addWidget(mFontUnderlineButton);
		}

		if (mItem->containsProperty("Font Overline"))
		{
			mFontOverlineButton = new QToolButton();
			mFontOverlineButton->setIcon(QIcon(":/icons/oxygen/format-text-overline.png"));
			mFontOverlineButton->setToolTip("Font Overline");
			mFontOverlineButton->setCheckable(true);
			mFontOverlineButton->setChecked(mItem->propertyValue("Font Overline").toBool());
			fontStyleLayout->addWidget(mFontOverlineButton);
		}

		if (mItem->containsProperty("Font Strike-Out"))
		{
			mFontStrikeOutButton = new QToolButton();
			mFontStrikeOutButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
			mFontStrikeOutButton->setToolTip("Font Strike-Out");
			mFontStrikeOutButton->setCheckable(true);
			mFontStrikeOutButton->setChecked(mItem->propertyValue("Font Strike-Out").toBool());
			fontStyleLayout->addWidget(mFontStrikeOutButton);
		}

		fontStyleWidget = new QWidget();
		fontStyleLayout->addWidget(new QWidget(), 100);
		fontStyleLayout->setSpacing(1);
		fontStyleLayout->setContentsMargins(0, 0, 0, 0);
		fontStyleWidget->setLayout(fontStyleLayout);
	}

	return fontStyleWidget;
}

QWidget* DiagramSingleItemPropertiesWidget::createTextAlignmentWidget()
{
	QWidget* textAlignmentWidget = nullptr;

	if (mItem->containsProperty("Text Horizontal Alignment") ||
		mItem->containsProperty("Text Vertical Alignment"))
	{
		QHBoxLayout* textAlignmentLayout = new QHBoxLayout();

		if (mItem->containsProperty("Text Horizontal Alignment"))
		{
			mTextAlignmentHorizontalCombo = new AlignmentHorizontalComboBox();
			mTextAlignmentHorizontalCombo->setAlignment((Qt::Alignment)mItem->propertyValue("Text Horizontal Alignment").toUInt());
			textAlignmentLayout->addWidget(mTextAlignmentHorizontalCombo);
		}

		if (mItem->containsProperty("Text Vertical Alignment"))
		{
			mTextAlignmentVerticalCombo = new AlignmentVerticalComboBox();
			mTextAlignmentVerticalCombo->setAlignment((Qt::Alignment)mItem->propertyValue("Text Vertical Alignment").toUInt());
			textAlignmentLayout->addWidget(mTextAlignmentVerticalCombo);
		}

		textAlignmentWidget = new QWidget();
		textAlignmentLayout->setSpacing(1);
		textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
		textAlignmentWidget->setLayout(textAlignmentLayout);
	}

	return textAlignmentWidget;
}

int DiagramSingleItemPropertiesWidget::labelWidth() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.width("Pen Join Style: ") + 8;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramSingleItemPropertiesDialog::DiagramSingleItemPropertiesDialog(DrawingItem* item,
	QWidget* parent) : QDialog(parent)
{
	mPropertiesWidget = new DiagramSingleItemPropertiesWidget(item);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mPropertiesWidget, 100);
	vLayout->addWidget(createButtonBox());
	setLayout(vLayout);

	setWindowTitle("Item Properties");
	resize(item->containsProperty("Caption") ? 320 : 280, 10);
}

DiagramSingleItemPropertiesDialog::~DiagramSingleItemPropertiesDialog() { }

//==================================================================================================

QPointF DiagramSingleItemPropertiesDialog::position() const
{
	return mPropertiesWidget->position();
}

QHash<DrawingItemPoint*, QPointF> DiagramSingleItemPropertiesDialog::pointPositions() const
{
	return mPropertiesWidget->pointPositions();
}

QHash<QString,QVariant> DiagramSingleItemPropertiesDialog::properties() const
{
	return mPropertiesWidget->properties();
}

bool DiagramSingleItemPropertiesDialog::shouldExec() const
{
	return mPropertiesWidget->shouldExec();
}

//==================================================================================================

QDialogButtonBox* DiagramSingleItemPropertiesDialog::createButtonBox()
{
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->setCenterButtons(true);

	QPushButton* okButton = buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
	QPushButton* cancelButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	okButton->setMinimumSize(72, 28);
	cancelButton->setMinimumSize(72, 28);

	return buttonBox;
}
