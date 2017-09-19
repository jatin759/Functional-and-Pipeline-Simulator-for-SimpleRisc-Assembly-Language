
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

class Pipeline
{
public:
    int pc_p;
    unsigned int instruction_p;
    unsigned int btarget_p;
    unsigned int a_p;
    unsigned int b_p;
    unsigned int op2_p;
    int isadd_p;int issub_p;int ismul_p;int isdiv_p;int ismod_p;int iscmp_p;
    int isand_p;int isor_p;int isnot_p;int ismov_p;int islsl_p;
    int islsr_p;int isasr_p;int isnop_p;int isld_p;int isst_p;int isbeq_p;
    int isbgt_p;int isub_p;int iscall_p;int isret_p;int iswb_p;int isexit_p;
    int isimme_p;
    int alures_p;
    int ldres_p;
    Pipeline()
    {
        pc_p=0;
        instruction_p=0X68000000;
        btarget_p=0;
        a_p=0;
        b_p=0;
        op2_p=0;
        alures_p=0;
        ldres_p=0;
        isadd_p=0;issub_p=0;ismul_p=0;isdiv_p=0;ismod_p=0;iscmp_p=0;isand_p=0;isor_p=0;isnot_p=0;ismov_p=0;islsl_p=0;
        islsr_p=0;isasr_p=0;isnop_p=0;isld_p=0;isst_p=0;isbeq_p=0;isbgt_p=0;isub_p=0;iscall_p=0;isret_p=0;iswb_p=0;isexit_p=0;
        isimme_p=0;
    }
};

Pipeline *ifof=new Pipeline();
Pipeline *ofex=new Pipeline();
Pipeline *exme=new Pipeline();
Pipeline *mewb=new Pipeline();

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

bool isstall=0;

int p=0;
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

    ifof->instruction_p=0x68000000;
    ofex->instruction_p=0x68000000;
    exme->instruction_p=0x68000000;
    mewb->instruction_p=0x68000000;

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
  write_word(MEM,0,0x68000000);
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address+4, instruction);
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

    ifof->pc_p=PC;
    ifof->instruction_p=instruction_word;

    if(instruction_word==0Xffffffff && p<3)
    {
        ifof->instruction_p=0X68000000;
        isexit=0;
        p++;
    }
    if(p>0 && p<3)
    {
        isexit=0;
        p++;
    }
    else if(p>=3)
    {
        isexit=1;
    }



    printf("Fetch Unit:\n\n   Instruction : 0X%x\n",instruction_word);
    cout<<endl;
    //cout<<isbtaken<<endl;
}
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode() {
    unsigned int a,h,u;
    a=ifof->instruction_p & 0x07ffffff;
    a=a<<2;
    if(((a&0x10000000)>>28 )==1)
    {
        a=a|0xf0000000;
    }
    btarget=a+ifof->pc_p;
    ofex->btarget_p=btarget;

    unsigned int c;
    c=ifof->instruction_p & 0x0000ffff;
    u=(ifof->instruction_p>>16) & 0x00000001;
    h=(ifof->instruction_p>>17) & 0x00000001;
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
    if(isret==1){
        operand1=R[15];
        ofex->isret_p=1;
    }
    else
    {
        a=ifof->instruction_p>>18;
        a=a & 0x0000000f;
        operand1=R[a];
        ofex->isret_p=0;
        //cout<<"R["<<a<<"]"<<endl;
    }

    if(isst==1)
    {
        a=ifof->instruction_p>>22;
        a=a & 0x0000000f;
        operand2=R[a];
        ofex->isst_p=1;
    }
    else
    {
        a=ifof->instruction_p>>14;
        a=a & 0x0000000f;
        operand2=R[a];
        ofex->isst_p=0;
        //cout<<"R["<<a<<"]"<<endl;
    }

    ofex->a_p=operand1;
    if(isimme==1)
        ofex->b_p=immx;
    else
        ofex->b_p=operand2;

    ofex->op2_p=operand2;

    ofex->instruction_p=ifof->instruction_p;
    ofex->pc_p=ifof->pc_p;

    //cout<<"\nDecode Unit : \n\n";
    printf("Decode Unit:\n\n   Instruction : 0X%x\n",ifof->instruction_p);
    cout<<"  Operand 1 : "<<ofex->a_p<<endl;
    cout<<"  Operand 2 : "<<ofex->b_p<<endl;
    cout<<endl;
}
//executes the ALU operation based on ALUop
void execute() {
    if(isret==0)
        branchPC=ofex->btarget_p;
    else
        branchPC=ofex->a_p;

    if(ofex->isub_p==1 || (ofex->isbeq_p ==1 && eq==1) || (ofex->isbgt_p == 1 && gt==1)){
        isbtaken=1;
    }
    else
        isbtaken=0;

    int a,b;

    a=ofex->a_p;
    b=ofex->b_p;

    //cout<<b<<endl;
    if(ofex->isadd_p==1)
        alures=a+b;
    else if(ofex->issub_p==1)
        alures=a-b;
    else if(ofex->iscmp_p==1)
    {
        gt=0;eq=0;
        if(a>b)
            gt=1;
        else if(a==b)
            eq=1;
    }
    else if(ofex->ismul_p==1)
        alures=a*b;
    else if(ofex->isdiv_p==1)
        alures=a/b;
    else if(ofex->ismod_p==1)
        alures=a%b;
    else if(ofex->islsl_p==1)
        alures=a<<b;
    else if(ofex->islsr_p==1)
        alures=a>>b;
    else if(ofex->isasr_p==1)
    {
        unsigned int p;
        int i;
        p=ofex->instruction_p >> 31;
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
    else if(ofex->isor_p==1)
        alures=a|b;
    else if(ofex->isnot_p==1)
        alures=!b;
    else if(ofex->isand_p==1)
        alures=a&b;
    else if(ofex->ismov_p==1){
        alures=b;
        //cout<<"b is: "<<b<<endl;
    }

    exme->alures_p=alures;
    exme->op2_p=ofex->op2_p;
    exme->instruction_p=ofex->instruction_p;
    exme->pc_p=ofex->pc_p;

    exme->isadd_p = ofex-> isadd_p;exme->issub_p = ofex-> issub_p;exme->ismul_p = ofex-> ismul_p;
    exme->isdiv_p = ofex-> isdiv_p;exme->ismod_p = ofex-> ismod_p;exme->iscmp_p = ofex-> iscmp_p;
    exme->isand_p = ofex-> isand_p;exme->isor_p = ofex-> isor_p;exme->isnot_p = ofex-> isnot_p;
    exme->ismov_p = ofex-> ismov_p;exme->islsl_p = ofex-> islsl_p;exme->islsr_p = ofex-> islsr_p;
    exme->isasr_p = ofex-> isasr_p;exme->isnop_p = ofex-> isnop_p;exme->isld_p = ofex-> isld_p;
    exme->isst_p = ofex-> isst_p;exme->isbeq_p = ofex-> isbeq_p;exme->isbgt_p = ofex-> isbgt_p;
    exme->isub_p = ofex-> isub_p;exme->iscall_p = ofex-> iscall_p;exme->isret_p = ofex-> isret_p;
    exme->iswb_p = ofex-> iswb_p;exme->isexit_p = ofex-> isexit_p;

    //cout<<"\nExecute Unit : \n\n";
    printf("ALU Unit:\n\n   Instruction : 0X%x\n",ofex->instruction_p);
    cout<<"  AluResult : "<<alures<<endl;
    cout<<"  isBranchTaken : "<<isbtaken<<endl;
    cout<< endl;
}
//perform the memory operation
void mem() {

    //cout<<"\nMemory Unit : \n\n";
    printf("Memory Unit:\n\n   Instruction : 0X%x\n",exme->instruction_p);
    if((exme->alures_p >=Starting_point) && (exme->alures_p <4000) ){
    if(exme->isld_p==1)
    {
        ldres=read_word(MEM,exme->alures_p);
        mewb->ldres_p=ldres;
        cout<<"  Address : "<<exme->alures_p<<endl;
        cout<<"  Ld Result : "<<ldres<<endl;
        /*a=(unsigned int*)alures;
        b=*a;
        ldres=b;*/
    }
    else if(exme->isst_p==1)
    {
        write_word(MEM,exme->alures_p,exme->op2_p);
         cout<<"  Address : "<<exme->alures_p<<endl;
         cout<<"  Data : "<<operand2<<endl;

       // a=(unsigned int *)alures;
        //*a=operand2;
    }
    }
    else if((exme->isld_p==1)||(exme->isst_p==1))
        cout<<"  Unavailable Memory :- Error\n";
    else
        cout<<"  No change\n";

        cout << endl;

        mewb->instruction_p=exme->instruction_p;
        mewb->alures_p=exme->alures_p;
        mewb->pc_p=exme->pc_p;

        mewb->isadd_p = exme-> isadd_p;mewb->issub_p = exme-> issub_p;mewb->ismul_p = exme-> ismul_p;
        mewb->isdiv_p = exme-> isdiv_p;mewb->ismod_p = exme-> ismod_p;mewb->iscmp_p = exme-> iscmp_p;
        mewb->isand_p = exme-> isand_p;mewb->isor_p = exme-> isor_p;mewb->isnot_p = exme-> isnot_p;
        mewb->ismov_p = exme-> ismov_p;mewb->islsl_p = exme-> islsl_p;mewb->islsr_p = exme-> islsr_p;
        mewb->isasr_p = exme-> isasr_p;mewb->isnop_p = exme-> isnop_p;mewb->isld_p = exme-> isld_p;
        mewb->isst_p = exme-> isst_p;mewb->isbeq_p = exme-> isbeq_p;mewb->isbgt_p = exme-> isbgt_p;
        mewb->isub_p = exme-> isub_p;mewb->iscall_p = exme-> iscall_p;mewb->isret_p = exme-> isret_p;
        mewb->iswb_p = exme-> iswb_p;mewb->isexit_p = exme-> isexit_p;

}
//writes the results back to register file
void write_back() {

    //cout<<"\nWrite Back unit : \n\n";
    printf("Write Back Unit:\n\n   Instruction : 0X%x\n",mewb->instruction_p);
    unsigned int a;
    int result;
    if(mewb->iswb_p==1)
    {
        if(mewb->isld_p==0 && mewb->iscall_p==0)
            result=mewb->alures_p;
        else if(mewb->isld_p==1 && mewb->iscall_p==0)
            result=mewb->ldres_p;
        else if(mewb->isld_p==0 && mewb->iscall_p==1)
            result=mewb->pc_p+4;

        if(mewb->iscall_p==1){
            R[15]=result;
            cout<<"  Register Number : 15 \n";
            cout<<"  Result : "<<result<<endl;
        }
        else
        {
            a=mewb->instruction_p>>22;
            a=a & 0x0000000f;
            R[a]=result;
            cout<<"  Register Number : "<<a<<endl;
            cout<<"  Result : "<<result<<endl;
        }
    }
    else
        cout<<"  isWb = 0 , So Nothing to Write \n";

        a=mewb->instruction_p>>27;
        a=a&0x0000001f;
        if(a==31)
            isexit=1;

    cout<<endl;

}

// Control Unit

void cunit()
{
    cout <<"\nControl Unit :\n\n";
    unsigned int a;
    isadd=0;issub=0;ismul=0;isdiv=0;ismod=0;iscmp=0;isand=0;isor=0;isnot=0;ismov=0;islsl=0;
    islsr=0;isasr=0;isnop=0;isld=0;isst=0;isbeq=0;isbgt=0;isub=0;iscall=0;isret=0;iswb=0;isexit=0;
    a=ifof->instruction_p>>27;
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
    else if(a==16){
        isbeq=1;
    }
    else if(a==17){
        isbgt=1;
    }
    else if(a==18){
        isub=1;
    }
    else if(a==19){
        iscall=1;
        isub=1;
        iswb=1;
    }
    else if(a==20){
        isret=1;
        isub=1;
    }
   // else if(a==31)
       // isexit=1;

    a=ifof->instruction_p>>26;
    a=a & 0x00000001;
    if(a==1)
        isimme=1;
    else
        isimme=0;

    ofex->isadd_p = isadd;ofex->issub_p = issub;ofex->ismul_p = ismul;ofex->isdiv_p = isdiv;ofex->ismod_p = ismod;ofex->iscmp_p = iscmp;
    ofex->isand_p = isand;ofex->isor_p = isor;ofex->isnot_p = isnot;ofex->ismov_p = ismov;ofex->islsl_p = islsl;
    ofex->islsr_p = islsr;ofex->isasr_p = isasr;ofex->isnop_p = isnop;ofex->isld_p = isld;ofex->isst_p = isst;ofex->isbeq_p = isbeq;
    ofex->isbgt_p = isbgt;ofex->isub_p = isub;ofex->iscall_p = iscall;ofex->isret_p = isret;ofex->iswb_p = iswb;ofex->isexit_p = isexit;


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


bool Check_DataLock(unsigned int A ,unsigned int B)
{
    unsigned int a,b,c;

    a=A>>27;
    a=a&0x0000001f;
    b=B>>27;
    b=b&0x0000001f;

    if(a==13 || a==16 || a==17 || a==18 || a==19)
        return false;

    if(b==5 || b==13 || b==15 || b==16 || b==17 || b==18 || b==20)
        return false;

    int src1,src2,dest,issrc2=1;
    c=A>>18;
    c=c & 0x0000000f;
    src1=R[c];
    c=A>>14;
    c=c & 0x0000000f;
    src2=R[c];

    if(a==15)
    {
        c=A>>22;
        c=c & 0x0000000f;
        src2=R[c];
    }
    if(a==20)
    {
        src1=R[15];
    }
    c=B>>22;
    c=c & 0x0000000f;
    dest=R[c];
    if(b==19)
        dest=R[15];

    if(a!=15)
    {
        c=A>>26;
        c=c & 0x00000001;
        if(c==1)
            issrc2=0;
    }

    if(src1==dest)
        return true;
    else if(issrc2==1 && src2==dest)
        return true;

    return false;
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

  /*while(isexit==0) {
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
  */

  while(isexit==0)
  {
      cout<<"Cycle Number : "<<cycle_number<<endl;
      cout<<endl;
      isstall=0;
      isstall=Check_DataLock(ifof->instruction_p,ofex->instruction_p)||Check_DataLock(ifof->instruction_p,exme->instruction_p)||Check_DataLock(ifof->instruction_p,mewb->instruction_p);
      write_back();
      mem();
      execute();
      cunit();
      if(isstall==0){
      decode();
      if(isbtaken==0)
        PC=PC+4;
      else
        PC=branchPC;
      fetch();
      }
      else
      {
          ofex->instruction_p=0X68000000;
          ofex->isadd_p=0;ofex->issub_p=0;ofex->ismul_p=0;ofex->isdiv_p=0;ofex->ismod_p=0;ofex->iscmp_p=0;ofex->isand_p=0;
          ofex->isor_p=0;ofex->isnot_p=0;ofex->ismov_p=0;ofex->islsl_p=0;
          ofex->islsr_p=0;ofex->isasr_p=0;ofex->isnop_p=0;ofex->isld_p=0;ofex->isst_p=0;ofex->isbeq_p=0;ofex->isbgt_p=0;
          ofex->isub_p=0;ofex->iscall_p=0;ofex->isret_p=0;ofex->iswb_p=0;ofex->isexit_p=0;
      }
      cout<<R[0]<<" "<<R[1]<<endl;

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

