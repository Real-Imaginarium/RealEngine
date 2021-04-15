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
    RegionsAllocator_Tester tester;

    tester.Test_ManagedBlockCreation();
    tester.Test_Allocate_Most_Possible_And_Too_Big_Block();
    tester.Test_Alloc_Dealloc_Random();


    std::vector<RegionsAllocator_Tester::All_Perf_Test_Config> configs{
        {1000, 50,  1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 100, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 150, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 200, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {1000, 250, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 300, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 350, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 400, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {1000, 450, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {1000, 500, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 50,  1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 100, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {2000, 150, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 200, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 250, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 300, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {2000, 350, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 400, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 450, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 500, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {2000, 550, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 600, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 650, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 700, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {2000, 750, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 800, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 850, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 900, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {2000, 950, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {2000, 1000,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 50,  1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 100, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 150, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 200, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 250, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 300, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 350, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 400, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 450, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 500, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 550, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 600, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 650, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 700, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 750, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 800, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 850, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 900, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 950, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1000,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1050,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1100,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 1150,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1200,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1250,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1300,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {3000, 1350,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1400,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1450,1, 1000, 1, 1000, "1_1000_diap.txt"}, {3000, 1500,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 50,  1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 100, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 150, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 200, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 250, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 300, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 350, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 400, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 450, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 500, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 550, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 600, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 650, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 700, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 750, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 800, 1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 850, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 900, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 950, 1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1000,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 1050,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1200,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1250,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1300,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 1350,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1400,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1450,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1500,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 1550,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1600,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1650,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1700,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 1750,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1800,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1850,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 1900,1, 1000, 1, 1000, "1_1000_diap.txt"},
        {4000, 1950,1, 1000, 1, 1000, "1_1000_diap.txt"}, {4000, 2000,1, 1000, 1, 1000, "1_1000_diap.txt"}
    };

    for( auto &config : configs ) {
        config.min_initial_obj_size = 1;
        config.max_initial_obj_size = 100;
        config.min_ins_size = 1;
        config.max_ins_size = 100;
        config.logFile = "1_100_diap.txt";
        tester.Test_Allocator_Performance( config );
    }
    for( auto &config : configs ) {
        config.min_initial_obj_size = 100;
        config.max_initial_obj_size = 1000;
        config.min_ins_size = 100;
        config.max_ins_size = 1000;
        config.logFile = "100_1000_diap.txt";
        tester.Test_Allocator_Performance( config );
    }
    for( auto &config : configs ) {
        config.min_initial_obj_size = 500;
        config.max_initial_obj_size = 600;
        config.min_ins_size = 500;
        config.max_ins_size = 600;
        config.logFile = "500_600_diap.txt";
        tester.Test_Allocator_Performance( config );
    }
    for( auto &config : configs ) {
        config.min_initial_obj_size = 1;
        config.max_initial_obj_size = 10;
        config.min_ins_size = 1;
        config.max_ins_size = 10;
        config.logFile = "1_10_diap.txt";
        tester.Test_Allocator_Performance( config );
    }

    system( "Pause" );
    
    return 0;
}