
#include <TFile.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <iostream>

vector<string> GetFileNames();
void getHistograms(TFile* file, int limit, vector<TH1*>& histos, vector<long>& timestamps);
void SubtractBaseline(TH1* waveform, int iterations = 5);
void FancyPlots();

void read_waveforms() {

    FancyPlots();
    
    // get the name of all Part Files in the current folder.    
    std::vector<string> PartFilesNameVec = GetFileNames();
    std::vector<TH1*> waveforms;
    vector<long> timestamps;
    int limit = 10000;

    // loop through all Part Files.
    for (int PartFileIndex = 0; PartFileIndex < PartFilesNameVec.size(); ++PartFileIndex)
    {
        TFile* file = TFile::Open(PartFilesNameVec[PartFileIndex].c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file!" << std::endl;
            return;
        }

        getHistograms(file, limit, waveforms, timestamps);

    }

    for (int event = 0; event < waveforms.size(); ++event)
    {
        SubtractBaseline(waveforms[event], 3);
        // waveforms[event]->Scale(2.415 / pow(2,12));
        // waveforms[event]->GetXaxis()->Set(waveforms[event]->GetNbinsX(), 0, waveforms[event]->GetNbinsX() * 2.0);
        waveforms[event]->GetXaxis()->SetTitle("Time [ns]");
        waveforms[event]->GetYaxis()->SetTitle("Voltage [mV]");

        if (event%100 == 0)
        {
            cout << event << endl;
        }
    }


    TRandom3 rng(time(nullptr));  
    int event_single = rng.Integer(waveforms.size());

    TCanvas *singleEv = new TCanvas("Single Event", "Single Event", 1920, 1080);
    singleEv->cd();
    waveforms[event_single]->Draw("hist");


    int grid = 3;
    TCanvas *fewEv = new TCanvas("Few envents", "Few envents", 1920, 1080);
    fewEv->Divide(grid, grid);


    for (int i = 0; i < grid*grid; ++i)
    {
        bool abovethreshold = false;
        int event = 0;
        for (int i = 0; i < 100; ++i)
        {
            event = rng.Integer(waveforms.size());
            double ymax = waveforms[event]->GetMaximum();
            if (ymax>=0.03)
            {
                abovethreshold = true;
                break;
            }

        }
        if (abovethreshold == false)
        {
            cout << "no waveform above 30mv" << endl;
        }

        // Clip to 2000 ns
        double t_max = 2000.0;
        int bin_max = waveforms[event]->GetXaxis()->FindBin(t_max);
        waveforms[event]->GetXaxis()->SetRange(1, bin_max);  // set display range only
        fewEv->cd(i + 1);
        waveforms[event]->Draw("hist");
    }


    // now computing the psd
    vector<float> f_prompt, f_delayed, total_integral;

    for (int event = 0; event < waveforms.size(); ++event)
    {
        // Integration ranges in ns
        double prompt_start     = 690.0;
        double prompt_end       = 850.0;
        double delayed_start    = 850.0;
        double delayed_end      = 900.0;
        double full_start       = 690.0;
        double full_end         = 900.0;

        // Convert to bins
        int bin_prompt_start  = waveforms[event]->GetXaxis()->FindBin(prompt_start);
        int bin_prompt_end    = waveforms[event]->GetXaxis()->FindBin(prompt_end);
        int bin_delayed_start = waveforms[event]->GetXaxis()->FindBin(delayed_start);
        int bin_delayed_end   = waveforms[event]->GetXaxis()->FindBin(delayed_end);
        int bin_full_start    = waveforms[event]->GetXaxis()->FindBin(full_start);
        int bin_full_end      = waveforms[event]->GetXaxis()->FindBin(full_end);

        // Integrate using ROOT’s built-in method
        double int_prompt  = waveforms[event]->Integral(bin_prompt_start, bin_prompt_end);
        double int_delayed = waveforms[event]->Integral(bin_delayed_start, bin_delayed_end);
        double int_total   = waveforms[event]->Integral(bin_full_start, bin_full_end);

        // Store to vectors
        f_prompt.push_back(int_prompt);
        f_delayed.push_back(int_delayed);
        total_integral.push_back(int_total);

        // cout << int_total << endl;
    }



    // Create the 2D histogram
    TH2F* h_fprompt_vs_energy = new TH2F("h_fprompt_vs_energy", 
        "F_{prompt} vs Energy;Total Charge [arb];F_{prompt}", 
        200, -10, 30,       // X bins: energy range (adjust as needed)
        120, 0.0, 1.2);      // Y bins: F_prompt is always between 0 and 1

    // Fill the histogram
    for (size_t i = 0; i < total_integral.size(); ++i) {
        double total = total_integral[i];
        if (total <= 0) continue;  // Avoid divide-by-zero

        double fprompt = f_prompt[i] / total;
        h_fprompt_vs_energy->Fill(total, fprompt);
    }


    // Plot
    TCanvas* c3 = new TCanvas("c3", "Fprompt vs Energy", 800, 600);
    h_fprompt_vs_energy->Draw("COLZ");
}

void FancyPlots()
{
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);
    gStyle->SetStatY(0.9);
    gStyle->SetStatX(0.95);
    gStyle->SetStatW(0.3);
    gStyle->SetStatH(0.2);
    gStyle->SetStatBorderSize(3);
    gStyle->SetLineWidth(3);
}


std::vector<string> GetFileNames()
{
    cout << "Reading the folder for .root PartFilesNameVec. " << endl;

    string fileLs = "ls | grep .root > partfiles.txt";

    cout << "Filling partfiles.txt with the ls contents" << endl;
    int resultLs = system(fileLs.c_str());
    if (resultLs == 0) {
        std::cout << "  -> Filling partfiles.txt  was successful!" << std::endl;
    } else {
        std::cerr << "  -> Filling partfiles.txt was NOT successful." << std::endl;
    }


    cout << "Now reading this new partfiles.txt and storing at the fileNames vector" << endl;
    string filename = "partfiles.txt";

    std::vector<std::string> fileNames;
    FILE* file = fopen(filename.c_str(), "r");

    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return fileNames;
    }

    char buffer[256];
    while (fscanf(file, "%255s", buffer) == 1) {
        fileNames.push_back(buffer);
    }

    fclose(file);
    return fileNames;
}


void getHistograms(TFile* file, int limit, vector<TH1*>& histos, vector<long>& timestamps)
{


    TIter next_hist(file->GetListOfKeys());
    TKey* hist_key = nullptr;
    int nhists = 0;

    while ((hist_key = (TKey*)next_hist())) 
    {

        std::string name = hist_key->GetName();
        if (name.rfind("BRF_", 0) != 0) continue;  // rfind returns 0 if it starts with "BRF_"

        TObject* obj = hist_key->ReadObj();

        if (obj->InheritsFrom("TH1")) 
        {
            TH1* h = (TH1*)obj;

            size_t last_underscore = name.rfind('_');
            if (last_underscore == std::string::npos) continue;

            std::string timestamp_str = name.substr(last_underscore + 1);
            long timestamp = std::stol(timestamp_str);
            timestamps.push_back(timestamp);
            // cout << name << " at " << timestamp << endl;
            histos.push_back(h);
            nhists++;

            if (nhists>limit && limit>0)
            {
                break;
            }

        }

    }
    // file->Close();
    // cout << file->GetName() << " : " << nhists << endl;
}

void SubtractBaseline(TH1* waveform, int iterations = 5) {
    if (!waveform) return;

    std::vector<double> values;

    // Fill initial vector with all bin contents
    int nbins = waveform->GetNbinsX();
    for (int i = 1; i <= nbins; ++i) 
    {
        values.push_back(waveform->GetBinContent(i));
    }

    for (int iter = 0; iter < iterations; ++iter) 
    {
        // Calculate mean and stdev of current values
        double sum = 0;
        for (double v : values) sum += v;
        double mean = sum / values.size();

        double sq_sum = 0;
        for (double v : values) sq_sum += (v - mean) * (v - mean);
        double sigma = std::sqrt(sq_sum / values.size());

        // Filter values within ±1 sigma
        std::vector<double> clipped;
        for (double v : values) {
            if (std::abs(v - mean) < sigma) clipped.push_back(v);
        }

        if (clipped.empty()) break; // nothing left to clip

        values = clipped;
    }

    // Final baseline is the mean of the last clipped set
    double baseline = 0;
    for (double v : values) baseline += v;
    baseline /= values.size();

    values.clear();
    // Subtract baseline from waveform
    for (int i = 1; i <= nbins; ++i) {
        double v = waveform->GetBinContent(i);
        waveform->SetBinContent(i, v - baseline);
    }
}