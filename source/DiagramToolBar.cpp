// DiagramToolBar.cpp

#include "DiagramToolBar.h"
#include "DiagramView.h"
#include "ColorButton.h"
#include "EnumToolButton.h"
#include "UnitsValueEdit.h"

EditToolBar::EditToolBar(DiagramView* diagram) : QToolBar("Edit")
{
	QList<QAction*> actions = diagram->actions();

	setObjectName("EditToolBar");
	addAction(actions[DiagramView::UndoAction]);
	addAction(actions[DiagramView::RedoAction]);
	addSeparator();
	addAction(actions[DiagramView::CutAction]);
	addAction(actions[DiagramView::CopyAction]);
	addAction(actions[DiagramView::PasteAction]);
}

EditToolBar::~EditToolBar() { }

//==================================================================================================
//==================================================================================================
//==================================================================================================

DiagramToolBar::DiagramToolBar(DiagramView* diagram) : QToolBar("Diagram")
{
	QList<QAction*> actions = diagram->actions();

	mDiagram = diagram;

	mDiagramZoomCombo = new QComboBox();
	for(int i = 0; i < DiagramView::kZoomLevels.size(); i++)
		mDiagramZoomCombo->addItem(QString::number(DiagramView::kZoomLevels[i] * 100) + "%");
	mDiagramZoomCombo->setEditable(true);
	connect(mDiagramZoomCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateDiagramZoomLevel(const QString&)));
	connect(mDiagram, SIGNAL(zoomLevelChanged(qreal)), this, SLOT(updateDiagramZoomCombo(qreal)));

	QFontMetrics fontMetrics(mDiagramZoomCombo->font());
	mDiagramZoomCombo->setMinimumWidth(fontMetrics.width("8888.8888%") + 16);

	setObjectName("DiagramToolBar");
	addAction(actions[DiagramView::RotateAction]);
	addAction(actions[DiagramView::RotateBackAction]);
	addAction(actions[DiagramView::FlipAction]);
	addSeparator();
	addAction(actions[DiagramView::ZoomInAction]);
	addWidget(mDiagramZoomCombo);
	addAction(actions[DiagramView::ZoomOutAction]);
	addAction(actions[DiagramView::ZoomFitAction]);
}

DiagramToolBar::~DiagramToolBar() { }

//==================================================================================================

void DiagramToolBar::updateDiagramZoomLevel(const QString& level)
{
	QString zoomTextStr = level;
	QTextStream textStream(&zoomTextStr);
	qreal zoomLevel = 0;

	textStream >> zoomLevel;
	emit zoomChanged(zoomLevel / 100);
}

void DiagramToolBar::updateDiagramZoomCombo(qreal level)
{
	QString text = QString::number(level * 100, 'f', 1) + "%";
	int textIndex = mDiagramZoomCombo->findText(text);

	if (0 <= textIndex && textIndex < mDiagramZoomCombo->count())
		mDiagramZoomCombo->setCurrentIndex(textIndex);
	else
		mDiagramZoomCombo->setEditText(text);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PenBrushToolBar::PenBrushToolBar(DiagramView* diagram) : QToolBar("Pen and Brush")
{
	mDiagram = diagram;
	connect(mDiagram, SIGNAL(propertiesChanged()), this, SLOT(updateDiagramUnits()));
	connect(this, SIGNAL(propertyChanged(const QString&,const QVariant&)),
		mDiagram, SLOT(updateItemProperty(const QString&,const QVariant&)));
	connect(this, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
		mDiagram, SLOT(updateDefaultItemProperties(const QHash<QString,QVariant>&)));

	mPenWidthEdit = new UnitsValueEdit(mDiagram->scene()->units(), UnitsValueEdit::NonNegativesOnly, false);
	mPenWidthEdit->setValue(mPenWidthEdit->units() == UnitsMils ? 16 : 0.4);
	mPenWidthEdit->setToolTip("Pen width");
	connect(mPenWidthEdit, SIGNAL(editingFinished()), this, SLOT(updatePenWidth()));

	mPenStyleButton = new PenStyleToolButton();
	mPenStyleButton->setToolTip("Pen Style");
	connect(mPenStyleButton, SIGNAL(styleChanged(Qt::PenStyle)), this, SLOT(updatePenStyle(Qt::PenStyle)));

	mStartArrowButton = new ArrowStyleToolButton(false);
	mStartArrowButton->setToolTip("Start Arrow Style");
	mStartArrowButton->setStyle(DrawingArrow::None);
	connect(mStartArrowButton, SIGNAL(styleChanged(DrawingArrow::Style)), this, SLOT(updateStartArrowStyle(DrawingArrow::Style)));

	mEndArrowButton = new ArrowStyleToolButton(true);
	mEndArrowButton->setToolTip("End Arrow Style");
	mEndArrowButton->setStyle(DrawingArrow::None);
	connect(mEndArrowButton, SIGNAL(styleChanged(DrawingArrow::Style)), this, SLOT(updateEndArrowStyle(DrawingArrow::Style)));

	mPenColorButton = new ColorToolButton(QIcon(":/icons/oxygen/pen.png"), "Pen Color");
	mPenColorButton->setColor(QColor(0, 0, 0));
	mPenColorButton->setToolTip("Pen Color");
	connect(mPenColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(updatePenColor(const QColor&)));

	mBrushColorButton = new ColorToolButton(QIcon(":/icons/oxygen/fill-color.png"), "Brush Color");
	mBrushColorButton->setColor(QColor(255, 255, 255));
	mBrushColorButton->setToolTip("Brush Color");
	connect(mBrushColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(updateBrushColor(const QColor&)));

	setObjectName("PenBrushToolBar");
	addWidget(mPenStyleButton);
	addWidget(mPenWidthEdit);
	addSeparator();
	addWidget(mStartArrowButton);
	addWidget(mEndArrowButton);
	addSeparator();
	addWidget(mPenColorButton);
	addWidget(mBrushColorButton);

	emit propertiesChanged(properties());
}

PenBrushToolBar::~PenBrushToolBar() { }

//==================================================================================================

QHash<QString,QVariant> PenBrushToolBar::properties() const
{
	QHash<QString,QVariant> properties;

	properties["Pen Width"] = QVariant(mPenWidthEdit->value());
	properties["Pen Style"] = QVariant((unsigned int)mPenStyleButton->style());
	properties["Pen Color"] = QVariant(mPenColorButton->color());
	properties["Brush Color"] = QVariant(mBrushColorButton->color());
	properties["Start Arrow Style"] = QVariant((unsigned int)mStartArrowButton->style());
	properties["End Arrow Style"] = QVariant((unsigned int)mEndArrowButton->style());

	return properties;
}

//==================================================================================================

void PenBrushToolBar::save(QSettings& settings)
{
	settings.setValue("PenWidthUnits", (unsigned int)mPenWidthEdit->units());
	settings.setValue("PenWidth", mPenWidthEdit->value());
	settings.setValue("PenStyle", (unsigned int)mPenStyleButton->style());
	settings.setValue("StartArrowStyle", (unsigned int)mStartArrowButton->style());
	settings.setValue("EndArrowStyle", (unsigned int)mEndArrowButton->style());
	settings.setValue("PenColor", Drawing::colorToString(mPenColorButton->color()));
	settings.setValue("BrushColor", Drawing::colorToString(mBrushColorButton->color()));
}

void PenBrushToolBar::load(QSettings& settings)
{
	if (settings.contains("PenWidth") && settings.contains("PenWidthUnits"))
	{
		mPenWidthEdit->setValue(settings.value("PenWidth", 16).toDouble(),
			(DrawingUnits)settings.value("PenWidthUnits", (unsigned int)UnitsMils).toUInt());
	}

	if (settings.contains("PenStyle"))
		mPenStyleButton->setStyle((Qt::PenStyle)settings.value("PenStyle", (unsigned int)Qt::SolidLine).toUInt());
	if (settings.contains("StartArrowStyle"))
		mStartArrowButton->setStyle((DrawingArrow::Style)settings.value("StartArrowStyle", (unsigned int)DrawingArrow::None).toUInt());
	if (settings.contains("EndArrowStyle"))
		mEndArrowButton->setStyle((DrawingArrow::Style)settings.value("EndArrowStyle", (unsigned int)DrawingArrow::None).toUInt());

	if (settings.contains("PenColor"))
		mPenColorButton->setColor(settings.value("PenColor", QVariant(QColor(0, 0, 0))).value<QColor>());
	if (settings.contains("BrushColor"))
		mBrushColorButton->setColor(settings.value("BrushColor", QVariant(QColor(255, 255, 255))).value<QColor>());
}

//==================================================================================================

void PenBrushToolBar::updateDiagramUnits()
{
	mPenWidthEdit->setUnits(mDiagram->scene()->units());
}

//==================================================================================================

void PenBrushToolBar::updatePenWidth()
{
	emit propertyChanged("Pen Width", QVariant(mPenWidthEdit->value()));
	emit propertiesChanged(properties());
}

void PenBrushToolBar::updatePenStyle(Qt::PenStyle style)
{
	emit propertyChanged("Pen Style", QVariant((unsigned int)style));
	emit propertiesChanged(properties());
}

void PenBrushToolBar::updateStartArrowStyle(DrawingArrow::Style style)
{
	emit propertyChanged("Start Arrow Style", QVariant(style));
	emit propertiesChanged(properties());
}

void PenBrushToolBar::updateEndArrowStyle(DrawingArrow::Style style)
{
	emit propertyChanged("End Arrow Style", QVariant(style));
	emit propertiesChanged(properties());
}

void PenBrushToolBar::updatePenColor(const QColor& color)
{
	emit propertyChanged("Pen Color", QVariant(color));
	emit propertiesChanged(properties());
}

void PenBrushToolBar::updateBrushColor(const QColor& color)
{
	emit propertyChanged("Brush Color", QVariant(color));
	emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

TextToolBar::TextToolBar(DiagramView* diagram) : QToolBar("Text")
{
	mDiagram = diagram;
	connect(mDiagram, SIGNAL(propertiesChanged()), this, SLOT(updateDiagramUnits()));
	connect(this, SIGNAL(propertyChanged(const QString&,const QVariant&)),
		mDiagram, SLOT(updateItemProperty(const QString&,const QVariant&)));
	connect(this, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
		mDiagram, SLOT(updateDefaultItemProperties(const QHash<QString,QVariant>&)));

	mFontFamilyCombo = new QFontComboBox();
	mFontFamilyCombo->setCurrentFont(QFont("Arial"));
	mFontFamilyCombo->setToolTip("Font Family");
	connect(mFontFamilyCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateFontFamily(const QString&)));

	mFontSizeEdit = new UnitsValueEdit(mDiagram->scene()->units(), UnitsValueEdit::PositivesOnly, false);
	mFontSizeEdit->setValue(mFontSizeEdit->units() == UnitsMils ? 100 : 2.5);
	mFontSizeEdit->setToolTip("Font Size");
	connect(mFontSizeEdit, SIGNAL(editingFinished()), this, SLOT(updateFontSize()));

	mFontBoldButton = new QToolButton();
	mFontBoldButton = new QToolButton();
	mFontBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
	mFontBoldButton->setToolTip("Font Bold");
	mFontBoldButton->setCheckable(true);
	mFontBoldButton->setChecked(false);
	connect(mFontBoldButton, SIGNAL(toggled(bool)), this, SLOT(updateFontBold(bool)));

	mFontItalicButton = new QToolButton();
	mFontItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
	mFontItalicButton->setToolTip("Font Italic");
	mFontItalicButton->setCheckable(true);
	mFontItalicButton->setChecked(false);
	connect(mFontItalicButton, SIGNAL(toggled(bool)), this, SLOT(updateFontItalic(bool)));

	mFontUnderlineButton = new QToolButton();
	mFontUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
	mFontUnderlineButton->setToolTip("Font Underline");
	mFontUnderlineButton->setCheckable(true);
	mFontUnderlineButton->setChecked(false);
	connect(mFontUnderlineButton, SIGNAL(toggled(bool)), this, SLOT(updateFontUnderline(bool)));

	mFontOverlineButton = new QToolButton();
	mFontOverlineButton->setIcon(QIcon(":/icons/oxygen/format-text-overline.png"));
	mFontOverlineButton->setToolTip("Font Overline");
	mFontOverlineButton->setCheckable(true);
	mFontOverlineButton->setChecked(false);
	connect(mFontOverlineButton, SIGNAL(toggled(bool)), this, SLOT(updateFontOverline(bool)));

	mFontStrikeOutButton = new QToolButton();
	mFontStrikeOutButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
	mFontStrikeOutButton->setToolTip("Font Strike-Out");
	mFontStrikeOutButton->setCheckable(true);
	mFontStrikeOutButton->setChecked(false);
	connect(mFontStrikeOutButton, SIGNAL(toggled(bool)), this, SLOT(updateFontStrikeOut(bool)));

	mHorizontalAlignmentButton = new AlignmentHorizontalToolButton();
	mHorizontalAlignmentButton->setToolTip("Horizontal Alignment");
	connect(mHorizontalAlignmentButton, SIGNAL(alignmentChanged(Qt::Alignment)), this, SLOT(updateAlignmentHorizontal(Qt::Alignment)));

	mVerticalAlignmentButton = new AlignmentVerticalToolButton();
	mVerticalAlignmentButton->setToolTip("Vertical Alignment");
	connect(mVerticalAlignmentButton, SIGNAL(alignmentChanged(Qt::Alignment)), this, SLOT(updateAlignmentVertical(Qt::Alignment)));

	mTextColorButton = new ColorToolButton(QIcon(":/icons/oxygen/text.png"), "Text Color");
	mTextColorButton->setColor(QColor(0, 0, 0));
	connect(mTextColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(updateTextColor(const QColor&)));

	setObjectName("TextToolBar");
	addWidget(mFontFamilyCombo);
	addWidget(mFontSizeEdit);
	addSeparator();
	addWidget(mFontBoldButton);
	addWidget(mFontItalicButton);
	addWidget(mFontUnderlineButton);
	addWidget(mFontOverlineButton);
	addWidget(mFontStrikeOutButton);
	addSeparator();
	addWidget(mHorizontalAlignmentButton);
	addWidget(mVerticalAlignmentButton);
	addSeparator();
	addWidget(mTextColorButton);

	emit propertiesChanged(properties());
}

TextToolBar::~TextToolBar() { }

//==================================================================================================

QHash<QString,QVariant> TextToolBar::properties() const
{
	QHash<QString,QVariant> properties;

	properties["Font Family"] = QVariant(mFontFamilyCombo->currentFont().family());
	properties["Font Size"] = QVariant(mFontSizeEdit->value());
	properties["Font Bold"] = QVariant(mFontBoldButton->isChecked());
	properties["Font Italic"] = QVariant(mFontItalicButton->isChecked());
	properties["Font Underline"] = QVariant(mFontUnderlineButton->isChecked());
	properties["Font Overline"] = QVariant(mFontOverlineButton->isChecked());
	properties["Font Strike-Out"] = QVariant(mFontStrikeOutButton->isChecked());
	properties["Text Horizontal Alignment"] = QVariant((unsigned int)mHorizontalAlignmentButton->alignment());
	properties["Text Vertical Alignment"] = QVariant((unsigned int)mVerticalAlignmentButton->alignment());
	properties["Text Color"] = QVariant(mTextColorButton->color());

	return properties;
}

//==================================================================================================

void TextToolBar::save(QSettings& settings)
{
	settings.setValue("FontFamily", mFontFamilyCombo->currentFont().family());
	settings.setValue("FontSizeUnits", (unsigned int)mFontSizeEdit->units());
	settings.setValue("FontSize", mFontSizeEdit->value());
	settings.setValue("FontBold", mFontBoldButton->isChecked());
	settings.setValue("FontItalic", mFontItalicButton->isChecked());
	settings.setValue("FontUnderline", mFontUnderlineButton->isChecked());
	settings.setValue("FontOverline", mFontOverlineButton->isChecked());
	settings.setValue("FontStrikeOut", mFontStrikeOutButton->isChecked());
	settings.setValue("TextHorizontalAlignment", (unsigned int)mHorizontalAlignmentButton->alignment());
	settings.setValue("TextVerticalAlignment", (unsigned int)mVerticalAlignmentButton->alignment());
	settings.setValue("TextColor", Drawing::colorToString(mTextColorButton->color()));
}

void TextToolBar::load(QSettings& settings)
{
	if (settings.contains("FontFamily"))
	{
		QFont font = mFontFamilyCombo->currentFont();
		font.setFamily(settings.value("FontFamily", "sans").toString());
		mFontFamilyCombo->setCurrentFont(font);
	}

	if (settings.contains("FontSize") && settings.contains("FontSizeUnits"))
	{
		mFontSizeEdit->setValue(settings.value("FontSize", 16).toDouble(),
			(DrawingUnits)settings.value("FontSizeUnits", (unsigned int)UnitsMils).toUInt());
	}

	if (settings.contains("FontBold")) mFontBoldButton->setChecked(settings.value("FontBold", false).toBool());
	if (settings.contains("FontItalic")) mFontItalicButton->setChecked(settings.value("FontItalic", false).toBool());
	if (settings.contains("FontUnderline")) mFontUnderlineButton->setChecked(settings.value("FontUnderline", false).toBool());
	if (settings.contains("FontOverline")) mFontOverlineButton->setChecked(settings.value("FontOverline", false).toBool());
	if (settings.contains("FontStrikeOut")) mFontUnderlineButton->setChecked(settings.value("FontStrikeOut", false).toBool());


	if (settings.contains("TextHorizontalAlignment"))
		mHorizontalAlignmentButton->setAlignment((Qt::Alignment)settings.value("TextHorizontalAlignment", (unsigned int)Qt::AlignHCenter).toUInt());
	if (settings.contains("TextVerticalAlignment"))
		mVerticalAlignmentButton->setAlignment((Qt::Alignment)settings.value("TextVerticalAlignment", (unsigned int)Qt::AlignVCenter).toUInt());

	if (settings.contains("TextColor"))
		mTextColorButton->setColor(settings.value("TextColor", QVariant(QColor(0, 0, 0))).value<QColor>());
}

//==================================================================================================

void TextToolBar::updateDiagramUnits()
{
	mFontSizeEdit->setUnits(mDiagram->scene()->units());
}

//==================================================================================================

void TextToolBar::updateFontFamily(const QString& family)
{
	emit propertyChanged("Font Family", QVariant(family));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontSize()
{
	emit propertyChanged("Font Size", QVariant(mFontSizeEdit->value()));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontBold(bool bold)
{
	emit propertyChanged("Font Bold", QVariant(bold));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontItalic(bool italic)
{
	emit propertyChanged("Font Italic", QVariant(italic));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontUnderline(bool underline)
{
	emit propertyChanged("Font Underline", QVariant(underline));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontOverline(bool overline)
{
	emit propertyChanged("Font Overline", QVariant(overline));
	emit propertiesChanged(properties());
}

void TextToolBar::updateFontStrikeOut(bool strikeOut)
{
	emit propertyChanged("Font Strike-Out", QVariant(strikeOut));
	emit propertiesChanged(properties());
}

void TextToolBar::updateAlignmentHorizontal(Qt::Alignment alignment)
{
	emit propertyChanged("Text Horizontal Alignment", QVariant((unsigned int)alignment));
	emit propertiesChanged(properties());
}

void TextToolBar::updateAlignmentVertical(Qt::Alignment alignment)
{
	emit propertyChanged("Text Vertical Alignment", QVariant((unsigned int)alignment));
	emit propertiesChanged(properties());
}

void TextToolBar::updateTextColor(const QColor& color)
{
	emit propertyChanged("Text Color", QVariant(color));
	emit propertiesChanged(properties());
}
