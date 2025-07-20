import numpy as np
import ROOT
from array import array
import time
import random

# === Parameters
bin_width = 2  # ns
nbins = 1500   # 2000 ns total
time_max = nbins * bin_width
trigger_threshold = 0.030
trigger_time = 700
limit = 1e12

# Convert the table to two NumPy arrays (energy in eV, QE as 0–1)
energy_eV = np.array([
    1.771, 1.784, 1.797, 1.810, 1.823, 1.837, 1.850, 1.864, 1.878, 1.893,
    1.907, 1.922, 1.937, 1.952, 1.968, 1.984, 2.000, 2.016, 2.032, 2.049,
    2.066, 2.084, 2.101, 2.119, 2.138, 2.156, 2.175, 2.194, 2.214, 2.234,
    2.254, 2.275, 2.296, 2.317, 2.339, 2.362, 2.384, 2.407, 2.431, 2.455,
    2.480, 2.505, 2.530, 2.556, 2.583, 2.610, 2.638, 2.666, 2.695, 2.725,
    2.755, 2.786, 2.818, 2.850, 2.883, 2.917, 2.952, 2.987, 3.024, 3.061,
    3.100, 3.139, 3.179, 3.220, 3.263, 3.306, 3.351, 3.397, 3.444, 3.492,
    3.542, 3.594, 3.646, 3.701, 3.757, 3.815, 3.874, 3.936, 3.999, 4.065,
    4.133, 4.203, 4.275, 4.350, 4.428, 4.508, 4.592, 4.678, 4.768, 4.862,
    4.959, 5.060, 5.166, 5.276, 5.390, 5.510, 5.635, 5.767, 5.904, 6.048,
    6.199, 6.358, 6.525, 6.702, 6.888, 7.085])

qe_percent = np.array([
     0.000,  0.000,  0.000,  0.042,  0.057,  0.083,  0.116,  0.153,  0.197,  0.256,
     0.336,  0.441,  0.568,  0.716,  0.886,  1.075,  1.283,  1.512,  1.764,  2.040,
     2.338,  2.658,  3.001,  3.370,  3.770,  4.204,  4.676,  5.189,  5.741,  6.322,
     6.923,  7.533,  8.144,  8.763,  9.380,  9.997, 10.634, 11.302, 12.025, 12.699,
    13.401, 14.094, 14.800, 15.489, 16.190, 16.911, 17.656, 18.365, 19.010, 19.542,
    20.040, 20.541, 21.102, 21.692, 22.310, 22.883, 23.360, 23.673, 23.903, 24.082,
    24.276, 24.408, 24.363, 24.240, 24.074, 23.900, 23.718, 23.505, 23.170, 22.827,
    22.414, 21.974, 21.498, 20.970, 20.491, 20.076, 19.691, 19.317, 18.954, 18.584,
    18.180, 17.704, 17.156, 16.541, 15.937, 15.321, 14.699, 14.044, 13.355, 12.648,
    11.913, 11.153, 10.384,  9.636,  8.906,  8.192,  7.494,  6.788,  6.053,  5.289,
     4.577,  4.003,  3.408,  0.000,  0.000,  0.000])

# Normalize QE to 0–1 range
qe = qe_percent / 100.0

# Interpolation function
get_qe = lambda E: np.interp(E, energy_eV, qe)


# === Define Single Photon Response
def spr_model(t, tau_r=1.5, tau_f=5.0):
    resp = 1 * (1 - np.exp(-t / tau_r)) * np.exp(-t / tau_f)
    resp[t < 0] = 0
    return resp

# Time axis for SPR, shifted by transit time 23 ns
spr_time = np.arange(-50, 100, bin_width)
spr = spr_model(spr_time - 23)

# Normalize to unit area
spr /= np.sum(spr)

# === Open input ROOT file and tree
f = ROOT.TFile.Open("OutPut_merged_0mm.root")
tree = f.Get("ph")

# === Prepare output ROOT file and TTree
outf = ROOT.TFile("waveforms_0mm.root", "RECREATE")

# Auxiliary root tree with a relevant summary per event
outtree = ROOT.TTree("events", "Photon event summary") 

# Define branch variables
event_id = array("i", [0])
was_triggered = array("i", [0])
n_cherenkov = array("i", [0])
n_scintillation = array("i", [0])

# Create branches
outtree.Branch("event", event_id, "event/I")
outtree.Branch("triggered", was_triggered, "triggered/I")
outtree.Branch("n_cherenkov", n_cherenkov, "n_cherenkov/I")
outtree.Branch("n_scintillation", n_scintillation, "n_scintillation/I")

# === Step 1: Load all hits and group by event and process type
event_hits_ceren = {}  # process == 1
event_hits_scin = {}   # process == 0

nentries = tree.GetEntries()
for i in range(nentries):
    tree.GetEntry(i)
    eid = int(tree.evt)
    process = int(tree.process)
    t_corr = tree.t + trigger_time  # ANNIE offset (will be done properly afterwards, here is just to make sure it is not in the edge)
    energy = tree.e                 # Photon energy

    # Apply QE filter — photon is accepted if random number < QE
    if random.random() > get_qe(energy):
        continue  # Photon not detected

    if process == 1:
        event_hits_ceren.setdefault(eid, []).append(t_corr)
    elif process == 0:
        event_hits_scin.setdefault(eid, []).append(t_corr)

# === Step 2: Loop over events and process
processed = 0
noise_std = 0.001

all_eids = set(event_hits_scin.keys()) | set(event_hits_ceren.keys())

for eid in sorted(all_eids):
    sci_hits = event_hits_scin.get(eid, [])   # Default to empty list if not present
    cher_hits = event_hits_ceren.get(eid, []) # Same here

    hist_sci, _ = np.histogram(sci_hits, bins=nbins, range=(0, time_max))
    hist_cher, _ = np.histogram(cher_hits, bins=nbins, range=(0, time_max))

    # combine them
    hist = hist_sci + hist_cher

    # Apply gain smearing to each bin — realistic PMT response variation
    gain_mean = 5.0/1000     # mV peak per photon
    gain_sigma = 0.6/1000    # mV stddev for 1 PE (adjust as needed)

    smeared_hist = np.zeros_like(hist, dtype=np.float64)

    for i, n_photons in enumerate(hist):
        if n_photons > 0:
            mean = n_photons * gain_mean
            sigma = np.sqrt(n_photons) * gain_sigma
            smeared_hist[i] = np.random.normal(loc=mean, scale=sigma)
        # else stays 0

    # Convolve with SPR (already normalized to unit area)
    signal = np.convolve(smeared_hist, spr, mode='full')[:nbins]

    
    # Add Gaussian noise
    signal += np.random.normal(0, noise_std, nbins)
    
    # Decides if the pulse crosses threshold
    peak = np.max(signal)
    bin_width = time_max / nbins  # Make sure time_max and nbins are set properly
    triggered = False

    if peak > trigger_threshold:
        triggered = True
        trigger_index = np.argmax(signal > trigger_threshold)
        target_index = int(trigger_time / bin_width)
        shift = target_index - trigger_index
        signal_shifted = np.random.normal(0, noise_std, nbins)

        if shift >= 0:
            start_src = 0
            end_src = min(len(signal), nbins - shift)
            signal_shifted[shift:shift + end_src] = signal[start_src:end_src]
        else:
            shift = abs(shift)
            start_src = shift
            end_src = min(len(signal), len(signal) - shift, nbins)
            signal_shifted[0:end_src] = signal[start_src:start_src + end_src]

    # Also save TH1 histogram for analysis reuse
    timestamp = int(time.time())

    hWVFname = f"WVF_{eid}_{timestamp}"
    hSCIname = f"SCI_{eid}_{timestamp}"
    hCHEname = f"CHE_{eid}_{timestamp}"
    
    if triggered:
        hBRFname = f"BRF_{eid}_{timestamp}"
        
        hBRF = ROOT.TH1D(hBRFname, hBRFname, nbins, 0, time_max)

        for bin_i in range(nbins):
            hBRF.SetBinContent(bin_i+1, signal_shifted[bin_i])

        hBRF.Write()



    hWavef  = ROOT.TH1D(hWVFname, hWVFname, nbins, 0, time_max)
    hScint  = ROOT.TH1D(hSCIname, hSCIname, nbins, 0, time_max)
    hChern  = ROOT.TH1D(hCHEname, hCHEname, nbins, 0, time_max)

    for bin_i in range(nbins):
        hWavef.SetBinContent(bin_i+1, signal[bin_i])
        hScint.SetBinContent(bin_i+1, hist_sci[bin_i])
        hChern.SetBinContent(bin_i+1, hist_cher[bin_i])


    hWavef.Write()
    hScint.Write()
    hChern.Write()

    # write to the auxiliary tree
    event_id[0] = eid
    
    # Determine trigger status
    if triggered:
        was_triggered[0] = 1
    else:
        was_triggered[0] = 0
    
    # Count photons
    n_scintillation[0]  = len(sci_hits)#int(hScint.GetEntries())
    n_cherenkov[0]      = len(cher_hits)#int(hChern.GetEntries())

    outtree.Fill()

    # print the progress
    processed += 1
    print(f"Processed event {processed} (ID {eid})")
    if processed >= limit:
        break

# === Write and close files
outtree.Write()
# outf.Write()
outf.Close()
f.Close()




# import numpy as np
# import ROOT
# from array import array
# import time

# # === Parameters
# bin_width = 2  # ns
# nbins = 1500   # 2000 ns total
# time_max = nbins * bin_width

# # === Define Single Photon Response
# def spr_model(t, tau_r=1.5, tau_f=5.0):
#     resp = 1 * (1 - np.exp(-t / tau_r)) * np.exp(-t / tau_f)
#     resp[t < 0] = 0
#     return resp

# # Time axis for SPR, shifted by transit time 23 ns
# spr_time = np.arange(-50, 100, bin_width)
# spr = spr_model(spr_time - 23)

# # Normalize to unit area
# spr /= np.sum(spr)

# # === Open input ROOT file and tree
# f = ROOT.TFile.Open("OutPut_merged_10mm.root")
# tree = f.Get("ph")

# # === Prepare output ROOT file and TTree
# outf = ROOT.TFile("waveforms_10mm.root", "RECREATE")

# # === Step 1: Load all hits and group by event (fast!)
# event_hits = {}
# nentries = tree.GetEntries()
# for i in range(nentries):
#     tree.GetEntry(i)
#     eid = int(tree.evt)
#     process = int(tree.process)
#     t_corr = tree.t + 700  # ANNIE offset
#     event_hits.setdefault(eid, []).append(t_corr)

# print(f"Loaded hits for {len(event_hits)} events")
# print(f"number of cherenkov events is  {len(event_cher)}")

# # === Step 2: Loop over events and process
# processed = 0
# noise_std = 0.001

# for eid, hits in event_hits.items():
#     # Histogram of photon arrival times for event
#     hist, _ = np.histogram(hits, bins=nbins, range=(0, time_max))

#     # Apply gain smearing to each bin — realistic PMT response variation
#     gain_mean = 5.0/1000     # mV peak per photon
#     gain_sigma = 0.6/1000    # mV stddev for 1 PE (adjust as needed)

#     smeared_hist = np.zeros_like(hist, dtype=np.float64)

#     for i, n_photons in enumerate(hist):
#         if n_photons > 0:
#             mean = n_photons * gain_mean
#             sigma = np.sqrt(n_photons) * gain_sigma
#             smeared_hist[i] = np.random.normal(loc=mean, scale=sigma)
#         # else stays 0

#     # Convolve with SPR (already normalized to unit area)
#     signal = np.convolve(smeared_hist, spr, mode='full')[:nbins]

    
#     # Add Gaussian noise
#     signal += np.random.normal(0, noise_std, nbins)
    
    
#     # Also save TH1 histogram for analysis reuse
#     timestamp = int(time.time())

#     hname = f"BRF_{eid}_{timestamp}"
#     hScintname = f"RAW_{eid}_{timestamp}"
    
#     h = ROOT.TH1D(hname, f"BRF_{eid}_{timestamp}", nbins, 0, time_max)
#     hScint = ROOT.TH1D(hScintname, f"RAW_{eid}_{timestamp}", nbins, 0, time_max)
#     for bin_i in range(nbins):
#         h.SetBinContent(bin_i+1, signal[bin_i])
#     h.Write()

#     for bin_i in range(nbins):
#         hScint.SetBinContent(bin_i+1, hist[bin_i])
#     hScint.Write()
    
#     processed += 1
#     print(f"Processed event {processed} (ID {eid})")
#     if processed >= 20:
#         break

# # === Write and close files
# outf.Write()
# outf.Close()
# f.Close()
