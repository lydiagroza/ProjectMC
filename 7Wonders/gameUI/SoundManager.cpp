#include "SoundManager.h"

SoundManager& SoundManager::instance()
{
    static SoundManager _instance;
    return _instance;
}

SoundManager::SoundManager()
    : QObject(nullptr)
{
    //Setup Quack Player
    m_quackPlayer = new QMediaPlayer(this);
    m_quackOutput = new QAudioOutput(this);
    m_quackPlayer->setAudioOutput(m_quackOutput);
    m_quackOutput->setVolume(1.0);
    m_quackPlayer->setSource(QUrl("qrc:/resources/UI/quack.mp3"));

    // Setup Click Player
    m_clickPlayer = new QMediaPlayer(this);
    m_clickOutput = new QAudioOutput(this);
    m_clickPlayer->setAudioOutput(m_clickOutput);
    m_clickOutput->setVolume(0.8);
    m_clickPlayer->setSource(QUrl("qrc:/resources/UI/click.mp3"));

    // Setup Sword Player
    m_swordPlayer = new QMediaPlayer(this);
    m_swordOutput = new QAudioOutput(this);
    m_swordPlayer->setAudioOutput(m_swordOutput);
    m_swordOutput->setVolume(1.0);
    m_swordPlayer->setSource(QUrl("qrc:/resources/UI/sword.mp3"));

    // Setup Victory Player
    m_victoryPlayer = new QMediaPlayer(this);
    m_victoryOutput = new QAudioOutput(this);
    m_victoryPlayer->setAudioOutput(m_victoryOutput);
    m_victoryOutput->setVolume(1.0);
    m_victoryPlayer->setSource(QUrl("qrc:/resources/UI/victory.mp3"));
}

SoundManager::~SoundManager()
{
}

void SoundManager::playQuack()
{
    if (m_quackPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_quackPlayer->stop();
    }
    m_quackPlayer->play();
}

void SoundManager::playClick()
{
    if (m_clickPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_clickPlayer->stop();
    }
    m_clickPlayer->play();
}

void SoundManager::playSword()
{
    if (m_swordPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_swordPlayer->stop();
    }
    m_swordPlayer->play();
}

void SoundManager::playVictory()
{
    if (m_victoryPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_victoryPlayer->stop();
    }
    m_victoryPlayer->play();
}
