#pragma once

#include "DataTypes.h"
#include "Descriptor.h"
#include "INamed.h"

#include <string>

// ������ ����������� ��������� � ���������
enum class BindWay : uint8_t
{
    BY_DESCRIPTOR,          // ������������
    BY_VIRTUAL_ADDRESS      // �������� ��������� ������ ( ����� SetGraphicsRootConstantBufferView() )
};


template<class T>
class ConstantCPU : public INamed
{
public:
    ConstantCPU( const std::string &name, T *map_, Descriptor *cbv_ );

    ConstantCPU( const std::string &name, T *map_, D3D12_GPU_VIRTUAL_ADDRESS cbv_va_ );


    T           *map = nullptr;                         // ��� �� ��������� � GPU
    BindWay     way_to_bind;                            // ������ ����������� ��������� � ���������
    union {
        Descriptor                  *cbv = nullptr;     // ���������� �� ���������
        D3D12_GPU_VIRTUAL_ADDRESS   va;                 // ��� ����������� ����� �� GPU
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
