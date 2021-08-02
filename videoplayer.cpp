#include "videoplayer.h"
#include "userinfomanager.h"

#include <QDebug>

VideoPlayer::VideoPlayer(int _volume,WId _wid , QObject *parent) : wid(_wid), QObject(parent), is_playing(false), process(nullptr)
{
    time_current = 0.0f;
    time_length = 0.0f;
    time_percent = 0.0f;
    volume = _volume;
}

bool VideoPlayer::onCommand()
{
//    mutex.lock();

//    while(!command.empty())
//    {
//        QStringList list = command.front();
//        process->write(list);
//        command.pop_front();

//    }
    //    mutex.unlock();
}



void VideoPlayer::refresh()
{
    mutex.lock();
    if(process != nullptr && is_playing)
    {
        process->write("get_time_length\n");
        process->write("get_time_pos\n");
        process->write("get_percent_pos\n");
    }
    mutex.unlock();
}

void VideoPlayer::play(const QString &_filepath)
{
    {
        mutex.lock();
        if(process != nullptr)
        {
            process->write("quit\n");
            disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
            process->close();
            process->kill();
            delete process;
            process = nullptr;
            is_playing = false;
        }
        mutex.unlock();
    }

    time_current = 0.0f;
    time_length = 0.0f;
    time_percent = 0.0f;

    filepath = _filepath;
    QFileInfo playfile(filepath);
    if (!playfile.isFile())
    {
        return;
    }

    QStringList args;
    args << "-wid" << QString::number(wid);
    args << "-slave";
    args << "-quiet";
    args  << "-loop";
    args  << "0";
    //args << "-nokeepaspect";
    //args << "-osdlevel";
    //args << "0";
    args << "-framedrop";
    //args << "-colorkey";
    //args << "0x0";
    //args << "-mc";
    //args << "0";
    args << "-subcp";
    args << "cp936,UTF-8,UTF-16";
    //args << "-subfont";
    //args << "C:\\Windows\\Fonts\\simhei.ttf";
    args << "-autosync";
    args << "0";
    args << "-mc";
    args << "2.0";
    args << "-volume";
    args << QString::number(volume);
    args << "2";
    args << filepath;

    mutex.lock();
    process = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));

    process->start(UserInfoManager::Instance()->GetPlayerPath(), args);
    is_playing = true;
    mutex.unlock();
}

void VideoPlayer::stop()
{
    mutex.lock();
    if(is_playing)
    {
        process->write("pause\n");
        disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        is_playing = false;
    }
    mutex.unlock();
}

void VideoPlayer::quit()
{
    mutex.lock();
    if(process != nullptr)
    {
        process->close();
        disconnect(process);
        delete process;
        process = nullptr;
        is_playing = false;
        filepath = "";
    }
    mutex.unlock();
}

void VideoPlayer::pause(bool is_play)
{
    mutex.lock();
    if(process == nullptr)
    {
        mutex.unlock();
        return;
    }
    is_playing = !is_playing;
//    if(is_playing)
//    {
//        setVolumeEx(volume);
//    }

    process->write("pause\n");
//    if (is_play)
//    {
//        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));

//    }
//    else
//    {
//        disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
//    }
    mutex.unlock();
    if (!is_play)
    {
        dataRecieve();
    }
}

void VideoPlayer::setSpeed(double speed)
{
    mutex.lock();
    if(speed > 0 && process != nullptr)
    {
        process->write(QString("speed_set " + QString::number(speed) + " 2\n").toUtf8());
    }
    mutex.unlock();
}

void VideoPlayer::setCurrent(float current)
{
    {
        mutex.lock();
        if(process == nullptr)
        {
            mutex.unlock();
            return;
        }

        time_current = current;

        //disconnect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        //connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataRecieve()));
        mutex.unlock();
    }
    //dataRecieve();
    {
        qDebug()<<"seek current"<<current<<process->state();
        mutex.lock();
        process->write("pause\n");
        process->write(QString("seek " + QString::number(current) + " 2\n").toUtf8());
        qDebug()<<"volume"<<volume<<endl;
        is_playing = true;
        mutex.unlock();
    }


}

void VideoPlayer::setVolume(int _volume)
{
    mutex.lock();
    if(_volume >= 0 && process != nullptr)
    {

        volume = _volume;
        if(is_playing)
        {
            process->write(QString("volume " + QString::number(volume) + " 2\n").toUtf8());
        }
    }
    mutex.unlock();
}

void VideoPlayer::setVolumeEx(int _volume)
{
    if(_volume >= 0 && process != nullptr)
    {
        volume = _volume;
        if(is_playing)
        {
            process->write(QString("volume " + QString::number(volume) + " 2\n").toUtf8());
        }
    }
}

void VideoPlayer::dataRecieve()
{
    mutex.lock();
    //qDebug()<<"process"<<process;
    if(process != nullptr)
    {
        //qDebug()<<"all"<<process->readAll();
        while (process->canReadLine())
        {

            QByteArray b = process->readLine();

            b.replace(QByteArray("\n"), QByteArray(""));
            QString s(b);
            //qDebug()<<"readline "<<s;
            if (b.startsWith("ANS_TIME_POSITION"))
            {
                time_current = s.mid(18).toFloat();
            }
            else if (b.startsWith("ANS_LENGTH"))
            {
                time_length = s.mid(11).toFloat();
            }
            else if(b.startsWith("ANS_PERCENT_POSITION"))
            {
                time_percent = s.mid(21).toFloat();

            }
            else if(b.startsWith("Starting playback..."))
            {
                setVolumeEx(volume);
            }
        }
    }
    mutex.unlock();
}

float VideoPlayer::getCurrent() const
{
    return time_current;
}

float VideoPlayer::getPercent() const
{
    return time_percent;
}

float VideoPlayer::getTotal() const
{
    return time_length;
}

QString VideoPlayer::getFilepath() const
{
    return filepath;
}
