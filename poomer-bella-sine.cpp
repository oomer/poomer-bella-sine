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

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int DL_main(dl::Args& args) {

    int s_oomBellaLogContext = 0; 
    dl::subscribeLog(&s_oomBellaLogContext, oom::bella::log);
    dl::flushStartupMessages();

    args.add("tp",  "thirdparty",   "",   "prints third party licenses");
    args.add("li",  "licenseinfo",   "",   "prints license info");

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

    auto belVoxelMat1 = belScene.createNode("quickMaterial", "oomVoxelMat1");
    auto belVoxelMat2 = belScene.createNode("quickMaterial", "oomVoxelMat2");
    auto belColor1 = belScene.createNode("color", "oomColor1");
    auto belColor2 = belScene.createNode("color", "oomColor2");
    belColor1["color"] = dl::Rgba{1.0f, 0.0f, 0.0f, 1.0f};
    belColor2["color"] = dl::Rgba{1.0f, 0.0f, 0.0f, 1.0f};
    belVoxelMat1["color"] |= belColor1.output("outColor");
    belVoxelMat2["color"] |= belColor2.output("outColor");

    // belColor["variation"] = 0.3f; // Uncomment to add color variation

    for (int i = 0; i < 500; i+=1) {
        int myMod = 10;
        auto eachVoxel = belScene.createNode("xform", dl::String::format("eachVoxelXform%04d",i));
        // Uncomment to add a material to each voxel
        // auto eachVoxelMat = belScene.createNode("quickMaterial", dl::String::format("eachVoxelMat%04d",i));
        // eachVoxelMat["color"] = dl::Rgba{
        //                                             (i%128+128)/255.0f, 
        //                                             0.0f, 
        //                                             0.0f, 
        //                                             1.0f};
        eachVoxel.parentTo(belWorld);
        if (i%myMod == 0) {
            belMeshVoxel.parentTo(eachVoxel);
        } else {
            belVoxel.parentTo(eachVoxel);
        }
        dl::Mat4 myXform = dl::Mat4::identity;
        dl::Mat4 myTranslate = dl::Mat4::identity;
        dl::Mat4 myScale = dl::math::makeScale<4>(2.0,2.0,2.0);  
        myTranslate = dl::math::makeTranslation( i*1.0, 
                                                 0.0, 
                                                 12.0 + 10.0 * sin(0.1 * i));

        // Uncomment to add a scale to each voxel
        //myScale = dl::math::makeScale<4>(  (i%10+10)*0.25,
        //                                    (i%10+10)*0.25,
        //                                    (i%10+10)*0.25);

        myXform = myTranslate * myScale * myXform;

        // Uncomment to print the matrix of each voxel
        /*
        dl::logCustom("\nXform %d", static_cast<int>(i));
        dl::logInfo("Mat4: %f %f %f %f", myXform.m00, myXform.m01, myXform.m02, myXform.m03);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m10, myXform.m11, myXform.m12, myXform.m13);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m20, myXform.m21, myXform.m22, myXform.m23);
        dl::logInfo("Mat4: %f %f %f %f", myXform.m30, myXform.m31, myXform.m32, myXform.m33);
        */

        eachVoxel["steps"][0]["xform"] = myXform;
        if (i%myMod == 0) {
            eachVoxel["material"] = belVoxelMat1;
        } else {
            eachVoxel["material"] = belVoxelMat2;
        }
        // Uncomment to add a color to each voxel
        // eachVoxel["material"] = eachVoxelMat;

    }

    /*
    engine.start();
    while(engine.rendering()) { 
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } 
    */

    dl::logInfo("Writing scene to file");
    belScene.write("poomer-bella-sine.bsa");
    return 0;
}
