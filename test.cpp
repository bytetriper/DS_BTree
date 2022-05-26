#include<cstring>
#include<cmath>
#include<cstdio>
#include<fstream>
#include<iostream>
using namespace std;
typedef long long ll;
const int N=1e5+1;
const int INF=1e9+7;
#define test(x) cout<<(x)<<endl;
template<class T>
class vari{
    public:
    int a[10];
    T *b;
    vari(){
       // a=new int [30];
    }
};
const int block=1000;
int main()
{
    fstream f("tmp.txt",ios::in|ios::out|ios::binary|ios::trunc);
    if(!f.is_open())
        cout<<"NO"<<endl;
    vari<int> T;
    T.a[3]=11;
    T.b=new int [20];
    T.b[2]=1002;
    f.write(reinterpret_cast<const char *>(&T),sizeof(T));
    if(!f.good())
        cout<<"1Error"<<endl; 
    f.seekg(0);
    T.b=new int[200];
    T.a[3]=100;
    T.b[2]=13;
    f.read(reinterpret_cast<char *>(&T),40);
    if(!f.good())
        cout<<"2Error"<<endl;
    cout<<T.a[3]<<endl<<T.b[2]<<endl;
    f.close();
    return 0;
}