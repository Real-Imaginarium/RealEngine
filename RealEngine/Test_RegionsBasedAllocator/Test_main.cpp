#include "RegionsBasedAllocator_Tester.h"


int main( int argc, const char **argv )
{
    std::string args_str;
    for( int i = 1; i < argc; ++i ) {
        args_str += std::string( " " ) += std::string( argv[i] );
    }
    if( args_str.find( "--help" ) != std::string::npos ) {
        std::cout << "RTFM!!!" << std::endl;
        return 0;
    }
    RegionsBasedAllocator_Tester tester;

    tester.Test_ManagedBlockCreation<uint8_t>();
    tester.Test_ManagedBlockCreation<uint64_t>();
    tester.Test_ManagedBlockCreation<double>();
    tester.Test_ManagedBlockCreation<Type_1>();

    tester.Test_Allocate_Most_Possible_And_Too_Big_Block<uint8_t>();
    tester.Test_Allocate_Most_Possible_And_Too_Big_Block<uint64_t>();
    tester.Test_Allocate_Most_Possible_And_Too_Big_Block<double>();
    tester.Test_Allocate_Most_Possible_And_Too_Big_Block<Type_1>();

    tester.Test_Alloc_Dealloc_Random<uint8_t>();
    tester.Test_Alloc_Dealloc_Random<uint16_t>();
    tester.Test_Alloc_Dealloc_Random<uint32_t>();
    tester.Test_Alloc_Dealloc_Random<uint64_t>();

    tester.Test_Allocator_Performance();

    system( "Pause" );
    return 0;
}