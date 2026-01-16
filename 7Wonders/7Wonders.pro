QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 7Wonders
TEMPLATE = app
CONFIG += c++20

# Dezactivează DLL export pentru Qt build
DEFINES += GAMEBASE_STATIC

# Include paths pentru toate folderele
INCLUDEPATH += GameBase \
               gameAI \
               ConsoleGame \
               gameUI \
               UI

# ========================================
# GameBase - toate clasele principale
# ========================================
HEADERS += GameBase/Board.h \
           GameBase/CardBase.h \
           GameBase/CardNode.h \
           GameBase/CardsPyramid.h \
           GameBase/Game.h \
           GameBase/GameConstants.h \
           GameBase/gameExport.h \
           GameBase/GameSetup.h \
           GameBase/MilitaryTrack.h \
           GameBase/Player.h \
           GameBase/ProgressTokenLoader.h \
           GameBase/ProgressTokens.h \
           GameBase/UniversalCardLoader.h \
           GameBase/Wonder.h

SOURCES += GameBase/Board.cpp \
           GameBase/CardBase.cpp \
           GameBase/CardNode.cpp \
           GameBase/CardsPyramid.cpp \
           GameBase/Game.cpp \
           GameBase/GameSetup.cpp \
           GameBase/MilitaryTrack.cpp \
           GameBase/Player.cpp \
           GameBase/UniversalCardLoader.cpp \
           GameBase/ProgressTokenLoader.cpp \
           GameBase/ProgressTokens.cpp \
           GameBase/Wonder.cpp

# CSV files din GameBase
OTHER_FILES += GameBase/AgeI.csv \
               GameBase/AgeII.csv \
               GameBase/AgeIII.csv \
               GameBase/Guilds.csv \
               GameBase/ProgressTokens.csv \
               GameBase/Wonders.csv

# ========================================
# gameAI - inteligența artificială
# ========================================
HEADERS += gameAI/AI_Player.h
SOURCES += gameAI/AI_Player.cpp

# ========================================
# ConsoleGame - testEngine
# ========================================
HEADERS += ConsoleGame/testEngine.h
SOURCES += ConsoleGame/testEngine.cpp


# NU includem ConsoleGame/main.cpp pentru că gameUI are propriul main!

# ========================================
# gameUI - interfața grafică Qt
# ========================================
HEADERS += gameUI/MainWindow.h \
           gameUI/SplashScreen.h \
           gameUI/WonderSelectionWidget.h \
           gameUI/BoardWidget.h \
           gameUI/MilitaryTrackWidget.h \
           gameUI/CardWidget.h \
           gameUI/PlayerDashboardWidget.h \
           gameUI/NameSelectionWidget.h \
           gameUI/WonderChoiceDialog.h \
           gameUI/DiscardedCardsDialog.h

SOURCES += gameUI/main.cpp \
           gameUI/MainWindow.cpp \
           gameUI/SplashScreen.cpp \
           gameUI/WonderSelectionWidget.cpp \
           gameUI/BoardWidget.cpp \
           gameUI/MilitaryTrackWidget.cpp \
           gameUI/CardWidget.cpp \
           gameUI/PlayerDashboardWidget.cpp \
           gameUI/NameSelectionWidget.cpp \
           gameUI/WonderChoiceDialog.cpp \
           gameUI/DiscardedCardsDialog.cpp

# ========================================
# UI - fișiere de design .ui (opțional)
# ========================================
HEADERS += UI/ui_globals.h
OTHER_FILES += UI/README.txt
FORMS += UI/WonderSelectionWidget.ui \
         UI/MainWindow.ui \
         UI/SplashScreen.ui \
         UI/CardWidget.ui \
         UI/BoardWidget.ui \
         UI/MilitaryTrackWidget.ui \
         UI/PlayerDashboardWidget.ui \
         UI/NameSelectionWidget.ui \
         UI/WonderChoiceDialog.ui \
         UI/DiscardedCardsDialog.ui

# Dacă ai fișier .ui (design vizual)
# Decomentează linia de mai jos dacă există MainWindow.ui
# FORMS += gameUI/MainWindow.ui

# ========================================
# Resurse
# ========================================
RESOURCES += resources.qrc
