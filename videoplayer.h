#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QProcess>
#include <QWidget>
#include <QMutex>
#include <QProcess>
class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(int _volume,WId _wid , QObject *parent = 0);

    int GetVolume() const
    {
        return volume;
    }

    bool onCommand();

public slots:
    void refresh();
    void play(const QString &_filepath);
    void stop();
    void pause(bool is_pause);
    void quit();
    void setSpeed(double speed);
    void setCurrent(float current);
    void setVolume(int volume);
    void setVolumeEx(int volume);
    float getCurrent() const;
    float getPercent() const;
    float getTotal() const;
    QString getFilepath() const;
private slots:
    void dataRecieve();

private:
    QMutex  mutex;
    WId wid;
    QString filepath;
    QProcess *process = nullptr;
    bool is_playing = false;
    float time_current;
    float time_length;
    float time_percent;
    int volume =50;

    int state = 0;
    std::list<QStringList > command;
};

#endif // VIDEOPLAYER_H
