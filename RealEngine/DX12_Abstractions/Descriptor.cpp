#include "DescriptorsAllocator.h"
#include "LogWarning.h"


Descriptor::Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res )
    : INamed( name )
    , m_rtv_desc( desc )
    , m_device( dev )
    , m_type( DescriptorType::RTV )
    //, m_name( name )
    , m_res( res )
    , m_cnt_res( nullptr )
    , m_alloc( nullptr )
{
    if( m_cpu_handle.ptr == NULL || m_gpu_handle.ptr == NULL)
    {
        std::cout << "Descriptor::Descriptor() - catcha!";
        throw "Error";
    }
    m_device->CreateRenderTargetView( m_res, m_rtv_desc.ViewDimension ? &m_rtv_desc : nullptr, m_cpu_handle );
}


Descriptor::Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res )
    : INamed( name )
    , m_dsv_desc( desc )
    , m_device( dev )
    , m_type( DescriptorType::DSV )
    //, m_name( name )
    , m_res( res )
    , m_cnt_res( nullptr )
    , m_alloc( nullptr )
{
    m_device->CreateDepthStencilView( m_res, m_dsv_desc.ViewDimension ? &m_dsv_desc : nullptr, m_cpu_handle );
}


Descriptor::Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res )
    : INamed( name )
    , m_uav_desc( desc )
    , m_device( dev )
    , m_type( DescriptorType::UAV )
    //, m_name( name )
    , m_res( res )
    , m_cnt_res( cnt_res )
    , m_alloc( nullptr )
{
    m_device->CreateUnorderedAccessView( m_res, m_cnt_res, m_uav_desc.ViewDimension ? &m_uav_desc : nullptr, m_cpu_handle );
}


Descriptor::Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res )
    : INamed( name )
    , m_srv_desc( desc )
    , m_device( dev )
    , m_type( DescriptorType::SRV )
    //, m_name( name )
    , m_res( res )
    , m_cnt_res( nullptr )
    , m_alloc( nullptr )
{
    m_device->CreateShaderResourceView( m_res, m_srv_desc.ViewDimension ? &m_srv_desc : nullptr, m_cpu_handle );
}


Descriptor::Descriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc )
    : INamed( name )
    , m_cbv_desc( desc )
    , m_device( dev )
    , m_type( DescriptorType::CBV )
    //, m_name( name )
    , m_res( nullptr )
    , m_cnt_res( nullptr )
    , m_alloc( nullptr )
{
    m_device->CreateConstantBufferView( &m_cbv_desc, m_cpu_handle );
}


Descriptor::Descriptor( const Descriptor &other ) : INamed( other )
{
    m_device = other.m_device;
    m_type = other.m_type;
    m_name = other.m_name;
    m_res = other.m_res;
    m_cnt_res = other.m_cnt_res;
    m_alloc = other.m_alloc;

    switch( m_type ) {
    case DescriptorType::RTV: m_rtv_desc = other.m_rtv_desc; m_device->CreateRenderTargetView( m_res, m_rtv_desc.ViewDimension ? &m_rtv_desc : nullptr, m_cpu_handle ); break;
    case DescriptorType::DSV: m_dsv_desc = other.m_dsv_desc; m_device->CreateDepthStencilView( m_res, m_dsv_desc.ViewDimension ? &m_dsv_desc : nullptr, m_cpu_handle ); break;
    case DescriptorType::UAV: m_uav_desc = other.m_uav_desc; m_device->CreateUnorderedAccessView( m_res, m_cnt_res, m_uav_desc.ViewDimension ? &m_uav_desc : nullptr, m_cpu_handle ); break;
    case DescriptorType::SRV: m_srv_desc = other.m_srv_desc; m_device->CreateShaderResourceView( m_res, m_srv_desc.ViewDimension ? &m_srv_desc : nullptr, m_cpu_handle ); break;
    case DescriptorType::CBV: m_cbv_desc = other.m_cbv_desc; m_device->CreateConstantBufferView( &m_cbv_desc, m_cpu_handle ); break;
    }
}


UPtr_Descriptor Descriptor::CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    return UPtr_Descriptor( CreateDescriptor( dev, name, desc, res, alloc ), []( Descriptor *p ) { Descriptor::DestroyDescriptor( p ); } );
}


UPtr_Descriptor Descriptor::CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    return UPtr_Descriptor( CreateDescriptor( dev, name, desc, res, alloc ), []( Descriptor *p ) { Descriptor::DestroyDescriptor( p ); } );
}


UPtr_Descriptor Descriptor::CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, DescriptorsAllocator *alloc )
{
    return UPtr_Descriptor( CreateDescriptor( dev, name, desc, res, cnt_res, alloc ), []( Descriptor *p ) { Descriptor::DestroyDescriptor( p ); } );
}


UPtr_Descriptor Descriptor::CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    return UPtr_Descriptor( CreateDescriptor( dev, name, desc, res, alloc ), []( Descriptor *p ) { Descriptor::DestroyDescriptor( p ); } );
}


UPtr_Descriptor Descriptor::CreateDescriptorUPtr( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, DescriptorsAllocator *alloc )
{
    return UPtr_Descriptor( CreateDescriptor( dev, name, desc, alloc ), []( Descriptor *p ) { Descriptor::DestroyDescriptor( p ); } );
}


Descriptor* Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    Descriptor *descr = (Descriptor*)alloc->Allocate( 1 );
    if( descr->m_cpu_handle.ptr == NULL )
    {
        std::cout << "Descriptor::CreateDescriptor() - catcha CPU!";
        throw "Error";
    }
    if( descr->m_gpu_handle.ptr == NULL )
    {
        std::cout << "Descriptor::CreateDescriptor() - catcha GPU!";
        throw "Error";
    }
    CreateDescriptor( dev, name, desc, res, descr );
    descr->RememberAllocator( alloc );
    return descr;
}


Descriptor* Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    Descriptor *descr = (Descriptor*)alloc->Allocate( 1 );
    CreateDescriptor( dev, name, desc, res, descr );
    descr->RememberAllocator( alloc );
    return descr;
}


Descriptor* Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, DescriptorsAllocator *alloc )
{
    Descriptor *descr = (Descriptor*)alloc->Allocate( 1 );
    CreateDescriptor( dev, name, desc, res, cnt_res, descr );
    descr->RememberAllocator( alloc );
    return descr;
}


Descriptor* Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, DescriptorsAllocator *alloc )
{
    Descriptor *descr = (Descriptor*)alloc->Allocate( 1 );
    CreateDescriptor( dev, name, desc, res, descr );
    descr->RememberAllocator( alloc );
    return descr;
}


Descriptor* Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, DescriptorsAllocator *alloc )
{
    Descriptor *descr = (Descriptor*)alloc->Allocate( 1 );
    CreateDescriptor( dev, name, desc, descr );
    descr->RememberAllocator( alloc );
    return descr;
}


void Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_RENDER_TARGET_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr )
{
    ::new( (void *)out_descr ) Descriptor( dev, name, desc, res );
}


void Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr )
{
    ::new( (void *)out_descr ) Descriptor( dev, name, desc, res );
}


void Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc, ID3D12Resource *res, ID3D12Resource *cnt_res, Descriptor *out_descr )
{
    ::new( (void *)out_descr ) Descriptor( dev, name, desc, res, cnt_res );
}


void Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc, ID3D12Resource *res, Descriptor *out_descr )
{
    ::new( (void *)out_descr ) Descriptor( dev, name, desc, res );
}


void Descriptor::CreateDescriptor( ID3D12Device *dev, const std::string &name, const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc, Descriptor *out_descr )
{
    ::new( (void *)out_descr ) Descriptor( dev, name, desc );
}


void Descriptor::DestroyDescriptor( Descriptor *p )
{
    if( !p || !p->m_alloc )
    {
        Log::warning() << "Failed to Deallocate descriptor: don't know what allocator it's belonged to" << Log::endlog{};
        return;
    }
    auto alloc = p->m_alloc;
    DestroyDescriptor( p, alloc );
}


void Descriptor::DestroyDescriptor( Descriptor *p, DescriptorsAllocator *alloc )
{
    p->~Descriptor();
    alloc->Deallocate( p, 1 );
}


Descriptor::~Descriptor()
{
    m_uav_desc = {};
    m_device = nullptr;
    m_type = DescriptorType::UNDEFINED;
    m_name.clear();
    m_res = nullptr;
    m_cnt_res = nullptr;
    m_alloc = nullptr;
}