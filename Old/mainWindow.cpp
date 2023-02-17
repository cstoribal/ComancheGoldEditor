//
// Created by krist on 04/01/2023.
//

#include "mainWindow.h"
#include "qapplication.h"



#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <cstdlib>


#include <QThread>

Timer::Timer() {

}

Timer::~Timer(){

}

void Timer::loop(){
    while(1){
       Sleep(1000);
       emit update();
       QApplication::processEvents();
    }
    emit finished();
}

Worker::Worker() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
}

Worker::~Worker() { // Destructor
    // free resources
}

void Worker::process() { // Process. Start processing data.
    // allocate resources using new here
    while(1){
        loop();
    }
    qDebug("Hello World!");
    emit finished();
}

void Worker::loop(){
    emit update();
    return;
}






MainWindow::MainWindow():mDirection(0){
    mPoint=cv::Point2i(0,0);
    mColor[0]=0;mColor[1]=0;mColor[2]=0;
    mButton = new QPushButton("Button",this);
    mOpenSelector = new QPushButton("Open Selector",this);
    mVBoxLayout = new QVBoxLayout(this);
    mImageShower = new QLabel(this);
    mVBoxLayout->addWidget(mButton);
    mVBoxLayout->addWidget(mOpenSelector);
    mVBoxLayout->addWidget(mImageShower);
    mImageShower->show();

    QObject::connect(mButton,
            SIGNAL(clicked()),
            this,
            SLOT(changeDirection()));



}

MainWindow::~MainWindow(){};

void MainWindow::setImage(const cv::Mat3b & image){
    if(image.empty()){
        return ;
    }
    mImage = image;
    mImageShower->setPixmap(QPixmap::fromImage(cvMatToQImage(mImage)));



    QThread* thread2 = new QThread();
    Timer* timer = new Timer();
    timer->moveToThread(thread2);
    //connect( worker, &Worker::error, this, &MyClass::errorString);
    connect( thread2, &QThread::started,  timer, &Timer::loop);
    connect( timer, &Timer::update,    this,    &MainWindow::updateImage);
    connect( timer, &Timer::finished,  thread2, &QThread::quit);
    connect( timer, &Timer::finished,  timer, &Timer::deleteLater);
    connect( thread2, &QThread::finished, thread2, &QThread::deleteLater);
    thread2->start();




    QThread* thread = new QThread();
    Worker* worker = new Worker();
    worker->moveToThread(thread);
    //connect( worker, &Worker::error, this, &MyClass::errorString);
    connect( mOpenSelector, &QPushButton::pressed,  worker, &Worker::process);
    connect( worker, &Worker::update,    this,   &MainWindow::internalEvolution);
    connect( worker, &Worker::finished,  thread, &QThread::quit);
    connect( worker, &Worker::finished,  worker, &Worker::deleteLater);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();

    return;
}





void MainWindow::internalEvolution() {
    if(mImage.empty()){return;}
    int cols = mImage.cols;
    int rows = mImage.rows;

    if(mDirection==0){
        mPoint.x = (mPoint.x+1) % cols;
        if(mPoint.x==0){
            mPoint.y=(mPoint.y+1) % rows;
            if(mPoint.y==0){
                mColor[0]+=20;
                mColor[1]+=40;
                mColor[2]+=70;
            }
        }
    }
    if(mDirection==1){
        mPoint.y = (mPoint.y+1) % rows;
        if(mPoint.y==0){
            mPoint.x=(mPoint.x+1) % cols;
            if(mPoint.x==0){
                mColor[0]+=20;
                mColor[1]+=40;
                mColor[2]+=70;
            }
        }
    }
    if(mDirection==2){
        mPoint.x = mPoint.x-1;
        if(mPoint.x==-1){
            mPoint.x=cols-1;
            mPoint.y=mPoint.y - 1;
            if(mPoint.y==-1){
                mPoint.y=rows -1 ;
                mColor[0]+=20;
                mColor[1]+=40;
                mColor[2]+=70;
            }
        }
    }
    if(mDirection==3){
        mPoint.y = mPoint.y-1;
        if(mPoint.y==-1){
            mPoint.y=rows-1;
            mPoint.x=mPoint.x - 1;
            if(mPoint.x==-1){
                mPoint.x=cols-1 ;
                mColor[0]+=20;
                mColor[1]+=40;
                mColor[2]+=70;
            }
        }
    }

    mImage(mPoint)=cv::Vec3b((mColor[0]+mImage(mPoint)[0])%256,
                             (mColor[1]+mImage(mPoint)[1])%256,
                             (mColor[2]+mImage(mPoint)[2])%256);
    return;
}


void MainWindow::updateImage(){
    mImageShower->setPixmap(QPixmap::fromImage(cvMatToQImage(mImage)));
    return;
}

