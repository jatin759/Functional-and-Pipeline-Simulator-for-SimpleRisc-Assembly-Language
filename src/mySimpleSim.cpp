
/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name:
Developer's Email id:
Date:

*/

/* mySimpleSim.cpp
   Purpose of this file: implementation file for mySimpleSim
*/

//#include "mySimpleSim.h"
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

//Register file
static unsigned int R[16];
static int PC;
//flags
static int gt,eq;
//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
static unsigned int immx,btarget,branchPC;
static int isadd,issub,ismul,isdiv,ismod,iscmp,isand,isor,isnot,ismov,islsl,islsr,isasr,isnop,isld,isst,isbeq,isbgt,isub,iscall,isret;
static int isimme,isbtaken,iswb,isexit;

int alures,ldres,result,noi;
int cycle_number=1;
unsigned int Starting_point;

// it is used to set the reset values
//reset all registers and memory content to 0

void write_word(unsigned char *mem, unsigned int address, unsigned int data) {
  int *data_p;
  data_p = (int*) (mem + address);
  *data_p = data;
}

int read_word(unsigned char *mem, unsigned int address) {
  int *data;
  data =  (int*) (mem + address);
  return *data;
}


void reset_proc() {

    isadd=0;
    issub=0;
    ismul=0;
    isdiv=0;
    ismod=0;
    iscmp=0;
    isand=0;
    isor=0;
    isnot=0;
    ismov=0;
    islsl=0;
    islsr=0;
    isasr=0;
    isnop=0;
    isld=0;
    isst=0;
    isbeq=0;
    isbgt=0;
    isub=0;
    iscall=0;
    isret=0;

    isimme=0;
    isbtaken=0;
    iswb=0;
    isexit=0;
    for(int i=0;i<16;i++)
        R[i]=0;
    R[14]=3000;

}

//load_program_memory reads the input memory, and pupulates the instruction
// memory
void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
  }
  Starting_point=address+4;
  fclose(fp);
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }

  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
  fclose(fp);
}

//reads from the instruction memory and updates the instruction register
void fetch() {
    instruction_word=read_word(MEM,PC);
    printf("Fetch Unit:\n\n   Instruction : 0X%x\n",instruction_word);
    cout<<endl;
    //cout<<isbtaken<<endl;
}
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode() {
    unsigned int a,h,u;
    a=instruction_word & 0x07ffffff;
    a=a<<2;
    if(((a&0x10000000)>>28 )==1)
    {
        a=a|0xf0000000;
    }
    btarget=a+PC;

    unsigned int c;
    c=instruction_word & 0x0000ffff;
    u=(instruction_word>>16) & 0x00000001;
    h=(instruction_word>>17) & 0x00000001;
    if(u==0&&h==0)
        immx=c;
    else if(u==0 && h==1)
    {
        immx=c;
        immx=immx<<16;
    }
    else if(u==1 && h==0)
    {
        a=c>>15;
        a=a & 0x00000001;
        if(a==1)
        {
            immx=c|0xffff0000;
        }
        else
            immx=c;
    }
    if(isret==1)
        operand1=R[15];
    else
    {
        a=instruction_word>>18;
        a=a & 0x0000000f;
        operand1=R[a];
        //cout<<"R["<<a<<"]"<<endl;
    }

    if(isst==1)
    {
        a=instruction_word>>22;
        a=a & 0x0000000f;
        operand2=R[a];
    }
    else
    {
        a=instruction_word>>14;
        a=a & 0x0000000f;
        operand2=R[a];
        //cout<<"R["<<a<<"]"<<endl;
    }
    cout<<"\nDecode Unit : \n\n";
    cout<<"  Operand 1 : "<<operand1<<endl;
    cout<<"  Operand 2 : "<<operand2<<endl;
    cout<<endl;
}
//executes the ALU operation based on ALUop
void execute() {
    if(isret==0)
        branchPC=btarget;
    else
        branchPC=operand1;

    if(isub==1 || (isbeq ==1 && eq==1) || (isbgt == 1 && gt==1)){
        isbtaken=1;
    }
    else
        isbtaken=0;

    int a,b;

    a=operand1;
    if(isimme==1)
        b=immx;
    else
        b=operand2;

    //cout<<b<<endl;
    if(isadd==1)
        alures=a+b;
    else if(issub==1)
        alures=a-b;
    else if(iscmp==1)
    {
        gt=0;eq=0;
        if(a>b)
            gt=1;
        else if(a==b)
            eq=1;
    }
    else if(ismul==1)
        alures=a*b;
    else if(isdiv==1)
        alures=a/b;
    else if(ismod==1)
        alures=a%b;
    else if(islsl==1)
        alures=a<<b;
    else if(islsr==1)
        alures=a>>b;
    else if(isasr==1)
    {
        unsigned int p;
        int i;
        p=instruction_word >> 31;
        if(p==1)
        {
            for(i=1;i<=b;i++)
            {
                alures=a>>1;
                alures=alures | 0x80000000;
            }
        }
        else
            alures=a>>b;
    }
    else if(isor==1)
        alures=a|b;
    else if(isnot==1)
        alures=!b;
    else if(isand==1)
        alures=a&b;
    else if(ismov==1){
        alures=b;
    }

    cout<<"\nExecute Unit : \n\n";
    cout<<"  AluResult : "<<alures<<endl;
    cout<<"  isBranchTaken : "<<isbtaken<<endl;
    cout<< endl;
}
//perform the memory operation
void mem() {

    cout<<"\nMemory Unit : \n\n";
    if((alures>=Starting_point) && (alures <4000) ){
    if(isld==1)
    {
        ldres=read_word(MEM,immx+operand1);
        cout<<"  Address : "<<immx+operand1<<endl;
        cout<<"  Ld Result : "<<ldres<<endl;
        /*a=(unsigned int*)alures;
        b=*a;
        ldres=b;*/
    }
    else if(isst==1)
    {
        write_word(MEM,immx+operand1,operand2);
         cout<<"  Address : "<<immx+operand1<<endl;
         cout<<"  Data : "<<operand2<<endl;

       // a=(unsigned int *)alures;
        //*a=operand2;
    }
    }
    else if((isld==1)||(isst==1))
        cout<<"  Unavailable Memory :- Error\n";
    else
        cout<<"  No change\n";

        cout << endl;
}
//writes the results back to register file
void write_back() {

    cout<<"\nWrite Back unit : \n\n";
    unsigned int a;
    int result;
    if(iswb==1)
    {
        if(isld==0 && iscall==0)
            result=alures;
        else if(isld==1 && iscall==0)
            result=ldres;
        else if(isld==0 && iscall==1)
            result=PC+4;

        if(iscall==1){
            R[15]=result;
            cout<<"  Register Number : 15 \n";
            cout<<"  Result : "<<result<<endl;
        }
        else
        {
            a=instruction_word>>22;
            a=a & 0x0000000f;
            R[a]=result;
            cout<<"  Register Number : "<<a<<endl;
            cout<<"  Result : "<<result<<endl;
        }
    }
    else
        cout<<"  isWb = 0 , So Nothing to Write \n";

    cout<<endl;

}

// Control Unit

void cunit()
{
    cout <<"\nControl Unit :\n\n";
    unsigned int a;
    isadd=0;issub=0;ismul=0;isdiv=0;ismod=0;iscmp=0;isand=0;isor=0;isnot=0;ismov=0;islsl=0;
    islsr=0;isasr=0;isnop=0;isld=0;isst=0;isbeq=0;isbgt=0;isub=0;iscall=0;isret=0;iswb=0;isexit=0;
    a=instruction_word>>27;
    a=a&0x0000001f;
    if(a==0){
        isadd=1;
        iswb=1;
    }
    else if(a==1){
        issub=1;
        iswb=1;
    }
    else if(a==2){
        ismul=1;
        iswb=1;
    }
    else if(a==3){
        isdiv=1;
        iswb=1;
    }
    else if(a==4){
        ismod=1;
        iswb=1;
    }
    else if(a==5)
        iscmp=1;
    else if(a==6){
        isand=1;
        iswb=1;
    }
    else if(a==7){
        isor=1;
        iswb=1;
    }
    else if(a==8){
        isnot=1;
        iswb=1;
    }
    else if(a==9){
        ismov=1;
        iswb=1;
    }
    else if(a==10){
        islsl=1;
        iswb=1;
    }
    else if(a==11){
        islsr=1;
        iswb=1;
    }
    else if(a==12){
        isasr=1;
        iswb=1;
    }
    else if(a==13)
        isnop=1;
    else if(a==14){
        isld=1;
        isadd=1;
        iswb=1;
    }
    else if(a==15){
        isst=1;
        isadd=1;
    }
    else if(a==16)
        isbeq=1;
    else if(a==17)
        isbgt=1;
    else if(a==18)
        isub=1;
    else if(a==19){
        iscall=1;
        isub=1;
        iswb=1;
    }
    else if(a==20){
        isret=1;
        isub=1;
    }
    else if(a==31)
        isexit=1;

    a=instruction_word>>26;
    a=a & 0x00000001;
    if(a==1)
        isimme=1;
    else
        isimme=0;

        cout<<" isAdd : "<<isadd<<endl;
        cout<<" isSub : "<<issub<<endl;
        cout<<" isMul : "<<ismul<<endl;
        cout<<" isDiv : "<<isdiv<<endl;
        cout<<" isMod : "<<ismod<<endl;
        cout<<" isCmp : "<<iscmp<<endl;
        cout<<" isAnd : "<<isand<<endl;
        cout<<" isOr : "<<isor<<endl;
        cout<<" isNot : "<<isnot<<endl;
        cout<<" isMov : "<<ismov<<endl;
        cout<<" isLsl : "<<islsl<<endl;
        cout<<" isLsr : "<<islsr<<endl;
        cout<<" isAsr : "<<isasr<<endl;
        cout<<" isNop : "<<isnop<<endl;
        cout<<" isLd : "<<isld<<endl;
        cout<<" isSt : "<<isst<<endl;
        cout<<" isBeq : "<<isbeq<<endl;
        cout<<" isBgt : "<<isbgt<<endl;
        cout<<" isUb : "<<isub<<endl;
        cout<<" isCall : "<<iscall<<endl;
        cout<<" isRet : "<<isadd<<endl;
        cout<<" isWb : "<<iswb<<endl;
        cout<<" isImmediate : "<<isimme<<endl;
        cout<<" isExit : "<<isexit<<endl;
        cout<<endl;

}




/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for SimpleRISC Processor

Developer's Name:
Developer's Email id:
Date:

*/

/* main.cpp
   Purpose of this file: The file handles the input and output, and
   invokes the simulator
*/

//#include "mySimpleSim.h"
//#include <stdio.h>
//#include <stdlib.h>

void run_simplesim() {

  while(isexit==0) {
        cout<<"Cycle Number : "<<cycle_number<<endl;
        cout<<endl;
    fetch();
    cunit();
    if(isexit==0){
    decode();
    execute();
    mem();
    write_back();
    if(isbtaken==0)
        PC=PC+4;
    else
        PC=branchPC;
    }
  cycle_number++;

  cout<<"Value of PC : "<<PC<<endl;
  cout<<"\n ************************************************ \n";
  }
}


int main() {
  char* prog_mem_file;

    for(int i=0;i<4000;i++)
        MEM[i]=' ';
  //reset the processor
  reset_proc();
  //load the program memory
  load_program_memory("input.txt");

  //run the simulator
  run_simplesim();
 // cout<<R[0]<<" "<<R[1]<<" "<<R[2]<<" "<<R[3]<<" "<<R[4]<<" "<<R[5]<<" "<<R[6]<<" "<<R[7]<<" "<<endl;
 cout<<endl;
  for(int j=0;j<16;j++)
    cout<<"R["<<j<<"] : "<<R[j]<<endl;

  return 1;
}

