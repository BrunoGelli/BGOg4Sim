# Geant4 Optical Photon Simulation with ROOT Analysis

This repository contains a Geant4-based simulation for optical photon generation, tracking, and detection in a water Cherenkov detector context. It includes Cherenkov and scintillation photon generation, visualization macros, and a ROOT-based Python waveform builder for analysis.

## 🔧 Project Structure

```
.
├── src/                  # Colection of Source files
│   └── src/              # Geant4 source files
│   └── include/          # Geant4 headers
│   └── vis.mac           # Visualization setup
│   └── CMakeLists.txt        # Build configuration
│   
├── scripts/                  # Colection of Source files
│   └── photon_histogram_builder.py  # ROOT Python script to build waveforms
│
├── .gitignore
└── README.md             # This file
```

## 🛠️ Building the Project

### Prerequisites

- [Geant4](https://geant4.web.cern.ch/) (with data sets and visualization options)
- CMake (≥ 3.16)
- ROOT (for waveform analysis)
- Python 3 with `ROOT`, `numpy`, and `matplotlib` for analysis scripts

### Build Instructions

```bash
mkdir build
cd build
cmake ../src
make
```

## 🚀 Running the Simulation

Run a simple test using:

```bash
./filtro
```
Running in non-batch mode may require to compile it with single core run manager. Modify filtro.cc if getting errors.

```cpp
 G4RunManager* runManager = new G4RunManager;
```

You can batch run with the following, where N is the PMT to BGO distance (in mm)

```bash
./filtro filtro.in N
```
Running for many events benefit greatly of having multi-core run manager. Modify filtro.cc to use N cores. 

```cpp
G4MTRunManager* runManager = new G4MTRunManager;
runManager->SetNumberOfThreads(N);
runManager->SetVerboseLevel(0);
```

## 📈 Waveform Processing

After running the simulation and producing a ROOT output tree:

Make sure you combine all outputs with hadd. Then cleanup removing the unecessary files.

```bash
hadd OutPut_merged_0mm.root OutPut_t*.root
rm OutPut_t*.root
rn OutPut.root
```

Then use the python script to go from the photon arrival histogram to pmt response waveforms:
```bash
python photon_histogram_builder.py
```

This script builds histograms of photon arrival times per event and separates Cherenkov and scintillation contributions. Ita also applies the PMT QE and simulate PMT response.

## 🧪 ROOT Analysis Tips

From within ROOT, you can explore the output tree using the `TBrowser`:

```cpp
TBrowser b;
```

Or plot Cherenkov/scintillation ratios for triggered events:

```cpp
tree->Draw("n_cher/n_scint:event", "trigger==1", "COLZ")
```

## 🎨 Customization

colors in `vis.mac`:

- Optical photons 	→ Green
- Gammas			→ Purple
- Electrons 		→ Red
- Positrons 		→ Blue
- Default 			→ Grey	
