//
// Created by phonix on 27/03/2020.
//

#include "miscFunctions.h"

bool sff::tools::normalizeNeighborhood(std::map<uint,std::map<uint,float>> & mapNeighbors){
    for(auto & map : mapNeighbors){
        float sum=0;
        for(const auto & pair : map.second){
            sum+=pair.second;
        }
        if(sum<1e-5f){sum=1e-5f;}
        sum = 1.0f/sum;
        for(auto & pair : map.second){
            pair.second*=sum;
        }
    }
    return true;
}

std::map<uint, std::map<uint, float>> sff::tools::replicate2DNeighborhoodTo3D(
        const std::map<uint, std::map<uint, float>> &nbh, int nbSlices, int sliceDepth, uint sliceSize, bool cube) {
    std::map<uint,std::map<uint,float>> output;
    if(sliceSize==0){
        sliceSize=nbh.size();
    }
    if(sliceDepth<0){return output;}
    if(nbSlices<0){return output;}

    for(int depth = 0; depth<nbSlices; depth ++){
        for(const auto & site : nbh){
            output[site.first+sliceSize*depth];
            for(int doffset = std::max(-depth,-sliceDepth); doffset<std::min(nbSlices-depth,sliceDepth+1); doffset++) {
                if(cube || doffset==0) {
                    //if cube==false, we just add nbh of the same slice + 1 above and 1 under
                    for (const auto &tgtSite : site.second) {
                        output[site.first + sliceSize * depth]
                              [tgtSite.first + sliceSize * (depth + doffset)] = tgtSite.second;
                        // adds all neighbors that are at the same depth as the target +/- doffset
                    }
                }
                if(doffset!=0){
                    // above and under
                    output[site.first + sliceSize*depth]
                          [site.first + sliceSize*(depth+doffset)] = 0.25f;
                }
            }
        }
    }

    return output;
}


bool sff::tools::updateComboBox(QComboBox *comboBox, const std::set<std::string> &sString) {
    // if comboBox already have one element of the set as selected, we keep it as the next selected item...
    if(!comboBox){
        comboBox = new QComboBox();
    }
    std::string previousSelected="";
    if(!comboBox->currentText().toStdString().empty()){
        previousSelected = comboBox->currentText().toStdString();
    }

    std::set<std::string> setClone=sString;

    for(uint i=0; i<comboBox->count(); i++){
        if(sString.count(comboBox->itemText(i).toStdString())){
            setClone.erase(comboBox->itemText(i).toStdString());
        } else {
            comboBox->removeItem(i);
            i--;
        }
    }
    for(const std::string & item:setClone){
        comboBox->insertItem(comboBox->count(),QString::fromStdString(item));
    }

    if(!previousSelected.empty()){
        comboBox->setCurrentText(QString::fromStdString(previousSelected));
    }
    return true;
}