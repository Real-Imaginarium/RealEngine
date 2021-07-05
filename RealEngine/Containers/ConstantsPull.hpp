#pragma once

#include "AtomicConstantsAllocator.h"
#include "DescriptorsAllocator.h"
#include "PullAllocator.h"
#include "RegionsAllocator.h"

#include "Constant.hpp"


template<class T>
class ConstantsPull
{
public:
    ConstantsPull();

    ConstantsPull( size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc );

    ~ConstantsPull();

    void Initialize( size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc );

    void Deinitialize();

    ConstantCPU<T> *Take();

    Error_BasePtr ReturnBack( ConstantCPU<T> *c );

private:
    size_t                          m_size_256 = 0;                                             // –еальный размер константы на основе sizeof(T) - кратный 256 байтам
    PullAllocator<ConstantCPU<T>>   m_cpu_constants;
    RegionsAllocator                *m_general_alloc = nullptr;
    AtomicConstantsAllocator        *m_atomic_constants_alloc = nullptr;
    bool                            m_ready_to_use = false;
};


template<class T>
ConstantsPull<T>::ConstantsPull()
    : m_size_256( ( sizeof( T ) + 255 ) & ~255 )
    , m_cpu_constants( static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ))
{}


template<class T>
ConstantsPull<T>::ConstantsPull( size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc )
    : m_size_256(( sizeof( T ) + 255 ) & ~255)
    , m_cpu_constants( static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ))
{
    Initialize( constants_num, general_alloc, atomic_constants_alloc );
}


template<class T>
ConstantsPull<T>::~ConstantsPull()
{
    Deinitialize();
}


template<class T>
void ConstantsPull<T>::Initialize( size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc )
{
    if( m_ready_to_use ) {
        return;
    }
    m_general_alloc = general_alloc;
    m_atomic_constants_alloc = atomic_constants_alloc;

    // ѕолучаем кусок пам€ти у общего аллокатора дл€ размещени€ 'constants_num' констант. ѕосле заполнени€ константами, этой пам€тью будет
    // инициализирован внутренний PullAllocator<ConstantCPU>
    ConstantCPU<T> *mem_start = ( ConstantCPU<T> * )m_general_alloc->Allocate( constants_num * sizeof( ConstantCPU<T> ) );

    for( size_t i = 0; i < constants_num; ++i )
    {
        T *mapped = (T *)m_atomic_constants_alloc->Allocate( m_size_256 / 256 );                 // «апрашиваем соответствующее количество элементарных констант

        ::new( (void *)( mem_start + i ) ) ConstantCPU<T>( "constant_" + std::to_string( i ), mapped, m_atomic_constants_alloc->CalculateVirtualAddress( mapped ) );
    }
    m_cpu_constants.SetupManagedBlock( (uint8_t *)mem_start, constants_num * sizeof( ConstantCPU<T> ) );
    m_ready_to_use = true;
}


template<class T>
void ConstantsPull<T>::Deinitialize()
{
    if( !m_ready_to_use ) {
        return;
    }
    // ¬ернуть в RegionsAllocator и AtomicConstantsAllocator всЄ что вз€ли
    m_ready_to_use = false;
}


template<class T>
ConstantCPU<T> *ConstantsPull<T>::Take()
{
    return m_cpu_constants.Allocate();
}


template<class T>
Error_BasePtr ConstantsPull<T>::ReturnBack( ConstantCPU<T> *c )
{
    *(T*)(c->map) = {};
    return m_cpu_constants.Deallocate( c );
}
