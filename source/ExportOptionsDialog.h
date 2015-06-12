/* ExportOptionsDialog.h
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

#ifndef EXPORTOPTIONSDIALOG_H
#define EXPORTOPTIONSDIALOG_H

#include <Drawing>

class DiagramView;
class RenderModeComboBox;

class ExportOptionsDialog : public QDialog
{
	Q_OBJECT

private:
	QLineEdit* mWidthEdit;
	QLineEdit* mHeightEdit;
	QCheckBox* mMaintainAspectRatioCheck;

	RenderModeComboBox* mRenderModeCombo;
	QCheckBox* mDrawBackgroundCheck;
	QCheckBox* mDrawBorderCheck;
	QCheckBox* mDrawGridCheck;

	DiagramView* mDiagram;

public:
	ExportOptionsDialog(DiagramView* diagram);
	~ExportOptionsDialog();

	void setPrevious(int width, int height,
		DrawingStyleOptions::ColorMode renderMode, DrawingStyleOptions::RenderFlags renderFlags);
	QSize exportSize() const;
	DrawingStyleOptions::ColorMode renderMode() const;
	DrawingStyleOptions::RenderFlags renderFlags() const;

private slots:
	void updateWidth();
	void updateHeight();

private:
	QGroupBox* createSizeGroup();
	QGroupBox* createOptionsGroup();
	QDialogButtonBox* createButtonBox();
};

#endif
