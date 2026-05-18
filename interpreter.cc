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
 * @brief Debug function to see the contents of IM in hexadecimal.
 */
void debugPrintIM()
{
    std::cout << "opCode |  src" << std::endl;
    int i = 0;
    while (i < 1024 && IM[i].opCode != 0x00)
    {
        instruction instr = IM[i];
        int space = 0;
        if (instr.opCode - 16 < 16)
        {
            space = 2;
        }
        else
        {
            space = 1;
        }
        
        for (int j = 0; j < space; ++j)
        {
            std::cout << ' ';
        }
        
        std::cout << "0x" << std::hex << IM[i].opCode;

        for (int j = 0; j < 2; ++j)
        {
            std::cout << ' ';
        }

        std::cout << "|  0x" << std::hex << instr.src << std::endl;

        ++i;
    }
}

/**
 * @brief Debug function to see the contents of IM in hexadecimal.
 * 
 * @param rows Upper limit of rows you want to print. Values from 0 to 1024
 */
void debugPrintMM(int rows)
{
    std::cout << "Main Memory Contents" << std::endl;
    std::cout << "#col | MM" << std::endl;
    int i = 0;
    while (i < rows)
    {
        std::cout << ' ' << std::hex << i;

        int space = 3;
        if (i > 255)
        {
            space = 1;
        }
        else if (i > 15)
        {
            space = 2;
        }

        for (int j = 0; j < space; ++j)
        {
            std::cout << ' ';
        }
        
        std::cout << "| 0x" << std::hex << MM[i] << std::endl;
        ++i;
    }
}

bool isLetter(char s)
{
    bool letter = false;
    if ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z'))
    {
        letter = true;
    }

    return letter;
}

/**
 * @brief Encodes an instruction and stores it into an `instruction` type.
 * 
 * @param instr   The instruction to be encoded
 * @param encoded The instruction encoded
 * 
 * @returns The instruction already encoded in the `encoded` argument.
 */
instruction instructionEncoder(const std::string instr, int pass)
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
                if (labels[operation] == -1)
                {
                    labels[operation] = PC;
                }
                else if (pass == 1)
                {
                    std::cerr << "Error at line: " << PC + 1 << "Label " 
                        << operation << " already used." << std::endl;
                    exit(1);
                }
            }
            
            operation = "";
            i += 2; // Skips ": "
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
        opCode = 0x01;
    }
    else if (operation == "pop")
    {
        opCode = 0x02;
    }
    else if (operation == "add")
    {
        opCode = 0x03;
    }
    else if (operation == "sub")
    {
        opCode = 0x04;
    }
    else if (operation == "mul")
    {
        opCode = 0x05;
    }
    else if (operation == "AND")
    {
        opCode = 0x06;
    }
    else if (operation == "OR")
    {
        opCode = 0x07;
    }
    else if (operation == "NOT")
    {
        opCode = 0x08;
    }
    else if (operation == "cmp")
    {
        std::cerr << "Error at line: " << PC + 1  << std::endl
            << "Instruction cmp is not implemented yet." << std::endl;
        exit(1);
    }
    else if (operation == "jmp")
    {
        opCode = 0x0A;
    }
    else if (operation == "load")
    {
        opCode = 0x0B;
    }
    else if (operation == "stor")
    {
        opCode = 0x0C;
    }
    else
    {
        std::cerr << "Instruction " << instr << " does not exist." 
            << std::endl;
        exit(1);
    }

    encoded.opCode = opCode;

    // Gets the src of the instruction if it has
    if (opCode == 0x01 || opCode == 0x02 || opCode == 0x0B || opCode == 0x0C)
    {
        while (instr[i] == ' ') // Skips the space
        {
            ++i;
        }
        
        std::string src = "";
        while (i < length && instr[i] != ' ') // Avoids the comments
        {
            src += instr[i];
            ++i;
        }

        encoded.src = std::stoi(src, nullptr, 0);
    }
    else if (opCode == 0x0A)
    {
        // Gets the address or the label of the address
        while (instr[i] == ' ') // Skips the space
        {
            ++i;
        }
        
        std::string src = "";
        while (i < length && instr[i] != ' ') // Avoids the comments
        {
            src += instr[i];
            ++i;
        }

        if (isLetter(src[0])) // Its a label
        {
            if (labels.find(src) != labels.end())
            {
                encoded.src = labels[src];
            }
            else // If not exists, creates a slot but without a valid address
            {
                encoded.src = -1;
            }
        }
        else // Its an address
        {
            encoded.src = std::stoi(src, nullptr, 0);
        }
    }

    return encoded;
}

/** 
 * @brief Loads all the intructions in the code by encoding each one and saving
 *        them into a memory.
 * 
 * @param file An std::ifstream type wich contains the code.
 */
void loadInstructions(std::ifstream &file, int pass)
{
    // Reads line by line the code
    std::string instr;
    while (std::getline(file, instr) && PC < 1024)
    {
        // Saves instruction in IM
        IM[PC] = instructionEncoder(instr, pass);
        ++PC;
    }

    // Changes the PC to the address of the main function, 
    // otherwise starts at the first instruction.
    if (labels.find("main") != labels.end())
    {
        PC = labels["main"];
    }
    else
    {
        PC = 0;
    }
}

/**
 * @brief Handles the push operation. Pushes the src value into SM.
 */
void handlePush()
{
    SM.push(IM[PC].src);
}

/**
 * @brief Handles the pop operation. Pops the top of the SM.
 */
void handlePop()
{
    if (SM.empty())
    {
        std::cerr << "Trying to pop Stack Memory with no elements." 
            << std::endl;
    }
    else
    {
        SM.pop();
    }
}

/**
 * @brief Handles the add operation. Adds the two last elements of SM. 
 *        The result is pushed in SM.
 */
void handleAdd()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    if (!SM.empty())
    {
        b = SM.top();
        SM.pop();
    }

    SM.push(a + b);
}

/**
 * @brief Handles the sub operation. Substracts the element before top with the
 *        top element of the SM. The result is pushed in SM.
 */
void handleSub()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    if (!SM.empty())
    {
        b = SM.top();
        SM.pop();
    }

    SM.push(b - a);
}

/**
 * @brief Handles the mul operation. Multiplies the last two elements of SM. 
 *        The result is pushed in SM.
 */
void handleMul()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    if (!SM.empty())
    {
        b = SM.top();
        SM.pop();
    }

    SM.push(a * b);
}

/**
 * @brief Handles the AND operation. Operates a bitwise and with the two last 
 *        elements of SM. The result is pushed in SM.
 */
void handleAND()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    if (!SM.empty())
    {
        b = SM.top();
        SM.pop();
    }

    SM.push(a & b);
}

/**
 * @brief Handles the OR operation. Operates a bitwise or with the two last 
 *        elements of SM. The result is pushed in SM.
 */
void handleOR()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    if (!SM.empty())
    {
        b = SM.top();
        SM.pop();
    }

    SM.push(a | b);
}

/**
 * @brief Handles the NOT operation. Operates a bitwise not with the top of SM.
 *        The result is pushed in SM.
 */
void handleNOT()
{
    int a, b;
    if (!SM.empty())
    {
        a = SM.top();
        SM.pop();
    }

    SM.push(~a);
}

/**
 * @brief Handles the jmp operation. Modifies the PC register making a jump 
 *        from one instruction to another.
 */
void handleJmp()
{
    // PC set to the instruction before the one chosen so the next is the one 
    // we want to execute.
    PC = IM[PC].src - 1;
}

/**
 * @brief Handles the load operation. Loads a value from memory and pushes it 
 *        to SM.
 */
void handleLoad()
{
    SM.push(MM[IM[PC].src]);
}

/**
 * @brief Handles the stor operation. Stores the top of SM on memory and pops 
 *        the top of SM.
 */
void handleStor()
{
    if (SM.empty())
    {
        std::cerr << "Trying to store from Stack Memory with no elements." 
            << std::endl;
    }
    else
    {
        // Save value on memory and pop from stack
        MM[IM[PC].src] = SM.top();
        SM.pop();
    }
}


/**
 * @brief Executes the code stored in the IM.
 */
void executeCode()
{
    PC = 0;
    while (PC < 1024 && IM[PC].opCode != 0x00)
    {
        instruction instr = IM[PC];
        switch (instr.opCode)
        {
        case 0x01:
            handlePush();
            break;
        case 0x02:
            handlePop();
            break;
        case 0x03:
            handleAdd();
            break;
        case 0x04:
            handleSub();
            break;
        case 0x05:
            handleMul();
            break;
        case 0x06:
            handleAND();
            break;
        case 0x07:
            handleOR();
            break;
        case 0x08:
            handleNOT();
            break;
        /*case 0x09:
            handleCmp();
            break;*/
        case 0x0A:
            handleJmp();
            break;
        case 0x0B:
            handleLoad();
            break;
        case 0x0C:
            handleStor();
            break;
        
        default:
            break;
        }

        ++PC;
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

    loadInstructions(file, 1);
    file.clear();  // Second pass to solve unsolved labels
    file.seekg(0);
    loadInstructions(file, 2);
    file.close();

    executeCode();

    debugPrintIM();
    debugPrintMM(10);

    return 0;
}