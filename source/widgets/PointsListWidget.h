// PointsListWidget.h

#ifndef POINTSLISTWIDGET_H
#define POINTSLISTWIDGET_H

#include <Drawing>

class PositionWidget;

class PointsListWidget : public QScrollArea
{
	Q_OBJECT

private:
	QFormLayout* mPointsFormLayout;
	QList<PositionWidget*> mPositionWidgets;

public:
	PointsListWidget(int numberOfPoints, DrawingUnits units);
	~PointsListWidget();

public slots:
	void setLabelWidth(int width);
	void setLabel(int index, const QString& text);
	void setPoint(int index, const QPointF& position);

public:
	int numberOfPoints() const;
	QPointF point(int index) const;
};

#endif
