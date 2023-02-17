#include "pffReading.h"
#include <io/HexaParser.h>
#include <io/pffParser.hpp>
//#include <io/HexaParserFunctions.hpp>

#include <iostream>
#include <random>

PffIO::PffIO()
{

}



std::string PffIO::load(const std::string & file){

    //TODO move to Pff Tests
    //TODO insertion file test
    //TODO remove file test
    //TODO
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> dist255(0,255); // distribution in range [1, 6]
    std::uniform_real_distribution<> distR(-1e3,1e3);
    std::uniform_int_distribution<> distI1000(1,1000);

    /*
    for(unsigned int i=0; i<130; i++){
        unsigned int N = distI1000(rng);
        std::vector<float> realList(N);

        NovalogicPffParser hexWriter;

        for(unsigned int k=0; k<N;k++){
            float real = distR(rng);
            realList[k]=real;
            PffWrapperRaw<float>* wrapper = new PffWrapperRaw<float>();
            wrapper->setFileName("F"+std::to_string(real));
            hexWriter.appendToBuffer(real, wrapper);
        }
        //hexWriter.showHexContent();
        hexWriter.writeToFile(file+".hex");

        std::string txt = file +".hex";

        NovalogicPffParser hexParser(txt); //Check parseHeader()
        for(unsigned int k=0; k<N;k++){
            float test = hexParser.readFromBuffer<float>(k);
            if(test!=realList[k]){
                std::cout<<"warning, data n°"<<k<<"from batch #"<<i
                        <<" does not match: "<< test<<" against "<<realList[k]<<"."<<std::endl;
            }

        }
        hexParser.writeToFile(file+"2.hex");
        //hexParser.showHexContent();

    }
    */

    // Still have some deletion problems
    NovalogicPffParser hexParser2(file); //Check parseHeader()
    //unsigned int test = hexParser.readFromBuffer<unsigned int>(1);
    //hexParser.readFromFile(txt);
    //hexParser.showHexContent();
    hexParser2.showHexContent();
    return hexParser2.getStringHexContent();
}
