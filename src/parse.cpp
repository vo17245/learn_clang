#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include "fmt/core.h"

static std::vector<CXCursor> children;
CXChildVisitResult record_child(CXCursor current_cursor, CXCursor parent, CXClientData client_data){
    children.push_back(current_cursor);
    return CXChildVisit_Continue;
}
std::string get_type_name(CXCursor node)
{
    CXType cursor_type = clang_getCursorType(node);
    CXString type_kind_spelling = clang_getTypeKindSpelling(cursor_type.kind);
    std::string res(clang_getCString(type_kind_spelling)) ;
    clang_disposeString(type_kind_spelling);
    return res;
}
std::string get_display_name(CXCursor node)
{
    CXString current_display_name = clang_getCursorDisplayName(node);
    std::string res(clang_getCString(current_display_name)) ;
    clang_disposeString(current_display_name);
    return res;
}
static int cur_depth=0;
static bool flag=false;
void begin(int depth)
{
   
    flag=true;
    cur_depth=depth;
}
void print(const std::string& msg)
{
    if(flag)
    {
        
        for(size_t i=0;i<cur_depth;i++)
        {
            std::cout<<"|  ";
        }
        flag=false;
        std::cout<<msg<<std::endl;;
    }
    else {
        for(size_t i=0;i<cur_depth;i++)
        {
            std::cout<<"   ";
        }
        std::cout<<"-"<<msg<<std::endl;
    }
    
    
}
void end()
{
}
void traversal(CXCursor node,int depth)
{
    begin(depth);
    print(fmt::format("{}",get_display_name(node) ));
    print(fmt::format("type_name: {}",get_type_name(node) ));
    end();
    children.clear();
    clang_visitChildren(
    node, //Root cursor
    record_child, //CXCursorVisitor: a function pointer
    nullptr //client_data
    );
    for(auto child:children)
    {
        traversal(child, depth+1);
    }
}
int main(){
    CXIndex index = clang_createIndex(0, 0); //Create index
    CXTranslationUnit unit = clang_parseTranslationUnit(
      index,
      "input/file.cpp", nullptr, 0,
      nullptr, 0,
      CXTranslationUnit_None); 


    if (unit == nullptr){
      std::cerr << "Unable to parse translation unit. Quitting.\n";
      return 0;
    }
    CXCursor cursor = clang_getTranslationUnitCursor(unit); //Obtain a cursor at the root of the translation  unit
    traversal(cursor,0);
  
}