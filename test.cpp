#include<iostream> 
#include<fstream>
using namespace std;
template<class T,class T2>
class tmp{
    public:
    T *a1;
    T a2;
    T2 a3;
    tmp(){
        a2=T();
        a3=3.123123213;
    }
};
int main(){
    tmp<int,double> p;
    fstream f("lose.txt",ios::in|ios::out|ios::trunc|ios::binary);
    p.a1=new int(3);
    f.write(reinterpret_cast<char *>(&p),sizeof(tmp<int,double>));
    f.read(reinterpret_cast<char *>(&p),sizeof(tmp<int,double>));
    cout<<*p.a1<<p.a2<<p.a3<<endl;
    return 0;
}