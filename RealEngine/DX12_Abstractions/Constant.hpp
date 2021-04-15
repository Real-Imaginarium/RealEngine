#pragma once

#include "Descriptor.h"


template<class T>
class Constant
{
public:
    Constant(T *mapped, Descriptor *cbv);


private:
    T           *m_mapped_ptr = nullptr;    // ”казатель, отображаемый на константу в GPU (живущую в Buffer-ресурсе)
    Descriptor  *m_cbv = nullptr;           // ƒескриптор на константу (с его помощью константа подключаетс€ к DX'12-pipline)
};



template<class T>
Constant<T>::Constant( T *mapped, Descriptor *cbv )
    : m_mapped_ptr( mapped )
    , m_cbv( cbv )
{}