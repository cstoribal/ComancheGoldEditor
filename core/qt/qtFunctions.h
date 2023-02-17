//
// Created by zomby on 03/09/18.
//

#ifndef SPX_FUNCTIONS_H
#define SPX_FUNCTIONS_H

#include <QtGui/QImage>
#include <QDebug>
#include <opencv2/core/mat.hpp>

// By Bastien Oudot
QImage cvMatToQImage(const cv::Mat &inMat);

// By Cstoribal - erroneous
//cv::Mat qImageToCvMat(const QImage &inMat);

//http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);
QImage mat_to_qimage_cpy(cv::Mat const &mat, QImage::Format format);
cv::Mat qimage_to_mat_ref(QImage &img, int format);
cv::Mat qimage_to_mat_cpy(QImage const &img, int format);


#endif //SPX_FUNCTIONS_H
