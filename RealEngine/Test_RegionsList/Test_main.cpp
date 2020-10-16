#include "RegionsList_Tester.h"
#include "LogDebug.h"
#include "LogError.h"
#include "LogWarning.h"
#include "LogInfo.h"
#include "LogTest.h"

#include <iostream>
#include <memory>


int main(int argc, const char** argv)
{
    std::string args_str;
    for (int i = 1; i < argc; ++i) {
        args_str += std::string( " " ) += std::string( argv[i] );
    }

    if (args_str.find( "--help" ) != std::string::npos) {
        std::cout << "Usage: Test_RegionsList [ testcase name ][--iterations = xxx]\n" \
            "Possible testcase names are:\n" \
            "--DoubleInserion\n--OverlappedInsertion\n--GrabbingFromEmptyList\n--GrabbingTooBigRegion\n--ListsExpanding\n--InitialReleaseRegion\n--SecondRelease_LeftRightAdj\n" \
            "--SecondRelease_Left_Less_Size\n--SecondRelease_Left_Great_Size\n--SecondRelease_Left_Equal_Size\n--SecondRelease_Right_Less_Size\n--SecondRelease_Right_Great_Size\n" \
            "--SecondRelease_Right_Equal_Size\n--GrabbingFromSingleSizedList\n--GrabingsComplex\n--InsertionsComplex\n--All\n";
        return 0;
    }

    bool AllTests = args_str.find( "--All" ) != std::string::npos || argc == 1;
    bool DoubleInserion = args_str.find( "--DoubleInserion" ) != std::string::npos;
    bool OverlappedInsertion = args_str.find( "--OverlappedInsertion" ) != std::string::npos;
    bool GrabbingFromEmptyList = args_str.find( "--GrabbingFromEmptyList" ) != std::string::npos;
    bool GrabbingTooBigRegion = args_str.find( "--GrabbingTooBigRegion" ) != std::string::npos;
    bool ListsExpanding = args_str.find( "--ListsExpanding" ) != std::string::npos;
    bool InitialReleaseRegion = args_str.find( "--InitialReleaseRegion" ) != std::string::npos;
    bool SecondRelease_LeftRightAdj = args_str.find( "--SecondRelease_LeftRightAdj" ) != std::string::npos;
    bool SecondRelease_Left_Less_Size = args_str.find( "--SecondRelease_Left_Less_Size" ) != std::string::npos;
    bool SecondRelease_Left_Great_Size = args_str.find( "--SecondRelease_Left_Great_Size" ) != std::string::npos;
    bool SecondRelease_Left_Equal_Size = args_str.find( "--SecondRelease_Left_Equal_Size" ) != std::string::npos;
    bool SecondRelease_Right_Less_Size = args_str.find( "--SecondRelease_Right_Less_Size" ) != std::string::npos;
    bool SecondRelease_Right_Great_Size = args_str.find( "--SecondRelease_Right_Great_Size" ) != std::string::npos;
    bool SecondRelease_Right_Equal_Size = args_str.find( "--SecondRelease_Right_Equal_Size" ) != std::string::npos;
    bool GrabbingFromSingleSizedList = args_str.find( "--GrabbingFromSingleSizedList" ) != std::string::npos;
    bool GrabingsComplex = args_str.find( "--GrabingsComplex" ) != std::string::npos;
    bool InsertionsComplex = args_str.find( "--InsertionsComplex" ) != std::string::npos;
    bool GrabbingsInsertionsRandom = args_str.find( "--GrabbingsInsertionsRandom" ) != std::string::npos;

    std::string iterations_str;
    Error_BasePtr err = nullptr;
    bool founded;
    std::tie( err, iterations_str ) = utils::FindStrRegular( args_str, Regex( "[\\s]+--iterations[\\s=]+\\d+" ), founded );         TRACE_CUSTOM_PRNT_RET_VAL( err, -1, "Error in search \"--iterations\" arg" );
    std::tie( err, iterations_str ) = utils::FindStrRegular( iterations_str, Regex( "\\d+" ), founded );                            TRACE_CUSTOM_PRNT_RET_VAL( err, -1, "Error in search \"--iterations\" arg" );

    int iterations = iterations_str.empty() ? 1 : std::stoi( iterations_str );
    iterations = iterations == 0 ? 1 : iterations;

    std::unique_ptr<RegionsList_Tester> tester = std::make_unique<RegionsList_Tester>();

    for( int i = 0; i < 50; ++i ) {
        if( DoubleInserion || AllTests ) {
            tester->Test_DoubleInserion();
        }
        if( OverlappedInsertion || AllTests ) {
            tester->Test_OverlappedInsertion();
        }
        if( GrabbingFromEmptyList || AllTests ) {
            tester->Test_GrabbingFromEmptyList();
        }
        if( GrabbingTooBigRegion || AllTests ) {
            tester->Test_GrabbingTooBigRegion();
        }
        if( ListsExpanding || AllTests ) {
            tester->Test_ListsReorganise();
        }
        if( InitialReleaseRegion || AllTests ) {
            tester->Test_InitialReleaseRegion();
        }
        if( SecondRelease_LeftRightAdj || AllTests ) {
            tester->Test_SecondRelease_LeftRightAdj();
        }
        if( SecondRelease_Left_Less_Size || AllTests ) {
            tester->Test_SecondRelease_Left_Less_Size();
        }
        if( SecondRelease_Left_Great_Size || AllTests ) {
            tester->Test_SecondRelease_Left_Great_Size();
        }
        if( SecondRelease_Left_Equal_Size || AllTests ) {
            tester->Test_SecondRelease_Left_Equal_Size();
        }
        if( SecondRelease_Right_Less_Size || AllTests ) {
            tester->Test_SecondRelease_Right_Less_Size();
        }
        if( SecondRelease_Right_Great_Size || AllTests ) {
            tester->Test_SecondRelease_Right_Great_Size();
        }
        if( SecondRelease_Right_Equal_Size || AllTests ) {
            tester->Test_SecondRelease_Right_Equal_Size();
        }
        if( GrabbingFromSingleSizedList || AllTests ) {
            tester->Test_GrabbingFromSingleSizedList();
        }
    }

    if (GrabingsComplex || AllTests) {
        tester->Test_GrabbingsComplex();
    }
    if (InsertionsComplex || AllTests) {
        tester->Test_InsertionsComplex();
    }
    if (GrabbingsInsertionsRandom || AllTests) {
                                                        /* Trans   MemSize  Min  Max     */
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  1000,    0,   0      } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  1000,    0,   1      } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  1000,    1,   1      } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  100,     99,  100    } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  100,     101, 101    } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  100,     100, 101    } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  100,     100, 100    } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 10000,  100,     0,   100    } );
        tester->Test_GrabbingsInsertionsRandom( ConfigGIR{ 100000, 100000,  0,   120000 } );
    }
    system( "Pause" );
    return 0;
}