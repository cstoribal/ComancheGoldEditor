//
// Created by krist on 04/01/2023.
//

#ifndef HELLOWORLD_MAINWINDOW_H
#define HELLOWORLD_MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "qt/qtFunctions.h"



class Timer : public QObject{
    Q_OBJECT
public:
    Timer();
    ~Timer();
public slots:
    void loop();
signals:
    void finished();
    void update();
};


class Worker : public QObject {
    Q_OBJECT
public:
    Worker();
    ~Worker();
    void loop();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
    void update();
private:
    // add your variables here
};






class MainWindow : public QWidget {
    Q_OBJECT
    QPushButton *mButton=nullptr;
    QPushButton *mOpenSelector=nullptr;
    QLabel *mImageShower=nullptr;

    QVBoxLayout *mVBoxLayout=nullptr;

    int mDirection;
    cv::Mat3b mImage;
    cv::Point2i mPoint;
    cv::Vec3b mColor;

public:
    MainWindow();
    virtual ~MainWindow();

    void setImage(const cv::Mat3b & image);

    void updateImage();


 public slots:
    void changeDirection(int dir=-1){
        if(dir<0){
            mDirection += 1;
        } else {
            mDirection = dir;
        }
        mDirection = mDirection%4;
    }


private slots:
    void internalEvolution();

};


#endif //HELLOWORLD_MAINWINDOW_H
