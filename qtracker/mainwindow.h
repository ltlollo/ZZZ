#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <vector>
#include <memory>

namespace Ui
{
class MainWindow;
}

namespace
{
constexpr int frameWidth{640}, frameHeight{480};
constexpr int maxObjCount{50};
constexpr int minObjArea{20*20}, maxObjArea = frameHeight*frameWidth/1.5;
constexpr int pointerRad{12};
constexpr int fps{30};
constexpr int cameraDev{1};
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private:
    int x{frameWidth/2}, y{frameHeight/2}, smallDim{0};
    cv::VideoCapture capture;
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QTimer> timer{new QTimer(this)};
    cv::Mat frameFiltered;  // filtered black and white
    cv::Mat frameHSV;       // hsv colored
    cv::Mat frameOrig;      // original frame
    int hmin{0}, hmax{256}, smin{0}, smax{256}, vmin{0}, vmax{256};
    bool showHSV{false};

    void removeSmallParticles();
    void trackFilteredObject();
    void drawObject();

private slots:
    void togglePlay();
    void processUpdate();
};

#endif // MAINWINDOW_H
