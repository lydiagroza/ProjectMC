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
               gameUI

# ========================================
# GameBase - toate clasele principale
# ========================================
HEADERS += GameBase/Board.h \
           GameBase/CardBase.h \
           GameBase/CardLoader.h \
           GameBase/CardNode.h \
           GameBase/CardsPyramid.h \
           GameBase/Game.h \
           GameBase/GameConstants.h \
           GameBase/gameExport.h \
           GameBase/GameSetup.h \
           GameBase/GuildLoader.h \
           GameBase/MilitaryTrack.h \
           GameBase/Player.h \
           GameBase/ProgressTokenLoader.h \
           GameBase/ProgressTokens.h \
           GameBase/Wonder.h \
           GameBase/WonderLoader.h

SOURCES += GameBase/Board.cpp \
           GameBase/CardBase.cpp \
           GameBase/CardLoader.cpp \
           GameBase/CardNode.cpp \
           GameBase/CardsPyramid.cpp \
           GameBase/Game.cpp \
           GameBase/GameSetup.cpp \
           GameBase/GuildLoader.cpp \
           GameBase/MilitaryTrack.cpp \
           GameBase/Player.cpp \
           GameBase/ProgressTokenLoader.cpp \
           GameBase/ProgressTokens.cpp \
           GameBase/WonderLoader.cpp \
           GameBase/Wonders.cpp

# CSV files din GameBase
OTHER_FILES += GameBase/AgeI.csv \
               GameBase/AgeII.csv \
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
           gameUI/CardWidget.h

SOURCES += gameUI/main.cpp \
           gameUI/MainWindow.cpp \
           gameUI/SplashScreen.cpp \
           gameUI/WonderSelectionWidget.cpp \
           gameUI/BoardWidget.cpp \
           gameUI/MilitaryTrackWidget.cpp \
           gameUI/CardWidget.cpp

# Dacă ai fișier .ui (design vizual)
# Decomentează linia de mai jos dacă există MainWindow.ui
# FORMS += gameUI/MainWindow.ui

# ========================================
# Resurse (imagini, icoane - optional)
# ========================================
# Dacă adaugi mai târziu imagini/resurse:
# RESOURCES += resources.qrc
