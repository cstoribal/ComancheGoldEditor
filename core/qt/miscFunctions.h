//
// Created by phonix on 27/03/2020.
//

#ifndef THESE_MISCFUNCTIONS_H
#define THESE_MISCFUNCTIONS_H

#include <vector>
#include <map>
#include <set>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <QComboBox>
//#include <inc_sff/obj/preSeg_lite.h>

namespace sff{
    namespace tools{
        bool normalizeNeighborhood(std::map<uint,std::map<uint,float>> & mapNeighbors);
        std::map<uint,std::map<uint,float>> replicate2DNeighborhoodTo3D(const std::map<uint,std::map<uint,float>> & nbh,int nbSlices, int sliceDepth = 0, uint sliceSize =0, bool cube=true);


        template<typename MatType> bool checkMatrixConsistency(const std::vector<MatType> & vMatrix);
        // idéalement pour les matrices et les vecteurs de floats
        template<typename Type> bool interpolateOverVector(std::vector<Type> & vector, float scale, int interpolation=cv::INTER_LINEAR);

        template<typename MatType> bool scaleVolume(std::vector<MatType> & vMatrix, float scaleX, float scaleY=-1, float scaleZ=-1, int interpolation=cv::INTER_LINEAR);

        template <typename Type, typename Disp> void showMatrix(const cv::Mat_<Type> & mat,Disp type=Disp());
        template <typename Type, typename Disp, int n> void showMatrix(const cv::Mat_<Type> & mat,cv::Vec<Disp,n> type=cv::Vec<Disp,n>());
        template <typename Type, typename Callable> void showVector(const std::vector<Type> & vec,Callable &&func);

        template <typename Type, typename Disp> void showVolume(const std::vector<cv::Mat_<Type>> & vec,Disp type=Disp());

        bool updateComboBox(QComboBox* comboBox,const std::set<std::string> & vString);

        template<typename T> std::map<uint,T> dilateMap(const std::map<uint,T> & data, const std::map<uint,std::map<uint,float>> & adjMap);
        template<typename T> std::map<uint,T> erodeMap(const std::map<uint,T> & data, const std::map<uint,std::map<uint,float>> & adjMap);




        template <typename T> T(*none)(const std::vector<T> &)=[](const std::vector<T> & inV){return T();};
//        template <typename T> bool mapSwitchBetweenSpx(const PreSegLite & segSource,
//                const PreSegLite & segDest, std::map<uint,T> & inOut,
//                T(*combLambda)(const std::vector<T> &) = sff::tools::none<T> );
//        template <typename T> bool mapSwitchBetweenSpxFromVol(const PreSegLite & segSource,
//                const PreSegLite & segDest, const std::vector<cv::Mat_<T>> & input, std::map<uint,T> & output,
//                T(*combLambda)(const std::vector<T> &) = sff::tools::none<T> );
//        template <typename T> bool mapSwitchBetweenSpxFromSurf(const PreSegLite & segSource,
//                const PreSegLite & segDest, const cv::Mat_<T> & input, std::map<uint,T> & output,
//                T(*combLambda)(const std::vector<T> &) = sff::tools::none<T> );

        template<typename T> std::map<T,uint> computeHistogram(const cv::Mat_<T> & mat);

        template<typename T,int N> cv::Mat_<cv::Vec<T,N>> medianFilter(const cv::Mat_<cv::Vec<T,N>> & srcMat, int medianSize);

    }

}

#include "miscFunctions.hpp"


#endif //THESE_MISCFUNCTIONS_H
