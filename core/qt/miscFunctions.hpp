//
// Created by stophe on 03/09/2020.
//

#ifndef SFF_MISCFUNCTIONS_HPP
#define SFF_MISCFUNCTIONS_HPP

template<typename MatType> bool sff::tools::checkMatrixConsistency(const std::vector<MatType> & vMatrix) {
    if (vMatrix.size() == 0) { return false; }
    int sizeA(vMatrix[0].rows), sizeB(vMatrix[0].cols);
    for (const auto &mat : vMatrix) {
        if (mat.rows != sizeA || mat.cols != sizeB) {
            return false;
        }
    }
    return true;
}

template<typename Type> bool sff::tools::interpolateOverVector(std::vector<Type> & vector, float scale, int interpolation){
    //TODO rewrite
    if(scale==1){return true;}
    int newSize = (vector.size()-1)*scale+1;
    if(scale>1){newSize=vector.size()*scale;}
    std::vector<Type> vTarget(newSize,Type());
    std::map<uint,std::set<uint>> mapInterpol1;
    std::map<uint,std::pair<uint,uint>> mapInterpol2;
    for(int i=0; i<vector.size(); i++){
        mapInterpol1[i*scale+i*0.5f/vector.size()].insert(i);
        mapInterpol2[i]=std::pair<uint,uint>(i>0?(i-0.5f)*scale:0,(i+0.5f)*scale);
    }
    mapInterpol1.erase(mapInterpol1.rend()->first);
    switch(interpolation){
    case cv::INTER_LINEAR:default:{
        if(scale<1){
            for(const auto & interp : mapInterpol1){
                bool start=true;
                for(const auto & index : interp.second){
                    if(start){
                        vTarget[interp.first] = vector[index]/interp.second.size();
                        start=false;
                    }
                    else{
                        vTarget[interp.first]+= vector[index]/interp.second.size();
                    }
                }
                //if(!interp.second.empty()){
                //    vTarget[interp.first]/=interp.second.size();
                //}
            }
        } else {
            uint currentIndex=0;
            for(uint i=0;i<vTarget.size();i++){
                std::pair<uint,uint> ab = mapInterpol2[currentIndex];
                vTarget[i]=(vector[currentIndex>0?currentIndex-1:0]*(i-ab.first)
                        +vector[currentIndex<vector.size()?currentIndex:currentIndex-1]*(ab.second-i))
                                /static_cast<float>(ab.first-ab.second);
                if(i==mapInterpol2[currentIndex].second){currentIndex++;}
            }
            assert(currentIndex==vector.size());
        }
    } break;
    }
    std::swap(vector,vTarget);
    return true;
}


template<typename MatType> bool sff::tools::scaleVolume(std::vector<MatType> & vMatrix, float scaleX, float scaleY, float scaleZ, int interpolation){
    if(!checkMatrixConsistency(vMatrix)){return false;}
    if(scaleY==-1 || scaleZ==-1){scaleY=scaleX; scaleZ=scaleX;}
    MatType tmpMatrix;
    for(uint z=0; z<vMatrix.size(); z++){
        cv::resize(vMatrix[z],tmpMatrix, cv::Size(), scaleX, scaleY,interpolation);
        cv::swap(tmpMatrix,vMatrix[z]);
    }
    interpolateOverVector(vMatrix,scaleZ,interpolation);
    return true;
}


template <typename Type, typename Disp> void sff::tools::showMatrix(const cv::Mat_<Type> & mat,Disp type){
    std::cout<<"====showMat====\n";
    for(uint i=0;i<mat.rows; i++){
        for(uint j=0; j<mat.cols;j++){
            std::cout<<(Disp)mat(i,j)<<"\t";
        }
        std::cout<<"\n";
    }
    std::cout<<"=====end=====\n";
}

template <typename Type, typename Disp, int n> void sff::tools::showMatrix(const cv::Mat_<Type> & mat,cv::Vec<Disp,n> type){
    std::cout<<"====showMat====\n";
    for(uint i=0;i<mat.rows; i++){
        for(uint j=0; j<mat.cols;j++){
            for(uint k=0; k<n; k++) {
                std::cout << (Disp) mat(i, j)[k] << ":";
            }
            std::cout<<"\t";
        }
        std::cout<<"\n";
    }
    std::cout<<"=====end=====\n";
}




template <typename Type, typename Callable> void sff::tools::showVector(const std::vector<Type> & vec,Callable&& func){
    std::cout<<"====showVec====\n";
    std::cout<<"size: "+std::to_string(vec.size())+"\n";
    for(uint i=0;i<vec.size(); i++){
            std::cout<<"<#"<<i<<">  "<<(vec[i].*func)()<<"\t";
    }
    std::cout<<"\r=====end=====\n";
}


template <typename Type, typename Disp> void sff::tools::showVolume(const std::vector<cv::Mat_<Type>> & vec,Disp type){
    std::cout<<"====showVec====\n";
    for(uint z=0;z<vec.size();z++) {
        showMatrix<Type, Disp>(vec);
    }
}
namespace sff{
    namespace tools{
        template <typename T> T(*mean)(const std::vector<T> &)=[](const std::vector<T> & inV){
            if(inV.empty()){return T();}
            T out = *(inV.begin());
            for(uint i=1;i<inV.size(); i++){
                out+=inV[i];
            }
            return T(out/inV.size());
        };
    }
}


template <typename T> std::map<uint, T> sff::tools::dilateMap(const std::map<uint, T> &data,
                                                              const std::map<uint, std::map<uint, float>> &adjMap) {
    std::map<uint,T> mapOut = data;
    for(const auto & src : data){
        for(const auto & tgt : adjMap.at(src.first)){
            mapOut[tgt.first] = std::max(data.at(tgt.first),src.second);
        }
    }

    return data;
}

template <typename T> std::map<uint, T> sff::tools::erodeMap(const std::map<uint, T> &data,
                                                              const std::map<uint, std::map<uint, float>> &adjMap) {
    std::map<uint,T> mapOut = data;
    for(const auto & src : data){
        for(const auto & tgt : adjMap.at(src.first)){
            mapOut[tgt.first] = std::min(data.at(tgt.first),src.second);
        }
    }

    return data;
}

///////////////////////////////////////////////////////////////////////
//////      MapSwitches                                         ///////
///////////////////////////////////////////////////////////////////////
/*
template <typename T> bool sff::tools::mapSwitchBetweenSpx(const PreSegLite & segSource,
        const PreSegLite & segDest, std::map<uint,T> & inOut,
        T(*combLambda)(const std::vector<T> &)){
    if(segSource.Idx()==segDest.Idx()){
        return true;
    }
    if(!segSource.sameSize(segDest)){
        return false;
    }
    //if(combLambda==sff::tools::none<T>){
    //    combLambda=mean<T>;
    //}
    int dZ = segSource.Indexes().size();
    if(dZ==0){
        return false;
    }
    int dY = segSource[0].rows;
    int dX = segSource[0].cols;

    std::map<uint,std::vector<T>> out;
    for(int z=0;z<dZ;z++){
        for(int y=0;y<dY;y++){
            for(int x=0;x<dX;x++){
                if(inOut.count(segSource[z](y,x))) {
                    out[segDest[z](y, x)].push_back(inOut.at(segSource[z](y, x)));
                }
            }
        }
    }
    inOut.clear();
    for(const auto & pair: out){
        inOut.emplace(pair.first,combLambda(pair.second));
    }
    return true;
}


template <typename T> bool sff::tools::mapSwitchBetweenSpxFromVol(const PreSegLite &segSource,
         const PreSegLite &segDest,
         const std::vector<cv::Mat_<T>> &input,
         std::map<uint, T> &output,
         T (*combLambda)(const std::vector<T> &)) {
    //if(segSource.Idx()==segDest.Idx()){
    //    return true;
    //}
    if(!segSource.sameSize(segDest) || !segSource.sameSize(input)){
        return false;
    }
    //if(combLambda==sff::tools::none<T>){
    //    combLambda=mean<T>;
    //}
    int dZ = segSource.Indexes().size();
    if(dZ==0){
        return false;
    }
    int dY = segSource[0].rows;
    int dX = segSource[0].cols;

    std::map<uint,std::vector<T>> out;
    for(int z=0;z<dZ;z++){
        for(int y=0;y<dY;y++){
            for(int x=0;x<dX;x++){
                out[segDest[z](y,x)].push_back(input[z](y,x));
            }
        }
    }
    output.clear();
    for(const auto & pair: out){
        output.emplace(pair.first,combLambda(pair.second));
    }
    return true;
}

template <typename T> bool sff::tools::mapSwitchBetweenSpxFromSurf(const PreSegLite &segSource,
         const PreSegLite &segDest,
         const cv::Mat_<T> &input,
         std::map<uint, T> &output,
         T (*combLambda)(const std::vector<T> &)) {
    //if(segSource.Idx()==segDest.Idx()){
    //    return true;
    //}
    if(!segSource.sameSize(segDest)){
        return false;
    }
    if(combLambda==sff::tools::none<T>){
        combLambda=mean<T>;
    }
    int dZ = segSource.Indexes().size();
    if(dZ==0){
        return false;
    }
    int dY = segSource[0].rows;
    int dX = segSource[0].cols;
    if(input.rows!=dY || input.cols!=dX){
        return false;
    }

    std::map<uint,std::vector<T>> out;
    for(int y=0;y<dY;y++){
        for(int x=0;x<dX;x++){
            out[segDest[0](y,x)].push_back(input(y,x));
        }
    }
    output.clear();
    for(const auto & pair: out){
        output.emplace(pair.first,combLambda(pair.second));
    }
    return true;
}
*/


template<typename T> std::map<T,uint> sff::tools::computeHistogram(const cv::Mat_<T> & mat){
    std::map<T,uint> map;
    for(uint j=0; j<mat.rows; j++){
        for(uint i=0; i<mat.cols; i++){
            map[mat(j,i)]++;
        }
    }

    return map;
}

template<typename T,int N> cv::Mat_<cv::Vec<T,N>> sff::tools::medianFilter(
        const cv::Mat_<cv::Vec<T,N>> & srcMat, int medianSize){
    cv::Mat_<cv::Vec<T,N>> dstMat = cv::Mat_<cv::Vec<T,N>>::zeros(srcMat.rows,srcMat.cols);

    std::vector<T> values = std::vector<T>();
    for(int k=0; k<N; k++) {
        for (int i = 0; i < srcMat.rows; i++) {
            for (int j = 0; j < srcMat.cols; j++) {
                for (int di = std::max(0, i - medianSize/2); di < std::min(srcMat.rows, i + medianSize/2 + 1); di++) {
                    for (int dj = std::max(0, j - medianSize/2); dj < std::min(srcMat.cols, j + medianSize/2 + 1); dj++) {
                        values.push_back(srcMat(di, dj)[k]);
                    }
                }
                std::sort(values.begin(), values.end());
                dstMat(i, j)[k] = values[values.size() / 2];
                values.clear();
            }
        }
    }

    return dstMat;
}




#endif //SFF_MISCFUNCTIONS_HPP
