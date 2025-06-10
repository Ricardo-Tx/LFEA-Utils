#include "LTgraph.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
// #include <cmath>

namespace LFEA {
    LTgraph::LTgraph(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << "\n";
            return;
        }

        std::stringstream stream;
        std::string line;
        std::string tok;

        // Headers
        getline(file, line);
        stream.str(line);

        int i = 0, j = 0;
        while(getline(stream, tok, '\t')){
            if(!i){
                if(tok == "Freq.") {
                    simType = SimulationType::AnalysisAC;
                    unit = "Hz";
                } else if(tok == "time") {
                    simType = SimulationType::Transient;
                    unit = "s";
                } else {
                    simType = SimulationType::SweepDC;
                    unit = (uint)tok[0]-((uint)tok[0]>90)*32;
                }
                name = tok;
            }else{
                waves.push_back(Waveform(tok));
                // std::cout <<"CHAR: " << std::string(1,(uint)tok[0]-((uint)tok[0]>90)*32) << "\n";
                waves.back().units.push_back(std::string(1,(uint)tok[0]-((uint)tok[0]>90)*32));
            }

            // std::cout << ++i << ": ";
            // std::cout << tok << "\n";
            ++i;
        }


        // Values
        while(getline(file,line)){
            i = 0;
            std::stringstream paramStream;

            stream.clear();
            stream.str(line);

            // x axis
            stream >> tok;
            values.push_back(std::stod(tok));

            // y axes
            while(stream >> tok){
                if(tok[0] == '(') {
                    paramStream.str(tok.substr(1,tok.size()-2));
                    int j = 0;
                    while(getline(paramStream,tok,',')){
                        std::string unit = "";
                        while((uint)tok.back() > 57) {
                            // std::cout << "PARSING: " << tok << " | " << tok.back() << "\n";
                            unit = tok.back() + unit;
                            tok = tok.substr(0,tok.size()-1);
                        }
                        if(j >= waves[i].values.size()) waves[i].values.push_back(std::vector<double>());                            
                        if(waves[i].units.size()-1 < waves[i].values.size()) waves[i].units.push_back(unit);
                        waves[i].values[j++].push_back(std::stod(tok));
                    }
                }else{
                    waves[i].values[0].push_back(std::stod(tok));
                }
                i++;
            }
        }
    }
}