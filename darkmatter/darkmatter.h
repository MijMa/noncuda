//This is a header file
// It contains class specifications

#pragma once
#ifndef DARKMATTER_H
#define DARKMATTER_H

//Nonni, tehdaas geneerinen luokka vektorin printtaamista varten
template<class T>
class TestTemp {

public:
    T m_Obj;

private:
    TestTemp();
    void SetValue(T obj_i);
    T Getalue();
};

#endif // !DARKMATTER_H