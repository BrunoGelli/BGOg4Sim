#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TDirectory.h>
#include <TString.h>
#include <map>
#include <iostream>



void MakePhotonTimingHistos(const char* infile = "simulation.root", const char* outfile = "EventHistograms.root", double binWidth = 2.0, double tMin = 0, double tMax = 2000) {
    // Load input
    TFile* fin = TFile::Open(infile);
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error opening file: " << infile << std::endl;
        return;
    }

    TTree* tree = (TTree*)fin->Get("ph");
    if (!tree) {
        std::cerr << "Could not find TTree 'ph' in " << infile << std::endl;
        return;
    }

    // Setup branches
    int evt;
    double t;
    tree->SetBranchAddress("evt", &evt);
    tree->SetBranchAddress("t", &t);

    // Output
    TFile* fout = new TFile(outfile, "RECREATE");
    std::map<int, TH1D*> histos;

    Long64_t nentries = tree->GetEntries();
    std::cout << "Processing " << nentries << " photons..." << std::endl;

    int nbins = (tMax - tMin) / binWidth;

    for (Long64_t i = 0; i < nentries; ++i) {
        tree->GetEntry(i);

        // Create histogram for event if it doesn't exist
        if (histos.find(evt) == histos.end()) {
            TString hname = Form("BRF_%d_%lld", evt, i);
            histos[evt] = new TH1D(hname, Form("BRF_%d_%lld", evt, i), nbins, tMin, tMax);
        }

        histos[evt]->Fill(t+700);
    }

    // Save histograms
    for (auto& [evt, hist] : histos) {
        hist->Write();
    }

    fout->Close();
    std::cout << "Histograms saved to " << outfile << std::endl;
}
