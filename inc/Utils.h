#ifndef __UTILS__
#define __UTILS__

#include <string>
#include <vector>
#include <locale>
#include <iomanip>

#include "TH1.h"
#include "TPaveText.h"

namespace LFEA {

    int ReadFileASC(std::string filename, std::vector<double>* counts, std::vector<int>* ROI=nullptr, int* elapsedTime=nullptr, int* realTime=nullptr, std::string* date=nullptr, std::string* time=nullptr);
    void FillHistogram(TH1* hist, std::vector<double> counts);
    void PaveTextContent(TPaveText* pt, std::vector<std::vector<std::string>> paveStrings, std::vector<int> lines, std::vector<int> lineStyles = {});
    int ReadHeaderCSV(std::string filename, std::vector<double>* flatValues, std::vector<std::string>* names = nullptr);
    int ReadHeaderRawCSV(std::string filename, std::vector<std::string>* flatValues, std::vector<std::string>* names = nullptr);

    std::vector<std::string> GetFormattedParam(std::string name, TF1* func, int parIndex);
    std::vector<std::string> GetFitStats(TF1* func);

    std::pair<TF1*, TF1*> FunctionsOutsideRegion(TF1* regionFunction, int lowerBound, int upperBound, Color_t color = kRed);


    std::string Sanitize(std::string str);

    std::string RoundToString(double num, int places = 3, char roundMethod = 'r', char sep = '.');

    int utf8_char_length(unsigned char c);


    template <class charT>
    class punct_facet: public std::numpunct<charT> {
    public:
        punct_facet(charT _sep) : sep(_sep){};
    protected:
        virtual charT do_decimal_point() const override { return sep; }
        charT sep;
    };

    void SetPrintEnvironment(int decimalPlaces = 3, const char sep = '.');

}

#endif