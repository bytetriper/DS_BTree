#include<functional>
#include<cstddef>
#include<fstream>
#include<iostream>
#include<cstring>
#include"utility.hpp"
#include"exception.hpp"
#define test(x) std::cout<<(x)<<std::endl
typedef long long ll;
namespace sjtu {
    template <class Key, class Value>
    class BTree {
    public:
        typedef pair<Key,Value> pr;
    private:
        // Your private members go here
        std::fstream f;
        const char* fn;
    public:
        static const int node_size=5;
        static const int leaf_size=10;
        static const int blocksiz=4096;
        class node{
            public:
            int ads;//address
            int siz;
            ll son[BTree::node_size+1];
            bool isleaf;
            int bro;
            pr val[BTree::leaf_size+1];//leaf or node value
            int nxt[BTree::leaf_size+1];//tricky way to contain the length
            int pre[BTree::leaf_size+1];
            int head,tail;
            node(BTree *t):bro(0),siz(0),isleaf(0),siz(0),ads(0){
                for(int i=0;i<BTree::leaf_size;++i)
                    son[i]=nxt[i]=pre[i]=0;
                head=tail=0;
            }
            node(const node &o):head(o.head),tail(o.tail),bro(o.bro),siz(o.siz),isleaf(o.isleaf),siz(o.siz),ads(o.ads){
                for(int i=0;i<siz;++i)
                {    son[i]=o.son[i];
                    pre[i]=o.pre[i];
                    nxt[i]=o.nxt[i];
                    
                }
                
            }
        };
        int cnt;
        ll root;
        template<class T>
        inline void read(T &x,ll filep=-1){
            if(filep!=-1)
                f.seekg(filep);
            f.read(reinterpret_cast<char *>(&x),sizeof(T));
            return;
        }
        template<class T>
        inline void write(const T &x,ll filep=-1){
            if(filep!=-1)
                f.seekp(filep);
            f.write(reinterpret_cast<char *>(&x),sizeof(T));
            return;
        }
        inline void check(){
            if(f.is_open())
                test("Open Successfully");
            else
            {    
                test("ERRORs occur while opening files");
                throw runtime_error();
            }
        }
        BTree():fn("tmp.txt"):cnt(0){
            f.open(fn,std::ios::in|std::ios::out|std::ios::binary);
            check();
        }

        BTree(const char *fname):cnt(0) {
            fn=fname;
            if(!fn)fn="tmp.txt";
            f.open(fn,std::ios::in|std::ios::out|std::ios::binary);
            check();
        }

        ~BTree() {
            //clear();

        }

        // Clear the BTree
        void clear(ll pos=root) {
            node t;
            read(t,pos*blocksiz);
            if(!t)return;
            for(int i=0;i<=siz;++i)
                clear(t->son[i]);
            delete t;
        }
        inline pair<node,bool> find(const Key &key){
            node p;
            read(p,root*blocksiz);
            while(!(p.isleaf)){
                if(key<p.val[1].first){
                    read(p,p.son[1]*blocksiz);
                    continue;
                }
                for(int i=p.head;i!=p.tail;i=nxt[i])
                {
                    if(!p.nxt[i]){
                        if(key>=p.val[i].first)
                            read(p,p.son[0]*blocksiz);
                        break;
                    }
                    if(key>=p.val[i].first&&key<p.val[p.nxt[i]].first)
                    {
                        read(p,p.son[p.nxt[i]]*blocksiz);
                        break;
                    }
                }
            }
            for(int i=0;i<p.siz;++i){
                if(p.val[i].first==key)
                    return pair<node,bool>(p,true);
            }
            return pair<node,bool>(p,false);
        }
        inline void ins(node &p,const pr &tar,ll ads,int pos){
            p.siz++;
            p.val[p.siz]=tar;
            p.son[p.siz]=ads;
            p.pre[p.nxt[pos]]=p.siz;
            p.nxt[p.siz]=p.nxt[pos];
            p.pre[p.siz]=pos;
            p.nxt[pos]=p.siz;
        }
        inline void del(node &p,int pos){
            p.pre[p.nxt[pos]]=p.pre[pos];
            p.nxt[p.pre[pos]]=p.nxt[pos];
            p.pre[pos]=p.nxt[pos]=0;
        }
        inline void split(node &p,node &fa,int pos){
            node newson;
            newson.isleaf=true;
            newson.ads=++cnt;
            newson.siz=p.siz-1-(p.siz>>1);
            newson.son[0]=p.son[0];
            int counter=1,midpos;
            for(int i=1;p.nxt[i];i=p.nxt[i],counter++){
                if(counter>1+(p.siz>>1)){
                    ins(newson,p.val[i],p.son[i],newson.pre[0]);//add element in tail
                }
                if(counter==(p.siz>>1)+1)
                    midpos=i;
            }          
            p.son[0]=p.son[midpos];
            ins(fa,p.val[midpos],p.ads,pos);
            fa.son[fa.nxt[fa.siz]]=newson.ads;
            newson.bro=p.bro;
            p.bro=newson.ads;
            write(newson,newson.ads*blocksiz);//save the node in the file
            return;
        }
        bool dfs_insert(const Key &key, const Value &value,node &fa,ll ads,ll pos) {
            node p;
            read(p,ads*blocksiz);
            bool fg=0;
            pr tmp(key,value);
            if(!(p->isleaf)){
                if(key<p.val[0].first)
                {
                    fg=dfs_insert(key,value,p,p.son[0],0);
                }
                for(int i=1;;i=nxt[i])
                {
                    if(!p.nxt[i]){
                        if(key>=p.val[i].first)
                        {
                            //read(p,p.val[0]*blocksiz);
                            fg=dfs_insert(key,value,p,p.son[i],i);
                        }
                        break;
                    }
                    if(key>=p.val[i].first&&key<p.val[p.nxt[i]].first)
                    {
                        fg=dfs_insert(key,value,p,p.sin[i],i);
                        break;
                    }
                }
            }
            else{
                for(int i=1;i<=p.leaf_siz;++i){
                    if(p.val[i].first==key)
                        return false;
                }
                for(int i=1;;i=nxt[i]){
                    if(!nxt[i]){
                        if(key>p.val[i].first)
                            ins(p,tmp,0,i);
                        fg=1;
                    }
                    if(key>=p.val[i].first&&key<p.val[nxt[i]].first)
                    {
                        ins(p,tmp,0,i);
                        break;
                    }
                }
            }
            if(p.siz>(p.isleaf?leaf_size:node_size)){
                if(fa.ads)
                    split(p,fa,pos);
                else{
                    node t;
                    t.ads=root=++cnt;
                    split(p,t,pos);
                    write(t,t.ads*blocksiz);
                }
            }
            write(p,ads*blocksiz);//maybe in the "if"
            return fg;
        }
        bool insert(const Key &key,const Value &value){
            node p=node();
            return dfs_insert(key,value,p,root,0);
        }
        bool modify(const Key &key, const Value &value) {
            pair<node,bool> tmp=find(key);
            if(!(tmp->second))
                return false;
            node p=tmp.first;
            for(int i=1;p.nxt[i];i=p.nxt[i]){
                if(p.val[i].first==key){
                    p.val[i].second=value;
                    write(p,p.ads*blocksiz);
                    return true;
                }
            }
            throw runtime_error();
        }

        Value at(const Key &key) {
            pair<node,bool> p=find(key);
            if(!(p->second)){
                throw runtime_error();
            }
            for(int i=1;p.nxt[i];i=p.nxt[i]){
                if(p.val[i].first==key){
                    return p.val[i].second;
                }
            }
            throw runtime_error();
        }

        bool dfs_erase(const Key &key,node &fa,int ads,int pos) {
            node p;
            read(p,ads*blocksiz);
            bool fg=0;
            if(!(p->isleaf)){
                if(key<p.val[0].first)
                {
                    //read(p,p.son[0]*blocksiz);
                    fg=dfs_erase(key,p,p.son[0],0);
                }
                for(int i=1;;i=nxt[i])
                {
                    if(!p.nxt[i]){
                        if(key>=p.val[i].first)
                        {
                            //read(p,p.val[0]*blocksiz);
                            fg=dfs_erase(key,p,p.son[i],i);
                        }
                        break;
                    }
                    if(key>=p.val[i].first&&key<p.val[p.nxt[i]].first)
                    {
                        fg=dfs_erase(key,p,p.son[i],i);
                        break;
                    }
                }
            }
            else{
                for(int i=1;i<=p.leaf_siz;++i){
                    if(p.val[i].first==key)
                    {
                        del(p,i);
                        fg=true;
                        break;
                    }
                }
            }
            write(p,ads*blocksiz);
            return fg;
        }
        bool erase(const Key &key) {
            node p=node();
            return dfs_erase(key,p,root,0);
        }
        
        class iterator {
        private:
            // Your private members go here
        public:
            node *tar;
            int pos;
            BTree *ot;
            iterator():tar(nullptr),ot(nullptr),pos(0) {
                
            }
            iterator(const iterator& other):tar(other.tar),ot(other.ot),pos(other.pos) {
                
            }

            // modify by iterator
            bool modify(const Value& value) {
                return ot->modify(tar->val[pos],value);
            }

            Key getKey() const {
                return tar->val[pos].first;
            }

            Value getValue() const {
                return tar->val[pos].second;
            }

            iterator operator++(int) {
                iterator tmp(*this);
                if(!nxt[pos]){
                    node *tmp=new node();
                    read(*tmp,tar->bro*blocksiz);
                    pos=1;
                }
            
            }

            iterator& operator++() {
                
            }
            iterator operator--(int) {
                
            }

            iterator& operator--() {
                
            }

            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                
            }

            bool operator!=(const iterator& rhs) const {
                
            }
        };
        
        iterator begin() {

        }
        
        // return an iterator to the end(the next element after the last)
        iterator end() {

        }

        iterator find(const Key &key) {
        
        }
        
        // return an iterator whose key is the smallest key greater or equal than 'key'
        iterator lower_bound(const Key &key) {
            
        }
    };
}  // namespace sjtu
