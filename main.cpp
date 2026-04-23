#include <iostream>
#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QIcon>

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

#include <QDir>
#include "QFileInfo"
#include <QUrl>

#include <QSlider>
#include <QPushButton>
#include <QFileDialog>

class MyAppEdits {
private:
    // Create window
    QWidget *win = new QWidget();

    // First layout
    QVBoxLayout *layV = new QVBoxLayout(win);
    QVideoWidget *videoW = new QVideoWidget(win); // Video

    vector<QUrl> urls;
    int IdVideo = 0;
    bool buttonRepeat = false;

    QMediaPlayer *player = new QMediaPlayer(win); // player
    QAudioOutput *audioOutput = new QAudioOutput(win); // audio

    QSlider *slider = new QSlider(Qt::Horizontal); // slider

    // Second layout
    QHBoxLayout *layH = new QHBoxLayout();
    QPushButton *back = new QPushButton(); // button - back
    QPushButton *repeat = new QPushButton(); // button - repeat
    QPushButton *playStop = new QPushButton(); // button - playStop
    QPushButton *ranDom = new QPushButton(); // button - random
    QPushButton *forward = new QPushButton(); // button - forward


    // Settings
    void setWin() {
        win->resize(365, 282);
        win->setWindowTitle("Edits");
        win->setWindowIcon(QIcon(":/Icons/EditsIcon.png"));
    }
    void loadStyle() {
        QFile styleFile(":/style.css");
        if (styleFile.open(QFile::ReadOnly)) {
            win->setStyleSheet(styleFile.readAll());
            styleFile.close();
        } else {
            qDebug() << "File don`t open";
        }
    }

    // Layouts
    void Layout() {
        layV->addWidget(videoW);
        setPlayer();
        layV->addWidget(slider);
        setSlider();
        layV->addLayout(layH);
        SecondLayout();
    }
    void SecondLayout() {
        setButton();
        layH->addWidget(back);
        layH->addWidget(repeat);
        layH->addWidget(playStop);
        layH->addWidget(ranDom);
        layH->addWidget(forward);
    }

    // Other
    void setPlayer() {
        player->setAudioOutput(audioOutput);
        player->setVideoOutput(videoW);
    }
    void setSlider() {
        slider->setRange(0, 100);
        slider->setValue(80);
        audioOutput->setVolume(0.8);
        slider->setSingleStep(1);
        slider->setPageStep(10);
    }
    void setButton() {
        back->setIcon(QIcon(":/Icons/Back.png"));
        repeat->setIcon(QIcon(":/Icons/Repeat.png"));
        playStop->setIcon(QIcon(":/Icons/Pause.png"));
        ranDom->setIcon(QIcon(":/Icons/Random.png"));
        forward->setIcon(QIcon(":/Icons/Forward.png"));

        back->setIconSize(QSize(16,16));
        repeat->setIconSize(QSize(32,32));
        playStop->setIconSize(QSize(16,16));
        ranDom->setIconSize(QSize(32,32));
        forward->setIconSize(QSize(16,16));

        back->setFixedSize(40, 40);
        repeat->setFixedSize(40, 40);
        repeat->setObjectName("repeatBtn");
        playStop->setFixedSize(40, 40);
        ranDom->setFixedSize(40, 40);
        forward->setFixedSize(40, 40);
    }

    // vector - urls
    void setUrls() { // find all videos
        QDir dir(QFileDialog::getExistingDirectory()); // set directory

        QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (QFileInfo file : files) {
            QString ext = file.suffix().toLower();
            if (ext == "mp4" || ext == "mkv" || ext == "avi") {
                urls.push_back(QUrl::fromLocalFile(file.absoluteFilePath()));
            }
        }
    }
    int restart(int sizeVector, int index) {
        if (index + 1 >= sizeVector) {
            return 0;
        }
        return index + 1;
    }
    int restartBack(int sizeVector, int index) {
        if (index - 1 < 0) {
            return sizeVector-1;
        }
        return index - 1;
    }
    void randomSort(int indexMoment) {
        int vecSize = urls.size();

        vector<QUrl> v;
        for (auto i : urls) {
            v.push_back(i);
        }

        random_device rd;
        mt19937 mt(rd());

        shuffle(v.begin(), v.end(), mt);

        for (int i = 0; i < vecSize; i++) {
            if (i == indexMoment) {
                auto it = v[0];
                v[0] = v[i];
                v[i] = it;
            }
        }

        for (int i = 0; i < vecSize; i++) {
            urls[i] = v[i];
        }
    }

    // Videos
    void videoLaunch(int index) {
        player->setSource(urls[index]);
        player->play();
    }

public:
    void start() {
        // Settings
        setWin(); // Window
        loadStyle(); // CSS Styles
        Layout(); // Layouts
        setUrls(); // Urls
        videoLaunch(0); // Start

        // Signals
        QObject::connect(player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) mutable {
            if (status == QMediaPlayer::EndOfMedia) {
                if (!buttonRepeat) {
                    IdVideo = restart(urls.size(), IdVideo);
                }
                videoLaunch(IdVideo);
            }
        });

        QObject::connect(slider, &QSlider::valueChanged, [this](int value) {
            audioOutput->setVolume(value/100.0);
        });

        QObject::connect(back, &QPushButton::clicked, [this]() {
            IdVideo = restartBack(urls.size(), IdVideo);
            videoLaunch(IdVideo);
        });
        QObject::connect(repeat, &QPushButton::clicked, [this]() {
            buttonRepeat = !buttonRepeat;

            repeat->setProperty("active", buttonRepeat);

            repeat->style()->unpolish(repeat);
            repeat->style()->polish(repeat);
            repeat->update();
        });
        QObject::connect(playStop, &QPushButton::clicked, [this]() {
            if (player->isPlaying()) {
                playStop->setIcon(QIcon(":/Icons/Play.png"));
                player->pause();
            } else {
                playStop->setIcon(QIcon(":/Icons/Pause.png"));
                player->play();
            }
        });
        QObject::connect(ranDom, &QPushButton::clicked, [this]() {
            randomSort(IdVideo);
            IdVideo = 0;
        });
        QObject::connect(forward, &QPushButton::clicked, [this]() {
            IdVideo = restart(urls.size(), IdVideo);
            videoLaunch(IdVideo);
        });

        win->show();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MyAppEdits myAppEdits;
    myAppEdits.start();

    return app.exec();
}
