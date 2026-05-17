#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

struct instruction
{
    int opCode = 0;
    int src = 0;
};

std::stack<int> SM;                         // Stack Memory
std::vector<int> MM (1024, 0);              // Main Memory
std::vector<instruction> IM (1024, {0, 0}); // Instruction Memory
std::map<std::string, int> labels;          // Label Memory

int *SP;    // Stack Pointer
int *IP;    // Instruction Pointer
int PC = 0; // Program Counter

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
instruction instructionEncoder(const std::string instr)
{
    instruction encoded;

    std::string lastChar = "";

    // Gets the instruction name
    std::string operation = "";
    int length = instr.length(), i = 0;
    while (i < length)
    {
        if (instr[i] == ':') // Save as label
        {
            if (!labels.emplace(operation, PC).second)
            {
                std::cerr << "Label " << operation 
                    << " already used. Error at line: " << PC << std::endl;
                    exit(1);
            }
            
            operation = "";
            i += 2;
        }

        if (instr[i] == ' ' && operation != "")
        {
            break;
        }

        if (instr[i] != ' ')
        {
            operation += instr[i];
        }
        ++i;
    }
    
    // Gets the opCode of the instruction
    int opCode = 0;
    if (operation == "push")
    {
        opCode = 0;
    }
    else if (operation == "pop")
    {
        opCode = 0x01;
    }
    else if (operation == "add")
    {
        opCode = 0x02;
    }
    else if (operation == "sub")
    {
        opCode = 0x03;
    }
    else if (operation == "mul")
    {
        opCode = 0x04;
    }
    else if (operation == "AND")
    {
        opCode = 0x05;
    }
    else if (operation == "OR")
    {
        opCode = 0x06;
    }
    else if (operation == "NOT")
    {
        opCode = 0x07;
    }
    else if (operation == "cmp")
    {
        std::cerr << "Instruction cmp is not implemented yet." << std::endl <<
            "Error at line: " << PC + 1 << std::endl;
        exit(1);
    }
    else if (operation == "jmp")
    {
        opCode = 0x09;
    }
    else if (operation == "load")
    {
        opCode = 0x0A;
    }
    else if (operation == "stor")
    {
        opCode = 0x0B;
    }
    else
    {
        std::cerr << "Instruction " << instr << " does not exist." 
            << std::endl;
        exit(1);
    }

    encoded.opCode = opCode;

    // Gets the src of the instruction if it has
    if (opCode == 0x00 || opCode == 0x01 || opCode == 0x09 || opCode == 0x0A ||
        opCode == 0x0B)
    {
        std::string src = "";
        ++i; // Skips the space
        while (i < length && instr[i] != ' ') // Avoids the comments
        {
            src += instr[i];
            ++i;
        }

        encoded.src = std::stoi(src, nullptr, 0);
    }

    return encoded;
}

/** 
 * @brief Loads all the intructions in the code by encoding each one and saving
 *        them into a memory.
 * 
 * @param file An std::ifstream type wich contains the code.
 */
void loadInstructions(std::ifstream &file)
{
    // Reads line by line the code
    std::string instr;
    while (std::getline(file, instr))
    {
        // Saves instruction in IM
        IM[PC] = instructionEncoder(instr);
        ++PC;
    }

    // Changes the PC to the address of the main function, 
    // otherwise starts at the first instruction.
    if (labels.find("main:") != labels.end())
    {
        PC = labels["main:"];
    }
    else
    {
        PC = 0;
    }
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

    loadInstructions(file);
    file.close();

    return 0;
}