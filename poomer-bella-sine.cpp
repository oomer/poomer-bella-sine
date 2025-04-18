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

struct Vec3 {
    float x, y, z;
};

struct BoundingBox {
    Vec3 min;
    Vec3 max;
    
    Vec3 getCenter() const {
        return {
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f,
            (min.z + max.z) * 0.5f
        };
    }
    
    float getRadius() const {
        Vec3 center = getCenter();
        
        // Calculate distance from center to each corner and return the maximum
        float dx = std::max(std::abs(center.x - min.x), std::abs(center.x - max.x));
        float dy = std::max(std::abs(center.y - min.y), std::abs(center.y - max.y));
        float dz = std::max(std::abs(center.z - min.z), std::abs(center.z - max.z));
        
        // Pythagorean distance to the furthest corner
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
};

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int DL_main(dl::Args& args) {

    BoundingBox bbox = {{1000000.0f, 1000000.0f, 1000000.0f},       // min point
                        {-1000000.0f, -1000000.0f, -1000000.0f} };  // max point


    int s_oomBellaLogContext = 0; 
    dl::subscribeLog(&s_oomBellaLogContext, oom::bella::log);
    dl::flushStartupMessages();

    args.add("tp", "thirdparty",    "",   "prints third party licenses");
    args.add("li", "licenseinfo",   "",   "prints license info");
    args.add( "n", "number",        "",   "number of plot points");
    args.add( "zo", "zoffset",      "",   "z offset");
    args.add( "sf", "sinefreq",     "",   "sine frequency");
    args.add( "sa", "sineampl",     "", "sine amplitude");
    args.add( "s", "scale",         "",   "scale");
    args.add( "m", "modulus",       "",   "modulus");
    args.add( "r", "render",        "",   "render");
    args.add( "or", "orbit",        "10", "orbit");
    args.add( "pm", "printMatrix",  "",   "print matrix4");
    args.add( "em", "eachmaterial", "",   "each voxel has own material");
    args.add( "sv", "scalevoxel",  "",   "each voxel has own scale");

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
    int argNumPlotPoints = 100;    
    if (args.have("--number")) { argNumPlotPoints = std::stoi(args.value("--number").buf()); }
    float argSineFreq = 0.1;    
    if (args.have("--sinefreq")) { argSineFreq = std::stof(args.value("--sinefreq").buf()); }
    float argSineAmpl = 10.0;    
    if (args.have("--sineampl")) { argSineAmpl = std::stof(args.value("--sineampl").buf()); }
    float argZOffset = argSineAmpl;    
    if (args.have("--zoffset")) { argZOffset = std::stof(args.value("--zoffset").buf()); }
    double argScale = 1.0;    
    if (args.have("--scale")) { argScale = std::stod(args.value("--scale").buf()); }
    int argModulus = 10;    
    if (args.have("--modulus")) { argModulus = std::stoi(args.value("--modulus").buf()); }

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

    for (int i = 0; i < argNumPlotPoints; i+=1) {
        auto eachVoxel = belScene.createNode("xform", dl::String::format("eachVoxelXform%04d",i));
        
        dl::bella_sdk::Node eachVoxelMat;
        if (args.have("--eachmaterial")) {
            eachVoxelMat = belScene.createNode("quickMaterial", dl::String::format("eachVoxelMat%04d",i));
            eachVoxelMat["color"] = dl::Rgba{ (i%255)/255.0f, 0.0f, 0.0f, 1.0f};
        }
        eachVoxel.parentTo(belWorld);
        if (i%argModulus == 0) {
            belMeshVoxel.parentTo(eachVoxel);
        } else {
            belVoxel.parentTo(eachVoxel);
        }
        dl::Mat4 myXform = dl::Mat4::identity;
        dl::Mat4 myTranslate = dl::Mat4::identity;
        dl::Mat4 myScale = dl::Mat4::identity;
        // Uncomment to get double scale
        //dl::Mat4 myScale = dl::math::makeScale<4>(2.0,2.0,2.0);  

        // Calculate the position of each point in the sine wave
        double myX = i*1.0;
        double myY = 0.0;
        double myZ = argZOffset + argSineAmpl * sin(argSineFreq * i); 

        // Update bounding box
        if (myX < bbox.min.x) bbox.min.x = myX;
        if (myY < bbox.min.y) bbox.min.y = myY;
        if (myZ < bbox.min.z) bbox.min.z = myZ;
        if (myX > bbox.max.x) bbox.max.x = myX;
        if (myY > bbox.max.y) bbox.max.y = myY;
        if (myZ > bbox.max.z) bbox.max.z = myZ;

        myTranslate = dl::math::makeTranslation( myX, myY, myZ);
        myScale = dl::math::makeScale<4>( argScale, argScale, argScale);

        // Override scale above if arg is passed
        if (args.have("--scalevoxel")) {
            myScale = dl::math::makeScale<4>(  (i%argModulus+5)*0.5,
                                               (i%argModulus+5)*0.5,
                                               (i%argModulus+5)*0.5);
        }

        // This is called matrix concatenation or composition
        myXform = myTranslate * myScale * myXform;

        if (args.have("--printMatrix")) {
            dl::logCustom("\nXform %d", static_cast<int>(i));
            dl::logInfo("Mat4: %f %f %f %f", myXform.m00, myXform.m01, myXform.m02, myXform.m03);
            dl::logInfo("Mat4: %f %f %f %f", myXform.m10, myXform.m11, myXform.m12, myXform.m13);
            dl::logInfo("Mat4: %f %f %f %f", myXform.m20, myXform.m21, myXform.m22, myXform.m23);
            dl::logInfo("Mat4: %f %f %f %f", myXform.m30, myXform.m31, myXform.m32, myXform.m33);
        }

        // apply the matrix to the voxel
        eachVoxel["steps"][0]["xform"] = myXform;

        if (i%argModulus == 0) { // alternate between voxel materials via modulus math
            eachVoxel["material"] = belVoxelMat1;
        } else {
            eachVoxel["material"] = belVoxelMat2;
        }

        // Possible override to use a different material for each voxel
        if (args.have("--eachmaterial")) {
            eachVoxel["material"] = eachVoxelMat;
        }

    }

    Vec3 centerOfPlots = bbox.getCenter(); 
    dl::logInfo("Bounding Box Center: (%f, %f, %f)", centerOfPlots.x, centerOfPlots.y, centerOfPlots.z);

    float radiusOfPlots = bbox.getRadius();
    dl::logInfo("Bounding Box Radius: %f", radiusOfPlots);

    auto belCameraPath = belScene.cameraPath(); // Since camera can be instanced, we get the full path of th one currently define din scene settings
    auto belCamera = belScene.camera();
    auto belCameraXform = belCameraPath.parent(); // thus the parent of the camera path is the xform node 99% of the time
    
    auto targetCenter = dl::Vec3{ centerOfPlots.x, centerOfPlots.y, centerOfPlots.z };
    
    // get a Transformation Matrix that will position the camera to view the entire scene
    dl::Mat4 newCamXform = dl::bella_sdk::zoomExtents(belCameraPath, targetCenter, radiusOfPlots);
    // Apply to camera xform
    belCameraXform["steps"][0]["xform"] = newCamXform;

    if (args.have("--render")) {
        engine.start();
        while(engine.rendering()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } 
    } 

    // orbit camera around plot points
    if (args.have("--orbit")) {
        int numFrames = std::stoi(args.value("--orbit").buf());

        belCamera["resolution"] = dl::Vec2{100, 100};
        for (int i = 0; i < numFrames; i++) {

            auto offset = dl::Vec2 {i*0.25, 0.0};
            dl::bella_sdk::orbitCamera(engine.scene().cameraPath(),offset);
            auto belBeautyPass = belScene.beautyPass();
            belBeautyPass["outputName"] = dl::String::format("poomer-bella-sine-%04d", i);
            engine.start();
            while(engine.rendering()) { 
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            } 
        } 
    } 
     
    dl::logInfo("Writing scene to file");
    belScene.write("poomer-bella-sine.bsa");
    return 0;
}
