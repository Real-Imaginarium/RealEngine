#include "Error_RegionsList.h"
#include "LogError.h"
#include "Error_Allocator.h"


Error_Allocator::Error_Allocator( EA_Type err_type, void *dealloc_start, size_t dealloc_size, void *managed_start, size_t managed_size, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start_1( (size_t)dealloc_start )
    , m_reg_size_1( dealloc_size )
    , m_reg_start_2( (size_t)managed_start )
    , m_reg_size_2( managed_size )
{
    if( err_type != EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS ) {
        m_errType = EA_Type::NONE;
        m_reg_start_1 = m_reg_start_2 = m_reg_size_1 = m_reg_size_2 = 0;
    }
}


Error_Allocator::Error_Allocator( EA_Type err_type, void *managed_start, size_t managed_size, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start_1( (size_t)managed_start )
    , m_reg_size_1( managed_size )
    , m_reg_start_2( 0 )
    , m_reg_size_2( 0 )
{
    if( err_type != EA_Type::REGIONS_LIST_CREATION_ERROR ) {
        m_errType = EA_Type::NONE;
        m_reg_start_1 = m_reg_start_2 = m_reg_size_1 = m_reg_size_2 = 0;
    }
}


Error_Allocator::Error_Allocator( EA_Type err_type, size_t size, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start_1( 0 )
    , m_reg_size_1( size )
    , m_reg_start_2( 0 )
    , m_reg_size_2( 0 )
{
    if( err_type != EA_Type::ALLOC_SIZE_TOO_BIG && err_type != EA_Type::MANAGED_BLOCK_CREATION_ERROR ) {
        m_errType = EA_Type::NONE;
        m_reg_start_1 = m_reg_start_2 = m_reg_size_1 = m_reg_size_2 = 0;
    }
}


std::string Error_Allocator::Name() const
{
    return "Allocator error";
}


void Error_Allocator::PrintSelf() const
{
    Error_Base::PrintSelf();
    std::string concretisation;
    switch( m_errType ) {
    case EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS:
        concretisation =
            "Trying to deallocate the memory which is out of block managed by allocator.\nDeallocated block: [start = " +
            std::to_string( m_reg_start_1 ) + " ; size = " + std::to_string( m_reg_size_1 ) + "]\nManaged block:     [start = " +
            std::to_string( m_reg_start_2 ) + " ; size = " + std::to_string( m_reg_size_2 ) + "]";
        break;
    case EA_Type::ALLOC_SIZE_TOO_BIG:
        concretisation = "Trying to allocate too big memory block (" + std::to_string( m_reg_size_1 ) + " cells) than allocator has";
        break;
    case EA_Type::MANAGED_BLOCK_CREATION_ERROR:
        concretisation = "Can't aquire managed memory block of such size: " + std::to_string( m_reg_size_1 );
        break;
    case EA_Type::REGIONS_LIST_CREATION_ERROR:
        concretisation = "Can't create RegionsList for allocator. Managed block is: [start = " + std::to_string( m_reg_start_1 ) + " ; size = " + std::to_string( m_reg_size_1 ) + "]";
        break;
    default:
        concretisation = "Undefined Allocator error";
        break;
    }
    Log::error( m_logFile_name.generic_string() ) << std::string( m_tab_size, ' ' ) << concretisation << Log::endlog();
    return;
}