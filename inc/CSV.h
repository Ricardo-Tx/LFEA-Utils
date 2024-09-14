#ifndef __CSV__
#define __CSV__

#include <string>
#include <vector>
#include <ostream>
#include <map>

namespace LFEA {

    struct CSVElement {
        CSVElement(std::string str);

        enum CSVElementType { Long, Double, String } type;
        union {
            double d;
            long l;
            char* s;
        } value;
    };

    struct CSVColumn {
        CSVColumn(std::string _name, std::vector<CSVElement> _data);
        CSVColumn(std::string _name);
        CSVColumn();

        std::string name;
        CSVElement::CSVElementType columnType;
        std::vector<CSVElement> data;

        uint size();
        CSVElement operator[](uint index);
        friend std::ostream& operator<<(std::ostream& out, const CSVColumn& col);

        template <typename T>
        void vectorCast(std::vector<T>* vct);
    };

    struct CSVRow {
        CSVRow(std::vector<std::string> _names, std::vector<CSVElement> _data);
        CSVRow(std::vector<std::string> _names);
        CSVRow();

        std::vector<std::string>* names;
        std::vector<CSVElement> data;

        uint size();
        CSVElement operator[](uint index);
        CSVElement operator[](std::string name);
    };

    class CSV {
    public:
        CSV(std::string filename, bool headers, char sep = ',');
        // HeaderCSV();

        uint colCount;
        uint rowCount;

        friend std::ostream& operator<<(std::ostream& out, CSV& csv);
        CSVRow row(uint index);
        CSVColumn col(std::string colName);
        std::vector<std::string> names;
    private:
        std::map<std::string, int> nameIndex;
        std::vector<CSVColumn> cols;
        std::vector<CSVRow> rows;
        std::vector<CSVElement> flatValues;
    };

}

#endif