#pragma once
#include "parser.h"

class Circuito {
public:
    Circuito();
    ~Circuito();
    void runFile(std::string filename);
    void setLine_from_ckt(std::string argumento);
    std::string parseCircuit(std::map<std::string, std::string> argumento,
                             int linha, std::string argC, int qtd_TOKEN);
private:
    int linha = 0;
    int stackSUBCKT = 0;
    bool cktChaveFecha = true;
};
