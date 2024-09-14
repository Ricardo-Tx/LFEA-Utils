#include "Utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "TLine.h"
#include "TF1.h"

namespace LFEA {

    int ReadFileASC(std::string filename, std::vector<double>* counts, std::vector<int>* ROI, int* elapsedTime, int* realTime, std::string* date, std::string* time){
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << "\n";
            return 1;
        }

        std::string line, token;

        getline(file, line);
        std::regex dateRegex("(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) \\d{1,2} \\d{4}");
        std::regex timeRegex("\\d{1,2}:\\d{2}:\\d{2} (am|pm)");
        std::regex elapsedTimeRegex("Elt: (\\d+) Seconds");
        std::regex realTimeRegex("Real Time: (\\d+)");
        std::regex binRegex("\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)");

        std::smatch match;

        if (date && regex_search(line, match, dateRegex)) *date = match[0];
        if (time && regex_search(line, match, timeRegex)) *time = match[0];
        if (elapsedTime && regex_search(line, match, elapsedTimeRegex)) *elapsedTime = stoi(match[1]);
        if (realTime && regex_search(line, match, realTimeRegex)) *realTime = stoi(match[1]);

        if(!counts) return 2;
        (*counts).resize(1024, 0.0);

        getline(file, line);
        getline(file, line);
        getline(file, line);
        getline(file, line);

        int lastRoi = 0;
        for(int i = 0; i < 1024; i++){
            getline(file, line);
            regex_search(line, match, binRegex);
            (*counts)[i] = stod(match[2]);

            int roi = stoi(match[3]);
            if(ROI && roi != lastRoi) (*ROI).push_back(i-!lastRoi);
            lastRoi = roi;
            // cout << "Counts: " << match[2] << "\tROI: " << match[3] << "\n";
        }
        
        file.close();
        return 0;
    }

    int ReadHeaderCSV(std::string filename, std::vector<double>* flatValues, std::vector<std::string>* names){
        if(!flatValues) return 1;
        flatValues->clear();
        if(names) names->clear();
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file.\n";
            return 1;
        }

        std::string line, token;
        std::istringstream iss;

        getline(file, line);
        iss.str(line);
        while(getline(iss, token, ',')) {
            if(names) names->push_back(token);
        }

        while (getline(file, line)) {
            iss.clear();
            iss.str(line);
            while(getline(iss, token, ',')) flatValues->push_back(stod(token));
        }

        file.close();
        return 0;
    }

    int ReadHeaderRawCSV(std::string filename, std::vector<std::string>* flatValues, std::vector<std::string>* names){
        if(!flatValues) return 1;
        flatValues->clear();
        if(names) names->clear();
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file.\n";
            return 1;
        }

        std::string line, token;
        std::istringstream iss;

        getline(file, line);
        iss.str(line);
        while(getline(iss, token, ',')) {
            if(names) names->push_back(token);
        }

        while (getline(file, line)) {
            iss.clear();
            iss.str(line);
            while(getline(iss, token, ',')) flatValues->push_back(token);
        }

        file.close();
        return 0;   
    }


    void FillHistogram(TH1* hist, std::vector<double> counts){
        for(int i = 0; i < hist->GetNbinsX(); i++) {
            double low = hist->GetBinLowEdge(i+1);
            double high = low + hist->GetBinWidth(i+1);
            // cout << "[" << low << "; " << high << "]\t"; 
            for(int j = floor(low); j < ceil(high); j++) {
                double w;
                if(j == floor(low)) w = floor(low)-low+1;
                else if(j == ceil(high)-1) w = high-ceil(high)+1;
                else w = 1.0;
                
                hist->Fill((low+high)/2, counts[j]*w);
                // cout << j << "(" << w << ") ";
            }
            // cout << "\n";
        }
    }

    void PaveTextContent(TPaveText* pt, std::vector<std::vector<std::string>> paveStrings, std::vector<int> lines, std::vector<int> lineStyles){
        for(std::vector<std::string>& vct : paveStrings){
            if(vct.size() != 3) continue;
            vct[1] += " #pm " + vct[2];
            vct.pop_back();
        }
        
        TText* text;
        double step = 1.0/paveStrings.size(), pos = 1.0 - step/2.0;
        for(std::vector<std::string> vct : paveStrings){
            if(vct.size() == 1){
                text = pt->AddText(0.5, pos, vct[0].c_str());
                text->SetTextAlign(22);
                // text->SetTextFont(102);
                text->SetTextFont(62);
            }
            else if(vct.size() == 2){
                text = pt->AddText(0, pos, vct[0].c_str());
                text->SetTextAlign(12);
                // text->SetTextFont(102);
                text->SetTextFont(62);
                text = pt->AddText(1, pos, vct[1].c_str());
                text->SetTextAlign(32);
                // text->SetTextFont(82);
                text->SetTextFont(62);
                //text->SetTextSize(0.025);
            }
            pos -= step;
        }

        if(!lines.size()) return;

        for(int i = 0; i < lines.size(); i++){
            int k = lines[i];
            TLine* line = pt->AddLine(0.0, 1.0-step*k, 1.0, 1.0-step*k);
            if(lineStyles.size() && i < lineStyles.size()) line->SetLineStyle(lineStyles[i]);
        }
    }


    std::vector<std::string> GetFormattedParam(std::string name, TF1* func, int parIndex){
        return {name, std::to_string(func->GetParameter(parIndex)), std::to_string(func->GetParError(parIndex))};
    }
    std::vector<std::string> GetFitStats(TF1* func){
        return {"#chi^{2} / ndf", std::to_string(func->GetChisquare())+" / "+std::to_string(func->GetNDF())};
    }


    std::pair<TF1*, TF1*> FunctionsOutsideRegion(TF1* regionFunction, int lowerBound, int upperBound, Color_t color){
        TF1* funcLower = new TF1((std::string(regionFunction->GetName())+"Lower").c_str(), [regionFunction](double* x, double* p){
            return regionFunction->Eval(x[0]);
        }, 0, lowerBound, 0);
        funcLower->SetNpx(1024);
        funcLower->SetLineColor(color);
        funcLower->SetLineStyle(2);

        TF1* funcUpper = new TF1((std::string(regionFunction->GetName())+"Upper").c_str(), [regionFunction](double* x, double* p){
            return regionFunction->Eval(x[0]);
        }, upperBound, 1024, 0);
        funcUpper->SetNpx(1024);
        funcUpper->SetLineColor(color);
        funcUpper->SetLineStyle(2);

        return {funcLower, funcUpper};
    }

    int utf8_char_length(unsigned char c) {
        if (c < 0x80) return 1; // ASCII character
        else if ((c & 0xE0) == 0xC0) return 2; // 2-byte UTF-8 character
        else if ((c & 0xF0) == 0xE0) return 3; // 3-byte UTF-8 character
        else if ((c & 0xF8) == 0xF0) return 4; // 4-byte UTF-8 character
        return 0; // Invalid UTF-8 character
    }

    std::string Sanitize(std::string str){
        for(int i = 0; i < str.size(); i++){
            if((str[i] & 0b11100000) != 0b11000000) continue;
            str[i] = (str[i] << 6) | (str[i+1] & 0b00111111);
            str.erase(i+1, 1);
        }
        return str;
    }

    std::string RoundToString(double num, int places, char roundMethod, char sep){
        int p = pow(10, places);
        std::string result = std::to_string((int)num);
        result += sep;
        if(roundMethod == 'r') result += std::to_string((int)round(num*p)%p);
        else if(roundMethod == 'u' || roundMethod == 'c') result += std::to_string((int)ceil(num*p)%p);
        else if(roundMethod == 'l' || roundMethod == 'f' || roundMethod == 'd') result += std::to_string((int)floor(num*p)%p);
        return result;
    }

    void SetPrintEnvironment(int decimalPlaces, char sep){
        std::cout << std::fixed << std::setprecision(decimalPlaces);
        std::cout.imbue(std::locale(std::cout.getloc(), new punct_facet<char>(sep)));
    }

}
