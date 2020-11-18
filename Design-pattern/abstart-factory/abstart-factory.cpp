#include<iostream>
using namespace std;

class ProductA{
public:
    virtual ~ProductA(){}
    virtual const char* GetName() = 0;
};

class ProductAX : public ProductA{
public:
    ~ProductAX(){}
    const char* GetName(){
        return "AX";
    }
};

class ProductAY : public ProductA{
public:
    ~ProductAY(){}
    const char* GetName(){
        return "AY";
    }
};

class ProductB{
public:
    virtual ~ProductB(){}
    virtual const char* GetName() = 0;
};

class ProductBX : public ProductB{
public:
    ~ProductBX(){}
    const char* GetName(){
        return "BX";
    }
};

class ProductBY : public ProductB{
public:
    ~ProductBY(){}
    const char* GetName(){
        return "BY";
    }
};

//创建一个工厂类专门用于创建对象，这样如果要加入一个新的产品，则只需要设计新产品后在工厂类中加入即可
class AbstartFactory{
public:
    virtual ~AbstartFactory(){}
    ProductAX *CreateProductAX(){
        return new ProductAX();
    }
    ProductAY *CreateProductAY(){
        return new ProductAY();
    }
    ProductBX *CreateProductBX(){
        return new ProductBX();
    }
    ProductBY *CreateProductBY(){
        return new ProductBY();
    }
};

int main(){
    AbstartFactory *factory = new AbstartFactory();
    ProductAX *p1= factory->CreateProductAX();
    ProductAY *p2= factory->CreateProductAY();
    ProductBX *p3= factory->CreateProductBX();
    ProductBY *p4=factory->CreateProductBY();
    cout<<p1->GetName()<<" "<<p2->GetName()<<" "<<p3->GetName()<<" "<<p4->GetName()<<endl;
    delete factory;
    return 0;
}
