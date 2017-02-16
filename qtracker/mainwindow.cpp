#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    capture(cameraDev),
    ui{new Ui::MainWindow}
{
    ui->setupUi(this);

    if (!capture.isOpened())
    {
        ui->dbgConsole->appendPlainText(QString("Broken capture"));
        return;
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);

    connect(ui->btnResume, &QPushButton::clicked, [&](){ togglePlay(); });
    connect(timer.get(), &QTimer::timeout, [&](){ processUpdate(); });

    hmin = ui->hminSlider->value();
    hmax = ui->hmaxSlider->value();
    vmin = ui->vminSlider->value();
    vmax = ui->vmaxSlider->value();
    smin = ui->sminSlider->value();
    smax = ui->smaxSlider->value();
    showHSV = ui->tglHSV->isChecked();
    smallDim = ui->dimSlider->value();

    connect(ui->hminSlider, &QSlider::valueChanged,
            [&](const int v){hmin = v;});
    connect(ui->hmaxSlider, &QSlider::valueChanged,
            [&](const int v){hmax = v;});
    connect(ui->sminSlider, &QSlider::valueChanged,
            [&](const int v){smin = v;});
    connect(ui->smaxSlider, &QSlider::valueChanged,
            [&](const int v){smax = v;});
    connect(ui->vminSlider, &QSlider::valueChanged,
            [&](const int v){vmin = v;});
    connect(ui->vmaxSlider, &QSlider::valueChanged,
            [&](const int v){vmax = v;});
    connect(ui->tglHSV, &QCheckBox::toggled,
            [&](const bool v){ showHSV = v; });
    connect(ui->dimSlider, &QSlider::valueChanged,
            [&](const int v){smallDim = v;});

    timer->start(fps);
}

MainWindow::~MainWindow()
{
    capture.release();
}

void MainWindow::processUpdate()
{
    if (!capture.read(frameOrig) || frameOrig.empty())
    {
        ui->dbgConsole->appendPlainText(QString("Broken capture"));
        togglePlay();
        return;
    }

    cvtColor(frameOrig, frameHSV, COLOR_BGR2HSV);
    inRange(frameHSV, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax),
            frameFiltered);
    removeSmallParticles();
    trackFilteredObject();

    QImage cameraFrame = showHSV ? QImage(static_cast<uchar*>(frameHSV.data),
                                          frameHSV.cols, frameHSV.rows,
                                          frameHSV.step,
                                          QImage::Format_RGB888) :
                                   QImage(static_cast<uchar*>(frameOrig.data),
                                          frameOrig.cols, frameOrig.rows,
                                          frameOrig.step,
                                          QImage::Format_RGB888);

    QImage cameraFilter(static_cast<uchar*>(frameFiltered.data),
                        frameFiltered.cols, frameFiltered.rows,
                        frameFiltered.step, QImage::Format_Indexed8);

    cameraFrame = cameraFrame.mirrored(true, false).rgbSwapped();
    cameraFilter = cameraFilter.mirrored(true, false);

    ui->capturedLbl->setPixmap(QPixmap::fromImage(cameraFrame));
    ui->filteredLbl->setPixmap(QPixmap::fromImage(cameraFilter));
}

void MainWindow::togglePlay()
{
    if (timer->isActive())
    {
        timer->stop();
        ui->btnResume->setText("Play");
        ui->btnResume->setChecked(false);
    }
    else
    {
        timer->start(fps);
        ui->btnResume->setText("Stop");
        ui->btnResume->setChecked(true);
    }
}


void MainWindow::removeSmallParticles()
{
    Mat erodeEle{getStructuringElement(MORPH_RECT, Size(smallDim, smallDim))};
    Mat dilateEle{getStructuringElement(MORPH_RECT, Size(smallDim, smallDim))};

    erode(frameFiltered, frameFiltered, erodeEle);
    dilate(frameFiltered, frameFiltered, dilateEle);
}

void MainWindow::trackFilteredObject()
{

    Mat tmp;
    frameFiltered.copyTo(tmp);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(tmp, contours, hierarchy, CV_RETR_CCOMP,
                 CV_CHAIN_APPROX_SIMPLE);

    if (!hierarchy.size()) return;
    if(hierarchy.size() > maxObjCount)
    {
        ui->dbgConsole->appendPlainText(QString("Too much noise"));
        return;
    }

    double currArea{.0};
    bool objectFound{false};
    int i{0};

    do
    {
        Moments moment{moments(static_cast<Mat>(contours[i]))};
        double area{moment.m00};

        if(area > minObjArea && area < maxObjArea && area > currArea)
        {
            x = moment.m10/area;
            y = moment.m01/area;
            objectFound = true;
            currArea = area;
        }
        //else objectFound = false;
        i = hierarchy[i][0];
    }
    while (i >= 0);

    if(objectFound) drawObject();

}

void MainWindow::drawObject()
{
    if(y>pointerRad)
    {
        line(frameOrig, Point(x, y), Point(x, y - pointerRad),
             Scalar(255, 0, 0), 2);
    }
    else line(frameOrig, Point(x, y), Point(x, 0), Scalar(255, 0, 0), 2);

    if(y < frameHeight - pointerRad)
    {
        line(frameOrig, Point(x, y), Point(x, y + pointerRad),
             Scalar(255, 0, 0), 2);
    }
    else
    {
        line(frameOrig, Point(x, y), Point(x, frameHeight - 1),
             Scalar(255, 0, 0), 2);
    }

    if(x > pointerRad)
    {
        line(frameOrig, Point(x, y), Point(x - pointerRad, y),
             Scalar(255, 0, 0), 2);
    }
    else line(frameOrig, Point(x, y), Point(0, y), Scalar(255, 0, 0), 2);

    if(x < frameWidth - pointerRad)
    {
        line(frameOrig, Point(x, y), Point(x + pointerRad, y),
             Scalar(255, 0, 0), 2);
    }
    else
    {
        line(frameOrig, Point(x, y), Point(frameWidth - 1, y),
             Scalar(255, 0, 0), 2);
    }
    ui->dbgConsole->appendPlainText(QString("Position: ") + QString::number(x)
                                    + ", " + QString::number(y));

}
