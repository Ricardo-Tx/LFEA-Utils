#ifndef __PARSER__
#define __PARSER__

#include <vector>
#include <string>

namespace LFEA {

    class Parser {
    public:
        struct Positional {
            Positional(): val() {};
            std::string val;
            operator std::string() const { return val; }
        };
        struct Flag {
            Flag(std::string _name, std::string _sname, bool _def = false): name(_name), sname(_sname), val(_def) {}
            std::string name, sname;
            bool val = false;
            operator bool() const { return val; }
        };
        struct Option {
            Option(std::string _name, std::string _sname, std::string _def = ""): name(_name), sname(_sname), val(_def) {}
            std::string name, sname, val;
            operator std::string() const { return val; }
        };

        Parser(int argc, char* argv[]);
        ~Parser() = default;

        bool Process();

        Parser& operator+=(Positional* pos); 
        Parser& operator+=(Flag* flg); 
        Parser& operator+=(Option* opt); 

        std::vector<std::string> args;

        std::vector<Positional*> posits;
        std::vector<Flag*> flags;
        std::vector<Option*> opts;
    };




}

#endif