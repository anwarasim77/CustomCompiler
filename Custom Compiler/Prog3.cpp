
#include "tokens.h"
#include "parse.h"
#include "parse.cpp"
#include <fstream>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using namespace std;

bool FileCheck(std::string S)
{  ifstream infile;
   bool a;
   infile.open(S);
   if( infile.is_open() == false ) 
   {
      a=false;
   }
   else
   {
      a=true;
   }
   
   infile.close();
   return a;
}

int
main(int argc, char *argv[])
{
	ifstream file;
	istream *in;
	int linenum = 0;
   std::string word;

	if(argc>2)
   {
      cerr<<"TOO MANY FILENAMES"<<endl;
      return 0;
   }
   
   if(argc==1)
   {
      in= &cin;
   }
   else
   {
      if(FileCheck(argv[1]))
      {   
         file.open(argv[1]);

         in= &file;
      }
      else
      {
         cerr<<"COULD NOT OPEN "<<argv[1]<<"\n";
         return 1;
      }
      
   }

	ParseTree *prog = Prog(in, &linenum);

	if( prog == 0 )
   {
		return 0; // quit on error
   }
   
   prog->interpret();

	return 0;
}