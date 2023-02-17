


#include <iostream>
#include <QApplication>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#include "gui/mainApplication.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


int main(int argc, char** argv) {
    std::cout<<"hi chere"<<std::endl;
    cv::Mat3f mat = cv::Mat3f::zeros(20,20);
    std::cout<<"let's dance" <<std::endl;



    QApplication* app = new QApplication(argc, argv);

    QCoreApplication::setApplicationName("MDI Example");
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationVersion("0.0");


    cv::Mat3b image = cv::imread("../T1/assets/Montage1.png");
    cv::resize(image,image,cv::Size(),0.3,0.3);



    MainApplication * mainApp = new MainApplication();
    mainApp->show();



    app->exec();
    mainApp->deleteLater();
    delete app;

    return 0;
}
