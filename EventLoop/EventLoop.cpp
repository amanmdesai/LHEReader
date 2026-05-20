// Author:  Aman Desai
// Date:    2026-03-02
// Input:   ROOT TTree 
// Output:  Normalized histograms as ROOT file and PDF plots
// Example file
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TSystem.h>
#include <vector>
#include <cmath>
#include <map>
#include <iostream>
#include <algorithm>


// ================= CONFIG =================
const float EL_PT_MIN   = 25.0;   // [GeV] minimum electron pT
const float MU_PT_MIN   = 25.0;   // [GeV] minimum muon pT
const float JET_PT_MIN  = 25.0;   // [GeV] minimum jet pT
const float ETA_MAX     = 2.5;    // maximum |η| for leptons and jets
const float ISO_MAX     = 0.1;    // maximum relative isolation I_rel
const float ISO_CONE    = 0.4;    // ΔR cone for isolation sum
const float OVERLAP_DR  = 0.4;    // ΔR threshold for lepton-jet overlap removal
const float Z_MASS_MIN  = 80.0;   // [GeV] lower edge of Z mass window
const float Z_MASS_MAX  = 100.0;  // [GeV] upper edge of Z mass window
const float BB_PT_MIN   = 80.0;   // [GeV] minimum pT of the bb system

// ================= Particle Classes =================
class Particle {
public:
    TLorentzVector p4;
    int charge;
    virtual ~Particle() {}
    double Pt()  const { return p4.Pt(); }
    double Eta() const { return p4.Eta(); }
    double Phi() const { return p4.Phi(); }
    double E() const { return p4.E(); }
};

class Electron : public Particle {
public:
    float iso = 0;
    bool pass = false;
};

class Muon : public Particle {
public:
    float iso = 0;
    bool pass = false;
};

class Jet : public Particle {
public:
    int flavour = 0;
    bool pass = false;
};

// ================= Histogram Manager =================
std::map<std::string, TH1F*> h1D;

void BookHist(const std::string &name,
              const std::string &title,
              int bins,double xmin,double xmax)
{
    h1D[name] = new TH1F(name.c_str(),title.c_str(),bins,xmin,xmax);
    h1D[name]->SetDirectory(nullptr);
}

void FillHist(const std::string &name,double value,double weight)
{
    if(h1D.count(name))
        h1D[name]->Fill(value,weight);
}

void ApplyPublicationStyle()
{
    gStyle->SetOptStat(0);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetLabelFont(42, "XYZ");
    gStyle->SetTextFont(42);
    gStyle->SetTitleSize(0.05, "XYZ");
    gStyle->SetLabelSize(0.042, "XYZ");
    gStyle->SetTitleOffset(1.15, "Y");
    gStyle->SetPadTopMargin(0.07);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLineWidth(2);
}

void SaveHistogramsAsPDF(const std::string &outDir = "plots",
                         const std::string &sampleLabel = "")
{
    ApplyPublicationStyle();
    gSystem->mkdir(outDir.c_str(), true);

    // Normalize all histograms to unit area once, before any drawing.
    for(auto &kv : h1D){
        TH1F *h = kv.second;
        if(!h || h->GetEntries() <= 0) continue;
        h->Scale(1.0/h->Integral());
        h->SetMaximum(h->GetMaximum()*1.5);
    }

    TCanvas c("c","c",900,700);

    // Style, annotate, and draw — called identically in both output passes.
    auto drawHist = [&](TH1F *h){
        h->SetLineColor(kBlue + 1);
        h->SetLineWidth(3);
        h->SetFillColorAlpha(kAzure - 9, 0.35);
        h->GetYaxis()->SetTitleOffset(1.25);

        c.Clear();
        c.SetTicks(1,1);
        h->Draw("HIST");

        TLatex label;
        label.SetNDC(true);
        label.SetTextFont(42);
        label.SetTextSize(0.038);
        label.DrawLatex(0.16, 0.82, "pp #rightarrow ZH, H #rightarrow b#bar{b}");
        label.DrawLatex(0.16, 0.86, "#sqrt{s} = 13.6 TeV");
        if(!sampleLabel.empty()){
            label.SetTextAlign(31); // right-aligned, top-right corner
            label.DrawLatex(0.9, 0.85, sampleLabel.c_str());
        }
        c.RedrawAxis();
    };

    // Pass 1: individual PDFs (no multipage driver open).
    for(auto &kv : h1D){
        TH1F *h = kv.second;
        if(!h || h->GetEntries() <= 0) continue;
        drawHist(h);
        c.SaveAs((outDir + "/" + kv.first + ".pdf").c_str());
    }

    // Pass 2: combined multipage PDF (no individual SaveAs calls open).
    const std::string multipage = outDir + "/all_histograms.pdf";
    c.Print((multipage + "[").c_str());
    for(auto &kv : h1D){
        TH1F *h = kv.second;
        if(!h || h->GetEntries() <= 0) continue;
        drawHist(h);
        c.Print(multipage.c_str());
    }
    c.Print((multipage + "]").c_str());
}

// ================= Isolation =================
double computeIsolation(const TLorentzVector& obj,
                        const std::vector<TLorentzVector>& particles,
                        double cone)
{
    double sumPt = 0;
    for(const auto &p : particles){
        if(obj.DeltaR(p) < 1e-6) continue;
        if(obj.DeltaR(p) < cone)
            sumPt += p.Pt();
    }
    return sumPt / obj.Pt();
}

// ================= Event Loop =================
void EventLoop(){
    auto processSample = [](const std::string &inputFile,
                            const std::string &plotDir,
                            const std::string &rootOutFile)
    {
        for(auto &kv : h1D){
            delete kv.second;
        }
        h1D.clear();

        TFile *f = TFile::Open(inputFile.c_str());
        if(!f || f->IsZombie()){
            std::cerr << "Cannot open file: " << inputFile << "\n";
            return;
        }

        TTree *tree = (TTree*)f->Get("events");
        if(!tree){
            std::cerr << "Cannot find TTree 'events' in " << inputFile << "\n";
            f->Close();
            return;
        }

        int   numParticles;
        float eventweight;
        std::vector<int>   *pid    = nullptr, *status = nullptr;
        std::vector<float> *px     = nullptr, *py     = nullptr,
                           *pz     = nullptr, *energy = nullptr;

        tree->SetBranchAddress("numParticles",&numParticles);
        tree->SetBranchAddress("eventweight",&eventweight);
        tree->SetBranchAddress("pid",&pid);
        tree->SetBranchAddress("status",&status);
        tree->SetBranchAddress("px",&px);
        tree->SetBranchAddress("py",&py);
        tree->SetBranchAddress("pz",&pz);
        tree->SetBranchAddress("energy",&energy);

        // ================= Book Histograms =================
        BookHist("electronPt","Electron transverse momentum; p_{T}^{e} [GeV]; Normalized events",50,0,200);
        BookHist("muonPt","Muon transverse momentum; p_{T}^{#mu} [GeV]; Normalized events",50,0,200);
        BookHist("electronIso","Electron isolation; I_{rel}^{e}; Normalized events",60,0,1.2);
        BookHist("muonIso","Muon isolation; I_{rel}^{#mu}; Normalized events",60,0,1.2);
        BookHist("jetPt","Jet transverse momentum; p_{T}^{jet} [GeV]; Normalized events",50,0,300);
        BookHist("MET","Missing transverse energy; E_{T}^{miss} [GeV]; Normalized events",50,0,300);

        BookHist("m_ee","Dielectron invariant mass; m_{ee} [GeV]; Normalized events",50,0,200);
        BookHist("m_mumu","Dimuon invariant mass; m_{#mu#mu} [GeV]; Normalized events",50,0,200);
        BookHist("m_jj","Dijet invariant mass; m_{jj} [GeV]; Normalized events",50,0,500);
        BookHist("m_bb","Selected b-jet pair mass; m_{bb} [GeV]; Normalized events",40,0,200);
        BookHist("n_cleanJets","Cleaned jet multiplicity; N_{jets}; Normalized events",12,0,12);
        BookHist("n_bjets","b-jet multiplicity; N_{b-jets}; Normalized events",8,0,8);
        BookHist("n_gluonJets","Gluon-jet multiplicity; N_{g-jets}; Normalized events",8,0,8);
        BookHist("bjet1Pt","Leading b-jet transverse momentum; p_{T}^{b_{1}} [GeV]; Normalized events",60,0,400);
        BookHist("bjet2Pt","Subleading b-jet transverse momentum; p_{T}^{b_{2}} [GeV]; Normalized events",60,0,400);
        BookHist("bbPt","Selected bb-system transverse momentum; p_{T}^{bb} [GeV]; Normalized events",60,0,500);
        BookHist("bbEta","Selected bb-system pseudorapidity; #eta_{bb}; Normalized events",50,-5,5);

        Long64_t nentries = tree->GetEntries();

        for(Long64_t i=0;i<nentries;i++){
            tree->GetEntry(i);

            std::vector<TLorentzVector> isoParticles;
            std::vector<Electron> electrons;
            std::vector<Muon> muons;
            std::vector<Jet> jets;

            TLorentzVector metVec(0,0,0,0);

            // ========= Build Objects =========
            for(int j=0;j<numParticles;j++){

                if((*status)[j] != 1) continue;

                TLorentzVector p4;
                p4.SetPxPyPzE((*px)[j],(*py)[j],(*pz)[j],(*energy)[j]);

                int absPID = std::abs((*pid)[j]);

                // Neutrinos → MET
                if(absPID==12 || absPID==14 || absPID==16){
                    metVec += p4;
                    continue;
                }

                isoParticles.push_back(p4);

                // Electrons
                if(absPID==11){
                    Electron el;
                    el.p4 = p4;
                    el.charge = ((*pid)[j]>0)? -1:+1;
                    el.pass = (el.Pt()>EL_PT_MIN &&
                               std::fabs(el.Eta())<ETA_MAX);
                    electrons.push_back(el);
                }

                // Muons
                else if(absPID==13){
                    Muon mu;
                    mu.p4 = p4;
                    mu.charge = ((*pid)[j]>0)? -1:+1;
                    mu.pass = (mu.Pt()>MU_PT_MIN &&
                               std::fabs(mu.Eta())<ETA_MAX);
                    muons.push_back(mu);
                }

                // Jets
                else if((absPID>=1 && absPID<=5) || absPID==21){
                    Jet jx;
                    jx.p4 = p4;
                    jx.flavour = absPID;
                    jx.pass = (jx.Pt()>JET_PT_MIN && std::fabs(jx.Eta())<ETA_MAX);
                    jets.push_back(jx);
                }
            }

            // ========= Isolation =========
            for(auto &el : electrons){
                el.iso = computeIsolation(el.p4, isoParticles, ISO_CONE);
                FillHist("electronIso", el.iso, eventweight);
                if(el.iso > ISO_MAX) el.pass = false;
                if(el.pass)
                    FillHist("electronPt", el.Pt(), eventweight);
            }

            for(auto &mu : muons){
                mu.iso = computeIsolation(mu.p4, isoParticles, ISO_CONE);
                FillHist("muonIso", mu.iso, eventweight);
                if(mu.iso > ISO_MAX) mu.pass = false;
                if(mu.pass)
                    FillHist("muonPt", mu.Pt(), eventweight);
            }

            // ========= Jet Cleaning =========
            std::vector<Jet> cleanJets;
            for(auto &jx : jets){
                if(!jx.pass) continue;

                bool overlap = false;
                for(auto &el : electrons)
                    if(el.pass && jx.p4.DeltaR(el.p4) < OVERLAP_DR)
                        overlap = true;

                for(auto &mu : muons)
                    if(mu.pass && jx.p4.DeltaR(mu.p4) < OVERLAP_DR)
                        overlap = true;

                if(!overlap){
                    cleanJets.push_back(jx);
                    FillHist("jetPt", jx.Pt(), eventweight);
                }
            }
            FillHist("n_cleanJets", cleanJets.size(), eventweight);

            FillHist("MET", metVec.Pt(), eventweight);

            // ================= Build Good Leptons =================
            std::vector<Electron> goodEle;
            for(auto &el : electrons)
                if(el.pass)
                    goodEle.push_back(el);

            std::vector<Muon> goodMu;
            for(auto &mu : muons)
                if(mu.pass)
                    goodMu.push_back(mu);

            // ---- Sort leptons by descending pT ----
            std::sort(goodEle.begin(), goodEle.end(),
                    [](const Electron &a, const Electron &b){
                        return a.Pt() > b.Pt();
                    });

            std::sort(goodMu.begin(), goodMu.end(),
                    [](const Muon &a, const Muon &b){
                        return a.Pt() > b.Pt();
                    });

            // ================= Z SELECTION =================
            bool passZ   = false;
            bool passZee = false;
            double mll   = -1.0;

            // ---- Try Z → ee using two highest-pT electrons ----
            if(goodEle.size() >= 2){

                const Electron &l1 = goodEle[0];
                const Electron &l2 = goodEle[1];

                if(l1.charge * l2.charge < 0){

                    mll = (l1.p4 + l2.p4).M();

                    if(mll > Z_MASS_MIN && mll < Z_MASS_MAX){
                        passZ   = true;
                        passZee = true;
                    }
                }
            }

            // ---- Try Z → μμ if ee failed ----
            if(!passZ && goodMu.size() >= 2){

                const Muon &l1 = goodMu[0];
                const Muon &l2 = goodMu[1];

                if(l1.charge * l2.charge < 0){

                    mll = (l1.p4 + l2.p4).M();

                    if(mll > Z_MASS_MIN && mll < Z_MASS_MAX)
                        passZ = true;
                }
            }

            // ---- Reject event if no valid Z ----
            if(!passZ)
                continue;

            if(passZee)
                FillHist("m_ee", mll, eventweight);
            else
                FillHist("m_mumu", mll, eventweight);

            // ================= SORT JETS BY pT =================
            std::sort(cleanJets.begin(), cleanJets.end(),
                    [](const Jet &a, const Jet &b){
                        return a.Pt() > b.Pt();
                    });

            // ================= Leading Two Jets =================
            if(cleanJets.size() >= 2){

                const Jet &j1 = cleanJets[0];
                const Jet &j2 = cleanJets[1];

                double mjj = (j1.p4 + j2.p4).M();
                FillHist("m_jj", mjj, eventweight);
            }

            // ================= Leading Two b-Jets =================
            std::vector<Jet> bjets;
            std::vector<Jet> gluonJets;
            for(auto &jx : cleanJets){
                if(jx.flavour == 5)
                    bjets.push_back(jx);
                else if(jx.flavour == 21)
                    gluonJets.push_back(jx);
            }
            FillHist("n_bjets", bjets.size(), eventweight);
            FillHist("n_gluonJets", gluonJets.size(), eventweight);

            // Sort bjets by pT
            std::sort(bjets.begin(), bjets.end(),
                    [](const Jet &a, const Jet &b){
                        return a.Pt() > b.Pt();
                    });

            if(bjets.size() < 2) continue;

            TLorentzVector bbP4 = bjets[0].p4 + bjets[1].p4;
            if(bbP4.Pt() < BB_PT_MIN) continue;

            FillHist("bjet1Pt", bjets[0].Pt(), eventweight);
            FillHist("bjet2Pt", bjets[1].Pt(), eventweight);
            FillHist("m_bb",    bbP4.M(),      eventweight);
            FillHist("bbPt",    bbP4.Pt(),     eventweight);
            FillHist("bbEta",   bbP4.Eta(),    eventweight);
        }

        // ========= Save =========
        TFile fout(rootOutFile.c_str(),"RECREATE");
        for(auto &h : h1D)
            h.second->Write();
        fout.Close();

        const std::string sampleLabel = (plotDir.find("parton") != std::string::npos)
            ? "Parton"
            : "Reconstructed";
        SaveHistogramsAsPDF(plotDir, sampleLabel);
        f->Close();

        std::cout << "Done: " << inputFile << " -> " << rootOutFile
                  << " and " << plotDir << "/*.pdf\n";
    };

    processSample("samples/ma5_zh.root", "plots/reco", "output_histos_reco.root");
    processSample("samples/parton_zh.root", "plots/parton", "output_histos_parton.root");
}
