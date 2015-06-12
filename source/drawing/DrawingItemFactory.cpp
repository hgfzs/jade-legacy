/* DrawingItemFactory.cpp
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

#include <DrawingItemFactory.h>
#include <DrawingItem.h>
#include <DrawingItemGroup.h>
#include <DrawingRectItems.h>
#include <DrawingTwoPointItems.h>
#include <DrawingPolyItems.h>
#include <DrawingTextItem.h>
#include <DrawingPathItem.h>
#include <DrawingPixmapItem.h>
#include <DrawingChartItems.h>

DrawingItemFactory::DrawingItemFactory() : QObject() { }

DrawingItemFactory::~DrawingItemFactory()
{
	clear();
}

//==================================================================================================

DrawingItemFactory& DrawingItemFactory::operator=(const DrawingItemFactory& factory)
{
	mDefaultProperties = factory.mDefaultProperties;
	return *this;
}

//==================================================================================================

void DrawingItemFactory::setDefaultProperties(const QHash<QString, QVariant>& defaultProperties)
{
	mDefaultProperties = defaultProperties;
}

QHash<QString, QVariant> DrawingItemFactory::defaultProperties() const
{
	return mDefaultProperties;
}

bool DrawingItemFactory::containsDefaultProperty(const QString& key) const
{
	return mDefaultProperties.contains(key);
}

void DrawingItemFactory::setDefaultPropertyValue(const QString& key, const QVariant& value)
{
	mDefaultProperties[key] = value;
}

QVariant DrawingItemFactory::defaultPropertyValue(const QString& key) const
{
	return (mDefaultProperties.contains(key)) ? mDefaultProperties[key] : QVariant();
}

//==================================================================================================

void DrawingItemFactory::registerItem(DrawingItem* item)
{
	if (item) mItems.append(item);
}

void DrawingItemFactory::clear()
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
}

bool DrawingItemFactory::isEmpty() const
{
	return mItems.isEmpty();
}

//==================================================================================================

DrawingItem* DrawingItemFactory::create(const QString& key) const
{
	DrawingItem* item = nullptr;

	for(auto itemIter = mItems.begin(); item == nullptr && itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->uniqueKey() == key) item = (*itemIter)->copy();
	}

	if (item)
	{
		for(auto pIter = mDefaultProperties.begin(); pIter != mDefaultProperties.end(); pIter++)
		{
			if (item->containsProperty(pIter.key()))
				item->setPropertyValue(pIter.key(), pIter.value());
		}
	}

	return item;
}
