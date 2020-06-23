#include "Error_RegionsList.h"
#include "LogError.h"

Error_RegionsList::Error_RegionsList( RegList_ErrType errType, size_t reg_start, size_t reg_size, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base(file_name, func_name, line, logFile_name)
    , m_errType(errType)
    , m_reg_start(reg_start)
    , m_reg_size(reg_size)
{}

Error_RegionsList::Error_RegionsList( RegList_ErrType errType, size_t reg_size, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_errType( errType )
    , m_reg_start( 0 )
    , m_reg_size( reg_size )
{}

Error_RegionsList::Error_RegionsList( RegList_ErrType errType, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_errType( errType )
    , m_reg_start( 0 )
    , m_reg_size( 0 )
{}

std::string Error_RegionsList::Name() const
{
    return "RegionsList error";
}

void Error_RegionsList::PrintSelf() const
{
    Error_Base::PrintSelf();
    std::string concretisation;
    switch (m_errType) {
        case RegList_ErrType::ERR_EXISTING_REG_INSERTION: 
            concretisation = "Trying to insert existing RegionP value in P-List: [start = " + std::to_string(m_reg_start) + " ; size = " + std::to_string(m_reg_size) + "]";
            break;
        case RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND:
            concretisation = "Trying to grab the region with size " + std::to_string(m_reg_size) + ". It's greater than the S-List has (e.g. requested region is too big)";
            break;
        case RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST:
            concretisation = "Trying to grab the region from empty S-List (e.g. nothing to grab)";
            break;
        case RegList_ErrType::ERR_REG_WITH_SUCH_START_NOTFOUND:
            concretisation = "Region with such .start (" + std::to_string( m_reg_start ) + ") hasn't been found in P-List";
            break;
        case RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION:
            concretisation = "Trying to insert overlapping RegionP value in P-List: [start = " + std::to_string( m_reg_start ) + " ; size = " + std::to_string( m_reg_size ) + "]";
            break;
    }
    Log::error( m_logFile_name.generic_string() ) << std::string( m_tab_size, ' ' ) << concretisation << Log::endlog();
    return;
}
