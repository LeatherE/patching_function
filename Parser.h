#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>

using namespace std;



class Wire;

class Gate{
public:
    string name;    //gate name
    string type;
    Wire* out = NULL;
    vector<Wire*> fanin;
    bool use;
};

class Wire{
public:
    string name;    //wire's name
    vector<Gate*> fanout;
    Gate* in;

    bool value; //logic value during simulation
};

class Circuit{
public:
    vector<Gate*> gates;
    map<string, Wire*> wireName_2_wireObject;  //map between wire name to wire object
    vector<Wire*> wires;
    vector<string> PIs; //store the wire name
    vector<string> POs; //store the wire name
    bool const_1 = false; //true if 1'b1 exist.
    bool const_0 = false; //true if 1'b0 exist.

    /*  1'b1, 1'b0, PIs, POs is stored as Wire object.
        wires vector contains PIs, POs and all internal wires. */
};


bool parse_circuit(char*, Circuit*);   //parse a file into a Circuit object. Return true if it finishes without error.


#endif