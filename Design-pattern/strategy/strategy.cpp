//策略模式
//即不同的算法表现出不同的行为，但是这些算法的调用来说又是平行的，因此实现一种策略，使得不同的算法继承该策略，则通过该策略即可调用不同的算法
//同时实现一个环境类，该环境类中调用基类的策略，则用户调用该环境的同时注入不同的算法，即可实现通过基类的策略来调用不同的算法的要求
//若以后需要产生新的算法，则只需产生新的算法继承初始策略，用户调用环境时注入新的算法即可
#include <iostream>
using namespace std;

class Strategy{
public:
    virtual ~Strategy(){}
    virtual void algorithm() = 0;
};

class ConcreteStrategyA :public Strategy{
public:
    ~ConcreteStrategyA(){}

    void algorithm(){
        cout<<"algorithm A\n";
    }
};

class ConcreteStrategyB : public Strategy{
public:
    ~ConcreteStrategyB(){}
    void algorithm(){
        cout<<"algorithm B\n";
    }
};

class ConcreteStrategyC : public Strategy{
public:
    ~ConcreteStrategyC(){}
    void algorithm(){
        cout << "algorithm C\n";
    }
};

class Context{
private:
    Strategy *strategy;
public:
    Context(Strategy *s):strategy(s){}
    ~Context(){
        delete strategy;
    }
    void context(){
        strategy->algorithm();
    }
};
int main()
{
    Context c = new ConcreteStrategyA;
    c.context();
    return 0;
}
