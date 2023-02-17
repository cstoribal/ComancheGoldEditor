#ifndef NOVARLENCODING_H
#define NOVARLENCODING_H

#include "tools/byRef.h"
#include <vector>
#include <map>

#include <opencv2/core.hpp>

bool decodeRLEIndex(const std::vector<char> & data, size_t & startPos, std::vector<unsigned char> & output);
bool encodeRLEIndex(std::vector<char> & data, size_t & startPos, const std::vector<unsigned char> & input);

template<typename DataType> class NovaPCXEncoding
{
    bool f_bigEndian = true;

    static const unsigned int DataTypeSize; //nombre d’octets de chaque élément
    // a priori, 24 bits
    std::vector<char> mDataChar;
    cv::Mat_<DataType> mDataMat;
public:
    NovaPCXEncoding():f_bigEndian(true){ }

    bool getBigEndian()const{return f_bigEndian;}


    bool readFile(const std::vector<char> & data);
    cv::Mat_<DataType> dataMat()const{return mDataMat.clone();}
    cv::Mat_<DataType> readMatrix(const std::vector<char> & data = {});
    std::vector<char> writePCX(const cv::Mat_<DataType> & matrice);

private:
    //void setDataSize(){mDataTypeSize=sizeof(DataType);}

    template<typename T> void writeToChar(std::vector<char> & data, size_t position, const T & t)const;
    //void writeToCharD(std::vector<char> & data, size_t position, const DataType & t)const;
    template<unsigned int Nb> void writeToCharPvt(std::vector<char> & data, size_t position, const std::aligned_storage<Nb> & t)const;
    template<typename T> T readFromChar(const std::vector<char> & data, size_t position, T& t=ByRef<T>())const;
    //DataType readFromCharD(const std::vector<char> & data, size_t position, DataType& t=ByRef<DataType>())const;
    template<unsigned int Nb> std::aligned_storage<Nb> & readFromCharPvt(const std::vector<char> & data, size_t position, std::aligned_storage<Nb>& t = ByRef<std::aligned_storage<Nb>>())const;
    std::string readStrFromChar(const std::vector<char> & data, size_t position, size_t n);
};


template<typename DataType> bool NovaPCXEncoding<DataType>::readFile(const std::vector<char> & data){

    //- Verify Header ? LZP1
    std::string head=readStrFromChar(data,0,4);

    //- Read size1, size2
    unsigned int size1 = readFromChar<unsigned int>(data,4);
    unsigned int size2 = readFromChar<unsigned int>(data,8);

    //- read And Set palette
    size_t sizePalette = 256;
    std::vector<DataType> vPalette(sizePalette);
    for(unsigned int i=0; i<sizePalette; i++){
        vPalette[i] = readFromChar<DataType>(data,12+DataTypeSize*i);
    }

    //- unknown value A
    unsigned int unknownA = readFromChar<unsigned int>(data,12+DataTypeSize*sizePalette);

    //- read and set bins
    std::vector<unsigned int> vBins(sizePalette);
    for(unsigned int i=0; i<sizePalette; i++){
        vBins[i] = readFromChar<unsigned int>(data,16+DataTypeSize*sizePalette + i*4);
    }

    //- unkown null bytes
    std::vector<char> ignoredUnkBytes(16);
    for(unsigned int i=0; i<16; i++){
        ignoredUnkBytes[i]=data[16+DataTypeSize*sizePalette + sizePalette*4 + i];
    }

    size_t dataStartPos = 16+DataTypeSize*sizePalette + sizePalette*4 + 16 + 1;

    //- start Run Length Decoding

    cv::Mat_<DataType> dataMat = cv::Mat_<DataType>(size1,size2); //check row/col position
    std::vector<unsigned char> vDataConvert(size1*size2);
    size_t endPos = dataStartPos;
    decodeRLEIndex(data,endPos,vDataConvert);

    std::map<unsigned char,unsigned int> binCountCheck;

    for(unsigned int i=0; i<size1; i++){
        for(unsigned int j=0; j<size2; j++){
            dataMat(i,j)=vPalette[vDataConvert[j+size2*i]];
            binCountCheck[vDataConvert[j+size2*i]]++;
        }
    }

    // compare Bins... //TODO
    size_t countElements = 0;
    size_t secondCount = 0;
    for(unsigned int i=0; i<sizePalette; i++){
        secondCount   += binCountCheck[i];
        countElements += vBins[i];
    }

    for(unsigned int i=0; i<sizePalette; i++){
        if(binCountCheck[i] != vBins[i]){
            // checksum failed
            //  return false ;
        }
    }
    mDataMat = dataMat.clone();
    return true;
}









///--- Classic byte reading
///

template<typename DataType>
template<typename T> void NovaPCXEncoding<DataType>::writeToChar(std::vector<char> & data, size_t position, const T & t)const{
    const int Nb = sizeof(T);
    const std::aligned_storage<Nb> & value = *reinterpret_cast<const std::aligned_storage<Nb>*>(&t);
    writeToCharPvt(data,position,value);
}



template<typename DataType>
template<unsigned int Nb> void NovaPCXEncoding<DataType>::writeToCharPvt(std::vector<char> & data,
                       size_t position, const std::aligned_storage<Nb>& t)const{
    //Safety check data size
    if(position+Nb>=data.size()){
        data.resize(position+Nb+1);
    }
    const char* ptr = reinterpret_cast<const char*>(&t);
    if(getBigEndian()){
        for(int i=position;i<Nb+position;i++){
            data[i] = *ptr;
            ptr++;
        }
    } else {
        for(int i=Nb+position-1;i>=position;i--){
            data[i] = *ptr;
            ptr++;
        }
    }
}

template<typename DataType>
template<typename T> T NovaPCXEncoding<DataType>::readFromChar(const std::vector<char> & data, size_t position, T& t)const{
    const unsigned int Nb = sizeof(T);
    std::aligned_storage<Nb> & value = *reinterpret_cast<std::aligned_storage<Nb>*>(&t);
    readFromCharPvt<Nb>(data,position,value);
    return t; //Copy
}



template<typename DataType>
template<unsigned int Nb> std::aligned_storage<Nb> & NovaPCXEncoding<DataType>::readFromCharPvt(const std::vector<char> & data,
                              size_t position, std::aligned_storage<Nb> & t)const{
    //if(position+Nb>=data.size()){
    //    return std::aligned_storage<Nb>(); //Failure.
    //}
    char* ptr = reinterpret_cast<char*>(&t);
    if(getBigEndian()){
        for(int i=position;i<Nb+position;i++){
            *ptr = data[i];
            ptr++;
        }
    } else {
        for(int i=Nb+position-1;i>=position;i--){
            *ptr = data[i];
            ptr++;
        }
    }
    return t;
}

template<typename DataType>
std::string NovaPCXEncoding<DataType>::readStrFromChar(const std::vector<char> & data, size_t position, size_t n){
    std::string output = "";
    output.resize(n);
    const char * ptr = reinterpret_cast<const char*>(&data[position]); // <pf1>
    for(int i=0; i<n; i++){
        int j=false?n-1-i:i;
        output[i] = *(ptr+j);
    }
    return output;
}

template<>template<>
void NovaPCXEncoding<cv::Vec3b>::writeToChar(std::vector<char> & data, size_t position, const cv::Vec3b & t)const;

template<>template<>
cv::Vec3b NovaPCXEncoding<cv::Vec3b>::readFromChar(const std::vector<char> & data, size_t position, cv::Vec3b& t)const;

template<> const unsigned int
NovaPCXEncoding<cv::Vec3b>::DataTypeSize;


#endif // NOVARLENCODING_H
