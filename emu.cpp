/*****************************************************************************
TITLE: Emulator																																
AUTHOR: Ali Haider
ROLL NUMBER: 2101CS07
Declaration of Authorship
This cpp file, emu.cpp, is part of the assignment of CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/
#include <bits/stdc++.h>
using namespace std;
map<int,string> MOT;

int32_t mem[10000];
int32_t sp, pc, a, b;
vector<int> vec;
//utility function to convert decimal number to hexadecimal number
string to_hex(int n)
{
    string s="";
    unsigned int x;
    if(n<0)
    x=((n+(1<<31))+(1<<31));
    else
    x=n;
    int r;
    for(int i=0; i<8; i++)
    {
        r=x%16;
        x=x/16;
        if(r<=9)
        s=s+to_string(r);
        else if(r==10)
        s=s+"a";
        else if(r==11)
        s=s+"b";
        else if(r==12)
        s=s+"c";
        else if(r==13)
        s=s+"d";
        else if(r==14)
        s=s+"e";
        else
        s=s+"f";
    }
        reverse(s.begin(),s.end());
        return s;
}
//function for execution of instructions
void exec(ofstream& trc, int x)
{
    int number=0;
    while(1)
    {
        int32_t inst= mem[pc];
        int32_t co = 0xffffff00 & inst;
        int32_t operation= 0xff & inst;
        co=co>>8;
        int prev=pc;

        trc<<"A: "<<to_hex(a)<<"\tB: "<<to_hex(b)<<"\tpc: "<<to_hex(pc)
        <<"\tsp: "<<to_hex(sp)<<"\t"<<MOT[operation]<<" "<<co<<"\n";
        cout<<"A: "<<to_hex(a)<<"\tB: "<<to_hex(b)<<"\tpc: "<<to_hex(pc)
        <<"\tsp: "<<to_hex(sp)<<"\t"<<MOT[operation]<<" "<<co<<"\n";
        int flag=0;
        for(int i=0; i<vec.size(); i++)
        {
            if(vec[i]==pc)
            {
                flag=1;
                break;
            }
        }
        if(flag==1)
        {
           pc++;
           continue; 
        }

        if(operation==0)
        {
            b=a;
            a=co;
        }
        else if(operation==1)
        {
            a+=co;
        }
        else if(operation==2)
        {
         b=a;
         a=mem[sp+co];
        }
        else if(operation==3)
        {
            mem[sp+co]=a;
            a=b;
        }
        else if(operation==4)
        {
            a=mem[a+co];
        }
        else if(operation==5)
        {
            mem[a+co]=b;
        }
        else if(operation==6)
        {
            a+=b;
        }
        else if(operation==7)
        {
            a=b-a;
        }
        else if(operation==8)
        {
            a=b<<a;
        }
        else if(operation==9)
        {
            a=b>>a;
        }
        else if(operation==10)
        {
            sp=sp+co;
        }
        else if(operation==11)
        {
            sp=a;
            a=b;
        }
        else if(operation==12)
        {
            b=a;
            a=sp;
        }
        else if(operation==13)
        {
            b=a;
            a=pc;
            pc+=co;
        }
        else if(operation==14)
        {
            pc=a;
            a=b;
        }
        else if(operation==15)
        {
          if(a==0)
          pc=pc+co;
        }
        else if(operation==16)
        {
            if(a<0)
            pc=pc+co;
        }
        else if(operation==17)
        {
            pc=pc+co;
        }
        else if(operation==18)
        {
            break;
        }

        if(prev==(pc+1))
        {
            cout<<"There is infinite loop at instruction "<<number<<"\n";
            break;
        }

        number++;
        pc++;
    }
    cout<<number<<" instructions executed"<<endl;
}

int main(int argc, char* argv[])
{
    MOT[0]="ldc";
    MOT[1]="adc";
    MOT[2]="ldl";
    MOT[3]="stl";
    MOT[4]="ldnl";
    MOT[5]="stnl";
    MOT[6]="add";
    MOT[7]="sub";
    MOT[8]="shl";
    MOT[9]="shr";
    MOT[10]="adj";
    MOT[11]="a2sp";
    MOT[12]="sp2a";
    MOT[13]="call";
    MOT[14]="return";
    MOT[15]="brz";
    MOT[16]="brlz";
    MOT[17]="br";
    MOT[18]="HALT";
    MOT[19]="data";
    MOT[20]="SET";

    if(argc<3)
    {
        cout<<"Usage: ./emu [option] file.o\nWhere [option] is one of the following:\n\t-trace : Show trace of the executed instruction.\n\t-after : Show memory dump after program execution.\n\t-before : Show memory dump before program execution.\n";
        return 0;
    }

    pc=0;
    a=0;
    b=0;
    sp=10000-1;

    string inputf =argv[2];
    string tracef = inputf.substr(0,inputf.find(".",0));
    tracef=tracef+".trace";

    ifstream inp;
    ofstream trc;

    int x=0;
    int32_t inst;

    inp.open(inputf, ios::out|ios::binary);
    trc.open(tracef);

    while(inp.read((char*) &inst,sizeof(inst)))
    {
        int32_t co=inst & 0xffffff00;
        int32_t operation = inst & 0xff;
        co=co>>8;

        if(operation>=0 && operation<=18)
        {
            mem[x]=inst;
            x++;
        }
        else if(operation==20||operation==19)
        {
            vec.push_back(x);
            mem[x]=co;
            x++;
        }
        else
        {
            cout<<"Instruction number "<<(x+1)<<" is not valid"<<endl;
            return 0;
        }
    }

    if(string(argv[1]) == "-after" || string(argv[1])=="-trace")
    {
    exec(trc, x);
    }

    if(string(argv[1])=="-before" || string(argv[1])=="-after")
    {
        cout<<"Memory Dumping \n";
        trc<<"Memory Dumping";

        int k=0;
        while(k<x)
        {
            if((k%4))
            {
                trc<<"\n"<<to_hex(k)<<" "<<to_hex(mem[k]);
                cout<<"\n"<<to_hex(k)<<" "<<to_hex(mem[k]);
            }
            else
            {
            trc<<to_hex(mem[k])<<" ";
            cout<<to_hex(mem[k])<<" ";
            }
            k++;
        }
    }

    inp.close();

}

/*
 g++ emu.cpp -o main
./main -trace myalgo.o
*/