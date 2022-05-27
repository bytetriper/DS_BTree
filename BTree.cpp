#include <cassert>
#include <iostream>
#include <string>
#include<fstream>
#include "BTree.hpp"
  //  test: constructor
using namespace std;
sjtu::BTree<int, int> bTree;
void insert(int key, int value){
  bTree.insert(key, value);
}


void erase(int key){
  bTree.erase(key);
}

int query(int key){
  return bTree.at(key);
}


void tester(){
  ifstream f("insert.data");
  if (!f.good())
      cout << "NO" << endl;
  int key, value;
  char cmd;
  int counter=0;
  while(f >> cmd){
    if(cmd == 'i'){
      try{
      f>> key >> value;
      insert(key, value);}
      catch(...)
      {
        cout<<"Unknown Error"<<endl;
        return;
      }
      if((++counter)>=2000)
          cout<<"count:"<<counter<<endl;
    }else
    if(cmd == 'e'){
      f >> key;
      erase(key);
    }else
    if(cmd == 'q'){
      f >> key;
      cout << query(key) << endl;
    }else{
      puts("bad_command");
    }
  }
}
int main() {
  tester();
  return 0;
}