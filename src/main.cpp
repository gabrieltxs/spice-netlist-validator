//============================================================================//
// The MIT License (MIT)
// Copyright (c) GABRIEL TEIXEIRA ANDRADE SOUSA
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//============================================================================//

#include "Circuito.h"
#include <exception>

int main(int argc, char* argv[]) {
    std::string filename;
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        std::cout << "Usage: Parser_Spice [-f|--filename] FILE\n";
        return 0;
    }
    if (argc == 2 && argv[1][0] != '-') filename = argv[1];
    else if (argc == 3 && (std::string(argv[1]) == "-f" || std::string(argv[1]) == "--filename")) filename = argv[2];
    else {
        std::cerr << "Usage: Parser_Spice [-f|--filename] FILE\n";
        return 1;
    }
    try {
        Circuito circuit;
        circuit.runFile(filename);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
