#ifndef CIRCUIT_H 
#define CIRCUIT_H

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "Results.h"
#include "Enum.h"

class Vertex;
typedef Vertex* v_ptr; 

class Vertex{
    public:
        Vertex() {};
        Vertex(std::string name): _name(name) {}
        ~Vertex() {};
        friend class Circuit;
    private:
        GateType _gate_type = DEFAULT;
        std::string _name;
        std::vector<v_ptr> _predecessor; 
        std::vector<v_ptr> _successor; 
};

class Circuit{
    public:
        Circuit();
        Circuit(unsigned and_constraint, unsigned or_constraint, unsigned not_constraint)
            :_AND_CONSTRAINT(and_constraint), _OR_CONSTRAINT(or_constraint), _NOT_CONSTRAINT(not_constraint) {}
        ~Circuit();

        unsigned completeCycle() { return _completeCycle; }
        std::string model() { return _model; }
        std::set<std::string> inputs() { return _inputs; }
        std::set<std::string> outputs() { return _outputs; }
        std::set<std::string> nodes() { return _nodes; }

        void parse(std::string blifFile);
        void print();
        v_ptr getVertex(std::string vertexName);

        void printNoScheduleV();
        bool checkCorrectness(std::string resultFile);
        bool checkVertexReady(v_ptr &vertex, std::set<std::string> &completeVertex);

    private:
        unsigned _AND_CONSTRAINT;
        unsigned _OR_CONSTRAINT;
        unsigned _NOT_CONSTRAINT;
        unsigned _completeCycle;
        Results _results;

        std::string _model;
        std::set<std::string> _inputs;
        std::set<std::string> _outputs;
        std::set<std::string> _nodes;
        
        std::map<std::string, v_ptr> _vertexs;
};

#endif
