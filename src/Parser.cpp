#include "Parser.h"

#include <iostream>


namespace LFEA {

    Parser::Parser(int argc, char* argv[]) {
        posits = std::vector<Positional*>();
        flags = std::vector<Flag*>();
        opts = std::vector<Option*>();

        args = std::vector<std::string>();
        for(int i = 1; i < argc; i++) args.push_back(argv[i]);
    }

    Parser& Parser::operator+=(Positional* pos){
        if(pos) posits.push_back(pos);
        return *this;
    }
    Parser& Parser::operator+=(Flag* flg){
        if(flg) flags.push_back(flg);
        return *this;
    }
    Parser& Parser::operator+=(Option* opt){
        if(opt) opts.push_back(opt);
        return *this;
    }

    bool Parser::Process(){
        int p = 0;
        Option* tempOpt = nullptr;
        for(std::string str : args) {
            // str is the value passed to an option argument
            if(tempOpt){
                tempOpt->val = str;
                tempOpt = nullptr;
                continue;
            }

            if(str[0] == '-'){
                bool found = false;

                // checking combinations of short options
                if(str.size() > 2 && str[1] != '-') {
                    int letter = 0;

                    while(++letter < str.size()){
                        for(Flag* flag : flags){
                            if(flag->sname[1] == str[letter]) {
                                flag->val = true;
                                found = true;
                                break;
                            }
                        }
                    }
                    
                }
                if(found) continue;

                int index = str.find('=');
                std::string prefix = index != std::string::npos ? str.substr(0, index) : str;
                std::string suffix = index != std::string::npos ? str.substr(index+1) : "";

                for(Flag* flag : flags){
                    if(!flag->val && (flag->name == str || flag->sname == str)) {
                        flag->val = true;
                        found = true;
                        break;
                    }
                }
                for(Option* opt : opts){
                    if(opt->name == prefix || opt->sname == prefix) {
                        if(suffix.empty()) tempOpt = opt;
                        else opt->val = suffix;
                        found = true;
                        break;
                    }
                }
                
                if(!found) return false;
            }else{
                if(p < posits.size()) posits[p]->val = str;
                else return false;
                p++;
            }
        }

        if(p < posits.size()) return false;

        return true;
    }

}