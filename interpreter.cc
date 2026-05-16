#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

struct instruction
{
    int opCode;
    int src;
};

std::stack<int> SM;                         // Stack Memory
std::vector<int> MM (1024, 0);              // Main Memory
std::vector<instruction> IM (1024, {0, 0}); // Instruction Memory

int *SP; // Stack Pointer
int *IP; // Instruction Pointer

void Usage(std::string s)
{
    std::cerr << "Usage: " << s << " <file.txt>" << std::endl;
    exit(1);
}

/**
 * @brief Encodes an instruction and stores it into an `instruction` type.
 * 
 * @param instr   The instruction to be encoded
 * @param encoded The instruction encoded
 * 
 * @returns The instruction already encoded in the `encoded` argument.
 */
void instructionEncoder(const std::string instr, instruction &encoded)
{
    // Gets the instruction name
    std::string operation = "";
    int length = instr.length(), i = 0;
    while (i < length && instr[i] != ' ')
    {
        operation += instr[i];
        ++i;
    }
    
    // Gets the opCode of the instruction
    int opCode = 0;
    if (instr == "push")
    {
        opCode = 0;
    }
    else if (instr == "pop")
    {
        opCode = 1;
    }
    else if (instr == "add")
    {
        opCode = 2;
    }
    else if (instr == "sub")
    {
        opCode = 3;
    }
    else if (instr == "mul")
    {
        opCode = 4;
    }
    else if (instr == "AND")
    {
        opCode = 5;
    }
    else if (instr == "OR")
    {
        opCode = 6;
    }
    else if (instr == "NOT")
    {
        opCode = 7;
    }
    else if (instr == "cmp")
    {
        std::cerr << "Instruction cmp is not implemented yet." 
            << std::endl;
        exit(1);
    }
    else if (instr == "jmp")
    {
        opCode = 9;
    }
    else
    {
        std::cerr << "Instruction " << instr << " does not exist." 
            << std::endl;
        exit(1);
    }

    encoded.opCode = opCode;

    // Gets the src of the instruction if it has
    std::string src = "";
    if (opCode == 0 || opCode == 1 || opCode == 9)
    {
        for (i = i; i < length; ++i)
        {
            src += instr[i];
        }

        encoded.src = std::stoi(src, nullptr, 0);
    }
}

/** 
 * @brief Interpretates the code in the file. First transforms the instructions in
 *        bytes, then operates each instruction in order.
 * 
 * @param file An std::ifstream type wich contains the code.
 */
void interpretateFile(const std::ifstream &file)
{
    
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        Usage(argv[0]);
    }

    std::string path = argv[1];
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open " << path << std::endl;
        exit(1);
    }

    interpretateFile(file);

    file.close();
    return 0;
}