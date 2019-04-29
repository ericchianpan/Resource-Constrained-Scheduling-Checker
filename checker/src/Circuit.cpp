#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "Circuit.h"

using namespace std;

Circuit::Circuit()
{
}
Circuit::~Circuit()
{
    for(map<string, v_ptr>::iterator it = _vertexs.begin(); it != _vertexs.end(); ++it) {
        delete it->second;
    }
}
void Circuit::parse(std::string blifFile)
{
    fstream fin(blifFile, fstream::in);
    if(!fin) {
        cout << "can not open blif file" << endl;
        exit(1);
    }

    string content;

    while(fin >> content) {
        if(content == ".model") { 
            fin>> content;
            _model = content; 
        }

        if(content == ".inputs") {
            while(true) {
                fin >> content;
                if(content != "\\") {
                    v_ptr vertex = new Vertex(content);
                    _vertexs.insert(make_pair(content, vertex));
                    _inputs.insert(content);
                    // _nodes.insert(content);
                    if(fin.peek() == '\n') break;
                }
            }
        }

        if(content == ".outputs") {
            while(true) {
                fin >> content;
                if(content != "\\") {
                    v_ptr vertex = new Vertex(content);
                    _vertexs.insert(make_pair(content, vertex));
                    _outputs.insert(content);
                    // _nodes.insert(content);
                    if(fin.peek() == '\n') break;
                }
            }
        }

        if(content == ".names") {
            vector<string> logicVertexs;
            while(true) {
                fin >> content;
                logicVertexs.push_back(content);
                // _nodes.insert(content);
                if(fin.peek() == '\n') break;
            }
            vector<string> logics;  
            while(true) {
                string str;
                getline(fin, str);
                logics.push_back(str);
                if(fin.peek() == '.') break;
            }

            string vertexName = logicVertexs.back();
            v_ptr vertex = getVertex(vertexName);
            if(vertex == nullptr) vertex = new Vertex(vertexName);

            unsigned one_count = 0;
            for(unsigned i = 0; i < logics.back().length(); ++i) {
                if(logics.back()[i] == '1') one_count++;
            }
            if(one_count < 2) vertex->_gate_type = NOT;
            else if(one_count == 2) vertex->_gate_type = OR;
            else vertex->_gate_type = AND;

            for(unsigned i = 0; i < logicVertexs.size() - 1; ++i) {
                v_ptr predecessorVertex = getVertex(logicVertexs[i]);
                if(predecessorVertex == nullptr) predecessorVertex = new Vertex(logicVertexs[i]);
                vertex->_predecessor.push_back(predecessorVertex);
                predecessorVertex->_successor.push_back(vertex);
            }
            _vertexs.insert(make_pair(content, vertex));
        }
        if(content == ".end") break;
    }

    fin.close();
}
void Circuit::print()
{
    // cout << "model: " << _model << endl;
    // cout << "inputs: ";
    // for(set<string>::iterator it = _inputs.begin(); it != _inputs.end(); ++it)
        // std::cout << *it << " "; 
    // cout << endl;
    // cout << "outpust: ";
    // for(set<string>::iterator it = _outputs.begin(); it != _outputs.end(); ++it)
        // std::cout << *it << " "; 
    // cout << endl;

    for(auto it = _vertexs.begin(); it != _vertexs.end(); ++it)
    {
        // cout << "vertex: " << it->first << endl;
        // cout << "successor: "; 
        // for(auto it2 = it->second->_successor.begin(); it2 != it->second->_successor.end(); ++it2)
            // cout << (*it2)->_name << " "; 
        // cout << endl;
        // cout << "predecessor: "; 
        // for(auto it2 = it->second->_predecessor.begin(); it2 != it->second->_predecessor.end(); ++it2)
            // cout << (*it2)->_name << " ";
        // cout << endl;
        cout << "gate type of vertex " << it->first << " : ";
        cout << it->second->_gate_type << endl; 
    }
    
}
v_ptr Circuit::getVertex(std::string vertexName)
{
    map<string, v_ptr>::iterator it = _vertexs.find(vertexName);
    return (it == _vertexs.end()) ? NULL : it->second;
}
bool Circuit::checkCorrectness(string resultFile)
{
    // parse result
    // check resource constraints
    // check graph scheduling completeness and ordering 

    _results.parse(resultFile);
    _completeCycle = _results.ANDs.size(); 

    // _results.print();

    vector<vector<string>> & ANDs = _results.ANDs;
    vector<vector<string>> & ORs = _results.ORs;
    vector<vector<string>> & NOTs = _results.NOTs;

    // std::cout << "number of vertex: " << _vertexs.size() << std::endl;
    // std::cout << "number of input:" << _inputs.size() << std::endl;
    
    unsigned outputCount = 0;
    for(unsigned i = 0; i < ANDs.size(); ++i) {
        outputCount += ANDs[i].size();
    } 
    for(unsigned i = 0; i < ORs.size(); ++i) {
        outputCount += ORs[i].size();
    } 
    for(unsigned i = 0; i < NOTs.size(); ++i) {
        outputCount += NOTs[i].size();
    } 
    if(outputCount != _vertexs.size() - _inputs.size()) {
        cout << "failure: wrong result vertices" << endl;
        // printNoScheduleV();
        return false;
    }

    set<string> completeVertex(_inputs);
    for(unsigned c = 0; c < _completeCycle; ++c) {
        if(ANDs[c].size() > _AND_CONSTRAINT) {
            cout << "failure: AND operation resource constraints check" << endl;
            return false;
        } 
        if(ORs[c].size() > _OR_CONSTRAINT) {
            cout << "failure: OR operation resource constraints check" << endl;
            return false;
        }
        if(NOTs[c].size() > _NOT_CONSTRAINT) {
            cout << "failure: NOT operation resource constraints check" << endl;
            return false;
        }

        set<string> tempVertex;
        for(unsigned i = 0; i < ANDs[c].size(); ++i) {
            v_ptr vertex;
            map<string, v_ptr>::iterator it = _vertexs.find(ANDs[c][i]);
            if(it == _vertexs.end()) {
                cout << "vertex " << ANDs[c][i] << " not found" << endl;
                exit(1);
            }
            else vertex = it->second;
            if(vertex->_gate_type != AND) {
                cout << "failure: operation type check" << endl;
                return false;
            }
            if(!checkVertexReady(vertex, completeVertex)) {
                cout << "failure: operation ordering check" << endl;
                return false;
            }
            tempVertex.insert(vertex->_name);
        }
       
        for(unsigned i = 0; i < ORs[c].size(); ++i) {
            v_ptr vertex;
            map<string, v_ptr>::iterator it = _vertexs.find(ORs[c][i]);
            if(it == _vertexs.end()) {
                cout << "vertex " << ORs[c][i] << " not found" << endl;
                exit(1);
            }
            else vertex = it->second;
            if(vertex->_gate_type != OR) {
                cout << "failure: operation type check" << endl;
                return false;
            }
            if(!checkVertexReady(vertex, completeVertex)) {
                cout << "failure: operation ordering check" << endl;
                return false;
            }
            tempVertex.insert(vertex->_name);
        }

        for(unsigned i = 0; i < NOTs[c].size(); ++i) {
            v_ptr vertex;
            map<string, v_ptr>::iterator it = _vertexs.find(NOTs[c][i]);
            if(it == _vertexs.end()) {
                cout << "vertex " << NOTs[c][i] << " not found" << endl;
                exit(1);
            }
            else vertex = it->second;
            if(vertex->_gate_type != NOT) {
                cout << "failure: operation type check" << endl;
                return false;
            }
            if(!checkVertexReady(vertex, completeVertex)) {
                cout << "failure: operation ordering check" << endl;
                return false;
            }
            tempVertex.insert(vertex->_name);
        }
        // completeVertex.insert(tempVertex.begin(), tempVertex.end());
        for(set<string>::iterator it = tempVertex.begin(); it != tempVertex.end(); ++it) {
            if(completeVertex.find(*it) != completeVertex.end()) {
                cout << "failure: vertex repeated" << endl;
                return false;
            }
            completeVertex.insert(*it);
        }
    }
    for(map<string, v_ptr>::iterator it = _vertexs.begin(); it != _vertexs.end(); ++it) {
        if(completeVertex.find(it->second->_name) == completeVertex.end()) {
            cout << "failure: graph scheduling completeness check" << endl;
            return false;
        }
    }
    return true;
}
bool Circuit::checkVertexReady(v_ptr &vertex, set<string> &completeVertex)
{
    for(unsigned i = 0; i < vertex->_predecessor.size(); ++i) {
        if(completeVertex.find(vertex->_predecessor[i]->_name) == completeVertex.end()) 
            return false;
    }
    return true;
}
void Circuit::printNoScheduleV()
{
    set<string> results;
    for(unsigned i = 0; i < _results.ANDs.size(); ++i) {
        for(unsigned j = 0; j < _results.ANDs[i].size(); ++j) {
            results.insert(_results.ANDs[i][j]);
        }
    }
    for(unsigned i = 0; i < _results.ORs.size(); ++i) {
        for(unsigned j = 0; j < _results.ORs[i].size(); ++j) {
            results.insert(_results.ORs[i][j]);
        }
    }
    for(unsigned i = 0; i < _results.NOTs.size(); ++i) {
        for(unsigned j = 0; j < _results.NOTs[i].size(); ++j) {
            results.insert(_results.NOTs[i][j]);
        }
    }
    cout << "No schedule vertex: ";
    for(map<string, v_ptr>::iterator it = _vertexs.begin(); it != _vertexs.end(); ++it) {
        if(_inputs.find(it->first) != _inputs.end()) continue;
        if(results.find(it->first) == results.end()) cout << it->first << " ";
    }
    cout << endl;
}
