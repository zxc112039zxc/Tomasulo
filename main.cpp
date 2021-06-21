#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <vector>
#include <string>
#include <iomanip>
#define ADDI 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5

using namespace std;

struct Instruction
{
    int operation, rd, r2, r3;
};

struct RS
{
    int operation , rd;
    int r2, r3;
    bool rs2, rs3;
    int cycEXE;
    bool busy;

    RS() : operation(0), rd(0), r2(0), r3(0), rs2(false), rs3(false), cycEXE(0), busy(false) {}
};

struct Buffer
{    
    int operation;
    int rs;
    int rd;
    int r2, r3;
    bool empty;
    int cycWB;
};

int doneInst = 0;
int RF[5] = {0, 2, 4, 6, 8};
int RAT[5] = {-1, -1, -1, -1, -1};
int cycOfADD = 1, cycOfMUL = 1, cycOfDIV = 1, CYCLE = 1;
bool needToPrint = false;
queue<Instruction> Instructions;
vector<RS> RS1(3, RS());
vector<RS> RS2(2, RS());
Buffer rs1Buffer = {0, 0, 0, 0.0, 0.0, true, 0};
Buffer rs2Buffer = {0, 0, 0, 0.0, 0.0, true, 0};

void issue()
{
    if (!Instructions.empty()) //if is empty not ISSUE
    {
        Instruction tmp = Instructions.front();

        if (tmp.operation <= 3 && tmp.operation >= 1) //instruction is ADDI or ADD or SUB
        {
            if (RS1.size() <= 3) //ADD RS have space
            {
                RS inputRS;

                inputRS.operation = tmp.operation;
                inputRS.rd = tmp.rd;
                if (RAT[tmp.r2 - 1] == -1) //RAT is empty
                {
                    inputRS.r2 = RF[tmp.r2 - 1];
                    inputRS.rs2 = false;
                }
                else // RAT is non-empty
                {
                    inputRS.r2 = RAT[tmp.r2 - 1];
                    inputRS.rs2 = true;
                }

                if (RAT[tmp.r3 - 1] == -1) //RAT is empty
                {
                    inputRS.r3 = RF[tmp.r3 - 1];
                    inputRS.rs3 = false;
                }
                else // RAT is non-empty
                {
                    inputRS.r3 = RAT[tmp.r3 - 1];
                    inputRS.rs3 = true;
                }

                if (tmp.operation == 1) //ADDI
                {
                    inputRS.r3 = tmp.r3;
                    inputRS.rs3 = false;
                }

                inputRS.cycEXE = CYCLE + 1;

                for (size_t i = 0; i < RS1.size(); i++) 
                {
                    if (RS1[i].busy == false) //find first space
                    {
                        RS1[i] = inputRS; //put in ADD RS
                        RS1[i].busy = true; //set to busy
                        RAT[inputRS.rd - 1] = i + 1; //update RAT
                        Instructions.pop();
                        needToPrint = true;

                        break;
                    }
                }
            }
        }
        else if (tmp.operation <= 5 && tmp.operation >= 4) //instruction is MUL or DIV
        {
            if (RS2.size() <= 2) //MUL & DIV RS have space
            {
                RS inputRS;

                inputRS.operation = tmp.operation;
                inputRS.rd = tmp.rd;
                if (RAT[tmp.r2 - 1] == -1) //RAT is empty
                {
                    inputRS.r2 = RF[tmp.r2 - 1];
                    inputRS.rs2 = false;
                }
                else // RAT is non-empty
                {
                    inputRS.r2 = RAT[tmp.r2 - 1];
                    inputRS.rs2 = true;
                }

                if (RAT[tmp.r3 - 1] == -1) //RAT is empty
                {
                    inputRS.r3 = RF[tmp.r3 - 1];
                    inputRS.rs3 = false;
                }
                else // RAT is non-empty
                {
                    inputRS.r3 = RAT[tmp.r3 - 1];
                    inputRS.rs3 = true;
                }

                inputRS.cycEXE = CYCLE + 1;
                for (size_t i = 0; i < RS2.size(); i++)
                {
                    if (RS2[i].busy == false) //find first space
                    {
                        RS2[i] = inputRS; //put in MUL & DIV RS
                        RS2[i].busy = true; //set to busy
                        RAT[inputRS.rd - 1] = i + 4; //update RAT
                        Instructions.pop();
                        needToPrint = true;

                        break;
                    }
                }
            }
        }
    }
}

void execute()
{
    if (rs1Buffer.empty) //check if ADD budder empty 
    {
        for (size_t i = 0; i < RS1.size(); i++)
        {
            if (RS1[i].rs2 == false && RS1[i].rs3 == false && RS1[i].cycEXE <= CYCLE && RS1[i].busy == true) //check for execute
            {
                needToPrint = true;

                rs1Buffer.cycWB = CYCLE + cycOfADD; //calculate cycle of write back

                rs1Buffer.operation = RS1[i].operation; //update buufer
                rs1Buffer.rs = i + 1;
                rs1Buffer.rd = RS1[i].rd;
                rs1Buffer.r2 = RS1[i].r2;
                rs1Buffer.r3 = RS1[i].r3;
                rs1Buffer.empty = false; //to non-empty

                break;
            }
        }
    }

    if (rs2Buffer.empty)
    {
        for (size_t i = 0; i < RS2.size(); i++) //RS2
        {
            if (RS2[i].rs2 == false && RS2[i].rs3 == false && RS2[i].cycEXE <= CYCLE && RS2[i].busy == true) //check for execute
            {
                needToPrint = true;

                if (RS2[i].operation == 4)
                {
                    rs2Buffer.cycWB = CYCLE + cycOfMUL; //calculate MUL cycle of write back
                }
                else
                {
                    rs2Buffer.cycWB = CYCLE + cycOfDIV; //calculate DIV cycle of write back
                }

                rs2Buffer.operation = RS2[i].operation; //update buufer
                rs2Buffer.rs = i + 4;
                rs2Buffer.rd = RS2[i].rd;
                rs2Buffer.r2 = RS2[i].r2;
                rs2Buffer.r3 = RS2[i].r3;
                rs2Buffer.empty = false; //to non-empty

                break;
            }
        }
    }
}

void writeBack()
{
    int result;
    if (rs1Buffer.empty == false && rs1Buffer.cycWB <= CYCLE) //check for write back
    {
        needToPrint = true;
        if (rs1Buffer.operation == 3)
        {
            result = rs1Buffer.r2 - rs1Buffer.r3; //calculate result for SUB
        }
        else
        {
            result = rs1Buffer.r2 + rs1Buffer.r3; //calculate result for ADD
        }

        size_t pos; //save for release RAT
        for (pos = 0; pos < RS1.size(); pos++) //release ADD RS
        {
            if (RS1[pos].operation == rs1Buffer.operation && RS1[pos].rd == rs1Buffer.rd && RS1[pos].r2 == rs1Buffer.r2 && RS1[pos].r3 == rs1Buffer.r3) //make sure for release
            {
                RS1[pos].busy = false; //change to non-busy

                break;
            }
        }
        
        for (size_t i = 0; i < RS1.size(); i++) //WB to ADD RS
        {
            if (RS1[i].r2 == rs1Buffer.rs && RS1[i].rs2 == true) //for r2
            {
                RS1[i].r2 = result;
                RS1[i].rs2 = false; //change to non-wait
            }

            if (RS1[i].r3 == rs1Buffer.rs && RS1[i].rs3 == true) //for r3
            {
                RS1[i].r3 = result;
                RS1[i].rs3 = false; //change to non-wait
            }
        }

        for (size_t i = 0; i < RS2.size(); i++) //WB to MUL & DIV RS
        {
            if (RS2[i].r2 == rs1Buffer.rs && RS2[i].rs2 == true) //for r2
            {
                RS2[i].r2 = result;
                RS2[i].rs2 = false; //change to non-wait
            }

            if (RS2[i].r3 == rs1Buffer.rs && RS2[i].rs3 == true) //for r3
            {
                RS2[i].r3 = result;
                RS2[i].rs3 = false; //change to non-wait
            }
        }

        if (RAT[rs1Buffer.rd - 1] == pos + 1) //check RAT position
        {
            RAT[rs1Buffer.rd - 1] = -1; //release RAT
        }
        RF[rs1Buffer.rd - 1] = result; //WB to RF

        rs1Buffer.operation = 0; //initialize ADD buffer
        rs1Buffer.rd = 0;
        rs1Buffer.r2 = 0;
        rs1Buffer.r3 = 0;
        rs1Buffer.rs = 0;
        rs1Buffer.cycWB = 0;
        rs1Buffer.empty = true;

        doneInst--; //done for one instruction
    }

    if (rs2Buffer.empty == false && rs2Buffer.cycWB <= CYCLE) //check for write back
    {
        needToPrint = true;

        if (rs2Buffer.operation == 4) 
        {
            result = rs2Buffer.r2 * rs2Buffer.r3; //calculate result for MUL
        }
        else
        {
            result = rs2Buffer.r2 / rs2Buffer.r3; //calculate result for DIV
        }

        size_t pos;
        for (pos = 0; pos < RS2.size(); pos++) //release MUL & DIV RS
        {
            if (RS2[pos].operation == rs2Buffer.operation && RS2[pos].rd == rs2Buffer.rd && RS2[pos].r2 == rs2Buffer.r2 && RS2[pos].r3 == rs2Buffer.r3) //make sure for release
            {
                RS2[pos].busy = false; //change to non-busy
                break;
            }
        }

        for (size_t i = 0; i < RS1.size(); i++) //WB to ADD RS
        {
            if (RS1[i].r2 == rs2Buffer.rs && RS1[i].rs2 == true) //for r2
            {
                RS1[i].r2 = result;
                RS1[i].rs2 = false; //change to non-wait
            }

            if (RS1[i].r3 == rs2Buffer.rs && RS1[i].rs3 == true) //for r3
            {
                RS1[i].r3 = result;
                RS1[i].rs3 = false; //change to non-wait
            }
        }

        for (size_t i = 0; i < RS2.size(); i++) //WB to MUL & DIV RS
        {
            if (RS2[i].r2 == rs2Buffer.rs && RS2[i].rs2 == true) //for r2
            {
                RS2[i].r2 = result;
                RS2[i].rs2 = false; //change to non-wait
            }

            if (RS2[i].r3 == rs2Buffer.rs && RS2[i].rs3 == true) //for r3
            {
                RS2[i].r3 = result;
                RS2[i].rs3 = false; //change to non-wait
            }
        }

        if (RAT[rs2Buffer.rd - 1] == pos + 4) //check RAT position
        {
            RAT[rs2Buffer.rd - 1] = -1; //release RAT
        }
        RF[rs2Buffer.rd - 1] = result; //WB to RF

        rs2Buffer.operation = 0; //initialize MUL & DIV RS
        rs2Buffer.rd = 0;
        rs2Buffer.r2 = 0;
        rs2Buffer.r3 = 0;
        rs2Buffer.rs = 0;
        rs2Buffer.cycWB = 0;
        rs2Buffer.empty = true;

        doneInst--; //done for one instruction
    }    
}

void print()
{
    if (needToPrint)
    {
        cout << endl
             << "Cycle: " << CYCLE << endl
             << endl;

        cout << "   ---RF---" << endl;
        for (size_t i = 0; i < 5; i++)
        {
            cout << "F" << i + 1 << " |" << setw(5) << RF[i] << " |" << endl;
        }
        cout << "   --------" << endl
             << endl;

        cout << "   ---RAT---" << endl;
        for (size_t i = 0; i < 5; i++)
        {
            if (RAT[i] == -1)
            {
                cout << "F" << i + 1 << " |" << setw(6) << " "
                     << " |" << endl;
            }
            else
            {
                cout << "F" << i + 1 << " |"
                     << "   RS" << RAT[i] << " |" << endl;
            }
        }
        cout << "   ---------" << endl
             << endl;

        cout << "    --------RS---------" << endl;
        for (size_t i = 0; i < 3; i++)
        {
            cout << "RS" << i + 1 << " |";
            if (RS1.size() == 0)
            {
                cout << "     |     |     |" << endl;
            }
            else
            {
                if (RS1[i].busy == false)
                {
                    cout << "     |     |     |" << endl;
                }
                else
                {
                    if (RS1[i].operation == 3)
                    {
                        cout << setw(4) << "-";
                    }
                    else
                    {
                        cout << setw(4) << "+";
                    }

                    cout << " |";

                    if (RS1[i].rs2 == true)
                    {
                        cout << " RS" << RS1[i].r2;
                    }
                    else
                    {
                        cout << setw(4) << RS1[i].r2;
                    }

                    cout << " |";

                    if (RS1[i].rs3 == true)
                    {
                        cout << " RS" << RS1[i].r3;
                    }
                    else
                    {
                        cout << setw(4) << RS1[i].r3;
                    }

                    cout << " |" << endl;
                }
            }
        }
        cout << "    -------------------" << endl;

        cout << "BUFFER: ";
        if (rs1Buffer.empty == true)
        {
            cout << "empty" << endl;
        }
        else
        {
            if (rs1Buffer.operation == 3)
            {
                cout << "(RS" << rs1Buffer.rs << ") " << rs1Buffer.r2 << " - " << rs1Buffer.r3 << endl;
            }
            else
            {
                cout << "(RS" << rs1Buffer.rs << ") " << rs1Buffer.r2 << " + " << rs1Buffer.r3 << endl;
            }
        }

        /***************************************************************************************/
        cout << "    -------------------" << endl;
        for (size_t i = 0; i < 2; i++)
        {
            cout << "RS" << i + 4 << " |";
            if (RS2.size() == 0)
            {
                cout << "     |     |     |" << endl;
            }
            else
            {
                if (RS2[i].busy == false)
                {
                    cout << "     |     |     |" << endl;
                }
                else
                {
                    if (RS2[i].operation == 4)
                    {
                        cout << setw(4) << "*";
                    }
                    else
                    {
                        cout << setw(4) << "/";
                    }

                    cout << " |";

                    if (RS2[i].rs2 == true)
                    {
                        cout << " RS" << RS2[i].r2;
                    }
                    else
                    {
                        cout << setw(4) << RS2[i].r2;
                    }

                    cout << " |";

                    if (RS2[i].rs3 == true)
                    {
                        cout << " RS" << RS2[i].r3;
                    }
                    else
                    {
                        cout << setw(4) << RS2[i].r3;
                    }

                    cout << " |" << endl;
                }
            }
        }
        cout << "    -------------------" << endl;
        cout << "BUFFER: ";
        if (rs2Buffer.empty == true)
        {
            cout << "empty" << endl;
        }
        else
        {
            if (rs2Buffer.operation == 4)
            {
                cout << "(RS" << rs2Buffer.rs << ") " << rs2Buffer.r2 << " * " << rs2Buffer.r3 << endl;
            }
            else
            {
                cout << "(RS" << rs2Buffer.rs << ") " << rs2Buffer.r2 << " / " << rs2Buffer.r3 << endl;
            }
        }

        needToPrint = false;
    }

    CYCLE++;
}

int main()
{
    ifstream inputFile("input.txt", ios::in);
    stringstream inputSS;
    string inputStr;

    if (!inputFile) //check file
    {
        cout << "File error." << endl;
        return -1;
    }

    cout << "Cycle of ADD: "; //set cycle
    cin >> cycOfADD;
    cout << "Cycle of MUL: ";
    cin >> cycOfMUL;
    cout << "Cycle of DIV: ";
    cin >> cycOfDIV;
    

    while (getline(inputFile, inputStr))
    {
        Instruction inputInst = {-1, -1, -1, -1};

        inputSS << inputStr;
        inputSS >> inputStr;

        if (inputStr == "ADDI") //generate operation
        {
            inputInst.operation = ADDI;
        }
        else if (inputStr == "ADD")
        {
            inputInst.operation = ADD;
        }
        else if (inputStr == "SUB")
        {
            inputInst.operation = SUB;
        }
        else if (inputStr == "MUL")
        {
            inputInst.operation = MUL;
        }
        else if (inputStr == "DIV")
        {
            inputInst.operation = DIV;
        }

        for (size_t i = 0; getline(inputSS, inputStr, ','); i++) //generate register
        {
            inputStr.erase(0, inputStr.find_first_not_of(" ")); //avoid space
            if (inputStr[0] == 'F') //erase F
            {
                inputStr.erase(0, 1);
            }

            if (i == 0)
            {
                inputInst.rd = stoi(inputStr); //rd
            }
            else if (i == 1)
            {
                inputInst.r2 = stoi(inputStr); //r2
            }
            else if (i == 2)
            {
                inputInst.r3 = stoi(inputStr); //r3
            }
        }

        doneInst++;
        Instructions.push(inputInst); //push to queue
        inputSS.clear();
        inputStr.clear();
    }

    while (doneInst != 0) //stop until all instructions is write back
    {
        issue();
        execute();
        writeBack();
        print();
    }
}