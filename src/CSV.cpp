#include "CSV.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace LFEA {

    CSVElement::CSVElement(std::string str){
        char* p;
        long longConvert = strtol(str.c_str(), &p, 10);
        if (*p) {
            // conversion failed because the input wasn't a number
            *p = '\0';
            double doubleConvert = strtod(str.c_str(), &p);
            if(*p){
                value.d = doubleConvert;
                type = CSVElementType::Double;
            }
            else{
                value.s = const_cast<char*>(str.c_str());
                type = CSVElementType::String;
            }
        }
        else {
            // use converted
            value.l = longConvert;
            type = CSVElementType::Long;
        }
    }

    // CSVColumn
    CSVColumn::CSVColumn(std::string _name): name(_name) {}
    CSVColumn::CSVColumn() = default;

    std::ostream& operator<<(std::ostream& out, const CSVColumn& col){
        out << "[\"" << col.name << "\" x" << col.data.size() << "]";
        return out;
    }


    // CSVRow
    CSVRow::CSVRow(std::vector<std::string> _names): names(&_names) {}



    // CSV
    CSV::CSV(std::string filename, bool headers, char sep) {
        std::cout << "ALIVE";
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file.\n";
            exit(1);
        }

        std::string line, token;
        std::istringstream iss;

        if(headers){
            getline(file, line);
            iss.str(line);
            while(getline(iss, token, sep)) names.push_back(token);
        }

        while (getline(file, line)) {
            iss.clear();
            iss.str(line);
            while(getline(iss, token, ',')) flatValues.push_back(CSVElement(token));

            // if(!headers && !names.size()) names = std::vector<std::string>(flatValues.size(), "");
            if(!colCount) colCount = flatValues.size();
        }
        file.close();
        
        rowCount = flatValues.size()/colCount;

        for(int i = 0; i < colCount; i++) {
            cols.push_back(CSVColumn(names[i]));
            nameIndex[names[i]] = i;
        }
        for(int i = 0; i < rowCount; i++) rows.push_back(CSVRow(names));

        for(int i = 0; i < flatValues.size(); i++){
            CSVElement e = CSVElement(flatValues[i]);
            cols[i%3].data.push_back(e);
            rows[i/3].data.push_back(e);
        }
    }

    CSVColumn CSV::col(std::string colName){
        return cols[nameIndex[colName]];
    }

    CSVRow CSV::row(uint index){
        return rows[index];
    }

    std::ostream& operator<<(std::ostream& out, CSV& csv){
        out << "<";
        for(int i = 0; i < csv.names.size(); i++){
            out << csv.col(csv.names[i]);
            if(i != csv.names.size()-1) out << "; ";
        }
        out << ">";
        return out;
    }

}
