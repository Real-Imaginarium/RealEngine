#pragma once

#include "d3dx12.h"

#include "INamed.h"

#include <functional>
#include <memory>
#include <string>


struct ID3D12Device;
class Descriptor;
class DescriptorsAllocator;
template<class T, class Alloc>
class Vector;

using UPtr_Descriptor = std::unique_ptr<Descriptor, std::function<void( Descriptor * )>>;


enum class DescriptorType : uint8_t
{
    UNDEFINED, RTV, DSV, CBV, SRV, UAV
};


//    ќбъект данного класса может быть создан только на участке пам€ти, полученном от DescriptorsAllocator'а. ¬ этом участке куски пам€ти, соответствующие 'm_cpu_handle'
//  и 'm_gpu_handle' уже инициализированы и готовы к использованию.  онструктор Descriptor'а использует поле 'm_cpu_handle' дл€ создани€ нативного DirectC'12 дескриптора
//  на GPU.
//     онструкторы скрыты дл€ предотвращени€ возможности построени€ дескриптора на участке пам€ти,  не принадлежащем DescriptorsAllocator'у,  т.к. это приведЄт к крашу в
//  рантайме. ¬место этого созданием/разрушением дескриптора занимаютс€ статические методы CreateDescriptor(...) и DestroyDescriptor(...) в разных вариаци€х.
class Descriptor : public INamed
{
    friend class ManagedBlockD_Impl;
    friend class Vector<Descriptor, DescriptorsAllocator>;

public:
    static UPtr_Descriptor CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static UPtr_Descriptor CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static UPtr_Descriptor CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, DescriptorsAllocator *alloc );
    static UPtr_Descriptor CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static UPtr_Descriptor CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, DescriptorsAllocator *alloc );

    static Descriptor* CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static Descriptor* CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static Descriptor* CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, DescriptorsAllocator *alloc );
    static Descriptor* CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc );
    static Descriptor* CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, DescriptorsAllocator *alloc );

    static void CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr );
    static void CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr );
    static void CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, Descriptor *out_descr );
    static void CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr );
    static void CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, Descriptor *out_descr );

    static void DestroyDescriptor( Descriptor *p );
    static void DestroyDescriptor( Descriptor *p, DescriptorsAllocator *alloc );

    void RememberAllocator( DescriptorsAllocator *alloc ) { m_alloc = alloc; }

    DescriptorType Type() const { return m_type; }

    //std::string Name() const { return m_name; }

    CD3DX12_CPU_DESCRIPTOR_HANDLE CPU_Handle() const { return m_cpu_handle; }

    CD3DX12_GPU_DESCRIPTOR_HANDLE GPU_Handle() const { return m_gpu_handle; }

private:
    Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res );
    Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res );
    Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res );
    Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res );
    Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc );

    Descriptor( const Descriptor &other );

    //Descriptor( Descriptor &&temp ) = delete;

    ~Descriptor();

    union {
        D3D12_RENDER_TARGET_VIEW_DESC       m_rtv_desc;
        D3D12_DEPTH_STENCIL_VIEW_DESC       m_dsv_desc;
        D3D12_UNORDERED_ACCESS_VIEW_DESC    m_uav_desc;
        D3D12_SHADER_RESOURCE_VIEW_DESC     m_srv_desc;
        D3D12_CONSTANT_BUFFER_VIEW_DESC     m_cbv_desc;
    };
    ID3D12Device                            *m_device;
    DescriptorType                          m_type;
    //std::string                             m_name;
    ID3D12Resource                          *m_res;
    ID3D12Resource                          *m_cnt_res;
    DescriptorsAllocator                    *m_alloc;
    CD3DX12_CPU_DESCRIPTOR_HANDLE           m_cpu_handle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE           m_gpu_handle;
};
