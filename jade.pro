TEMPLATE = app

win32:TARGET = Jade
!win32:TARGET = jade

DESTDIR = bin
INCLUDEPATH += source source/drawing source/widgets

CONFIG += release warn_on embed_manifest_dll c++11 qt
CONFIG -= debug
QT += widgets printsupport svg

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

# --------------------------------------------------------------------------------------------------

HEADERS += \
	source/drawing/DrawingChartItems.h \
	source/drawing/DrawingGlobals.h \
	source/drawing/DrawingItem.h \
	source/drawing/DrawingItemFactory.h \
	source/drawing/DrawingItemGroup.h \
	source/drawing/DrawingItemPoint.h \
	source/drawing/DrawingPathItem.h \
	source/drawing/DrawingPixmapItem.h \
	source/drawing/DrawingPolyItems.h \
	source/drawing/DrawingRectItems.h \
	source/drawing/DrawingScene.h \
	source/drawing/DrawingTextItem.h \
	source/drawing/DrawingTwoPointItems.h \
	source/drawing/DrawingTypes.h \
	source/drawing/DrawingUndo.h \
	source/drawing/DrawingView.h \
	source/drawing/Drawing \
	\
	source/widgets/ColorButton.h \
	source/widgets/EnumComboBox.h \
	source/widgets/EnumToolButton.h \
	source/widgets/ImageWidget.h \
	source/widgets/PointsListWidget.h \
	source/widgets/PositionWidget.h \
	source/widgets/UnitsValueEdit.h \
	\
	source/AboutDialog.h \
	source/DiagramMultipleItemPropertiesWidget.h \
	source/DiagramProperties.h \
	source/DiagramPropertiesWidget.h \
	source/DiagramScene.h \
	source/DiagramSingleItemPropertiesWidget.h \
	source/DiagramToolBar.h \
	source/DiagramToolBox.h \
	source/DiagramView.h \
	source/ExportOptionsDialog.h \
	source/MainWindow.h \
	source/PreferencesDialog.h

SOURCES += \
	source/drawing/DrawingChartItems.cpp \
	source/drawing/DrawingGlobals.cpp \
	source/drawing/DrawingItem.cpp \
	source/drawing/DrawingItemFactory.cpp \
	source/drawing/DrawingItemGroup.cpp \
	source/drawing/DrawingItemPoint.cpp \
	source/drawing/DrawingPathItem.cpp \
	source/drawing/DrawingPixmapItem.cpp \
	source/drawing/DrawingPolyItems.cpp \
	source/drawing/DrawingRectItems.cpp \
	source/drawing/DrawingScene.cpp \
	source/drawing/DrawingTextItem.cpp \
	source/drawing/DrawingTwoPointItems.cpp \
	source/drawing/DrawingTypes.cpp \
	source/drawing/DrawingUndo.cpp \
	source/drawing/DrawingView.cpp \
	\
	source/widgets/ColorButton.cpp \
	source/widgets/EnumComboBox.cpp \
	source/widgets/EnumToolButton.cpp \
	source/widgets/ImageWidget.cpp \
	source/widgets/PointsListWidget.cpp \
	source/widgets/PositionWidget.cpp \
	source/widgets/UnitsValueEdit.cpp \
	\
	source/AboutDialog.cpp \
	source/DiagramMultipleItemPropertiesWidget.cpp \
	source/DiagramProperties.cpp \
	source/DiagramPropertiesWidget.cpp \
	source/DiagramScene.cpp \
	source/DiagramSingleItemPropertiesWidget.cpp \
	source/DiagramToolBar.cpp \
	source/DiagramToolBox.cpp \
	source/DiagramView.cpp \
	source/ExportOptionsDialog.cpp \
	source/MainWindow.cpp \
	source/PreferencesDialog.cpp \
	source/main.cpp

RESOURCES += icons/icons.qrc lib/lib.qrc

OTHER_FILES += icons/icons.rc lib/electric.jlb lib/logic.jlb
RC_FILE = icons/icons.rc

# --------------------------------------------------------------------------------------------------

# Installation information
!win32:target.path += /usr/bin
INSTALLS += target

# For full uninstall on non-Windows OSes, delete the following files:
#     - Delete file /usr/bin/jade
#     - Delete folder /usr/share/apps/jade (if exists)
#     - Delete folder ~/.jade for each user on the system (if exists)
