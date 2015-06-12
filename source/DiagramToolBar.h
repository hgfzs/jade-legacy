// DiagramToolBar.h

#ifndef DIAGRAMTOOLBAR_H
#define DIAGRAMTOOLBAR_H

#include <Drawing>

class DiagramView;
class AlignmentHorizontalToolButton;
class AlignmentVerticalToolButton;
class ArrowStyleToolButton;
class ColorToolButton;
class PenStyleToolButton;
class UnitsValueEdit;

class EditToolBar : public QToolBar
{
	Q_OBJECT

public:
	EditToolBar(DiagramView* diagram);
	~EditToolBar();
};

//==================================================================================================

class DiagramToolBar : public QToolBar
{
	Q_OBJECT

private:
	QComboBox* mDiagramZoomCombo;

	DiagramView* mDiagram;

public:
	DiagramToolBar(DiagramView* diagram);
	~DiagramToolBar();

signals:
	void zoomChanged(qreal level);

private slots:
	void updateDiagramZoomLevel(const QString& level);
	void updateDiagramZoomCombo(qreal level);
};

//==================================================================================================

class PenBrushToolBar : public QToolBar
{
	Q_OBJECT

private:
	UnitsValueEdit* mPenWidthEdit;
	PenStyleToolButton* mPenStyleButton;
	ArrowStyleToolButton* mStartArrowButton;
	ArrowStyleToolButton* mEndArrowButton;

	ColorToolButton* mPenColorButton;
	ColorToolButton* mBrushColorButton;

	DiagramView* mDiagram;

public:
	PenBrushToolBar(DiagramView* diagram);
	~PenBrushToolBar();

	QHash<QString,QVariant> properties() const;

	void save(QSettings& settings);
	void load(QSettings& settings);

signals:
	void propertyChanged(const QString& name, const QVariant& value);
	void propertiesChanged(const QHash<QString,QVariant>& properties);

private slots:
	void updateDiagramUnits();

	void updatePenWidth();
	void updatePenStyle(Qt::PenStyle style);
	void updateStartArrowStyle(DrawingArrow::Style style);
	void updateEndArrowStyle(DrawingArrow::Style style);
	void updatePenColor(const QColor& color);
	void updateBrushColor(const QColor& color);
};


//==================================================================================================

class TextToolBar : public QToolBar
{
	Q_OBJECT

private:
	QFontComboBox* mFontFamilyCombo;
	UnitsValueEdit* mFontSizeEdit;
	QToolButton* mFontBoldButton;
	QToolButton* mFontItalicButton;
	QToolButton* mFontUnderlineButton;
	QToolButton* mFontOverlineButton;
	QToolButton* mFontStrikeOutButton;
	AlignmentHorizontalToolButton* mHorizontalAlignmentButton;
	AlignmentVerticalToolButton* mVerticalAlignmentButton;
	ColorToolButton* mTextColorButton;

	DiagramView* mDiagram;

public:
	TextToolBar(DiagramView* diagram);
	~TextToolBar();

	QHash<QString,QVariant> properties() const;

	void save(QSettings& settings);
	void load(QSettings& settings);

signals:
	void propertyChanged(const QString& name, const QVariant& value);
	void propertiesChanged(const QHash<QString,QVariant>& properties);

private slots:
	void updateDiagramUnits();

	void updateFontFamily(const QString& family);
	void updateFontSize();
	void updateFontBold(bool bold);
	void updateFontItalic(bool italic);
	void updateFontUnderline(bool underline);
	void updateFontOverline(bool overline);
	void updateFontStrikeOut(bool strikeOut);
	void updateAlignmentHorizontal(Qt::Alignment alignment);
	void updateAlignmentVertical(Qt::Alignment alignment);
	void updateTextColor(const QColor& color);
};

#endif
