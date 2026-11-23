#include "Circuito.h"
#include "Console.h"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cctype>

// Function to parse a circuit based on the input arguments
std::string Circuito::parseCircuit(std::map<std::string, std::string> argumento, int linha, std::string argC, int qtd_TOKEN)
{
    if (qtd_TOKEN == 0 || argumento["token0"].front() == '*') return "ok";
    parser parserOBJ;
    std::string status = "ok";
    std::string token0 = argumento["token0"];

    // Check the type of the first token and call the corresponding parser function
    if (token0[0] == 'R')
        status = parserOBJ.resistor(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'C')
        status = parserOBJ.capacitor(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'L')
        status = parserOBJ.inductor(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'V' || token0[0] == 'I')
        status = parserOBJ.sourceVI(argumento, linha, argC, qtd_TOKEN); //Independent Sources(Voltage - V) (Current - I)
    else if (token0[0] == 'G' || token0[0] == 'E')
        status = parserOBJ.sourceGE(argumento, linha, argC, qtd_TOKEN); //Linear Voltage-Controlled Current Sources(G)
                                                                        //Linear Voltage-Controlled Voltage Sources(E)
    else if (token0[0] == 'F' || token0[0] == 'H')
        status = parserOBJ.sourceFH(argumento, linha, argC, qtd_TOKEN); //Linear Current-Controlled Current Sources(F)
                                                                        //Linear Current-Controlled Voltage Sources(H)
    else if (token0[0] == 'D')
        status = parserOBJ.junctionDiode(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'Q')
        status = parserOBJ.bjt(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'J')
        status = parserOBJ.jfet(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == 'M')
        status = parserOBJ.mosfet(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".SUBCKT")
        status = parserOBJ.subckt(argumento, linha, argC, qtd_TOKEN, &stackSUBCKT);
    else if (token0 == ".ENDS")
        status = parserOBJ.ends(argumento, linha, argC, qtd_TOKEN, &stackSUBCKT);
    else if (token0[0] == 'X')
        status = parserOBJ.subcktCALL(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".END")
        status = parserOBJ.end(argumento, linha, argC, qtd_TOKEN, &cktChaveFecha);
    else if (token0 == ".OPTIONS")
        status = parserOBJ.options(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".NODESET")
        status = parserOBJ.nodeset(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".IC")
        status = parserOBJ.ic(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".AC")
        status = parserOBJ.ac(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".DC")
        status = parserOBJ.dc(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".PZ")
        status = parserOBJ.pz(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".SENS")
        status = parserOBJ.sens(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".TF")
        status = parserOBJ.tf(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".TRAN")
        status = parserOBJ.tran(argumento, linha, argC, qtd_TOKEN);
    else if (token0 == ".PRINT")
        status = parserOBJ.print(argumento, linha, argC, qtd_TOKEN);
    else if (token0[0] == '*' || qtd_TOKEN == 0 || token0 == ".SAVE")
    { /* DO NOTHING */
    }

    // If none of the known tokens match, set status to indicate an error
    else
    {
        status = "(path_UNDEFINED) line " + std::to_string(linha + 1) + ":\n" + argC + "\n^\n";
    }

    return status;
}

// Split punctuation without indexing past the input or reusing line state.
void Circuito::setLine_from_ckt(std::string argumento)
{
    ++linha;
    const auto first = argumento.find_first_not_of(" \t\r");
    if (first == std::string::npos || argumento[first] == '*') return;
    std::map<std::string, std::string> tokens;
    std::vector<std::size_t> parentheses;
    std::string word;
    int count = 0;
    const auto flush = [&]() {
        if (!word.empty()) {
            tokens["token" + std::to_string(count++)] = word;
            word.clear();
        }
    };
    for (std::size_t index = 0; index < argumento.size(); ++index) {
        const char ch = argumento[index];
        if (std::isspace(static_cast<unsigned char>(ch))) flush();
        else if (ch == '=' || ch == ',') {
            flush();
            tokens["token" + std::to_string(count++)] = std::string(1, ch);
        } else if (ch == '(' || ch == ')') {
            flush();
            if (ch == '(') parentheses.push_back(index);
            else if (parentheses.empty()) {
                throw std::string("line " + std::to_string(linha + 1) +
                    " column " + std::to_string(index + 1) + ": unmatched ')'");
            } else parentheses.pop_back();
        } else word += ch;
    }
    flush();
    if (!parentheses.empty()) {
        throw std::string("line " + std::to_string(linha + 1) +
            " column " + std::to_string(parentheses.back() + 1) + ": unmatched '('");
    }
    // Dispatch once: .SUBCKT, .ENDS and .END update circuit state.
    const auto status = parseCircuit(tokens, linha, argumento, count);
    if (status != "ok") std::cout << "parserError:" << status;
}

// Function to run the circuit from a file
void Circuito::runFile(std::string filename)
{
    linha = 0;
    stackSUBCKT = 0;
    cktChaveFecha = true;
    std::string oDisplay_input = "";
    std::ifstream openedFile(filename);
    if (!openedFile) throw std::runtime_error("Cannot open input file: " + filename);

    getline(openedFile, oDisplay_input); // Read and ignore the first line (TITLE)

    setConsoleColor(6); // Yellow text color
    std::cout << oDisplay_input << std::endl;
    setConsoleColor(7); // Back to white text color

    getline(openedFile, oDisplay_input);

    try
    {
        // Read and process lines from the file
        while (!openedFile.fail())
        {
            this->setLine_from_ckt(oDisplay_input);
            getline(openedFile, oDisplay_input);
        }

        // Check if the circuit is missing an ".END" statement
        if (cktChaveFecha)
        {
            setConsoleColor(12); // Red text color
            std::cout << "parseError:";
            setConsoleColor(7); // Back to white text color
            std::cout << "Missing argument '.END'." << std::endl;
            setConsoleColor(9); // Blue text color
            std::cout << "\nYou must end the circuit.";
            setConsoleColor(7); // Back to white text color
        }
    }
    catch (std::string msg)
    {
        std::cout << msg << std::endl;
    }

    openedFile.close();
}
