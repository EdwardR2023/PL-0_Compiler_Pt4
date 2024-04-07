//Alan Pons
//Edward Rosales
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_IDENT_LEN 11
#define MAX_NUM_LEN 5
#define MAX_CODE_LEN 1000
#define MAX_STR_LEN 256

// Token types
enum {
    oddsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6, slashsym = 7, fisym = 8, eqlsym = 9, neqsym = 10, lessym = 11,
    leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26
    , callsym = 27, constsym = 28, varsym = 29 , procsym = 30, writesym = 31,
    readsym = 32 /*, elsesym = 33*/
};


// Token structure
typedef struct {
    int type;
    char value[MAX_STR_LEN];
} Token;

// Array to store tokens
Token tokens[MAX_CODE_LEN];
int token_count = 0;

//Symbol structure
typedef struct {
    int kind; // const = 1, var = 2, proc = 3
    char name[MAX_IDENT_LEN]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} Symbol;

//Array to store symbols (for parser)
Symbol symbols[MAX_CODE_LEN];
int symbol_count = 0;

//Store OP code commands
int opCode[MAX_CODE_LEN];
int opIndex = 0;


//Boolean is true while inside a comment
//initialized as false
int isComment = 0;

//Period is found, halt program
int halt = 0;

//Increment through token list for parsing
int parserCount = 0;

//Total value to update a symbol's value
int total = 0;

//lexographic level
int lexLevel = -1;

// Function to check if a word is a keyword and return its token type
int is_keyword(char *word) {

    if (strcmp(word, "odd") == 0) return oddsym;
    if (strcmp(word, "begin") == 0) return beginsym;
    if (strcmp(word, "end") == 0) return endsym;
    if (strcmp(word, "if") == 0) return ifsym;
    if (strcmp(word, "fi") == 0) return fisym;
    if (strcmp(word, "then") == 0) return thensym;
    if (strcmp(word, "while") == 0) return whilesym;
    if (strcmp(word, "do") == 0) return dosym;
    if (strcmp(word, "call") == 0) return callsym;
    if (strcmp(word, "const") == 0) return constsym;
    if (strcmp(word, "var") == 0) return varsym;
    if (strcmp(word, "procedure") == 0) return procsym;
    if (strcmp(word, "write") == 0) return writesym;
    if (strcmp(word, "read") == 0) return readsym;
    //if (strcmp(word, "else") == 0) return elsesym;
    return identsym;

    // If not a keyword, consider it an identifier
}

//Function used to check for valid symbols and returns its token type
int is_symbol(char *word) {
    if (strcmp(word, "+") == 0) return plussym;
    if (strcmp(word, "-") == 0) return minussym;
    if (strcmp(word, "*") == 0) return multsym;
    if (strcmp(word, "/") == 0) return slashsym;
    if (strcmp(word, "=") == 0) return eqlsym;
    if (strcmp(word, "<>") == 0) return neqsym;
    if (strcmp(word, "<") == 0) return lessym;
    if (strcmp(word, "<=") == 0) return leqsym;
    if (strcmp(word, ">") == 0) return gtrsym;
    if (strcmp(word, ">=") == 0) return geqsym;
    if (strcmp(word, "(") == 0) return lparentsym;
    if (strcmp(word, ")") == 0) return rparentsym;
    if (strcmp(word, ",") == 0) return commasym;
    if (strcmp(word, ";") == 0) return semicolonsym;
    if (strcmp(word, ".") == 0) return periodsym;
    if (strcmp(word, ":=") == 0) return becomessym;
    return -1;
}

//adds into token array
void add_token(int type, char *value) {
    tokens[token_count].type = type;
    strcpy(tokens[token_count].value, value);
    token_count++;
}

void tokenize_line(char *line) {

    int lineCounter = 0, wordLen = 0;
    char c;

    if (!isComment)
        c = line[lineCounter++];

    //deals with string length of 1 (last line of ta case)
    if (strlen(line) < 2) {
        c = line[0];
        if (isalpha(c)) {
            add_token(is_keyword(line), line);
        } else if (isdigit(c)) {
            add_token(numbersym, line);
        } else if (!isspace(c)) {
            add_token(is_symbol(line), line);
        }
        return;
    }

    //loops to the end of the file
    while ((isComment || c != '\0')) {

        char word[MAX_STR_LEN] = {0};

        // Check for comments
        if (isComment == 1) {
            lineCounter++;
            // Move past the opening comment delimiter
            while (line[lineCounter] != '\0' && isComment) {
                if (line[lineCounter - 1] == '*' && line[lineCounter] == '/') {

                    // Reset comment flag
                    isComment = 0;
                }
                lineCounter++;
            }
            c = line[lineCounter];
            // Ignore characters inside comments
            if (isComment) {
                return; // Skip further processing for this line
            }
        }

            //Keyword/Identifier loop
        else if (isalpha(c) && !isComment) {

            while (isalpha(c) || isdigit(c)) {

                word[wordLen++] = c;
                c = line[lineCounter++];
            }

            if (strlen(word) <= MAX_IDENT_LEN)
                add_token(is_keyword(word), word);

            else
                add_token(-2, word);


        }

            //Number loop
        else if (isdigit(c)) {

            while (isdigit(c)) {

                word[wordLen++] = c;
                c = line[lineCounter++];
            }
            if (strlen(word) <= MAX_NUM_LEN)
                add_token(numbersym, word);

            else
                add_token(-3, word);

        }

            //Symbol check
        else if (!isspace(c)) {

            //while a symbol and not null
            while (!isspace(c) && !isalpha(c) && !isdigit(c) && !isComment && c != '\0') {
                //opening comment check
                if (line[lineCounter - 1] == '/' && line[lineCounter] == '*') {
                    isComment = 1;

                }
                    //not a comment therefore a symbols(s) that will later be checked
                else {
                    word[wordLen++] = c;
                    c = line[lineCounter++];
                }
            }
            //check for symbol in code that are no comments
            if (!isComment) {
                int symType, i = 0;
                char sym[2];

                //separates invalid symbols from valid string
                if (is_symbol(word) == -1) {
                    while (i < wordLen) {
                        sym[0] = word[i];

                        i++;
                        symType = is_symbol(sym);
                        add_token(symType, sym);

                        if (symType == periodsym) {
                            halt = 1;
                            return;
                        }

                    }

                } else {
                    symType = is_symbol(word);
                    add_token(symType, word);

                    if (symType == periodsym) {
                        halt = 1;
                        return;
                    }

                }
            }
                //currently in a comment, so we just increment through it and ignore it
            else {
                c = line[lineCounter++];
            }


        } else {    //a white space was encountered
            c = line[lineCounter++];
        }

        wordLen = 0;

    }

}

//function to run through our table of variables and constants
int symbolTableChecker(char *name, int startInd) {
    for (int i = startInd ; i >= 0; i--) {
        if (strcmp(symbols[i].name, name) == 0)
            return i;
    }
    return -1;
}

//function that adds newly intialized vairables to our table
void addToSymbolTable(int kind, char *name, int num, int level, int addr, int mark) {

    symbols[symbol_count].kind = kind;
    strcpy(symbols[symbol_count].name, name);
    symbols[symbol_count].val = num;
    symbols[symbol_count].level = level;
    symbols[symbol_count].addr = addr;
    symbols[symbol_count].mark = mark;
    symbol_count++;
}

//function to print the table
void printSymbolTable() {

    printf("\nSymbol Table:\n");
    printf("Kind\t| Name\t\t| Value\t| Level\t| Address\t| Mark\n");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < symbol_count; i++) {
        Symbol cur = symbols[i];
        printf("%d\t| %s\t\t| %d\t| %d\t| %d\t\t| %d\n", cur.kind, cur.name, cur.val, cur.level, cur.addr, cur.mark);

    }
}

//function to all OPCODE generated
void printOpCode() {
    printf("LINE\t OP\tL\tM\n");
    printf("----------------------------");
    for (int i = 0; i < opIndex; i++) {
        if (i % 3 == 0) {
            printf("\n%d\t", i / 3);
            switch (opCode[i]) {
                case 1:
                    printf("LIT\t");
                    break;

                case 2:
                    switch(opCode[i+2]){
                        case 0:
                        printf("RTN\t");
                        break;
                        case 1:
                            printf("ADD\t");
                            break;
                        case 2:
                            printf("SUB\t");
                            break;
                        case 3:
                            printf("MUL\t");
                            break;
                        case 4:
                            printf("DIV\t");
                            break;
                        case 5:
                            printf("EQL\t");
                            break;
                        case 6:
                            printf("NEQ\t");
                            break;
                        case 7:
                            printf("LSS\t");
                            break;
                        case 8:
                            printf("LEQ\t");
                            break;
                        case 9:
                            printf("GTR\t");
                            break;
                        case 10:
                            printf("GEQ\t");
                            break;
                        case 11:
                            printf("ODD\t");
                            break;
                    }
                    break;

                case 3:
                    printf("LOD\t");

                    break;

                case 4:
                    printf("STO\t");

                    break;

                case 5:
                    printf("CAL\t");

                    break;

                case 6:
                    printf("INC\t");

                    break;

                case 7:
                    printf("JMP\t");
                    break;

                case 8:
                    printf("JPC\t");
                    break;

                case 9:
                    printf("SYS\t");
                    break;

                default:
                    break;
            }

        } else
            printf("%d\t", opCode[i]);
    }
    printf("\n");
}

void printOpCodeFile() {
    FILE *fp;
    fp = fopen("elf.txt", "w");
    for (int i = 0; i < opIndex; i++) {

        if (i % 3 == 0 && i != 0) {
            fprintf(fp,"\n");
        }
        fprintf(fp,"%d ", opCode[i]);
    }
}
//function that converts chars to numbers
int numConvert(char *val) {

    int len = strlen(val);
    int number = 0;
    int pow = 1;

    for (int i = len - 1; i >= 0; i--) {
        number += ((int) val[i] - 48) * pow;
        pow *= 10;
    }

    return number;
}

//function prototpyes
int constDeclaration();

int varDeclaration();

int procDeclaration();

int Expression();

void Statement();

int Condition();

int Factor();

int Term();

void Block();
//function to created opcode
void emit(int op, int l, int m) { //used to write opcode

    opCode[opIndex] = op;
    opCode[opIndex + 1] = l;
    opCode[opIndex + 2] = m;
    opIndex += 3;

}

int constDeclaration() {

    //grabs current token
    Token curToken = tokens[parserCount];

    //checks if curtoken is a constant
    if (curToken.type == constsym) {
        do {
            curToken = tokens[++parserCount];


            //if curtoken is an identifier we print error and exit
            if (curToken.type != identsym) {
                printf("\nERROR: const, var, and read keywords must be followed by identifier\n");

                exit(-1);
            }
            //if curtoken is an identifier already declared print error and exit
            int temp = symbolTableChecker(curToken.value, symbol_count-1);

            if (temp != -1 && symbols[temp].level == lexLevel)  {
                printf("\nERROR: symbol name has already been declared\n");
                exit(-1);
            }


            char identName[MAX_IDENT_LEN+1];

            //copies the string of the current token(variable or constant) to the
            strcpy(identName, curToken.value);

            curToken = tokens[++parserCount];


            if (curToken.type != eqlsym) {
                printf("\nERROR: constants must be assigned with =\n");
                exit(-1);
            }

            //next token
            curToken = tokens[++parserCount];

            //error if the token is a number
            if (curToken.type != numbersym) {
                printf("\nERROR: constants must be assigned with an integer value\n");
                exit(-1);
            }

            //runs the function to convert chars into numbers
            int num = numConvert(curToken.value);

            //adds symbol to the table and assigns its attributes
            addToSymbolTable(1, identName, num, lexLevel, 0, 0);

            //next token
            curToken = tokens[++parserCount];


        } while (curToken.type == commasym);

        if (curToken.type != semicolonsym) {
            printf("\nERROR: constant and variable declarations must be followed by a semicolon\n");
            exit(-1);
        }
        parserCount++;

    }

    return 0;
}

int varDeclaration() {

    int numVar = 0;
    Token curToken = tokens[parserCount];

    if (curToken.type == varsym) {
        do {
            numVar++;
            curToken = tokens[++parserCount];

           //if the current token is not an identifier we print error and return
            if (curToken.type != identsym) {
                printf("ERROR: const, var, and read keywords must be followed by identifier\n");
                exit(-2);
            }

            //if curtoken is an identifier already declared print error and exit
            int temp = symbolTableChecker(curToken.value, symbol_count-1);

            if (temp != -1 && symbols[temp].level == lexLevel) {
                printf("\nERROR: symbol name has already been declared\n");
                exit(-2);
            }

            char identName[MAX_IDENT_LEN+1];
            //copies the variable string
            strcpy(identName, curToken.value);

            //adds the variable to the symbol table with assigned attributes
            addToSymbolTable(2, identName, 0, lexLevel, numVar + 2, 0);

            curToken = tokens[++parserCount];



        } while (curToken.type == commasym);

        if (curToken.type != semicolonsym) {
            printf("\nERROR: constant and variable declarations must be followed by a semicolon\n");
            exit(-2);
        }
        //next token
        ++parserCount;
    }
    //the total number of variables that were initalized
    return numVar;
}

int procDeclaration(){
    Token curToken = tokens[parserCount];
    while (curToken.type == procsym){
        curToken = tokens[++parserCount];
        if (curToken.type != identsym){
            printf("ERROR: procedure must be followed by an identifier\n");
            exit(-6);
        }

        addToSymbolTable(3,curToken.value,0,lexLevel,opIndex,0);
        curToken = tokens[++parserCount];
        if (curToken.type != semicolonsym){
            printf("ERROR: semicolon missing.\n");
            exit(-6);
        }
        ++parserCount;

        Block();
        emit(2,0,0);
        curToken = tokens[parserCount];
        if (curToken.type != semicolonsym){
            printf("ERROR: semicolon missing \n");
            exit(-6);
        }
        curToken = tokens[++parserCount];

    }
    return 0;
}
int Expression() {
    Token curToken;

    Term();

    //current token
    curToken = tokens[parserCount];

    //if the current token is a plus or minus
    while (curToken.type == plussym || curToken.type == minussym) {
        if (curToken.type == plussym) {

            ++parserCount;
            Term();
            curToken = tokens[parserCount];

            //emits add
            emit(2, 0, 1);
        } else {

            ++parserCount;
            Term();
            curToken = tokens[parserCount];

            //emits sub
            emit(2, 0, 2);
        }
    }
    return 0;
}

int Term() {

    Factor();
    //updates curToken to current token
    Token curToken = tokens[parserCount];

    //if the current token is * or /
    while (curToken.type == multsym || curToken.type == slashsym) {

        if (curToken.type == multsym) {

            ++parserCount;
            Factor();
            curToken = tokens[parserCount];

            //emit multiplication
            emit(2, 0, 3);
        } else {

            ++parserCount;
            Factor();
            curToken = tokens[parserCount];

            //emit division
            emit(2, 0, 4);
        }

    }
    return 0;
}

int Factor() {
    int symIdx;
    Token curToken = tokens[parserCount];

    if (curToken.type == identsym) {
        symIdx = symbolTableChecker(curToken.value, symbol_count-1);

        //identifier is undeclared
        if (symIdx == -1) {
            printf("ERROR: Undeclared Identifier\n");
            exit(-3);
        }

        if(symbols[symIdx].kind == 3){ //procedure

            printf("ERROR: expression must not contain a procedure identifier");
            exit(-3);
        }

        if(symbols[symIdx].kind == 1) {//const

            //emits literal
            emit(1, 0, symbols[symIdx].val);
            symbols[symIdx].mark = 1;
        } else { // variable

            //emits load
            emit(3, lexLevel-symbols[symIdx].level, symbols[symIdx].addr);
            symbols[symIdx].mark = 1;

        }
        //next token
        ++parserCount;

        //token is a number
    } else if (curToken.type == numbersym) {

        int number;
        //converts char array into number
        number = numConvert(curToken.value);

        //emit lit
        emit(1, 0, number);
        //next token
        ++parserCount;

        //left parentheses
    } else if (curToken.type == lparentsym) {
        ++parserCount;
        Expression();
        curToken = tokens[parserCount];

        //right parentheses not found
        if (curToken.type != rparentsym) {
            printf("ERROR: right parenthesis must follow left parenthesis\n");
            exit(-3);
        }
        //next token
        ++parserCount;

    } else {
        //following := no correct tokens found
        printf("ERROR: arithmetic equations must contain operands, parentheses, numbers, or\n"
               "symbols\n");
        exit(-3);
    }
    return 0;
}

void Statement() {
    Token curToken = tokens[parserCount];

    //if an identifier
    if (curToken.type == identsym) {
        int symidx = symbolTableChecker(curToken.value, symbol_count-1);

        //not found in symbol table
        if (symidx == -1) {
            printf("\nERROR: Undeclared Identifier \n");

            printf("%d\n", symbol_count);
            exit(-4);
        }

        //symbol was a constant and cant be modified thus print error and exit
        while (symbols[symidx].kind != 2) {
            symidx = symbolTableChecker(curToken.value, symidx-1);

            if(symidx == -1) {
                printf("\n ERROR: only variable values may be altered\n");
                exit(-4);
            }
        }

        //next token
        curToken = tokens[++parserCount];

        //become sym not found
        if (curToken.type != becomessym) {
            printf("\nERROR: assignment statements must use :=\n");
            exit(-4);
        }

        ++parserCount;
        total = 0;
        Expression();

        //emits store code
        emit(4, lexLevel - symbols[symidx].level, symbols[symidx].addr);
        symbols[symidx].val = total;
        symbols[symidx].mark = 1;

        return;

    }
    else if(curToken.type == callsym){
        curToken = tokens[++parserCount];
        if (curToken.type != identsym){
            printf("ERROR: call must be followed by an identifier\n");
            exit(-4);
        }
        int i = symbolTableChecker(curToken.value, symbol_count-1);


        if (i == -1){
            printf("ERROR: Undeclared Identifier");

            exit(-10);
        }
        if (symbols[i].kind == 3){
            emit(5,lexLevel-symbols[i].level,symbols[i].addr);
        }
        else{
            printf("ERROR: Call of a constant or variable is meaningless.");
            exit(-4);
        }
        curToken = tokens[++parserCount];

    }
    if (curToken.type == beginsym) {
        do {
            //next token
            ++parserCount;

            Statement();
            //updates curToken
            curToken = tokens[parserCount];

        } while (curToken.type == semicolonsym);
        if (curToken.type != endsym) {
            printf("ERROR: End expected");
            exit(-4);
        }
        //next token
        ++parserCount;
        return;
    }
    if (curToken.type == ifsym) {
        //next token
        ++parserCount;

        Condition();
        //updates curToken
        curToken = tokens[parserCount];

        //holds current opcode index
        int JpcInx = opIndex;

        //emit JPC
        emit(8, 0, JpcInx);

        //then not found
        if (curToken.type != thensym) {
            printf("ERROR: if must be followed by then\n");
            exit(-4);
        }

        //next token
        ++parserCount;

        Statement();
        //updates curtoken
        curToken = tokens[parserCount];

        if (curToken.type != fisym) {
            printf("ERROR: then must be followed by fi\n");
            exit(-4);
        }
        //next token
        ++parserCount;

        //JpcInx +2 is in the M spot of the opCode command
        opCode[JpcInx + 2] = opIndex;
        return;

    }
    if (curToken.type == whilesym) {
        //next token
        ++parserCount;

        //hold current index value
        int loopIdx = opIndex;

        Condition();
        curToken = tokens[parserCount];
        //do not found
        if (curToken.type != dosym) {
            printf("ERROR: while must be followed by do\n");
            exit(-4);
        }
        //next token
        ++parserCount;

        int JpcInx = opIndex;

        //emit JPC
        emit(8, 0, JpcInx);

        Statement();

        //emit jpc
        emit(8, 0, loopIdx);

        //+2 for the m spot of the opCode
        opCode[JpcInx + 2] = opIndex;
        return;
    }
    if (curToken.type == readsym) {
        curToken = tokens[++parserCount];
        if (curToken.type != identsym) {
            printf("ERROR: read must be followed by an identifier\n");
            exit(-4);
        }
        int symidx = symbolTableChecker(curToken.value, symbol_count-1);

        //identifier not found
        if (symidx == -1) {
            printf("ERROR: Identifier not found\n");
            exit(-4);
        }

        //constant therefore not valid print error and exit
        if (symbols[symidx].kind != 2) {
            printf("ERROR: Not a variable\n");
            exit(-4);
        }
        //next token
        ++parserCount;

        //emit read
        emit(9, 0, 2);

        //emit store
        emit(4, lexLevel - symbols[symidx].level, symbols[symidx].addr);
        return;
    }
    if (curToken.type == writesym) {
        //next token
        ++parserCount;
        Expression();
        //emit write
        emit(9, 0, 1);
        return;
    }


    return;
}

int Condition() {
    Token curToken = tokens[parserCount];
    if (curToken.type == oddsym) {
        ++parserCount;
        Expression();
        //emit odd
        emit(2, 0, 11);
    } else {
        Expression();
        curToken = tokens[parserCount];
        if (curToken.type == eqlsym) {
            ++parserCount;
            Expression();

            //emit EQL
            emit(2, 0, 5);
        } else if (curToken.type == neqsym) {
            ++parserCount;
            Expression();

            //emit NEQ
            emit(2, 0, 6);

        } else if (curToken.type == lessym) {
            ++parserCount;
            Expression();

            //emit LLS
            emit(2, 0, 7);

        } else if (curToken.type == leqsym) {
            ++parserCount;
            Expression();

            //emit LEQ
            emit(2, 0, 8);
        } else if (curToken.type == gtrsym) {
            ++parserCount;
            Expression();

            //emit GTR
            emit(2, 0, 9);
        } else if (curToken.type == geqsym) {
            ++parserCount;
            Expression();

            //emit GEQ
            emit(2, 0, 10);
        } else {
            printf("ERROR: condition must contain comparison operator\n");
            exit(-5);
        }
    }
    return 0;
}

void Block(){

    lexLevel++;
    int prev_sx = symbol_count;
    //this needs to change
    int jmpaddr = opIndex+2;
    emit(7, 0, -10);
    //jmpaddr = gen(JMP, 0, 666);

    int numVar;

    constDeclaration();

    numVar = varDeclaration();

    procDeclaration();

    //code[jmpaddr].addr = next_code_addr
    opCode[jmpaddr] = opIndex;

    //emit inc
    emit(6, 0, numVar + 3);

    Statement();
    symbol_count = prev_sx;
    lexLevel--;
}
void parser() {

    Block();

    if (tokens[token_count - 1].type != periodsym) {
        printf("\nERROR: program must end with period\n");
        exit(-5);
    }
    //emits halt(code ended correctly with no errors)
    emit(9, 0, 3);
}

// Main function
int main(int argc, char *argv[]) {
    // Check if a file name is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    // Open input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file.\n");
        return 1;
    }

    char line[MAX_CODE_LEN];
    //printf("SOURCE PROGRAM:\n");
    while (fgets(line, sizeof(line), file) && halt == 0) {
        printf("%s", line);
        tokenize_line(line);
    }
    printf("\n\n");
    //printf("\n\nLEXEME TABLE\n\nLexeme\t\t\tToken Type\n");
     for (int i = 0; i < token_count; i++) {
         if (tokens[i].type == -1) {
             printf("ERROR: INVALID SYMBOL '%s'\n", tokens[i].value);
             return 0;
         } else if (tokens[i].type == -2) {
             printf("ERROR: IDENTIFIER IS TOO LONG '%s'\n", tokens[i].value);
             return 0;
         } else if (tokens[i].type == -3) {
             printf("ERROR: NUMBER IS TOO LONG '%s'\n", tokens[i].value);
             return 0;
         }

     }

    /* printf("\nTOKEN LIST\n");
     for (int i = 0; i < token_count; i++) {

         if (tokens[i].type > 0)
             printf("%d ", tokens[i].type);

         if (tokens[i].type == 2 || tokens[i].type == 3)
             printf("%s ", tokens[i].value);


     }*/

    fclose(file);

    parser();
    printf("\n\nNO ERRORS, PROGRAM IS SYNTACTICALLY CORRECT\n\n");
    printOpCode();
    printOpCodeFile();


    return 0;
}