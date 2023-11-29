/*****************************************************************************
TITLE: Assembler																																
AUTHOR: ALI HAIDER
ROLL NUMBER: 2101CS07
Declaration of Authorship
This cpp file, asm.cpp, is part of the assignment of CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/
#include<bits/stdc++.h>
using namespace std;

map<string, string> MOT;
struct symbol{
    string sym_name;
    int sym_ad;
    bool sym_set;
    int sym_set_value;
};

struct literal{
    int lit;
    int lit_address;
};

vector<symbol> sym_tab;
vector<literal> lit_tab;
//function to get hexadecimal value of given decimal integer
string to_hex(int n)
{
    string s="";
    unsigned int x;
    if(n<0)
    x=(n+(1<<31))+(1<<31);
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
// function for trimming unnecessary space
string proper(string s)
{
    string s1="";
    vector<string> vec;
    for(int i=0; i<s.length(); i++)
    {
        if(s.at(i)==' '||s.at(i)=='\t')
        {
            if(s1!="")
            vec.push_back(s1);
            s1="";
        }
        else
        s1=s1+s.at(i);
    }
    if(s1!="")
            vec.push_back(s1);
            s1="";

            if(vec.size()>1)
            {
            for(int i=0; i<(vec.size()-1); i++)
            {
                s1=s1+vec[i]+" ";
            }
            }
            if(vec.size()>0)
            s1=s1+vec[vec.size()-1];
            return s1;
}
//function to convert string into integer
int tonum(string s)
{
    s=proper(s);
    if(s.find("+")==0)
    return stoul(s.substr(1,s.length()),nullptr,10);
    else if(s.find("-")==0)
    return -stoul(s.substr(1,s.length()),nullptr,10);
    else if(s.find("0x")==0)
    return stoul(s, nullptr, 16);
    else if(s.find("0")==0)
    return stoul(s, nullptr, 8);
    else
    return stoul(s, nullptr, 10);
}
//function for checking whether the given label name has been used before
bool is_duplicate(string s)
{
    int flag=0;
    for(int i=0; i<sym_tab.size(); i++)
    {
        if(sym_tab[i].sym_name==s)
        {
        flag=1;
        break;
    }
    } 
    if(flag==1)
    return true;
    else
    return false;    
}
//function to check whether the input is a valid number or not
bool valid_number(string s)
{
    string s1;
    if(s.substr(0,2)=="0x")
    {
    s1=s.substr(2,(s.length()-2));
    int flag=0;
    for(int i=0; i<s1.length(); i++)
    {
        if(!((s1.at(i)>='a' && s1.at(i)<='f')||(s1.at(i)>='0' && s1.at(i)<='9')))
        {
            flag=1;
            break;
        }
    }
    if(flag==1)
    return false;
    else
    return true;
    }
    else
    {
        if(s.at(0)=='+'||s.at(0)=='-')
        s1=s.substr(1,(s.length()-1));
        else
        s1=s;
        int flag=0;
    for(int i=0; i<s1.length(); i++)
    {
        if(!((s1.at(i)>='0' && s1.at(i)<='9')))
        {
            flag=1;
            break;
        }
    }
    if(flag==1)
    return false;
    else
    return true;
    }
}
//This function is used to add the symbols and literals to table and detect the errors in label name
string add_sym_lit(string inst, int* lo_pt, int line)
{
    vector<string> vec;
    string s="";
    for(int i=0; i<inst.length(); i++)
    {
        if(inst.at(i)==' ')
        {
        if(s!="")
        vec.push_back(s);
        s="";
        }
        else
        s=s+inst.at(i);
    }
    if(s!="")
    vec.push_back(s);
    s="";

    string errors="";
    int lo= *lo_pt;

    if(vec[0].at(vec[0].length()-1)==':') //Here I have assumed that there is no gap between the colon and the instruction following it
    {
        string label="";
        for(int i=0; i<(vec[0].length()-1); i++)
        label=label+vec[0].at(i);
        if(is_duplicate(label))
        errors=errors+"Error: label name "+label+" is used more than once  \n";

        if(!((vec[0].at(0)>='A' && vec[0].at(0)<='Z')||
        (vec[0].at(0)>='a' && vec[0].at(0)<='z')))
        errors=errors+"Error: label name "+label+" does not begin with an alphabet  \n";

        int x= vec.size();

        if(x>1)
        {
            string s1="";
            for(int i=1; i<(vec.size()-1); i++)
            s1=s1+vec[i]+" ";
            s1=s1+vec[vec.size()-1];

            add_sym_lit(s1, &lo, line);

            if(vec[1]=="SET")
            sym_tab.push_back({label, lo, 1, stoi(vec[2])});
            else
            sym_tab.push_back({label, lo, 0, -1});
        }
        else
       {
        x= *lo_pt-1;
        *lo_pt= x;
        sym_tab.push_back({label, lo, 0, -1});
        }
    }
    else
    {
        string s1="";
        for(int i=1; i<(vec.size()-1); i++)
        s1=s1+vec[i]+" ";
        s1=s1+vec[vec.size()-1];
        if(valid_number(s1))
        lit_tab.push_back({tonum(s1),-1});
    }
    return errors;
}
//This function is used to set the address of literals in the literal table
void lit_addition(int lo)
{
    int i=0;
    int x=lo;
    while(i<lit_tab.size())
    {
        if(lit_tab[i].lit_address==-1)
        {
        lit_tab[i].lit_address=x;
        x++;
        }
        i++;
    }
}
//This function is used to convert the instructions to proper form and then pass them to the addition of symbol and literal table
void readfile(string sr, ofstream& logfile)
{
    ifstream inp(sr);
    
    int lo=0;
    int lc=1;
    string s1;

    while(getline(inp, s1))
    {

        vector<string> vec;
        string temp="";
        for(int i=0; i<s1.length(); i++)
        {
            if(s1.at(i)==' '||s1.at(i)=='\t')
            {
            if(temp!="")
            vec.push_back(temp);
            temp="";
            }
            else if(s1.at(i)==';')
            break;
            else
            temp=temp+s1.at(i);
        }
        if(temp!="")
        vec.push_back(temp);

        if(vec.size()==0)
        {
            lc++;
            continue;
        }

        string fin="";

        for(int i=0; i<(vec.size()-1); i++)
        fin=fin+vec[i]+" ";

        fin=fin+vec[vec.size()-1];
        temp=add_sym_lit(fin, &lo, lc++);
        logfile << temp;
        lo++;
    }

    inp.close();

    lit_addition(lo);
}
//This function returns the 32 bit instruction, errors and machine code
string code_gen(string inst, int* lo_pt, int line)
{
    string s="";
    string s1="", s2="", s3="";
    int lo=(*lo_pt);
    s=to_hex(lo);
    s=s+" ";

    int flag=-1;
    for(int i=0; i<inst.length(); i++)
    {
    if(inst.at(i)==':')
    {
    flag=i;
    break;
    }
    }

    if(flag!=-1)
    {
        int col= flag;
        if(col!=inst.length()-1)
        {
            string temp1=inst.substr(col+1,inst.length());
            temp1=proper(temp1);
            string temp2=code_gen(temp1, &lo, line);
            vector<string> vec;
            string temp3="";

            for(int i=0; i<temp2.length(); i++)
            {
                if(temp2.at(i)=='%')
                {
                    vec.push_back(temp3);
                    temp3="";
                }
                else
                temp3=temp3+temp2.at(i);
            }
            vec.push_back(temp3);
            temp3="";
            s1=vec[0];
            s2=vec[1];
            s3=vec[2];
            string temp4=s1;
            temp4=temp4.substr(9,9);
            s=s+temp4+inst+"\n";          
        }
        else
        {
            *lo_pt=*lo_pt-1;
            s=s+"         "+inst+"\n";
        }        
    }
    else
    {
        inst=proper(inst);
        string temp1="";
        vector<string> vec1;
        for(int i=0; i<inst.length(); i++)
        {
            if(inst.at(i)==' '||inst.at(i)=='\t')
            {
                if(temp1!="")
                vec1.push_back(temp1);
                temp1="";
            }
            else
            temp1=temp1+inst.at(i);
        }
        if(temp1!="")
        vec1.push_back(temp1);
        temp1="";
        if(vec1[0]=="add"||vec1[0]=="sub"||vec1[0]=="shl"||vec1[0]=="shr"||
        vec1[0]=="a2sp"||vec1[0]=="sp2a"||vec1[0]=="return"||vec1[0]=="HALT")
        {
            if(vec1.size()>1 && vec1[1]!=";")
            {
            s2=s2+"Error: "+vec1[0]+" does not require arguments at line "+to_string(line)+"\n";
            }
            s=s+"000000"+MOT[vec1[0]]+" ";
            s3=s3+"000000"+MOT[vec1[0]]+" ";
        }
        else if(MOT[vec1[0]]=="")
        {
            s2=s2+"Error: valid instruction not given at line "+to_string(line)+"\n";            
        }
        else if(vec1.size()>1 && valid_number(vec1[1]))
        {
            string hex= to_hex(tonum(vec1[1]));
            s3=s3+hex.substr(hex.length()-6,hex.length())+MOT[vec1[0]]+" ";
            s=s+hex.substr(hex.length()-6,hex.length())+MOT[vec1[0]]+" ";
        }
        else
        {
            int flag=-1;
            if(vec1.size()>1)
            {
            for(int i=0; i<sym_tab.size(); i++)
            {
                if(sym_tab[i].sym_name==vec1[1])
                {
                    flag=i;
                    break;
                }
            }
            }
            string hex;

            if(flag!=-1)
            {
                if(sym_tab[flag].sym_set)
                hex=to_hex(sym_tab[flag].sym_set_value);
                else if(vec1[0]=="call"||vec1[0]=="br"||vec1[0]=="brlz"||vec1[0]=="brz")
                hex=to_hex(sym_tab[flag].sym_ad-lo-1);
                else
                hex=to_hex(sym_tab[flag].sym_ad);
                s3=s3+hex.substr(hex.length()-6,hex.length())+MOT[vec1[0]]+" ";
                s=s+hex.substr(hex.length()-6,hex.length())+MOT[vec1[0]]+" ";
            }

            if(vec1.size()==1)
            {
                s2=s2+"Error: operand not given at line "+to_string(line)+"\n";
            }
            else if(flag==-1)
            {
                s2=s2+"Error: unknown symbol at line "+ to_string(line)+"\n";
            }
        }
        s=s+inst+"\n";
    }
    string s4=s+"%"+s2+"%"+s3;
    return s4;  
}
//This function passes themnemonic for generating instruction, errors and machine code and store it int respective files
void func(string inp, ofstream& outf, ofstream& lf, ofstream& objf)
{
    ifstream inputf(inp);

    int l_c=1;
    int lo=0;
    string s;

    while(1)
    {
        if(!(getline(inputf,s)))
        break;
        int i=0;
        string temp1="";
        while(s.at(i)!=';')
        {
        temp1=temp1+s.at(i);
        i++;
        if(i==s.length())
        break;
        }
        string inst=proper(temp1);
        if(inst.length()==0)
        {
            l_c++;
            continue;
        }

        string temp2=code_gen(inst, &lo, l_c++);
        string s1,s2,s3;
        string temp3="";
        vector<string> vec;
        for(int i=0; i<temp2.length(); i++)
        {
            if(temp2.at(i)=='%')
            {
             vec.push_back(temp3);
             temp3="";
            }
            else
            temp3=temp3+temp2.at(i);
        }
        vec.push_back(temp3);
        temp3="";

        s1=vec[0];
        s2=vec[1];
        s3=vec[2];
        if(s3.length()>0)
        {
            uint32_t temp=stoul("0x"+proper(s3),nullptr,16);
            objf.write((char*)&temp, sizeof(temp));
        }
        lo++;
        lf<<s2;
        outf<<s1;
    }
}

int main(int argc, char* argv[])
{
    MOT["ldc"]="00";
    MOT["adc"]="01";
    MOT["ldl"]="02";
    MOT["stl"]="03";
    MOT["ldnl"]="04";
    MOT["stnl"]="05";
    MOT["add"]="06";
    MOT["sub"]="07";
    MOT["shl"]="08";
    MOT["shr"]="09";
    MOT["adj"]="0a";
    MOT["a2sp"]="0b";
    MOT["sp2a"]="0c";
    MOT["call"]="0d";
    MOT["return"]="0e";
    MOT["brz"]="0f";
    MOT["brlz"]="10";
    MOT["br"]="11";
    MOT["HALT"]="12";
    MOT["data"]="13";
    MOT["SET"]="14";

    string input_file=argv[1];
    string output_file=input_file.substr(0,input_file.find(".",0))+".L";
    string log_file= input_file.substr(0, input_file.find(".",0))+".log";
    string object_file=input_file.substr(0, input_file.find(".",0))+".o";

    ofstream outputfile(output_file);
    ofstream logfile(log_file);
    ofstream objectfile(object_file,ios::out | ios::binary);

    //This is pass1 of file for storing the literals and symbols and detectig some of the errors
    readfile(input_file,logfile);

    //This is pass2 of file for generating instruction file, binary instructions and detecting more errors
    func(input_file, outputfile, logfile, objectfile);
    outputfile.close();
    logfile.close();
    objectfile.close();

    return 0;
}