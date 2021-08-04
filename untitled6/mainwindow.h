#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAxWidget>
#include <QToolButton>
#include <QSlider>
#include <QWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    Q_ENUMS(ReadyStateConstants)

    enum PlayStateConstants { Stopped = 0, Paused = 1, Playing = 2 };
    enum ReadyStateConstants { Uninitialized = 0, Loading = 1,
     Interactive = 3, Complete = 4 };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



//播放进度
protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;

private:
 QAxWidget *axWidget;//播放器
 QToolButton *openButton;//浏览按钮
 QToolButton *playPauseButton;//暂停播放按钮
 QToolButton *stopButton;//停止播放按钮
 QSlider *seekSlider;//进度条
 QString fileFilters;//文件格式
 int updateTimer;//播放进度
 QWidget *widget;//QWidget
private slots:
 void onPlayStateChange(int oldState, int newState);
 void onReadyStateChange(ReadyStateConstants readyState);
 void onPositionChange(double oldPos, double newPos);
 void sliderValueChanged(int newValue);
 void openFile();
};

#endif // MAINWINDOW_H
