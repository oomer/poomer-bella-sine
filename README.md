# poomer-bella-sine
Learning prototype cross-platform command line for Bella scene creation with sine wave. Depends on https://github.com/oomer/oom


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

```
mkdir workdir
cd workdir
git clone https://github.com/oomer/oom.git
git clone https://github.com/oomer/poomer-bella-sine.git
cd poomer-bella-sine
make all -j4
```
