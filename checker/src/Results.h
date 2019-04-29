#ifndef Results_h
#define Results_h

#include <vector>
#include <string>
#include "Parser.h"

class Results : public Parser{
    public:
        Results() {};
        ~Results() {};

        std::vector<std::vector<std::string>> ANDs;
        std::vector<std::vector<std::string>> ORs;
        std::vector<std::vector<std::string>> NOTs;

        void parse(std::string resultFile);
        void print();
    private:
};

#endif
