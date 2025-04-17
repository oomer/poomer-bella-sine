#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <atomic>
#include <signal.h>
#include <tuple>
#include <cmath>

#include "../bella_engine_sdk/src/bella_sdk/bella_engine.h" // For rendering and scene creation in Bella
#include "../bella_engine_sdk/src/dl_core/dl_main.inl" // Core functionality from the Diffuse Logic engine
#include "../oom/oom_license.h" // common misc code
#include "../oom/oom_bella_long.h"    // common misc code
#include "../oom/oom_bella_engine.h"    // common misc code
#include "../oom/oom_bella_premade.h"    // common misc code
#include "../oom/oom_bella_misc.h"    // common misc code

//==============================================================================
// GLOBAL VARIABLES AND FUNCTIONS
//==============================================================================

// Global flag to indicate program termination
std::atomic<bool> STOP(false);

// Signal handler for clean shutdown
void sigend(int) {
    std::cout << std::endl << "Bye bye" << std::endl;
    STOP = true;
    // Give a short time for cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    exit(0);  // Force exit after cleanup
}

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int DL_main(dl::Args& args) {

    int s_oomBellaLogContext = 0; 
    dl::subscribeLog(&s_oomBellaLogContext, oom::bella::log);
    dl::flushStartupMessages();

    args.add("tp",  "thirdparty",   "",   "prints third party licenses");
    args.add("li",  "licenseinfo",   "",   "prints license info");
    args.add("i",  "input", "", "input bella file");

    dl::bella_sdk::Engine engine;
    engine.scene().loadDefs();
    oom::bella::MyEngineObserver engineObserver;
    engine.subscribe(&engineObserver);

    auto belScene = engine.scene();

    // use oom helper to populate the scene with default voxel objects
    // this returns a tuple of the world node, the mesh voxel node, the liquid voxel node, and the voxel node
    // The latter 3 are unparented, and the world node is parented to the scene root
    auto [  belWorld,
            belMeshVoxel,
            belLiqVoxel,
            belVoxel ] = oom::bella::defaultSceneVoxel(belScene);

    auto belVoxelMat = belScene.createNode("quickMaterial", "belVoxelMat");

    for (int i = 0; i < 500; i+=1) {
        auto eachVoxel = belScene.createNode("xform", dl::String::format("eachVoxelXform%04d",i));
        //auto eachVoxelMat = belScene.createNode("quickMaterial", dl::String::format("eachVoxelMat%04d",i));
        /*eachVoxelMat["color"] = dl::Rgba{
                                            (i%128+128)/255.0f, 
                                            0.0f, 
                                            0.0f, 
                                            1.0f};*/
        eachVoxel.parentTo(belWorld);
        belVoxel.parentTo(eachVoxel);
        dl::Mat4 myXform = dl::Mat4::identity;
        dl::Mat4 myTranslate = dl::Mat4::identity;
        dl::Mat4 myScale = dl::Mat4::identity;
        myTranslate = dl::math::makeTranslation( i*1.0, 
                                                 0.0, 
                                                 12.0 + 10.0 * sin(0.1 * i));
        /*myScale = dl::math::makeScale<4>(  (i%10+10)*0.1,
                                                    (i%10+10)*0.1,
                                                    (i%10+10)*0.1);*/
        myXform = myTranslate * myScale * myXform;
        dl::logCustom("\nXform %d", static_cast<int>(i));
        dl::logInfo("Mat4: %f %f %f %f", myXform.m00, myXform.m01, myXform.m02, myXform.m03);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m10, myXform.m11, myXform.m12, myXform.m13);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m20, myXform.m21, myXform.m22, myXform.m23);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m30, myXform.m31, myXform.m32, myXform.m33);
        eachVoxel["steps"][0]["xform"] = myXform;
        eachVoxel["material"] = belVoxelMat;
    }



    if (args.helpRequested()) {
        std::cout << args.help("poomer-efsw © 2025 Harvey Fong","", "1.0") << std::endl;
        return 0;
    }
    
    if (args.have("--licenseinfo")) {
        std::cout << "poomer-efsw © 2025 Harvey Fong" << std::endl;
        std::cout << oom::license::printLicense() << std::endl;
        return 0;
    }
 
    if (args.have("--thirdparty")) {
        std::cout << oom::license::printBellaSDK() << "\n====\n" << std::endl;
        return 0;
    }

    if (args.have("--input")) {
        auto inputFile = args.value("--input");
        if ( dl::fs::exists(inputFile) ) {
            engine.loadScene(inputFile);
            engine.scene().camera()["resolution"] = dl::Vec2{100, 100};
            engine.start();
        } else {
            dl::logError("File '%s' does not exist", inputFile.buf());
            return 1;
        }
        // Set up signal handler/callback for clean shutdown, global space of C standard library
        signal(SIGINT, sigend);

        while(engine.rendering()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    dl::logInfo("Writing scene to file");
    belScene.write("poomer-bella-sine.bsa");
    return 0;
}
