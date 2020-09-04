#include "Error_RegionsList.h"
#include "LogError.h"


Error_RegionsList::Error_RegionsList( ERL_Type err_type, void *start, size_t size, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start( (size_t)start )
    , m_reg_size( size )
{
    if( err_type != ERL_Type::EXISTING_REG_INSERTION && err_type != ERL_Type::OVERLAPPED_REG_INSERTION ) {
        m_errType = ERL_Type::NONE;
        m_reg_start = m_reg_size = 0;
    }
}


Error_RegionsList::Error_RegionsList( ERL_Type err_type, void *start, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start( 0 )
    , m_reg_size( 0 )
{
    if( err_type == ERL_Type::REG_WITH_SUCH_START_NOTFOUND )
        m_reg_start = (size_t)start;
    else
        m_errType = ERL_Type::NONE;
}


Error_RegionsList::Error_RegionsList( ERL_Type err_type, size_t size, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start( 0 )
    , m_reg_size( 0 )
{
    if( err_type == ERL_Type::CONSISTENT_REG_NOTFOUND )
        m_reg_size = size;
    else
        m_errType = ERL_Type::NONE;
}


Error_RegionsList::Error_RegionsList( ERL_Type err_type, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_errType( err_type )
    , m_reg_start( 0 )
    , m_reg_size( 0 )
{
    if(err_type != ERL_Type::GRAB_FROM_EMPTY_LIST && err_type != ERL_Type::P_LIST_ALLOCATION && err_type != ERL_Type::S_LIST_ALLOCATION )
        m_errType = ERL_Type::NONE;
}


std::string Error_RegionsList::Name() const
{
    return "RegionsList error";
}


void Error_RegionsList::PrintSelf() const
{
    Error_Base::PrintSelf();
    std::string concretisation;
    switch (m_errType) {
        case ERL_Type::EXISTING_REG_INSERTION:
            concretisation = "Trying to insert existing RegionP in P-List: [start = " + std::to_string(m_reg_start) + " ; size = " + std::to_string(m_reg_size) + "]";
            break;
        case ERL_Type::OVERLAPPED_REG_INSERTION:
            concretisation = "Trying to insert overlapping RegionP in P-List: [start = " + std::to_string( m_reg_start ) + " ; size = " + std::to_string( m_reg_size ) + "]";
            break;
        case ERL_Type::GRAB_FROM_EMPTY_LIST:
            concretisation = "Trying to grab the region from empty S-List (e.g. nothing to grab)";
            break;
        case ERL_Type::CONSISTENT_REG_NOTFOUND:
            concretisation = "Trying to grab the region with size " + std::to_string(m_reg_size) + ". It's greater than the S-List has (e.g. requested region is too big)";
            break;
        case ERL_Type::REG_WITH_SUCH_START_NOTFOUND:
            concretisation = "Region with such .start (" + std::to_string( m_reg_start ) + ") hasn't been found in P-List";
            break;
        case ERL_Type::P_LIST_ALLOCATION:
            concretisation = "Failed to allocate P-List";
            break;
        case ERL_Type::S_LIST_ALLOCATION:
            concretisation = "Failed to allocate S-List";
            break;
        default:
            concretisation = "Undefined ReguinsList error";
            break;
    }
    Log::error( m_logFile_name.generic_string() ) << std::string( m_tab_size, ' ' ) << concretisation << Log::endlog();
    return;
}


