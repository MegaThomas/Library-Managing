#include <iostream>
#include <string>
#include <vector>
#include <list>
#define MAXLEN 20011
using namespace std;

unsigned int hash_calc( string& src )
{
	unsigned int hash = 5381;// magic number here
	for( int i=0; i<src.size(); i++ )
	{// DJBHash
		hash = (hash<<5) + hash + src[i];
	}
	return hash;
}

template<class TYP>
struct Node
{// using a struct to store node data
	TYP node;
	Node<TYP>* next;
	Node<TYP>* prev;
	Node(){next=NULL;prev=NULL;}
};
template<class TYP>
class LinkedList
{
public:
	LinkedList() : now(NULL), head(NULL), tail(NULL){}
	LinkedList(int ln){
		now=head=new Node<TYP>;
		for(int i=1;i<=ln-1;i++)
		{
			Node<TYP>* temp=now;
			now->next=new Node<TYP>;
			now=now->next;
			now->prev=temp;
		}
		now->next=NULL;
		tail=now;
	}
	~LinkedList(){
		for(int i=0; ;i++)
		{
			if( head==NULL )
			{
				break;
			}
			now=head->next;
			if( now==NULL ){
				delete head;
				break;
			}else if( now->next!=NULL ){
				delete head;
				head=now;
				now=now->next;
			}else{
				delete head;
				delete now;
				break;
			}
		}
	}
	void push_back(TYP& src);
	void pop_back(void);
	void sort();
	void sort(bool (*cmp)(TYP&,TYP&));
	bool empty(void);
	//int shift(TYP);
	//int unshift(TYP);
	TYP& iterator(int);
	TYP& begin();
	TYP& operator [] (int pos)
	{
		if(this->head!=NULL)
		{
			Node<TYP>* temp=this->head;
			while(pos!=0)
			{
				temp=temp->next;
				pos--;
			}
			return temp->node;
		}
	}
private:
	Node<TYP>* head;			// head is the first of the list;
	Node<TYP>* now;
	Node<TYP>* tail;			// tail is the last of the list;
	typedef *TYP _value;
	typedef _value iterator;
};
template<class TYP>
void LinkedList<TYP>::pop_back(void)
{
	if( tail->prev!=NULL )
	{
		Node<TYP>* tailback=tail->prev;		// back-up for second-last;
		delete tail;
		tail=tailback;
		tail->next=NULL;
	}else
	{
		delete tail;
		tail=head=now=NULL;
	}
}
template<class TYP>
void LinkedList<TYP>::push_back(TYP& src)
{
	for(int i=1; i<10000; i++)
	{// i<10000 is to avoid infinite loop;
		if(head==NULL)
		{
			head=new Node<TYP>;
			head->node=src;
			head->next=NULL;
			tail=now=head;
			break;
		}else{
			tail->next=new Node<TYP>;
			tail->next->node=src;
			tail->next->next=NULL;
			tail->next->prev=tail;
			tail=tail->next;
			break;
		}
	}
	now=tail;
}
template<class TYP>
TYP& LinkedList<TYP>::iterator(int offset)
{// offset: 1=next, -1=prev, 2=head, 3=tail, 0=here;
	if(offset==1){
		if(now->next!=NULL)
			now=now->next;
		else
			return now->node;
		return now->node;
	}else if(offset==-1){
		if(now->prev!=NULL)
			now=now->prev;
		else
			return now->node;
		return now->node;
	}else if(offset==2){
		now=head;
		return now->node;
	}else if(offset==0){
		return now->node;
	}else if(offset==3){
		now=tail;
		return now->node;
	}else{
		return now->node;		// try catch
	}
}
template<class TYP>
TYP& LinkedList<TYP>::begin()
{
	return this->head->node;
}
template<class TYP>
void LinkedList<TYP>::sort()
{
	Node<TYP> *foo,*bar;
	TYP swap;
	for(foo=head;foo!=NULL;foo=foo->next)
	{
		for(bar=foo->next;bar!=NULL;bar=bar->next)
		{
			if( foo->node < bar->node )
			{
				swap=bar->node;
				bar->node=foo->node;
				foo->node=swap;
			}
		}
	}
}
template<class TYP>
void LinkedList<TYP>::sort(bool (*cmp)(TYP&,TYP&) )
{
	Node<TYP> *foo,*bar;
	TYP swap;
	for(foo=head;foo!=NULL;foo=foo->next)
	{
		for(bar=foo->next;bar!=NULL;bar=bar->next)
		{
			if( cmp(foo->node,bar->node) )
			{
				swap=bar->node;
				bar->node=foo->node;
				foo->node=swap;
			}
		}
	}
}
template<class TYP>
bool LinkedList<TYP>::empty(void)
{
	if( head==NULL )
		return true;
	else
		return false;
}

struct Book{
	string meta[7];
	// ID, name, author, ISBN, field, press, date;
	unsigned int hash[7];
	int lend_time;
	int dele;// 1 deleted, 0 exist
};

class HashTable{
public:
	HashTable(int len) : size(len) {
		mainTable.resize(len);
	};
	~HashTable(){};
	void setkey(int val);
	int insert(Book* src);
	void dele(string ID);
	int find(string tar, int time);
	void book_borrow(string ID, int time);
	void book_return(string ID);
private:
	vector< LinkedList<Book*> > mainTable;
	int key;// which tree
	int size;
	int collision();
	void output(Book* src, int time);
};
void HashTable::setkey(int k)
{
	key = k;
}
int HashTable::insert(Book* src)
{
	unsigned int offset = (unsigned int)(src->hash[key])%size;
	mainTable[offset].push_back(src);
	return 1;
}
int HashTable::find(string tar, int time)
{
	unsigned int val = hash_calc(tar)%size;
	if( mainTable[val].empty() )
	{
		return 0;
	}else{
		Book* it = mainTable[val].iterator(3);
		int flag = 0;
		while(true)
		{
			it = mainTable[val].iterator(0);
			if( ( it->meta[key].compare(tar) ) == 0 )
			{// equal
				if( it->dele == 0 )
				{
					flag |= 1;
					output( it, time );
				}
			}
			if( it == mainTable[val].begin() )
				break;
			else
				mainTable[val].iterator(-1);
		}
		return flag;
	}
}
void HashTable::dele(string ID)
{
	unsigned int val = hash_calc(ID)%size;
	if( ! mainTable[val].empty() )
	{
		Book* it = mainTable[val].iterator(3);
		while( true )
		{
			it = mainTable[val].iterator(0);
			if( ( it->meta[key].compare(ID) ) == 0 )
			{// equal
				it->dele = 1;
				break;
			}
			if( it == mainTable[val].begin() )
				break;
			else
				mainTable[val].iterator(-1);
		}
	}
}
void HashTable::book_borrow(string ID, int time)
{
	unsigned int val = hash_calc(ID)%size;
	if( ! mainTable[val].empty() )
	{
		Book* it = mainTable[val].iterator(3);
		while( true )
		{
			it = mainTable[val].iterator(0);
			if( ( it->meta[key].compare(ID) ) == 0 )
			{// equal
				it->lend_time = time;
				break;
			}
			if( it == mainTable[val].begin() )
				break;
			else
				mainTable[val].iterator(-1);
		}
	}
}
void HashTable::book_return(string ID)
{
	unsigned int val = hash_calc(ID)%size;
	if( ! mainTable[val].empty() )
	{
		Book* it = mainTable[val].iterator(3);
		while( true )
		{
			it = mainTable[val].iterator(0);
			if( ( it->meta[key].compare(ID) ) == 0 )
			{// equal
				it->lend_time = 0;
				break;
			}
			if( it == mainTable[val].begin() )
				break;
			else
				mainTable[val].iterator(-1);
		}
	}
}
void HashTable::output( Book* src, int time)
{
	cout<<"Book State: ";
	int src_time = src->lend_time;
	if( src_time == 0 )
	{
		cout<<0;
	}else if( time <= src_time+10 ){
		cout<<1<<' '<<src_time+10;
	}else if( time > src_time+10 ){
		cout<<2;
	}
	cout<<'\n'<<"Book Info: ";
	for( int i=0; i<6; i++)
	{
		cout<<src->meta[i]<<' ';
	}
	cout<<src->meta[6]<<'\n';
}

int main()
{
	list<Book> source;
	list<Book>::iterator src_iter;
	src_iter = source.begin();
	Book temp;
	char command;
	while( true )
	{
		cin>>command;
		if ( command=='a' ){
			for (int i = 0; i < 7; i++)
			{
				cin>>temp.meta[i];
				temp.hash[i] = hash_calc(temp.meta[i]);
			}
			temp.dele = temp.lend_time = 0;
			source.push_back(temp);
		}else{
			break;
		}
	}
	int s = source.size();
	HashTable library[7]={s,s,s,s,s,s,s};
	for (int i = 0; i < 7; i++)
	{
		library[i].setkey(i);
	}
	for( src_iter = source.begin(); src_iter != source.end(); src_iter++ )
	{
		for (int i = 0; i < 7; i++)
		{
			library[i].insert(&(*src_iter));
		}
	}
	string ID, info;
	int time, key, flag;
	switch (command)
	{
	case 'l':
		cin>>ID>>time;
		library[0].book_borrow(ID,time);
		break;
	case 'g':
		cin>>ID;
		library[0].book_return(ID);
		break;
	case 's':
		cin>>key>>info>>time;
		if( library[key].find(info,time) == 0 )
				cout<<-1<<'\n';
		break;
	default:
		break;
	}
	while( true )
	{
		cin>>command;
		if ( command == 'q' )
		{
			break;
		}
		switch (command)
		{
		case 'l':
			cin>>ID>>time;
			library[0].book_borrow(ID,time);
			break;
		case 'g':
			cin>>ID;
			library[0].book_return(ID);
			break;
		case 's':
			cin>>key>>info>>time;
			if( library[key].find(info,time) == 0 )
				cout<<-1<<'\n';
			break;
		case 'd':
			cin>>ID;
			library[0].dele(ID);
		default:
			break;
		}
	}
	//system("pause");
	return 0;
}