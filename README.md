# Tomasulo algorithm

## 流程介紹
   1. 從input.txt讀入欲執行的instructions
   2. 設定ADD、MUL、DIV須執行的cycle數
   3. 對每個instruction以FIFO的方式做Issue、Execute、Write Back
   4. 執行到Write back數量與instruction數量相同即停止
  
## 變數說明
+ `struct Instruction` 含有operation、rd、r2、r3 
+ `struct RS` 含有operation、rd、r2、r3、rs2、rs3、cycEXE、busy
+ `struct Buffer` 含有operation、rs、rd、r2、r3、empty、cycWB
+ `int doneInst` 紀錄剩下多少instruction須執行
+ `int RF[5]` Register File
+ `int RAT[5]` Register Allocation Table
+ `int cycOfADD, cycOfMUL, cycOfDIV, CYCLE` ADD、MUL、DIV須執行的cycle數與目前的CYCLE
+ `queue<Instruction> Instructions` 存放讀入的instruction
+ `vector<RS> RS1(3, RS())` ADD & SUB RS
+ `vector<RS> RS2(2, RS())` MUL & DIV RS
+ `Buffer rs1Buffer` ADD & SUB Buffer
+ `Buffer rs2Buffer` MUL & DIV Buffer

## Input Sample
      2
      10
      40
      
## Output Sample
      Cycle: 1

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS1 |
      F2 |       |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   2 |   1 |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |     |     |     |
      RS5 |     |     |     |
          -------------------
      BUFFER: empty

      Cycle: 2

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS2 |
      F2 |       |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   2 |   1 |
      RS2 |   - |   4 |   6 |
      RS3 |     |     |     |
          -------------------
      BUFFER: (RS1) 2 + 1
          -------------------
      RS4 |     |     |     |
      RS5 |     |     |     |
          -------------------
      BUFFER: empty

      Cycle: 3

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS4 |
      F2 |       |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   2 |   1 |
      RS2 |   - |   4 |   6 |
      RS3 |     |     |     |
          -------------------
      BUFFER: (RS1) 2 + 1
          -------------------
      RS4 |   / |   2 |   4 |
      RS5 |     |     |     |
          -------------------
      BUFFER: empty

      Cycle: 4

         ---RF---
      F1 |    3 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS4 |
      F2 |   RS5 |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |     |     |     |
      RS2 |   - |   4 |   6 |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   / |   2 |   4 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS4) 2 / 4

      Cycle: 5

         ---RF---
      F1 |    3 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS4 |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |   - |   4 |   6 |
      RS3 |     |     |     |
          -------------------
      BUFFER: (RS2) 4 - 6
          -------------------
      RS4 |   / |   2 |   4 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS4) 2 / 4

      Cycle: 6

         ---RF---
      F1 |    3 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS4 |
      F2 |   RS1 |
      F3 |       |
      F4 |   RS3 |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |   - |   4 |   6 |
      RS3 |   + | RS4 |   2 |
          -------------------
      BUFFER: (RS2) 4 - 6
          -------------------
      RS4 |   / |   2 |   4 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS4) 2 / 4

      Cycle: 7

         ---RF---
      F1 |   -2 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS4 |
      F2 |   RS1 |
      F3 |       |
      F4 |   RS3 |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |     |     |     |
      RS3 |   + | RS4 |   2 |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   / |   2 |   4 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS4) 2 / 4

      Cycle: 44

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |   RS1 |
      F3 |       |
      F4 |   RS3 |
      F5 |       |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |     |     |     |
      RS3 |   + |   0 |   2 |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |     |     |     |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: empty

      Cycle: 45

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |   RS1 |
      F3 |       |
      F4 |   RS3 |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |     |     |     |
      RS3 |   + |   0 |   2 |
          -------------------
      BUFFER: (RS3) 0 + 2
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS5) 4 * 6

      Cycle: 46

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    6 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS2 |
      F2 |   RS1 |
      F3 |       |
      F4 |   RS3 |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |   + | RS3 | RS3 |
      RS3 |   + |   0 |   2 |
          -------------------
      BUFFER: (RS3) 0 + 2
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS5) 4 * 6

      Cycle: 47

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS2 |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |   + |   2 |   2 |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS5) 4 * 6

      Cycle: 48

         ---RF---
      F1 |    0 |
      F2 |    2 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |   RS2 |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |   + |   2 |   2 |
      RS3 |     |     |     |
          -------------------
      BUFFER: (RS2) 2 + 2
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS5) 4 * 6

      Cycle: 50

         ---RF---
      F1 |    4 |
      F2 |    2 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 | RS5 |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |   * |   4 |   6 |
          -------------------
      BUFFER: (RS5) 4 * 6

      Cycle: 55

         ---RF---
      F1 |    4 |
      F2 |   24 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 |  24 |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |     |     |     |
          -------------------
      BUFFER: empty

      Cycle: 56

         ---RF---
      F1 |    4 |
      F2 |   24 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |   RS1 |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |   + |   6 |  24 |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: (RS1) 6 + 24
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |     |     |     |
          -------------------
      BUFFER: (RS4) 8 * 8

      Cycle: 58

         ---RF---
      F1 |    4 |
      F2 |   30 |
      F3 |    4 |
      F4 |    2 |
      F5 |    8 |
         --------

         ---RAT---
      F1 |       |
      F2 |       |
      F3 |       |
      F4 |       |
      F5 |   RS4 |
         ---------

          --------RS---------
      RS1 |     |     |     |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |   * |   8 |   8 |
      RS5 |     |     |     |
          -------------------
      BUFFER: (RS4) 8 * 8

      Cycle: 66

         ---RF---
      F1 |    4 |
      F2 |   30 |
      F3 |    4 |
      F4 |    2 |
      F5 |   64 |
         --------

         ---RAT---
      F1 |       |
      F2 |       |
      F3 |       |
      F4 |       |
      F5 |       |
         ---------

          --------RS---------
      RS1 |     |     |     |
      RS2 |     |     |     |
      RS3 |     |     |     |
          -------------------
      BUFFER: empty
          -------------------
      RS4 |     |     |     |
      RS5 |     |     |     |
          -------------------
      BUFFER: empty

      
## 程式內容
  讀入ADD、MUL、DIV須執行的cycle數與input.txt裡的instructions存入queue
  
```c++
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
```
---
  Issue: 放進相對應的RS並且更新RAT

```C++
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

                        break;
                    }
                }
            }
        }
    }
}
```
---
  Execute: 判斷Buffer狀態，從RS中選出已ready且可execute的instruction，放入Buffer準備執行

```C++
void execute()
{
    if (rs1Buffer.empty) //check if ADD budder empty 
    {
        for (size_t i = 0; i < RS1.size(); i++)
        {
            if (RS1[i].rs2 == false && RS1[i].rs3 == false && RS1[i].cycEXE <= CYCLE && RS1[i].busy == true) //check for execute
            {
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
```
---
  Write Back: 確認是否執行完成，若完成需write back回兩個RS中需要此result的instruction，根據RAT更新RF，然後release RS與RAT

```C++
void writeBack()
{
    int result;
    if (rs1Buffer.empty == false && rs1Buffer.cycWB <= CYCLE) //check for write back
    {
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
```
---
  print: 印出當前cycle、RF、RAT、RS、Buffer，當前cycle++

```C++
void print()
{
    cout << endl << "Cycle: " << CYCLE << endl << endl;

    cout << "   ---RF---" << endl;
    for (size_t i = 0; i < 5; i++)
    {
        cout << "F" << i + 1 << " |" << setw(5) << RF[i] << " |" << endl;
    }
    cout << "   --------" << endl << endl;

    cout << "   ---RAT---" << endl;
    for (size_t i = 0; i < 5; i++)
    {
        if (RAT[i] == -1)
        {
            cout << "F" << i + 1 << " |" << setw(6) << " " << " |" << endl;
        }
        else
        {
            cout << "F" << i + 1 << " |" << "   RS" << RAT[i] << " |" << endl;
        }
    }
    cout << "   ---------" << endl << endl;

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
    
    //MUL & DIV RS
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

    CYCLE++;
}
```
