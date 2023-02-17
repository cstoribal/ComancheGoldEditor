#include "novaPCXencoding.h"




template<> const unsigned int
NovaPCXEncoding<cv::Vec3b>::DataTypeSize = 3;


bool decodeRLEIndex(const std::vector<char> & data, size_t & startPos, std::vector<unsigned char> & vDataConvert){
    unsigned int i=0;
    size_t pos = startPos;
    while(pos<data.size() && i<vDataConvert.size()){
        if((data[pos]&0xc0) == 0xc0){
            unsigned char runLength = (data[pos]&0x3f);
            pos++;
            if(pos==data.size()){
                return false;
            }
            unsigned int ran=0;
            while(i<vDataConvert.size() && ran<runLength){
                vDataConvert[i]=static_cast<unsigned char>(data[pos]);
                i++;
                ran++;
            }
            pos++;
        }
        else
        {
            vDataConvert[i]=static_cast<unsigned char>(data[pos]);
            pos++;
            i++;
        }
    }
    startPos = pos;
    return true;
}


bool encodeRLEIndex(std::vector<char> & data, const std::vector<unsigned char> & vDataConvert){
    unsigned int i=0;
    while(i<vDataConvert.size()){
        unsigned char length=1;
        unsigned char value =vDataConvert[i];
        while(length<63 && ((length+i<vDataConvert.size())? value == vDataConvert[length+i] : 0)){
            length++;
        }
        if(length>2 || value >= 192){
            data.push_back(length|0x0c);
            data.push_back(value);
            i+=length;
        } else {
            data.push_back(value);
            i++;
        }
    }
    return true;
}



template<>
template<> void NovaPCXEncoding<cv::Vec3b>::writeToChar(std::vector<char> & data, size_t position, const cv::Vec3b & t)const{
    const unsigned int Nb = 3;
    const unsigned char vec[Nb]={t[0],t[1],t[2]};
    const std::aligned_storage<Nb> & value = *reinterpret_cast<const std::aligned_storage<Nb>*>(&(vec[0]));
    writeToCharPvt<Nb>(data,position,value);
}

template<>template<>
cv::Vec3b NovaPCXEncoding<cv::Vec3b>::readFromChar(const std::vector<char> & data, size_t position, cv::Vec3b& t)const{
    const unsigned int Nb = 3;
    unsigned char vec[Nb];
    std::aligned_storage<Nb> & value = *reinterpret_cast<std::aligned_storage<Nb>*>(&vec[0]);
    readFromCharPvt<Nb>(data,position,value);
    t[0]=vec[0];
    t[1]=vec[1];
    t[2]=vec[2];
    return t; //Copy
}

