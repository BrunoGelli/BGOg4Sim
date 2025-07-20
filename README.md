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
- Python 3 with `uproot`, `numpy`, and `matplotlib` for analysis scripts

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
./yourExecutable macros/run.mac
```

You can visualize the detector and photons with:

```bash
./yourExecutable macros/vis.mac
```

## 📈 Waveform Processing

After running the simulation and producing a ROOT output tree:

```bash
python photon_histogram_builder.py
```

This script builds histograms of photon arrival times per event and separates Cherenkov and scintillation contributions. You can apply QE and simulate PMT response.

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

Photon color in `vis.mac`:

- Cherenkov photons → Green
- Scintillation photons → Purple
- Electrons → Red
- Positrons → Blue

## 📄 License

[MIT License](LICENSE) or specify your own.

## ✍️ Author

Bruno P. Gelli – [brunopgelli.com](https://example.com)  
Inspired by ANNIE / DUNE / water Cherenkov physics.
