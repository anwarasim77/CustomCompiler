//Lexical Analyzer//
#include "tokens.h"
#include <string>
#include <cctype>
#include <fstream>
using namespace std;

Token KeyWord(Token tok)
{
         if(tok.GetLexeme()=="if")
         {
            return Token(IF,"if",tok.GetLinenum());
         }
         else
         if(tok.GetLexeme()=="then")
         {
            return Token(THEN,"then",tok.GetLinenum());
         }
         if(tok.GetLexeme()=="true")
         {
            return Token(TRUE,"true",tok.GetLinenum());
         }
         else
         if(tok.GetLexeme()=="false")
         {
            return Token(FALSE,"false",tok.GetLinenum());
         }
         else
         if(tok.GetLexeme()=="print")
         {
            return Token(PRINT,"print",tok.GetLinenum());
         }
         
   return tok;
}

std::string Etype(const Token& tok)
{
   if(tok==PRINT)
      return"PRINT";
   if(tok==IF)
      return"IF";
   if(tok==THEN)
      return"THEN";
   if(tok==TRUE)
      return"TRUE";
   if(tok==FALSE)
      return"FALSE";
   if(tok==IDENT)
      return"IDENT";
   if(tok==ICONST)
      return"ICONST";
   if(tok==SCONST)
      return"SCONST";
   if(tok==PLUS)
      return"PLUS";
   if(tok==MINUS)
      return"MINUS";
   if(tok==STAR)
      return"STAR";
   if(tok==SLASH)
      return"SLASH";
   if(tok==ASSIGN)
      return"ASSIGN";
   if(tok==EQ)
      return"EQ";
   if(tok==NEQ)
      return"NEQ";
   if(tok==LT)
      return"LT";
   if(tok==LEQ)
      return"LEQ";
   if(tok==GT)
      return"GT";
   if(tok==GEQ)
      return"GEQ";
   if(tok==LOGICAND)
      return"LOGICAND";
   if(tok==LOGICOR)
      return"LOGICOR";
   if(tok==LPAREN)
      return"LPAREN";
   if(tok==RPAREN)
      return"RPAREN";
   if(tok==SC)
      return"SC";

return " ";
}



ostream& operator<<(ostream& out, const Token& tok)
{  
   out<< tok.GetLexeme();
   return out;
}

bool isOp(char ch)
{
   if(ch=='+' || ch=='-' || ch=='*' || ch=='/' || ch=='(' || ch==')' || ch=='=' || ch==';' || ch=='>' || ch=='<' || ch=='!' || ch=='&' || ch=='|')
      return true;
   else
      return false;
}

Token getNextToken(istream *in,int *LineNumber)
{
   enum LexState { BEGIN,INID,INIC,INSC,INOP,CMNT};
   LexState lexstate=BEGIN;
   std::string lexeme;
   char ch;
   
   while(in->get(ch))
   {
      //cout<<ch<<"\n";
      if(ch=='\n')
      {
         (*LineNumber)++;
      }
      
      switch(lexstate)
      {
         case    BEGIN:
                 {
                        if(isspace(ch))
                        { 
                           continue;
                        }
                        
                        
                        lexeme=ch;
                        
                        if(ch=='#')
                           lexstate=CMNT;
                        else
                           if(isalpha(ch))
                              lexstate=INID;
                           else
                              if(ch=='"')
                                 lexstate=INSC;
                              else
                                 if(isdigit(ch))
                                    lexstate=INIC;
                                 else
                                    if(isOp(ch)) 
                                    {
                                       lexstate=INOP;
                                       if(lexeme=="+")
                                          return Token(PLUS,lexeme,*LineNumber);
                                       if(lexeme=="-")
                                          return Token(MINUS,lexeme,*LineNumber);
                                       if(lexeme=="*")
                                          return Token(STAR,lexeme,*LineNumber);
                                       if(lexeme=="/")
                                          return Token(SLASH,lexeme,*LineNumber);
                                       if(lexeme=="(")
                                          return Token(LPAREN,lexeme,*LineNumber);
                                       if(lexeme==")")
                                          return Token(RPAREN,lexeme,*LineNumber);
                                       if(lexeme==";")
                                          return Token(SC,lexeme,*LineNumber);
                                    }
                                    else
                                    {
                                       cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                                       return Token(ERR,lexeme,*LineNumber);
                                    }   
                                       
                  continue; 
                  }  
                           
         case     INID:
                     {
                        if(ch=='\n')
                           return KeyWord(Token(IDENT,lexeme,*LineNumber-1));
                     
                        if(isspace(ch))
                           return KeyWord(Token(IDENT,lexeme,*LineNumber));
                           
                        if(isalpha(ch) ||isdigit(ch) )
                        {
                           lexeme+=ch;
                           lexstate=INID;
                           continue;
                        }
                        else
                        {
                           in->putback(ch);
                           return KeyWord(Token(IDENT,lexeme,*LineNumber));
                        }
                     }
                  continue;  
                       
         case     INIC:
                     {
                           
                        if(isdigit(ch))
                        {
                           lexeme+=ch;
                           lexstate=INIC;
                           continue;
                        }
                        else
                        {  
                              
                           if(isalpha(ch))
                           {  
                                 lexeme+=ch;
                                 cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                                 return Token(ERR,lexeme,*LineNumber);
                           }
                           else
                           {
                              if(ch=='\n')
                              {(*LineNumber)--;}
                              
                              in->putback(ch);
                              return Token(ICONST,lexeme,*LineNumber);
                           }                                 
 
                        }
                     }
         case     INSC:
                  {
                        
                        if(ch=='\n')
                        {
                           cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<"\n";
                           cerr<<")\n";
                           return Token(ERR,lexeme,*LineNumber);
                        }                       
                        
                        if(ch=='"')
                        {
                           return Token(SCONST,lexeme.substr(1),*LineNumber);
                        }
                        else
                        {
                           lexeme+=ch;
                           lexstate=INSC;
                           continue;
                        }
                  }         
                        
         case     INOP:
         
                     if((ch=='\n' || ch==' ') && (lexeme=="!" || lexeme=="&" || lexeme=="|"))
                     {
                        cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                        return Token(ERR,lexeme,*LineNumber);
                     }
                     
                     if(lexeme=="!")
                     {
                        if(ch=='=')
                        {
                           lexeme+=ch;
                           return Token(NEQ,lexeme,*LineNumber);
                        }
                        else
                        {
                           cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                           return Token(ERR,lexeme,*LineNumber);
                        }
                     }
                     
                     if(lexeme=="&")
                     {
                        if((ch='&'))
                        {
                           lexeme+=ch;
                           return Token(LOGICAND,lexeme,*LineNumber);
                        }
                        else
                        {
                           cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                           return Token(ERR,lexeme,*LineNumber);
                        }
                     }
                     
                     if(lexeme=="|")
                     {
                        if(ch=='|')
                        {
                           lexeme+=ch;
                           return Token(LOGICOR,lexeme,*LineNumber);
                        }
                        else
                        {
                           cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
                           return Token(ERR,lexeme,*LineNumber);
                        }
                     } 

               if(isOp(ch))
               {
                        if(lexeme=="=" && ch=='=')
                        {
                           lexeme+=ch;
                           return Token(EQ,lexeme,*LineNumber);
                        }                                               
                        
                        if(lexeme==">" && ch=='=')
                        {
                           lexeme+=ch;
                           return Token(GEQ,lexeme,*LineNumber);
                        }
                        
                        if(lexeme=="<" && ch=='=')
                        {
                           lexeme+=ch;
                           return Token(LEQ,lexeme,*LineNumber);
                        }
                        
                        
                           if(ch=='\n')
                           {(*LineNumber)--;}
                           
                           in->putback(ch);

                           if(lexeme=="<")
                              return Token(LT,lexeme,*LineNumber);
                           if(lexeme==">")
                              return Token(GT,lexeme,*LineNumber);
                           if(lexeme=="=")
                              return Token(ASSIGN,lexeme,*LineNumber);  
                                                    
                  }
                  else
                  {        
                           if(ch=='\n')
                           {(*LineNumber)--;}
                           
                           in->putback(ch);
                  
                           if(lexeme=="<")
                              return Token(LT,lexeme,*LineNumber);
                           if(lexeme==">")
                              return Token(GT,lexeme,*LineNumber);
                           if(lexeme=="=")
                              return Token(ASSIGN,lexeme,*LineNumber);
                  }
                           
         case     CMNT:
                        if(ch=='\n')
                        {
                           lexstate=BEGIN;
                           continue;
                        }
                        else
                           continue;
      }
   
   }
   
      //After EOF
      switch(lexstate)
      {
         case BEGIN:break;
         case INSC: break;
         case CMNT: break;
      
         case INID:
         {
            return KeyWord(Token(IDENT,lexeme,*LineNumber));
            break;
         }
         
         case INIC:
         {
            return Token(ICONST,lexeme,*LineNumber);
            break;
         }
         
         case INOP:
         {
            if(lexeme=="!" || lexeme=="&" || lexeme=="|")
            {
               cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
               return Token(ERR,lexeme,*LineNumber);
            }
         

            if(lexeme=="<")
               return Token(LT,lexeme,*LineNumber);
            if(lexeme==">")
               return Token(GT,lexeme,*LineNumber);
            if(lexeme=="=")
               return Token(ASSIGN,lexeme,*LineNumber);
               
            break;
         }
      }

   
   
   if(lexstate!=BEGIN && lexstate!=CMNT)
   {
      cerr<<"Error on line "<<*LineNumber<<" ("<<lexeme<<")"<<"\n";
      return Token(ERR,lexeme,*LineNumber);
   }
   else   
      return Token(DONE,"",*LineNumber);
}
