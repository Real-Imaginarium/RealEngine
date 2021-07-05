#pragma once

#include "DataTypes.h"
#include "Descriptor.h"
#include "INamed.h"

#include <string>

// Способ подключения константы к пайплайну
enum class BindWay : uint8_t
{
    BY_DESCRIPTOR,          // Дескриптором
    BY_VIRTUAL_ADDRESS      // Напрямую указанием адреса ( через SetGraphicsRootConstantBufferView() )
};


template<class T>
class ConstantCPU : public INamed
{
public:
    ConstantCPU( const std::string &name, T *map_, Descriptor *cbv_ );

    ConstantCPU( const std::string &name, T *map_, D3D12_GPU_VIRTUAL_ADDRESS cbv_va_ );


    T           *map = nullptr;                         // Мап на константу в GPU
    BindWay     way_to_bind;                            // Способ подключения константы к пайплайну
    union {
        Descriptor                  *cbv = nullptr;     // Дескриптор на константу
        D3D12_GPU_VIRTUAL_ADDRESS   va;                 // Или виртуальный адрес на GPU
    };
};


template<class T>
ConstantCPU<T>::ConstantCPU( const std::string &name, T *map_, Descriptor *cbv_ )
    : INamed( name )
    , map( map_ )
    , way_to_bind( BindWay::BY_DESCRIPTOR )
    , cbv( cbv_ )
{}


template<class T>
ConstantCPU<T>::ConstantCPU( const std::string &name, T *map_, D3D12_GPU_VIRTUAL_ADDRESS cbv_va_ )
    : INamed( name )
    , map( map_ )
    , way_to_bind( BindWay::BY_VIRTUAL_ADDRESS )
    , va( cbv_va_ )
{}
