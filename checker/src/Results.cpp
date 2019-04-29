#include <fstream>
#include <iostream>
#include <regex.h> 
#include "Results.h"

using namespace std;

void Results::parse(std::string resultFile)
{
    fstream fin(resultFile, fstream::in);
    if(!fin) {
        cout << "can not open result file" << endl;
        exit(1);
    }

    string content;
    while(getline(fin, content)) {
        regex_t regex;
        if(regcomp(&regex, "(^[0-9]+):(.*)", REG_EXTENDED)) cerr << "regex compile error" << endl;
        if(!regexec(&regex, content.c_str(), 0, NULL, 0)) {
            size_t fromBracket, backBracket;

            fromBracket = content.find("{");
            backBracket = content.find("}");
            string AND_logics_str = content.substr(fromBracket + 1, backBracket - fromBracket - 1);
            vector<string> AND_vertexs = split(AND_logics_str, ' ');
            vector<string> AND_temp;
            for(unsigned i = 0; i < AND_vertexs.size(); ++i) {
                if(AND_vertexs[i] == " " || AND_vertexs[i] == "") continue;
                AND_temp.push_back(AND_vertexs[i]);
            }
            ANDs.push_back(AND_temp);

            fromBracket = content.find("{", backBracket + 1);
            backBracket = content.find("}", backBracket + 1);
            string OR_logics_str = content.substr(fromBracket + 1, backBracket - fromBracket - 1);
            vector<string> OR_vertexs = split(OR_logics_str, ' ');
            vector<string> OR_temp;
            for(unsigned i = 0; i < OR_vertexs.size(); ++i) {
                if(OR_vertexs[i] == " " || OR_vertexs[i] == "") continue;
                OR_temp.push_back(OR_vertexs[i]);
            }
            ORs.push_back(OR_temp);

            fromBracket = content.find("{", backBracket + 1);
            backBracket = content.find("}", backBracket + 1);
            string NOT_logics_str = content.substr(fromBracket + 1, backBracket - fromBracket - 1);
            vector<string> NOT_vertexs = split(NOT_logics_str, ' ');
            vector<string> NOT_temp;
            for(unsigned i = 0; i < NOT_vertexs.size(); ++i) {
                if(NOT_vertexs[i] == " " || NOT_vertexs[i] == "") continue;
                NOT_temp.push_back(NOT_vertexs[i]);
            }
            NOTs.push_back(NOT_temp);
        }
        if(content.find("END") != string::npos) break;
    }
    
    fin.close();
}
void Results::print()
{
    cout << "AND operation:" << endl;
    for(unsigned i = 0; i < ANDs.size(); ++i) {
        cout << "Time " << i+1 << ": ";
        for(unsigned j = 0; j < ANDs[i].size(); ++j) {
            cout << ANDs[i][j] << " ";
        }
        cout << endl;
    }
    cout << "OR operation:" << endl;
    for(unsigned i = 0; i < ORs.size(); ++i) {
        cout << "Time " << i+1 << ": ";
        for(unsigned j = 0; j < ORs[i].size(); ++j) {
            cout << ORs[i][j] << " ";
        }
        cout << endl;
    }
    cout << "NOT operation:" << endl;
    for(unsigned i = 0; i < NOTs.size(); ++i) {
        cout << "Time " << i+1 << ": ";
        for(unsigned j = 0; j < NOTs[i].size(); ++j) {
            cout << NOTs[i][j] << " ";
        }
        cout << endl;
    }
}
