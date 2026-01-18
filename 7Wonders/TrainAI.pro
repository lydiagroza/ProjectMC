QT += core
CONFIG += c++20 console
CONFIG -= app_bundle

TARGET = TrainAI
DESTDIR = .

# Include paths
INCLUDEPATH += GameBase gameAI ConsoleGame

# Headers & Sources
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
           GameBase/Wonder.h \
           GameBase/GuildEffects.h \
           gameAI/AI_Player.h

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
           GameBase/Wonder.cpp \
           GameBase/GuildEffects.cpp \
           gameAI/AI_Player.cpp \
           ConsoleGame/Source.cpp

DEFINES += GAMEBASE_STATIC
