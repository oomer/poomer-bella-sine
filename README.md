# poomer-bella-sine
Learning prototype cross-platform command line for Bella scene creation with sine wave. Depends on https://github.com/oomer/oom

<img src="resources/example.jpg">

# Demonstrates
- Auto camera framing of sine wave
- Bella scene file writing
- Using Diffuse Logic's logging
- Using render event callbacks with EngineObserver override
- Use of arguments
- Adding quickmaterials to the scene
- Shows how to write nodes ie belVoxelMat1["color"] |= belColor1.output("outColor");
- Using both procedural Bella box and a mesh box modelled from Blender
- Hiding boilerplate code like cube mesh and basic Bella scene population in .h files 

# Usage

```
poomer-bella-sine #generates a 100 voxel sine plot, output .bsa
poomer-bella-sine -n:1000 # plot 1000 voxels on sine curve
poomer-bella-sine -or:16 # render 16 frames of camera orbit
poomer-bella-sine -sa:60 # sets amplitude
poomer-bella-sine -sf:0.55 # sets sine frequency
poomer-bella-sine -cv:0.65 # sets color variation
poomer-bella-sine -em # sets one material per voxel plot with gradient
poomer-bella-sine -r # render current image
poomer-bella-sine -s:2 # set xfrom scale on voxels to double
poomer-bella-sine -sv # set xfrom scale on voxels using modulus
poomer-bella-sine -m:50 # set modulus event to occur every 50 voxels 
```

[todo] add checks for illegal argument values

# Build 
```
workdir/
├── build_engine_sdk/
├── oom/
└── poomer-bella-sine/
```

Download SDK for your OS and drag bella_engine_sdk into your workdir. On Windows rename unzipped folder by removing version ie bella_engine_sdk-24.6.0 -> bella_engine_sdk

- [bella_engine_sdk MacOS](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.dmg)
- [bella_engine_sdk Linux](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.tar.gz)
- [bella_engine_sdk Win](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.zip)



# MacOS (tested) and Linux (untested)
```
mkdir workdir
cd workdir
git clone https://github.com/oomer/oom.git
git clone https://github.com/oomer/poomer-bella-sine.git
cd poomer-bella-sine
make all -j4
```

# Windows (untested)
```
mkdir workdir
cd workdir
git clone https://github.com/oomer/oom.git
git clone https://github.com/oomer/poomer-bella-sine.git
cd poomer-bella-sine
msbuild poomer-bella-sine.vcxproj /p:Configuration=release /p:Platform=x64 /p:PlatformToolset=v143
```

