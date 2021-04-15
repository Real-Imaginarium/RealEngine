
#include "AtomicConstantsAllocator.h"
#include "DescriptorsAllocator.h"
#include "PullAllocator.h"
#include "RegionsAllocator.h"

#include "Constant.hpp"


template<class T>
class ConstantsPull
{
public:
    ConstantsPull( ID3D12Device *dev, size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc, DescriptorsAllocator  *cbv_alloc );

private:
    size_t                      m_size_of_constant = 0;                                             // Реальный размер константы на основе sizeof(T) - кратный 256 байтам
    //PullAllocator<Constant<T>>  m_constants;
    RegionsAllocator            *m_general_alloc = nullptr;
    AtomicConstantsAllocator    *m_atomic_constants_alloc = nullptr;
    DescriptorsAllocator        *m_cbv_alloc = nullptr;                 // НЕ НУЖЕН, ЗАПОМИНАЕТСЯ В ДЕСКРИПТОРЕ
    ID3D12Device                *m_device = nullptr;
};


template<class T>
ConstantsPull<T>::ConstantsPull( ID3D12Device *dev, size_t constants_num, RegionsAllocator *general_alloc, AtomicConstantsAllocator *atomic_constants_alloc, DescriptorsAllocator *cbv_alloc )
    : m_size_of_constant(( sizeof( T ) + 255 ) & ~255)
    , m_general_alloc( general_alloc )
    , m_atomic_constants_alloc( atomic_constants_alloc )
    , m_cbv_alloc( cbv_alloc )
    , m_device( dev )
{
    // Получаем кусок памяти у общего аллокатора для размещения 'constants_num' констант. После заполнения константами, этой памятью будет
    // инициализирован внутренний PullAllocator<Constant<T>>
    Constant<T> *mem_start = (Constant<T>*)general_alloc->Allocate( constants_num * sizeof( Constant<T> ));

    for( size_t i = 0; i < constants_num; ++i )
    {
        T *mapped = (T*)atomic_constants_alloc->Allocate( m_size_of_constant / 256 );             // Запрашиваем соответствующее количество элементарных констант

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = atomic_constants_alloc->CalculateVirtualAddress( mapped );            // Получаем виртуальный адрес ресурса, на который получили мап
        desc.SizeInBytes = static_cast<UINT>(m_size_of_constant);

        Descriptor *cbv = Descriptor::CreateDescriptor( dev, "constant_" + std::to_string( i ), desc, cbv_alloc );

        ::new( (void *)mem_start ) Constant<T>( mapped, cbv );
    }
}