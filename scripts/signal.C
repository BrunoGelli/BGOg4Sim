
	#include <stdlib.h>     /* srand, rand */
	#include <time.h>       /* time */
	#include "TH1F.h"
	#include "TVirtualFFT.h"
	#include "TRandom.h"
	#include "TF1.h"
	#include "TTree.h"
	#include "TCanvas.h"
	#include "TStyle.h"
	#include "TMath.h"
	#include <math.h> 
	#include <stdio.h>
	#include <string.h>
	#include <TSpectrum.h>
	#include <TSpectrumTransform.h>


void signal(float pinhole)
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
 
 	Double_t initial = 15;

 	std::vector<TString> fileNames;
 	fileNames.push_back("OutPut_t0.root");
 	fileNames.push_back("OutPut_t1.root");
 	fileNames.push_back("OutPut_t2.root");
 	fileNames.push_back("OutPut_t3.root");
 	fileNames.push_back("OutPut_t4.root");
 	fileNames.push_back("OutPut_t5.root");
 	fileNames.push_back("OutPut_t6.root");
	fileNames.push_back("OutPut_t7.root");
 	fileNames.push_back("OutPut_t8.root");
 	fileNames.push_back("OutPut_t9.root");
 	fileNames.push_back("OutPut_t10.root");
 	fileNames.push_back("OutPut_t11.root");
 	fileNames.push_back("OutPut_t12.root");
 	fileNames.push_back("OutPut_t13.root");
 	fileNames.push_back("OutPut_t14.root");
 	fileNames.push_back("OutPut_t15.root");
 	fileNames.push_back("OutPut_t16.root");
	fileNames.push_back("OutPut_t17.root");
 	fileNames.push_back("OutPut_t18.root");
 	fileNames.push_back("OutPut_t19.root");
 	fileNames.push_back("OutPut_t20.root");
 	fileNames.push_back("OutPut_t21.root");
 	fileNames.push_back("OutPut_t22.root");

	std::vector<double> dataX, dataY, dataZ, dataT, dataE, dataTheta, dataDx, dataDy, dataDz;

	int size = 0; 
	int nentries = 0;
	
	Double_t x, y, z, t, e, theta, dx, dy, dz;

	TFile *f;
	TTree *T;

 	for (int i = 0; i < fileNames.size(); ++i)
 	{
 		
		f = new TFile(fileNames[i],"READ");
		
		if ( f->IsOpen() )
		{
			// printf("File opened successfully\n");
		} 
		else 
		{
			printf("Error! Invalid File Name\n");
			return;
		}

		T = (TTree*)f -> Get("ntuple");

		T->SetBranchAddress("x",&x);
		T->SetBranchAddress("y",&y);
		T->SetBranchAddress("z",&z);
		T->SetBranchAddress("t",&t);
		T->SetBranchAddress("energy",&e);
		T->SetBranchAddress("theta",&theta);
		T->SetBranchAddress("dx",&dx);
		T->SetBranchAddress("dy",&dy);
		T->SetBranchAddress("dz",&dz);
		
		nentries = (int)T->GetEntries();

		for (int i = 0; i < nentries; ++i)
		{
			T->GetEntry(i);
			dataX.push_back(x);
			dataY.push_back(y);
			dataZ.push_back(z);
			dataT.push_back(t);
			dataE.push_back(e);
			dataTheta.push_back(theta);
			dataDx.push_back(dx);
			dataDy.push_back(dy);
			dataDz.push_back(dz);
		}

 	}

 	size = dataE.size();

 	printf("There were %d events.\n", size);

	std::vector<double> wasVisited;
	std::vector<double> howMany;



	double radius = 1;

	TH1D *simpleX	= new TH1D("simpleX", "simpleX", 100, -radius, radius);
	TH1D *simpleY	= new TH1D("simpleY", "simpleY", 100, -radius, radius);
	TH1D *simpleTh	= new TH1D("simple angle", "simple angle", 200, -10, 20);
	TH1D *simpleE	= new TH1D("simpleE", "simpleE", 200, 0, 0);

	for (Int_t i=0; i<size; i++)
	{
		simpleX->Fill(dataX[i]);
		simpleY->Fill(dataY[i]);
		simpleTh->Fill(dataTheta[i]);
		simpleE->Fill(dataE[i]);
	}
	
	simpleX->SetTitle("Eixo X");
	simpleX->GetXaxis()->SetTitle("Distancia (cm)");
	simpleX->GetYaxis()->SetTitle("#");
	simpleX->SetLineWidth(1);

	simpleY->SetTitle("Eixo Y");
	simpleY->GetXaxis()->SetTitle("Distancia (cm)");
	simpleY->GetYaxis()->SetTitle("#");
	simpleY->SetLineWidth(1);

	simpleTh->SetTitle("Angulo");
	simpleTh->GetXaxis()->SetTitle("Angulo (graus)");
	simpleTh->GetYaxis()->SetTitle("#");
	simpleTh->SetLineWidth(1);

	simpleE->SetTitle("Energia");
	simpleE->GetXaxis()->SetTitle("Energia (eV)");
	simpleE->GetYaxis()->SetTitle("Contagem de eventos");
	simpleE->SetLineWidth(1);


	TCanvas *Simple = new TCanvas("Simple", "Simple plots", 1600, 900);
	Simple->Divide(2,2);

	Simple->cd(1);
	simpleX->Draw();

	Simple->cd(2);
	simpleY->Draw();

	Simple->cd(3);
	gPad-> SetLogy();
	simpleTh->Draw();

	Simple->cd(4);
	// gPad-> SetLogy();
	simpleE->Draw();

	char str[1000];
	sprintf(str, "pinhole_size_%0.2f.png", pinhole);

	Simple->SaveAs(str);


	//*********************************************************************************
	
	// TCanvas *C_Frente = new TCanvas("C_Frente", "Parede frontal",0,0,1800,900);
	// C_Frente->cd();

	// gPad->SetRightMargin(.45);	
	// TH2D* Frente = new TH2D("Distribuicao na base da pir#hat{a}mide", "Distribuicao na base da pir#hat{a}mide", 60, -60, 60, 60, -60, 60);
	
	// Int_t bin = 0;

	// for (int i = 0; i < size; ++i)
	// {
	// 		Frente->Fill(dataX[i],dataY[i]);
	// }

	// for (int i = 0; i < size; ++i)
	// {
	// 	bin = Frente->FindBin(dataX[i],dataY[i]);
	// 	howMany.push_back(Frente->GetBinContent(bin));
	// }

	// Frente->Reset("ICES");

	// for (int i = 0; i < size; ++i)
	// {
	// 	bin = Frente->FindBin(dataX[i],dataY[i]);
	// 	Frente->SetBinContent(bin, dataE[i]/(howMany[i]) + Frente->GetBinContent(bin));
	// }

	// // Frente->Smooth(10,"k3a");

	// Frente->SetTitle("Mean energy of the muons at the pyramid base");
	// Frente->GetXaxis()->SetTitle("Distance (m)");
	// Frente->GetYaxis()->SetTitle("Distance (m)");
	// Frente->GetZaxis()->SetTitle("Mean energy (GeV)");
	// Frente->GetZaxis()->SetTitleOffset(1);
	// Frente->SetLineWidth(1);
	// Frente->Draw("colz");
	// // Frente->SetMaximum(max);




	return;
} 

