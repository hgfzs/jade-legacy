/* ImageWidget.cpp
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

#include "ImageWidget.h"

ImageWidget::ImageWidget() : QWidget()
{
	mLabel = new QLabel();
	mLabel->setAlignment(Qt::AlignCenter);

	mOpenButton = new QPushButton(QIcon(":/icons/oxygen/document-open.png"), "");
	mClearButton = new QPushButton(QIcon(":/icons/oxygen/edit-delete.png"), "");
	connect(mOpenButton, SIGNAL(clicked()), this, SLOT(openImage()));
	connect(mClearButton, SIGNAL(clicked()), this, SLOT(clearImage()));

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(mLabel, 0, 0, 3, 1);
	layout->addWidget(mOpenButton, 0, 1);
	layout->addWidget(mClearButton, 1, 1);
	layout->setRowStretch(2, 100);
	layout->setColumnStretch(0, 100);
	setLayout(layout);

	setFixedHeight(6 * QFontMetrics(font()).height());
}

//==================================================================================================

ImageWidget::~ImageWidget() { }

void ImageWidget::setPixmap(const QPixmap& pixmap)
{
	mPixmap = pixmap;

	if (mPixmap.isNull()) mLabel->setText("<no image>");
	else mLabel->setPixmap(mPixmap);
}

QPixmap ImageWidget::pixmap() const
{
	return mPixmap;
}

//==================================================================================================

void ImageWidget::openImage()
{
	QString filePath = QFileDialog::getOpenFileName(this, "Open Image");
	if (!filePath.isEmpty()) setPixmap(QPixmap(filePath));
}

void ImageWidget::clearImage()
{
	setPixmap(QPixmap());
}
