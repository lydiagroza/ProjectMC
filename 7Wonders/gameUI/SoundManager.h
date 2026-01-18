#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class SoundManager : public QObject
{
    Q_OBJECT
public:
    static SoundManager& instance();
    
    void playQuack();
    void playClick();
    void playSword();
    void playVictory();

private:
    SoundManager();
    ~SoundManager();
    
    QMediaPlayer* m_quackPlayer;
    QAudioOutput* m_quackOutput;
    
    QMediaPlayer* m_clickPlayer;
    QAudioOutput* m_clickOutput;
    
    QMediaPlayer* m_swordPlayer;
    QAudioOutput* m_swordOutput;
    
    QMediaPlayer* m_victoryPlayer;
    QAudioOutput* m_victoryOutput;
};

#endif // SOUNDMANAGER_H
