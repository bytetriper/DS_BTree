#include<functional>
#include<cstddef>
#include<fstream>
#include<iostream>
#include<cstring>
#include"utility.hpp"
#include"exceptions.hpp"
#include"list.hpp"
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
        std::string fn;
    public:
        static const int node_size=5;
        static const int leaf_size=5;
        static const int blocksiz=400;
        class node{
            public:
            using prn=pair<pr,node*> ;
            using piterator=typename list<prn>::iterator;
            static int node_cnt;
            const BTree *ot;
            list<prn> *t;
            node *fa;
            bool isleaf;
            int ads;//address_if is leaf
            int rb,lb;//next/last brother's address
            node(const BTree *tp=nullptr):fa(nullptr),ot(tp),lb(0),rb(0),isleaf(0),ads(0){
                t=new list<prn>();
            }
            node(const node &o):fa(o.fa),ot(o.ot),lb(o.lb),rb(o.rb),isleaf(o.isleaf),ads(o.ads)
            {
                t = new list<prn>(*(o.t));
            }
            inline pair<piterator,bool> ins(pr &x,node *tar=nullptr)//insert a pr by ascending order of Key
            {
                prn tmp(x,tar);
                prn tmp1(pr(), nullptr);
                if(t->empty())
                {
                    t->push_back(tmp);
                    t->push_back(tmp1);
                    return pair<piterator,bool>((t->begin()),true);
                }
                piterator k=t->begin();
                for(piterator i=t->begin();i!=--(t->end());)
                {
                    if((*i).first.first==x.first)
                        return pair<piterator,bool>(t->end(),false);
                    if((*i).first.first<x.first)
                        k=++i;
                    else
                        break;
                }
                return pair<piterator,bool>(t->insert(k,tmp),true);
            }
            inline bool del(const Key &key)
            {
                bool fg=0;
                for(piterator i=t->begin();i!=t->end();++i)
                {
                    if((*i).first.first==key)
                    {    t->erase(i);
                        fg=1;
                        break;
                    }
                }
                return fg;
            }
            ~node()
            {
                delete t;
            }
        };
        using piterator =typename list<pair<pr,node *>>::iterator;
        //int node::node_cnt=0;
        node *root;
        inline void read(node &x,ll filep=-1){//MD
            if(filep!=-1)
                f.seekg(filep);
            x.t->clear();
            int siz;
            f.read(reinterpret_cast<char *>(&siz),sizeof(int));
            pr tmp;
            if (!f.good())
            {
                test("Error while reading at position_01 :");
                test(filep);
                throw runtime_error();
            }
            test("read size equals:");
            test(siz);
            for(int i=1;i<=siz;++i)
            {
                f.read(reinterpret_cast<char *>(&tmp),sizeof(pr));
                if(!f.good())
                {
                    test("Error while reading at position_02 :");
                    test(filep);
                    throw runtime_error();
                }
                const pair<pr,node *> tp(tmp,nullptr);
                (x.t)->push_back(tp);
                //test(x.t->size());
            }
            if (!f.good())
            {
                test("Error while reading at position_03 :");
                test(filep);
                throw runtime_error();
            }
            return;
        }
        inline void write(const node &x,ll filep=-1){//MD
            if(filep!=-1)
                f.seekp(filep);
            int siz=x.t->size();
            f.write(reinterpret_cast<const char *>(&siz),sizeof(int));
            test("write size equals:");
            test(siz);
            for(piterator i=x.t->begin();i!=x.t->end();++i)
            {
                pr tmp((*i).first);
                f.write(reinterpret_cast<const char *>(&tmp),sizeof(pr));
                if(!f.good())
                {
                    test("Error while writing at position :");
                    test(filep);
                    throw runtime_error();
                }
               // test("write count");
            }
            return;
        }
        inline void check(){//MD
            if(f.is_open())
                test("Open Successfully");
            else
            {    
                test("ERRORs occur while opening files");
                throw runtime_error();
            }
        }
        BTree():fn("tmp.txt"){//MD
            root=new node(this);
            root->isleaf = true;
            root->ads = ++node::node_cnt;
            f.open(fn,std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc);
            check();
            write(*root,root->ads*blocksiz);
            if (!f.good())
            {
                test("Init Error\n");
                throw runtime_error();
            }
        }

        BTree(const char *fname) {//MD
            root=new node(this);
            root->isleaf = true;
            root->ads = ++node::node_cnt;
            fn=fname;
            if(fn=="")fn="tmp.txt";//clear the file before open
            f.open(fn,std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc);
            check();  
            write(*root, root->ads * blocksiz);
            if (!f.good())
            {
                test("Init Error\n");
                throw runtime_error();
            }
        }

        void release(node *p) {//MD recursively delete
            if(!p)return;
            for(piterator i=p->t->begin();i!=p->t->end();++i)
                release((*i).second);
            delete p;
            p=nullptr;
            return ;
        }
        ~BTree() {
            release(root);
            f.close();
            test("File Closed!");
        }

        // Clear the BTree
        void clear() {//MD recursively delete
            release(root);
            root=new node(this);
            root->isleaf=true;
            node::node_cnt=0;
            root->ads=++node::node_cnt;
            write(*root, root->ads * blocksiz);
        }
        inline pair<int,bool> fd_leaf(const Key &key)//MD
        {
            node *p=root; 
            while(!(p->isleaf))
            {
                piterator k=p->t->begin();
                for(piterator i=p->t->begin();i!=--(p->t->end());)
                {
                    if(key>=(*i).first.first)
                        k=++i;
                    else
                        break;
                }
                if(!((*k).second))
                    return pair<int,bool>(0,false);
                p=(*k).second;
            }
            return pair<int,bool>(p->ads,true);
        }
        inline void split(node *p){//MD
            if(!(p->fa))//No father ---->root
            {
                p->fa=new node(this);
                root = p->fa;
                pr tmp(p->t->front().first);
                piterator k=(root->ins(tmp, nullptr)).first;
                ++k;
                (*k).second = p;
            }
            node *newson=new node(*p);
            if(newson->isleaf)
                newson->ads=++node::node_cnt;
            int m=p->t->size()-1;
            int mp=m>>1;int mnew=m-mp;
            for(int i=1;i<=m-mp;++i)
                p->t->pop_back();
            for(int i=1;i<=m-mnew;++i)
                newson->t->pop_front();
            pr tmp1(newson->t->front().first);
            piterator k=(p->fa->ins(tmp1,p)).first;
            ++k;
            (*k).second=newson;
            p->rb=newson->ads;
            newson->lb=p->ads;
            if (newson->isleaf)
            {
                test("new node have been loaded to the file");
                write(*newson, (newson->ads) * blocksiz);
                write(*p, (p->ads) * blocksiz);
            }
            if(p->fa->t->size()>node_size)
                split(p->fa);
            return;
        }
        bool insert(const Key &key,const Value &value){//MD
            node *p=root; 
            pr tmp(key,value);
            while(!(p->isleaf))
            {
                piterator k=p->t->begin();
                for(piterator i=p->t->begin();i!=(--p->t->end());)
                {
                    if(key>=(*i).first.first)
                        k=++i;
                    else
                        break;
                }
                if(!((*k).second))
                {
                    (*k).second=new node(this);//ASK FOR STORAGE!
                    ((*k).second)->isleaf=true;
                    ((*k).second)->fa=p;
                    ((*k).second)->ads=++(node::node_cnt);
                    write(*((*k).second), node::node_cnt * blocksiz);
                }
                p=(*k).second;
            }
            //test("p.ads:");
            //test(p->ads);
            read(*p, p->ads * blocksiz);
            bool fg=(p->ins(tmp)).second;
            if (fg)
            {
                test(p->t->size());
                if (p->t->size() > leaf_size)
                {
                    split(p);
                    test("Split just happened!\n");
                }
                write(*p, p->ads * blocksiz);
            }
            else
                test("Insertion failed!");
            return fg;
        }
        bool modify(const Key &key, const Value &value) {//MD
            pair<int,bool> tmp=fd_leaf(key);
            if(!(tmp.second))
                return false;
            node p;
            read(p,tmp.first*blocksiz);
            for(piterator i=p->t->begin();i!=(--p->t->end());++i)
            {
                if((*i).first.first==key)
                {
                    (*i).first.second=value;
                    write(p,tmp.first*blocksiz);
                    return true;
                }
            }
            return false;
            throw runtime_error();
        }
        Value at(const Key &key) {
            pair<int,bool> p=fd_leaf(key);
            if(!(p.second)){
                test("at:can't find the corresponding value!\n");
                throw index_out_of_bound();
            }
            node tp;
            read(tp,p.first*blocksiz);
            for(piterator i=tp.t->begin();i!=tp.t->end();++i)
            {
                if((*i).first.first==key)
                    return (*i).first.second;
            }
            test("at:can't find the corresponding value!\n");
            return Value();
            throw index_out_of_bound();
        }
        bool erase(const Key &key) {
            pair<int,bool> p=fd_leaf(key);
            if(!(p.second))
                return false;
            node tp;
            read(tp,p.first*blocksiz);
            bool fg=tp.del(key);
            if (fg)
                write(tp,p.first*blocksiz);
            return fg;
        }
        
        class iterator {
        private:
            // Your private members go here
        public:
            node *tar;
            piterator pt;
            BTree *ot;
            iterator(const BTree *t=nullptr):tar(nullptr),ot(t),pt() {
            }
            iterator(const iterator& other):ot(other.ot) {
                tar=new node(*(other.tar));
                for(pt=tar->t->begin();pt!=tar->t->end();++pt)
                    if((*pt).first==(*other.pt).first)
                        break;
            }
            ~iterator()
            {
                delete tar;
            }
            // modify by iterator
            bool modify(const Value& value) {
                if(!ot)
                {
                    test("Invalid operator:No BTree\n");
                    throw index_out_of_bound();
                }
                return ot->modify((*pt).first.first,value);
            }

            Key getKey() const {
                return (*pt).first.first;
            }

            Value getValue() const {
                return (*pt).first.second;
            }

            iterator& operator++() {
                if(!tar)
                {    
                    int tmp;
                    if(ot->fd_leaf((*pt).first.first).second==false)
                    {
                        test("operator:can't find key\n");
                        throw index_out_of_bound();
                    }
                    tmp=ot->fd_leaf((*pt).first.first).first;
                    tar=new node();
                    read(*tar,tmp*blocksiz);
                }
                if(pt==tar->t->end())
                {
                    read(*tar,(tar->rb)*blocksiz);
                    pt=tar->t->begin();
                }
                else
                    ++pt;
                return *this;
            }
            iterator operator++(int) {
                iterator tmp(*this);
                ++(*this);
                return tmp;  
            }
            iterator& operator--() {
                if(!tar)
                {    
                    int tmp;
                    if(ot->fd_leaf((*pt).first.first).second==false)
                    {
                        test("operator:can't find key\n");
                        throw index_out_of_bound();
                    }
                    tmp=ot->fd_leaf((*pt).first.first).first;
                    tar=new node();
                    read(*tar,tmp*blocksiz);
                }
                if(pt==tar->t->begin())
                {
                    read(*tar,(tar->lb)*blocksiz);
                    pt=--(tar->t->end());
                }
                return *this;
            }
            iterator operator--(int) {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }

            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                return (rhs.ot==ot)&&((*pt).first==(*(rhs.pt)).first);
            }

            bool operator!=(const iterator& rhs) const {
                return !((*this)==rhs);
            }
        };
        
        iterator begin() {
            node *p=root;
            while(!(p->isleaf))
            {
                for(piterator i=p->t->begin();;++i)
                {
                    if((*i).second)
                    {
                        p=(*i).second;
                        break;
                    }
                }
            }
            iterator tmp;
            tmp.tar=new node();
            read(*tmp.tar,p->ads*blocksiz);
            tmp.ot=this;
            tmp.pt=tmp.tar->t->begin();
            return tmp;
        }
        
        // return an iterator to the end(the next element after the last)
        iterator end() {
            node *p=root;
            while(!(p->isleaf))
            {
                for(piterator i=--(p->t->end());;--i)
                {
                    if((*i).second)
                    {
                        p=(*i).second;
                        break;
                    }
                }
            }
            iterator tmp;
            tmp.tar=new node();
            read(*(tmp.tar),(p->ads)*blocksiz);
            tmp.ot=this;
            tmp.pt=tmp.tar->t->end();
            return tmp;
        }

        iterator find(const Key &key) {
            pair<int,bool> tp=fd_leaf(key);
            iterator tmp;
            tmp.tar=new node();
            read(*(tmp.tar),(tp.first)*blocksiz);
            tmp.ot=this;
            for(piterator i=tmp.tar->t->begin();i!=tmp.tar->t->end();++i)
            {
                if((*i).first.first==key)
                {
                    tmp.pt=i;
                    return tmp;
                }
            }
            return tmp;
        }
        
        // return an iterator whose key is the smallest key greater or equal than 'key'
        iterator lower_bound(const Key &key) {
            iterator tmp;
            tmp.ot=this;
            node *p=root;
            while(!(p->isleaf))
            {
                piterator k=p->t->begin();
                for(piterator i=p->t->begin();i!=(--p->t->end());)
                {
                    if((*i).first.first<=key)
                        k=++i;
                    else
                        break;
                }
                if((*k).second)
                    p=(*k).second;
                else
                {
                    if(k==(--p->t->end()))
                    {
                        test("lower_bound:key too large!\n");
                        throw index_out_of_bound();
                    }
                    p=(*(++k)).second;
                }
            }
            tmp.tar=new node();
            read(*(tmp.tar),p->ads*blocksiz);
            for(piterator i=tmp.tar->t->begin();i!=--(tmp.tar->t->end());++i)
            {
                if(key<=(*i).first.first)
                {
                    tmp.pt=i;
                    return tmp;
                }
            }
           test("lower_bound:key too large!_02\n");
           throw index_out_of_bound();
        }
    };
    template<class Key,class Value>
    int BTree<Key,Value>::node::node_cnt=0;
}  // namespace sjtu
/*
i -1 3
i -2 3
i -3 3
i -4 3
i 1 2
*/