// PointsListWidget.cpp

#include "PointsListWidget.h"
#include "PositionWidget.h"

PointsListWidget::PointsListWidget(int numberOfPoints, DrawingUnits units) : QScrollArea()
{
	PositionWidget* newWidget;

	mPointsFormLayout = new QFormLayout();

	for(int i = 0; i < numberOfPoints; i++)
	{
		newWidget = new PositionWidget(units);
		mPositionWidgets.append(newWidget);

		if (i == 0)
			mPointsFormLayout->addRow("First Point:", newWidget);
		else if (i == numberOfPoints - 1)
			mPointsFormLayout->addRow("Last Point:", newWidget);
		else
			mPointsFormLayout->addRow(" ", newWidget);
	}

	QFrame* mainFrame = new QFrame();
	mPointsFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	mPointsFormLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	mPointsFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	mainFrame->setLayout(mPointsFormLayout);

	setWidget(mainFrame);
	setWidgetResizable(true);
	setFrameStyle(0);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

PointsListWidget::~PointsListWidget() { }

//==================================================================================================

void PointsListWidget::setLabelWidth(int width)
{
	if (mPointsFormLayout->rowCount() > 0)
		mPointsFormLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(width);
}

void PointsListWidget::setLabel(int index, const QString& text)
{
	qobject_cast<QLabel*>(mPointsFormLayout->itemAt(index, QFormLayout::LabelRole)->widget())->setText(text);
}

void PointsListWidget::setPoint(int index, const QPointF& position)
{
	mPositionWidgets[index]->setPos(position);
}

int PointsListWidget::numberOfPoints() const
{
	return mPositionWidgets.size();
}

QPointF PointsListWidget::point(int index) const
{
	return mPositionWidgets[index]->pos();
}
