//
// Created by zomby on 03/09/18.
//

#include "qtFunctions.h"



QImage cvMatToQImage(const cv::Mat &inMat)
{
    switch ( inMat.type() )
    {
        // 8-bit, 4 channel
        case CV_8UC4:
        {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_ARGB32 );

            return image.rgbSwapped();
        }

            // 8-bit, 3 channel
        case CV_8UC3:
        {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_RGB888 );

            return image.rgbSwapped();
        }

            // 8-bit, 1 channel
        case CV_8UC1:
        {

            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Grayscale8 );
            return image;
        }
            // 32-bit, 1 channel
        case CV_32SC1:
        {
            QImage image(inMat.data,
                    inMat.cols, inMat.rows,
                    static_cast<int>(inMat.step),
                    QImage::Format_Mono); //TODO unsure
            return image;
        }
            // 32 bit, 3 channel
        case CV_32SC3:
        {
            QImage image(inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_RGB32); //TODO unsure
            return image.rgbSwapped();
        }

        default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}

//----------------------------------------------------------------
//http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html
//----------------------------------------------------------------

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

QImage mat_to_qimage_cpy(cv::Mat const &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows,
                  mat.step, format).copy();
}

cv::Mat qimage_to_mat_ref(QImage &img, int format)
{
    QImage imgs = img.rgbSwapped();
    return cv::Mat(imgs.height(), imgs.width(),
            format, imgs.bits(), imgs.bytesPerLine());
}

cv::Mat qimage_to_mat_cpy(QImage const &img, int format)
{
    QImage imgs = img.rgbSwapped();
    return cv::Mat(imgs.height(), imgs.width(), format,
                   const_cast<uchar*>(imgs.bits()),
                   imgs.bytesPerLine()).clone();
}

//----------------------------------------------------------------


cv::Mat qImageToCvMat(const QImage &inMat){
    switch(inMat.format()){
        //case QImage::Format_RGB32:
        //{
            //cv::Mat image(inMat.height(),inMat.width(),CV_32SC3,inMat.scanLine());
        //}


        default:
            qWarning() << "ASM::cvMatToQImage() Qimage format not handled in switch:" << inMat.format();
            break;
    }
    //inMat.data_ptr();
    //inMat.width();
    //inMat.height();
    //inMat.bytesPerLine();
    //inMat.format(); QImage::Format::Format_RGB32;

    return cv::Mat();
}