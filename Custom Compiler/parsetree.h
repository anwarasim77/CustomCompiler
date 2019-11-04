/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include "value.h"
#include "lexer.h"
#include <vector>
#include <map>
#include <stdlib.h>
using std::vector;
using std::map;

#include <iostream>
using namespace std;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE, BOOLTYPE, IDENTTYPE, stmtTYPE, IFTYPE, ASGNTYPE, PRINTTYPE,
 PLUSTYPE, MINTYPE, MULTYPE, DIVTYPE, ANDTYPE, ORTYPE, EQTYPE, NEQTYPE, LTTYPE, LEQTYPE, GTTYPE, GEQTYPE, };

// a "forward declaration" for a class to hold values
class Value;

class ParseTree {
	int			linenum;
	public :ParseTree	*left;
	public :ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLinenum() const { return linenum; }
   
   virtual std::string getval() const 
   { 
      string i;
      return i;
   }
   
   virtual int Getval() const 
   { 
      int i=0;
      return i;
   }

   virtual bool imid() const
   {
      return false;
   }

	virtual NodeType GetType() const { return ERRTYPE; }
   virtual Value Eval(std::map<string,Value> &val)=0;

	int LeafCount() const {
		int lc = 0;
		if( left ) lc += left->LeafCount();
		if( right ) lc += right->LeafCount();
		if( left == 0 && right == 0 )
			lc++;
		return lc;
	}
   
   int INTcount()
   {
      int ic=0;
      NodeType l;  
      
      if( left ) ic+= left->INTcount();
		if( right ) ic+= right->INTcount();
      
      l=this->GetType();
      
      if( left == 0 && right == 0 )
         if(l==INTTYPE)
            ic++;
         
      return ic;
   }
   
   int Scount()
   {
      int sc=0;
      NodeType l;  
      
      if( left ) sc+= left->Scount();
		if( right ) sc+= right->Scount();
      
      l=this->GetType();
      
      if( left == 0 && right == 0 )
         if(l==STRTYPE)
            sc++;
         
      return sc;
   }
   
   //accounting for identifiers
   
   int IDcount()
   {
      int id=0;
      
      if( left ) id+= left->IDcount();
		if( right ) id+= right->IDcount();
      
      if( left == 0 && right == 0 )
         if(this->imid())
         {  
            id++;
         }
         
      return id;
   }
   
   void IDgather(std::vector<std::string> &array)
   {
      
      if( left )
         left->IDgather(array);
		if( right ) 
         right->IDgather(array);
      
      if( left == 0 && right == 0 )
         if(this->imid())
         {  
            array.push_back(this->getval());
         }
         
   }
   
   void printids()
   {  
      int max=0;
      int maxc=0;
      int cc=1;
   
      std::vector<std::string> array;
      std::map<string,int> IDENT;

      this->IDgather(array);      

      for(int x=0;x<array.size();x++)
      {
         IDENT[array[x]]++;
      }
      
      for (std::map<string,int>::iterator it=IDENT.begin(); it!=IDENT.end(); ++it)
      {    
         if(IDENT[it->first]>max)
            max=IDENT[it->first];
      }
         
      for (std::map<string,int>::iterator it=IDENT.begin(); it!=IDENT.end(); ++it)
      {    
         if(cc<=IDENT.size() && it!=IDENT.begin() && IDENT[it->first]==max && maxc>0)
            cout<<", ";
            
            cc++;
            if(IDENT[it->first]==max)
            {
               maxc++;
               cout<<it->first;
               
            }
      }
      
      cout<<"\n";
    
   }

/////////////////////////////////////////////////////////////////////////////
   
   void RTError(int line, string msg)
   {
   	cout << line << ": RUNTIME ERROR " << msg << endl;
   }
   
   
   void mapID(std::map<string,Value> &val)
   {
      std::vector<std::string> ID;
      this->IDgather(ID);
      
      for(int x=0;x<ID.size();x++)
      {
         val[ID[x]]= Value();
      }
   }
   
   void interpret()
   {  
      std::map<string,Value> val;
      
      this->mapID(val);
      
      if (left)
      {   
         left->Eval(val);
      }
      if (right)
      {
         right->Eval(val);
      }
      
   }
    // other methods
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
   NodeType GetType() const { return stmtTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      if(left)
      {
          Value a1=left->Eval(val);
      }
      if(right)
      {
          Value a2=right->Eval(val);
      }
      
      return Value();
   }

};

class IfStatement : public ParseTree {
public:
	IfStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
   NodeType GetType() const { return IFTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {  
      Value a1=left->Eval(val);
      
      if(a1.isBoolType())
      {
         if(a1.getBoolean())
         {
            Value a2=right->Eval(val);
            return a2;
         }
         else
           return Value();
      }
      else
      {
         this->RTError(this->GetLinenum(),"If statement should return boolean");
         exit (EXIT_FAILURE);
      }
   }
   
};

class Assignment : public ParseTree {
public:
	Assignment(int line, ParseTree *lhs, ParseTree *rhs) : ParseTree(line, lhs, rhs) {}
   NodeType GetType() const { return ASGNTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
         if((left->GetType())!=IDENTTYPE)
         {
            this->RTError(this->GetLinenum(),"Can only assign to identifier");
            exit (EXIT_FAILURE);
         }
         
         if(right)
         {
            val[(left->getval())]=right->Eval(val);
            
         }
         
         //cout<<"val in map is "<<val->at(left->getval()).getInteger()<<endl;  
         
         return val[left->getval()] ;
   }
   
};

class PrintStatement : public ParseTree {
public:
	PrintStatement(int line, ParseTree *e) : ParseTree(line, e) {}
   NodeType GetType() const { return PRINTTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      
      if(a1.isBoolType())
      {
         if(a1.getBoolean())
            cout<<"True";
         else
            cout<<"False";
      }
      else if(a1.isIntType())
      {
         cout<<a1.getInteger();
      }
      else if(a1.isStringType())
      {
         cout<<a1.getString();
      }
      else
      {
         this->RTError(this->GetLinenum(),"Can not print expression");
         exit (EXIT_FAILURE);
      }
      
      cout<<"\n";
      
      return Value();
   }
   
};

class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return PLUSTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( (a1.isIntType() && a2.isIntType()))
      {
         return Value(a1.getInteger()+a2.getInteger());
      } 
      else if((a1.isStringType() && a2.isStringType()) )
      {
         return Value(a1.getString()+a2.getString());
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch");
         exit (EXIT_FAILURE);
      }
   }
   
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return MINTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isIntType() && a2.isIntType() )
         return Value(a1.getInteger()-a2.getInteger());
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch");
         exit (EXIT_FAILURE);
      }
   }
   
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return MULTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( (a1.isIntType() && a2.isIntType()))
      {
         return Value(a1.getInteger()*a2.getInteger());
      } 
      else if((a1.isStringType() && a2.isIntType()) )
      {
         int x=a2.getInteger();
         string S="";
         
         if(x<0)
         {
            this->RTError(this->GetLinenum(),"Cant multiply string with negative");
            exit (EXIT_FAILURE);
         }
         
         for(int a=0;a<x;a++)
         {
            S+=a1.getString();
         }
      
         return Value(S);
      }
      else if((a2.isStringType() && a1.isIntType()) )
      {
         int x=a1.getInteger();
         string S="";
         
         if(x<0)
         {
            this->RTError(this->GetLinenum(),"Cant multiply string with negative");
            exit (EXIT_FAILURE);
         }
         
         for(int a=0;a<x;a++)
         {
            S+=a2.getString();
         }
      
         return Value(S);
      }
      else if(a1.isIntType() && a2.isBoolType())
      {
         if(a1.getInteger()==-1)
         {
            bool b=a2.getBoolean();
            return Value(!b);
         }
         else
         {
            this->RTError(this->GetLinenum(),"Type Mismatch");
            exit (EXIT_FAILURE);
         }
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch");
         exit (EXIT_FAILURE);
      }
   }
   
};

class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return DIVTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isIntType() && a2.isIntType() )
      {
         if(a2.getInteger()==0)
         {
            this->RTError(this->GetLinenum(),"Can not divide by 0");
            exit (EXIT_FAILURE);
         }
         else
            return Value(a1.getInteger() / a2.getInteger());
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch");
         exit (EXIT_FAILURE);
      }
   }
   
};

class LogicAndExpr : public ParseTree {
public:
	LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return ANDTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isBoolType() && a2.isBoolType() )
      {
         if(a1.getBoolean()==true && a2.getBoolean()==true)
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"and should compare booleans");
         exit (EXIT_FAILURE);
      }
   }
   
};

class LogicOrExpr : public ParseTree {
public:
	LogicOrExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return ORTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isBoolType() || a2.isBoolType() )
      {
         if(a1.getBoolean()==true || a2.getBoolean()==true)
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Or should compare booleans");
         exit (EXIT_FAILURE);
      }
   }
   
};

class EqExpr : public ParseTree {
public:
	EqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return EQTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isBoolType() && a2.isBoolType() )
      {
         if(a1.getBoolean()==a2.getBoolean())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()==a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()==a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
};

class NEqExpr : public ParseTree {
public:
	NEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return NEQTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if( a1.isBoolType() && a2.isBoolType() )
      {
         if(a1.getBoolean()!=a2.getBoolean())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()!=a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()!=a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
};

class LtExpr : public ParseTree {
public:
	LtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return LTTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()<a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()<a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
   
};

class LEqExpr : public ParseTree {
public:
	LEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return LEQTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()<=a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()<=a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
};

class GtExpr : public ParseTree {
public:
	GtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return GTTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()>a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()>a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
};

class GEqExpr : public ParseTree {
public:
	GEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
   NodeType GetType() const { return GEQTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
      Value a1=left->Eval(val);
      Value a2=right->Eval(val);
      
      if(a1.isIntType() && a2.isIntType())
      {
         if(a1.getInteger()>=a2.getInteger())
            return Value(true);
         else
            return Value(false);
      }
      else if(a1.isStringType() && a2.isStringType())
      {
         if(a1.getString()>=a2.getString())
            return Value(true);
         else
            return Value(false);
      }
      else
      {
         this->RTError(this->GetLinenum(),"Type Mismatch while comparing");
         exit (EXIT_FAILURE);
      }
   }
   
};
//////////////////////////////////////////////
class IConst : public ParseTree {
	int val;

public:
	IConst(int l, int i) : ParseTree(l), val(i) {}
	IConst(Token &t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
   
   int Getval() const {return val;}
   Value Eval(std::map<string,Value> &val)
   {
      return Value(this->Getval());
   }
	NodeType GetType() const { return INTTYPE; }
};
/////////////////////////////////////////////
class BoolConst : public ParseTree {
	bool val;

public:
	BoolConst(Token& t, bool val) : ParseTree(t.GetLinenum()), val(val) {}
   
   std::string getval() const 
   {
      if(val)
         return "true";
      else
         return "false";
   }
   
   Value Eval(std::map<string,Value> &val)
   {
      if(this->getval()=="true")
         return Value(true);
      else
         return Value(false);
   }
   
	NodeType GetType() const { return BOOLTYPE; }
};
///////////////////////////////////
class SConst : public ParseTree {
	string val;

public:
	SConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}

   std::string getval() const {return val;}
	NodeType GetType() const { return STRTYPE; }
   Value Eval(std::map<string,Value> &val)
   {
      string ss=this->getval();
      return Value(ss);
   }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}
   
   std::string getval() const {return id;}
   bool imid() const {return true;}
   NodeType GetType() const { return IDENTTYPE; }
   
   Value Eval(std::map<string,Value> &val)
   {
   
      if(!(val.at(this->getval()).isError()))
      {
         if(val.at(this->getval()).isBoolType())
         {
            bool b=val.at(this->getval()).getBoolean();
            return Value(b);
         }
         else if(val.at(this->getval()).isIntType())
         {
            int i=val.at(this->getval()).getInteger();
            return Value(i);
         }
         else if(val.at(this->getval()).isStringType())
         {
            string s=val.at(this->getval()).getString();
            return Value(s);
         }
         else
         {
            this->RTError(this->GetLinenum(),"Variable not declared");
            exit (EXIT_FAILURE);
         }
      }
      else
      {
         this->RTError(this->GetLinenum(),"Variable not declared");
         exit (EXIT_FAILURE);
      }
   }
};

#endif /* PARSETREE_H_ */