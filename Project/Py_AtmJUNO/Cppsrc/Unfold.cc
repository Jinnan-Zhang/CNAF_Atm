/*
* @Purpose: to inplement the bayes unfolding procedure.
* @Author: Jinnan Zhang: jinnan.zhang@ihep.ac.cn
* @Date 2020.07.21
*/

#include <TChain.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TEntryList.h>
#include <TF1.h>
#include "TStyle.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include "TLine.h"
#include "TMath.h"
#include <TPad.h>
#include <TObject.h>
#include <TGraph.h>
#include <TRandom3.h>
#include <TLegend.h>
#include <TFitResultPtr.h>
#include <vector>
#include <string>
#include <fstream>
// #include "RooUnfoldBayes.h"
// #include "RooUnfoldResponse.h"

void LoadFile(std::string filename, std::vector<std::vector<double>> &v, int Length = 2, int SkipLines = 0);
void BayesUnfold(int Iter_NUM = 2);
void TryRooUnfold(int Iter_NUM = 2);
void Iter_Bayes_Once(TH1 *h_input, TH1 *h_prior, TH2 *h_Likelihd_M, TH1 *h_output, double *epsilon_i);
void Pre_Flux();
template <typename T>
void GetObjFromFile(TFile *File, T *h[], TString ObjNames[], int NUMObj)
{
    for (int i = 0; i < NUMObj; i++)
    {
        h[i] = dynamic_cast<T *>(File->Get(ObjNames[i]));
    }
}
void ShowUncertainty_stat(int MCPtsNUM = 1000, int statDisBinNUM = 50);
void ShowUncertainty_Xsec(int MCPtsNUM = 1000, int statDisBinNUM = 50);
void ShowUncertainty_SamSel(const int SampelNUM = 11);
void ShowUncertainty_Osci();
void ShowUncertainties_all();
const int Expected_evt_NUM_eCC[] = {40, 100, 125, 135, 80, 45, 20};
const int Expected_evt_NUM_muCC[] = {55, 237, 231, 171, 100, 48, 17};
const int NPE_BINNUM_eCC = 7;
const int NPE_BINNUM_muCC = 8;
const int Etrue_BINNUM_eCC = 7;
const int Etrue_BINNUM_muCC = 7;
double logNPE_range_eCC[2] = {5, 7.2};
double logNPE_range_muCC[2] = {5.7, 7.2};
double logEtrue_range_eCC[2] = {-1, 1.05};
double logEtrue_range_muCC[2] = {-0.3, 1.05};
//in ns, muCC eCC
const double Cut_sigma_tres_Sg_muCC = 113;
const double Cut_sigma_tres_Sg_eCC = 86;
const double Cut_NPE_low_Sg[2] = {5e5, 1e5};
const double Cut_NPE_up_Sg[2] = {1.585e7, 1.585e7};

int Unfold()
{
    // gSystem->Load("/home/jinnan/SoftWare/srcs/RooUnfold/libRooUnfold");
    // gSystem->Load("/home/jinnan/SoftWare/src/RooUnfold/libRooUnfold");
    // BayesUnfold(2);
    TryRooUnfold(2);

    // ShowUncertainty_stat();
    // ShowUncertainty_Xsec();
    // ShowUncertainty_SamSel(4);
    // ShowUncertainty_Osci();
    // ShowUncertainties_all();

    // Pre_Flux();
    return 0;
}

//apply Bayesian iterration to unfold the spectras
//* @param Iter_NUM number of Bayes iteration times.
void BayesUnfold(int Iter_NUM)
{
    TFile *ff_unfold_data = TFile::Open("../data/UnfoldData.root", "READ");
    TH2 *h_likeli_eCC = (TH2 *)ff_unfold_data->Get("eCC_Likely_hood");
    TH2 *h_likeli_muCC = (TH2 *)ff_unfold_data->Get("muCC_Likely_hood");
    TH1 *h_sel_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEeCCSel"));
    TH1 *h_sel_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEmuCCSel"));
    TH1 *h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
    TH1 *h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));
    // TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCTrue"));
    // TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCTrue"));
    TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCSel;1"));
    TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCSel;1"));
    TH1 *h_eCC_result = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_result"));
    TH1 *h_muCC_result = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_result"));
    const int Enu_BINNUM_eCC = h_Prior_eCC->GetNbinsX();
    const int Enu_BINNUM_muCC = h_Prior_muCC->GetNbinsX();
    double epsilon_i_eCC[] = {
        0.1104294479,
        0.0014534884,
        0.0002564892,
        0.0001964926,
        0.0000699447,
        0.0000000000,
        0.0000000000,
    };
    double epsilon_i_muCC[] = {
        0.0000391742,
        0.0000373650,
        0.0000413018,
        0.0000000000,
        0.0001890181,
        0.0000000000,
        0.0000000000,
    };
    // for (int i = 0; i < Enu_BINNUM_eCC; i++)
    // {
    // epsilon_i_eCC[i] = h_likeli_eCC->Integral(i + 1, i + 1, 1, NPE_BINNUM_eCC);
    //     epsilon_i_eCC[i] = 0;
    // }
    // for (int i = 0; i < Enu_BINNUM_muCC; i++)
    // {
    // epsilon_i_muCC[i] = h_likeli_muCC->Integral(i + 1, i + 1, 1, NPE_BINNUM_muCC);
    //     epsilon_i_muCC[i] = 0;
    // }
    h_Prior_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_Prior"));
    h_Prior_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_Prior"));
    h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
    h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());
    Iter_Bayes_Once(h_sel_eCC, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
    Iter_Bayes_Once(h_sel_muCC, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
    if (Iter_NUM > 1)
        for (int i = 1; i < Iter_NUM; i++)
        {
            h_Prior_eCC = dynamic_cast<TH1 *>(h_eCC_result->Clone("eCC_Prior"));
            h_Prior_muCC = dynamic_cast<TH1 *>(h_muCC_result->Clone("muCC_Prior"));
            h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
            h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());
            Iter_Bayes_Once(h_sel_eCC, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
            Iter_Bayes_Once(h_sel_muCC, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
        }
    // h_likeli_muCC->Draw("colz");
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TCanvas *c_CC_Spec = new TCanvas("muCC_Spec", "", 800, 600);
    TPad *p1 = new TPad("p1", "p1", 0.01, 0.3, 0.95, 0.95, 0, 0, 0);
    p1->SetTopMargin(0);
    p1->SetBottomMargin(0);
    p1->SetRightMargin(0.02);
    p1->Draw();

    TPad *p2 = new TPad("p2", "p2", 0.01, 0.0, 0.95, 0.3, 0, 0, 0);
    p2->SetTopMargin(0);
    //p2->SetLeftMargin(0);
    p2->SetRightMargin(0.02);
    p2->SetBottomMargin(0.25);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p2->Draw();

    p1->cd();
    TLegend *leg_[2];
    leg_[0] = new TLegend();
    leg_[1] = new TLegend();
    h_muCC_result->SetLineColor(kRed);
    h_muCC_result->SetMarkerColor(kRed);
    h_muCC_result->SetMarkerSize(2);

    h_muCC_result->SetAxisRange(0, h_muCC_result->GetMaximum() + 50, "Y");
    h_muCC_result->Draw();
    h_MC_true_muCC->Draw("SAME");
    leg_[0]->AddEntry(h_MC_true_muCC, "#nu_{#mu} MC Truth");
    leg_[0]->AddEntry(h_muCC_result, "#nu_{#mu} Unfolded");
    leg_[0]->DrawClone("SAME");

    // TCanvas *c_muCC = new TCanvas("muCC_");
    p2->cd();
    TH1 *h_ref_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("refLine_muCC"));
    TH1 *h_ratio_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("Ratio_muCC"));
    h_ref_muCC->Divide(h_MC_true_muCC);
    h_ratio_muCC->Divide(h_muCC_result, h_MC_true_muCC);
    h_ratio_muCC->SetLineColor(kRed);
    h_ratio_muCC->SetMarkerColor(kRed);
    h_ratio_muCC->SetMarkerSize(2);
    h_ratio_muCC->SetMarkerStyle(kFullCircle);
    h_ratio_muCC->SetYTitle("Reco/MC");
    h_ratio_muCC->GetYaxis()->SetTitleSize(0.15);
    h_ratio_muCC->GetYaxis()->SetTitleOffset(0.3);
    h_ratio_muCC->GetYaxis()->SetLabelSize(0.1);
    h_ratio_muCC->GetXaxis()->SetTitleSize(0.11);
    h_ratio_muCC->GetXaxis()->SetTitleOffset(0.9);
    h_ratio_muCC->GetXaxis()->SetLabelSize(0.1);
    h_ratio_muCC->SetAxisRange(0, 2, "Y");
    h_ratio_muCC->Draw("E1");
    h_ref_muCC->Draw("SAME");

    TCanvas *c_eCC_Spec = new TCanvas("eCC_Spec", "", 800, 600);
    TPad *p3 = new TPad("p3", "p3", 0.01, 0.3, 0.95, 0.95, 0, 0, 0);
    p3->SetTopMargin(0);
    p3->SetBottomMargin(0);
    //p3->SetLeftMargin(0);
    p3->SetRightMargin(0.02);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p3->Draw();

    TPad *p4 = new TPad("p4", "p4", 0.01, 0.0, 0.95, 0.3, 0, 0, 0);
    p4->SetTopMargin(0);
    //p4->SetLeftMargin(0);
    p4->SetRightMargin(0.02);
    p4->SetBottomMargin(0.25);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p4->Draw();
    p3->cd();
    h_eCC_result->SetLineColor(kRed);
    h_eCC_result->SetMarkerColor(kRed);
    h_eCC_result->SetMarkerSize(2);
    h_eCC_result->Draw();
    h_eCC_result->SetAxisRange(0, h_eCC_result->GetMaximum() + 20, "Y");
    h_MC_true_eCC->Draw("SAME");
    leg_[1]->AddEntry(h_MC_true_eCC, "#nu_{e} MC Truth");
    leg_[1]->AddEntry(h_eCC_result, "#nu_{e}  Unfolded");
    leg_[1]->DrawClone("SAME");
    // h_MC_true_eCC->Draw();

    // TCanvas *c_eCC = new TCanvas("eCC_");
    p4->cd();
    TH1 *h_ref_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("refLine_eCC"));
    TH1 *h_ratio_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("Ratio_eCC"));
    h_ref_eCC->Divide(h_MC_true_eCC);
    h_ratio_eCC->Divide(h_eCC_result, h_MC_true_eCC);
    h_ratio_eCC->SetLineColor(kRed);
    h_ratio_eCC->SetMarkerColor(kRed);
    h_ratio_eCC->SetMarkerSize(2);
    h_ratio_eCC->SetMarkerStyle(kFullCircle);
    h_ratio_eCC->SetYTitle("Reco/MC");
    h_ratio_eCC->GetYaxis()->SetTitleSize(0.15);
    h_ratio_eCC->GetYaxis()->SetTitleOffset(0.3);
    h_ratio_eCC->GetYaxis()->SetLabelSize(0.1);
    h_ratio_eCC->GetXaxis()->SetTitleSize(0.11);
    h_ratio_eCC->GetXaxis()->SetTitleOffset(0.9);
    h_ratio_eCC->GetXaxis()->SetLabelSize(0.1);

    h_ratio_eCC->SetAxisRange(0, 2, "Y");
    h_ratio_eCC->Draw("E1");
    h_ref_eCC->Draw("SAME");

    // TFile *ff_Sel=TFile::Open("../data/SampleSelection/SampleSelection.root","recreate");
    // TFile *ff_Sel = TFile::Open("../data/SampleSelection/SampleSelection.root", "update");
    // ff_Sel->cd();
    // // h_eCC_result->Write(Form("CuteCC%dns", 91), TObject::kOverwrite);
    // // h_muCC_result->Write(Form("CutmuCC%dns", 118), TObject::kOverwrite);
    // h_eCC_result->Write(Form("My_CuteCC%dns", 86), TObject::kOverwrite);
    // h_muCC_result->Write(Form("My_CutmuCC%dns", 113), TObject::kOverwrite);
}

//input NPE spectra and prios disribution,
//output neutrino spectra
void Iter_Bayes_Once(TH1 *h_input,
                     TH1 *h_prior,
                     TH2 *h_Likelihd_M,
                     TH1 *h_output,
                     double *epsilon_i)
{
    const int NPE_BINNUM = h_input->GetNbinsX();
    const int Enu_BINNUM = h_prior->GetNbinsX();
    double A_ji[NPE_BINNUM][Enu_BINNUM];
    double Response_SUM[Enu_BINNUM];
    for (int i = 0; i < Enu_BINNUM; i++)
    {
        Response_SUM[i] = h_Likelihd_M->Integral(i + 1, i + 1, 1, NPE_BINNUM);
        for (int j = 0; j < NPE_BINNUM; j++)
        {
            //normalize the the likelihood sum of a row to 1-epsilon
            A_ji[j][i] = h_Likelihd_M->GetBinContent(i + 1, j + 1) *
                         (1 - epsilon_i[i]) / Response_SUM[i];
            // A_ji[j][i] = h_Likelihd_M->GetBinContent(i + 1, j + 1);
        }
    }
    {
        for (int i = 0; i < Enu_BINNUM; i++)
        {
            for (int j = 0; j < NPE_BINNUM; j++)
            {
                printf("A_%d%d: %f\n", j + 1, i + 1, A_ji[j][i]);
            }
        }
    }

    double U_ij[Enu_BINNUM][NPE_BINNUM];
    double P_0Ei = 0;
    double nominator_i, denominator_i;
    for (int i = 0; i < Enu_BINNUM; i++)
    {
        P_0Ei = h_prior->GetBinContent(i + 1);
        for (int j = 0; j < NPE_BINNUM; j++)
        {
            nominator_i = A_ji[j][i] * P_0Ei;
            denominator_i = 0;
            for (int k = 0; k < Enu_BINNUM; k++)
            {
                denominator_i += (A_ji[j][k] *
                                  h_prior->GetBinContent(k + 1));
            }
            if (denominator_i == 0)
                denominator_i = 1;
            U_ij[i][j] = nominator_i / denominator_i;
        }
    }
    double E_hat = 0;
    double BinCenter = 0;
    h_output->Reset();
    for (int i = 0; i < Enu_BINNUM; i++)
    {
        E_hat = 0;
        for (int j = 0; j < NPE_BINNUM; j++)
        {
            E_hat += (U_ij[i][j] *
                      h_input->GetBinContent(j + 1));
        }
        // printf("E_hat: %f\n", E_hat);
        // if (epsilon_i[i] != 0)
        E_hat /= (1 - epsilon_i[i]);
        BinCenter = h_output->GetBinCenter(i + 1);
        h_output->SetBinContent(i + 1, E_hat);
        h_output->SetBinError(i + 1, sqrt(E_hat));
    }
}

TH2 *SwapX2Y(TH2 *h_innital)
{
    int nbinx = h_innital->GetNbinsX();
    int nbiny = h_innital->GetNbinsY();
    double x1 = h_innital->GetXaxis()->GetBinLowEdge(1);
    double x2 = h_innital->GetXaxis()->GetBinLowEdge(nbinx) + h_innital->GetXaxis()->GetBinWidth(nbinx);
    double y1 = h_innital->GetYaxis()->GetBinLowEdge(1);
    double y2 = h_innital->GetYaxis()->GetBinLowEdge(nbiny) + h_innital->GetYaxis()->GetBinWidth(nbiny);
    TH2D *h_after = new TH2D(Form("%sSwap", h_innital->GetName()), h_innital->GetTitle(), nbiny, y1, y2, nbinx, x1, x2);
    double c;
    for (int i = 1; i <= nbinx; i++)
    {
        for (int j = 1; j <= nbiny; j++)
        {
            c = h_innital->GetBinContent(i, j);
            h_after->SetBinContent(j, i, c);
        }
    }
    return h_after;
}
//apply rooUnfold
void TryRooUnfold(int Iter_NUM)
{
    gSystem->Load("/home/jinnan/SoftWare/srcs/RooUnfold/libRooUnfold");
    // gSystem->Load("/home/jinnan/SoftWare/src/RooUnfold/libRooUnfold");
    TFile *ff_unfold_data = TFile::Open("../data/UnfoldData.root", "READ");
    TH2 *h_likeli_eCC = SwapX2Y(dynamic_cast<TH2 *>(ff_unfold_data->Get("eCC_Likely_hood")));
    TH2 *h_likeli_muCC = SwapX2Y(dynamic_cast<TH2 *>(ff_unfold_data->Get("muCC_Likely_hood")));
    TH1 *h_sel_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEeCCSel"));
    TH1 *h_sel_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEmuCCSel"));
    TH1 *h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
    TH1 *h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));
    // TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCTrue"));
    // TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCTrue"));
    TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCSel;1"));
    TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCSel;1"));
    TH1 *h_eCC_result = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_result"));
    TH1 *h_muCC_result = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_result"));

    // RooUnfoldResponse *R_atm_eCC = new RooUnfoldResponse(h_sel_eCC, h_MC_true_eCC, h_likeli_eCC);
    // RooUnfoldResponse *R_atm_muCC = new RooUnfoldResponse(h_sel_muCC, h_MC_true_muCC, h_likeli_muCC);
    RooUnfoldResponse *R_atm_eCC = new RooUnfoldResponse(0, 0, h_likeli_eCC);
    RooUnfoldResponse *R_atm_muCC = new RooUnfoldResponse(0, 0, h_likeli_muCC);

    // RooUnfoldResponse *R_atm_eCC = new RooUnfoldResponse(NPE_BINNUM_eCC, logNPE_range_eCC[0], logNPE_range_eCC[1], Etrue_BINNUM_eCC, logEtrue_range_eCC[0], logEtrue_range_eCC[1]);
    // RooUnfoldResponse *R_atm_muCC = new RooUnfoldResponse(NPE_BINNUM_muCC, logNPE_range_muCC[0], logNPE_range_muCC[1], Etrue_BINNUM_muCC, logEtrue_range_muCC[0], logEtrue_range_muCC[1]);
    // double Sigma_cut_muCC = Cut_sigma_tres_Sg_muCC; //ns
    // double Sigma_cut_eCC = Cut_sigma_tres_Sg_eCC;   //ns
    // double NPE_cut_muCC[2] = {Cut_NPE_low_Sg[0], Cut_NPE_up_Sg[0]};
    // double NPE_cut_eCC[2] = {Cut_NPE_low_Sg[1], Cut_NPE_up_Sg[1]};
    // TChain muCC_NPETresE("muCC_NPETresE");
    // TChain eCC_NPETresE("eCC_NPETresE");
    // TChain NC_NPETresE("NC_NPETresE");
    // muCC_NPETresE.Add("../results/result_NPETE*_100.root");
    // eCC_NPETresE.Add("../results/result_NPETE*_100.root");
    // NC_NPETresE.Add("../results/result_NPETE*_100.root");
    // float sigma_tres[3] = {0}, NPE_LPMT[3] = {0}, E_nu_true[3] = {0};
    // {
    //     muCC_NPETresE.SetBranchAddress("sigma_tres", &sigma_tres[0]);
    //     muCC_NPETresE.SetBranchAddress("NPE_LPMT", &NPE_LPMT[0]);
    //     muCC_NPETresE.SetBranchAddress("E_nu_true", &E_nu_true[0]);
    //     eCC_NPETresE.SetBranchAddress("sigma_tres", &sigma_tres[1]);
    //     eCC_NPETresE.SetBranchAddress("NPE_LPMT", &NPE_LPMT[1]);
    //     eCC_NPETresE.SetBranchAddress("E_nu_true", &E_nu_true[1]);
    //     NC_NPETresE.SetBranchAddress("sigma_tres", &sigma_tres[2]);
    //     NC_NPETresE.SetBranchAddress("NPE_LPMT", &NPE_LPMT[2]);
    //     NC_NPETresE.SetBranchAddress("E_nu_true", &E_nu_true[2]);
    // }
    // for (int i = 0; i < muCC_NPETresE.GetEntries(); i++)
    // {
    //     muCC_NPETresE.GetEntry(i);
    //     if (sigma_tres[0] >= Sigma_cut_muCC)
    //     {
    //         if (NPE_LPMT[0] >= NPE_cut_muCC[0] &&
    //             NPE_LPMT[0] <= NPE_cut_muCC[1])
    //             R_atm_muCC->Fill(log10(NPE_LPMT[0]), log10(E_nu_true[0])); //all
    //         else
    //             R_atm_muCC->Miss(log10(E_nu_true[0]));
    //     }
    //     if (sigma_tres[0] <= Sigma_cut_eCC)
    //     {
    //         if (NPE_LPMT[0] >= NPE_cut_eCC[0] &&
    //             NPE_LPMT[0] <= NPE_cut_eCC[1])
    //             R_atm_eCC->Fill(log10(NPE_LPMT[0]), log10(E_nu_true[0])); //all
    //         else
    //             R_atm_eCC->Miss(log10(E_nu_true[0]));
    //     }
    //     if (i < eCC_NPETresE.GetEntries())
    //     {
    //         eCC_NPETresE.GetEntry(i);
    //         if (sigma_tres[1] >= Sigma_cut_muCC)
    //         {
    //             if (NPE_LPMT[1] >= NPE_cut_muCC[0] &&
    //                 NPE_LPMT[1] <= NPE_cut_muCC[1])
    //                 R_atm_muCC->Fill(log10(NPE_LPMT[1]), log10(E_nu_true[1])); //all
    //             else
    //                 R_atm_muCC->Miss(log10(E_nu_true[1]));
    //         }
    //         if (sigma_tres[1] <= Sigma_cut_eCC)
    //         {
    //             if (NPE_LPMT[1] >= NPE_cut_eCC[0] &&
    //                 NPE_LPMT[1] <= NPE_cut_eCC[1])
    //                 R_atm_eCC->Fill(log10(NPE_LPMT[1]), log10(E_nu_true[1])); //all
    //             else
    //                 R_atm_eCC->Miss(log10(E_nu_true[1]));
    //         }
    //     }
    //     if (i < NC_NPETresE.GetEntries())
    //     {
    //         NC_NPETresE.GetEntry(i);
    //         if (sigma_tres[2] >= Sigma_cut_muCC)
    //         {
    //             if (NPE_LPMT[2] >= NPE_cut_muCC[0] &&
    //                 NPE_LPMT[2] <= NPE_cut_muCC[1])
    //                 R_atm_muCC->Fill(log10(NPE_LPMT[2]), log10(E_nu_true[2])); //all
    //             else
    //                 R_atm_muCC->Miss(log10(E_nu_true[2]));
    //         }
    //         if (sigma_tres[2] <= Sigma_cut_eCC)
    //         {
    //             if (NPE_LPMT[2] >= NPE_cut_eCC[0] &&
    //                 NPE_LPMT[2] <= NPE_cut_eCC[1])
    //                 R_atm_eCC->Fill(log10(NPE_LPMT[2]), log10(E_nu_true[2])); //all
    //             else
    //                 R_atm_eCC->Miss(log10(E_nu_true[2]));
    //         }
    //     }
    // }
    // TH2 *h_eCC=R_atm_eCC->Hresponse();
    // h_eCC->Draw("colz");

    RooUnfoldBayes *unfold_eCC = new RooUnfoldBayes(R_atm_eCC, h_sel_eCC, Iter_NUM);
    RooUnfoldBayes *unfold_muCC = new RooUnfoldBayes(R_atm_muCC, h_sel_muCC, Iter_NUM);
    unfold_eCC->PrintTable(cout, h_MC_true_eCC);
    h_eCC_result = dynamic_cast<TH1 *>(unfold_eCC->Hreco());
    unfold_muCC->PrintTable(cout, h_MC_true_muCC);
    h_muCC_result = dynamic_cast<TH1 *>(unfold_muCC->Hreco());

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TCanvas *c_CC_Spec = new TCanvas("muCC_Spec", "", 800, 600);
    TPad *p1 = new TPad("p1", "p1", 0.01, 0.3, 0.95, 0.95, 0, 0, 0);
    p1->SetTopMargin(0);
    p1->SetBottomMargin(0);
    p1->SetRightMargin(0.02);
    p1->Draw();

    TPad *p2 = new TPad("p2", "p2", 0.01, 0.0, 0.95, 0.3, 0, 0, 0);
    p2->SetTopMargin(0);
    //p2->SetLeftMargin(0);
    p2->SetRightMargin(0.02);
    p2->SetBottomMargin(0.25);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p2->Draw();

    p1->cd();
    TLegend *leg_[2];
    leg_[0] = new TLegend();
    leg_[1] = new TLegend();
    h_muCC_result->SetLineColor(kRed);
    h_muCC_result->SetMarkerColor(kRed);
    h_muCC_result->SetMarkerSize(2);

    h_muCC_result->SetAxisRange(0, h_muCC_result->GetMaximum() + 50, "Y");
    h_muCC_result->Draw();
    h_MC_true_muCC->Draw("SAME");
    leg_[0]->AddEntry(h_MC_true_muCC, "#nu_{#mu} MC Truth");
    leg_[0]->AddEntry(h_muCC_result, "#nu_{#mu} Unfolded");
    leg_[0]->DrawClone("SAME");

    // TCanvas *c_muCC = new TCanvas("muCC_");
    p2->cd();
    TH1 *h_ref_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("refLine_muCC"));
    TH1 *h_ratio_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("Ratio_muCC"));
    h_ref_muCC->Divide(h_MC_true_muCC);
    h_ratio_muCC->Divide(h_muCC_result, h_MC_true_muCC);
    h_ratio_muCC->SetLineColor(kRed);
    h_ratio_muCC->SetMarkerColor(kRed);
    h_ratio_muCC->SetMarkerSize(2);
    h_ratio_muCC->SetMarkerStyle(kFullCircle);
    h_ratio_muCC->SetYTitle("Reco/MC");
    h_ratio_muCC->GetYaxis()->SetTitleSize(0.15);
    h_ratio_muCC->GetYaxis()->SetTitleOffset(0.3);
    h_ratio_muCC->GetYaxis()->SetLabelSize(0.1);
    h_ratio_muCC->GetXaxis()->SetTitleSize(0.11);
    h_ratio_muCC->GetXaxis()->SetTitleOffset(0.9);
    h_ratio_muCC->GetXaxis()->SetLabelSize(0.1);
    h_ratio_muCC->SetAxisRange(0, 2, "Y");
    h_ratio_muCC->Draw("E1");
    h_ref_muCC->Draw("SAME");

    TCanvas *c_eCC_Spec = new TCanvas("eCC_Spec", "", 800, 600);
    TPad *p3 = new TPad("p3", "p3", 0.01, 0.3, 0.95, 0.95, 0, 0, 0);
    p3->SetTopMargin(0);
    p3->SetBottomMargin(0);
    //p3->SetLeftMargin(0);
    p3->SetRightMargin(0.02);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p3->Draw();

    TPad *p4 = new TPad("p4", "p4", 0.01, 0.0, 0.95, 0.3, 0, 0, 0);
    p4->SetTopMargin(0);
    //p4->SetLeftMargin(0);
    p4->SetRightMargin(0.02);
    p4->SetBottomMargin(0.25);
    //gPad->SetTickx(2);
    //gPad->SetTicky(2);
    p4->Draw();
    p3->cd();
    h_eCC_result->SetLineColor(kRed);
    h_eCC_result->SetMarkerColor(kRed);
    h_eCC_result->SetMarkerSize(2);
    h_eCC_result->Draw();
    h_eCC_result->SetAxisRange(0, h_eCC_result->GetMaximum() + 20, "Y");
    h_MC_true_eCC->Draw("SAME");
    leg_[1]->AddEntry(h_MC_true_eCC, "#nu_{e} MC Truth");
    leg_[1]->AddEntry(h_eCC_result, "#nu_{e}  Unfolded");
    leg_[1]->DrawClone("SAME");
    // h_MC_true_eCC->Draw();

    // TCanvas *c_eCC = new TCanvas("eCC_");
    p4->cd();
    TH1 *h_ref_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("refLine_eCC"));
    TH1 *h_ratio_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("Ratio_eCC"));
    h_ref_eCC->Divide(h_MC_true_eCC);
    h_ratio_eCC->Divide(h_eCC_result, h_MC_true_eCC);
    h_ratio_eCC->SetLineColor(kRed);
    h_ratio_eCC->SetMarkerColor(kRed);
    h_ratio_eCC->SetMarkerSize(2);
    h_ratio_eCC->SetMarkerStyle(kFullCircle);
    h_ratio_eCC->SetYTitle("Reco/MC");
    h_ratio_eCC->GetYaxis()->SetTitleSize(0.15);
    h_ratio_eCC->GetYaxis()->SetTitleOffset(0.3);
    h_ratio_eCC->GetYaxis()->SetLabelSize(0.1);
    h_ratio_eCC->GetXaxis()->SetTitleSize(0.11);
    h_ratio_eCC->GetXaxis()->SetTitleOffset(0.9);
    h_ratio_eCC->GetXaxis()->SetLabelSize(0.1);

    h_ratio_eCC->SetAxisRange(0, 2, "Y");
    h_ratio_eCC->Draw("E1");
    h_ref_eCC->Draw("SAME");

    // TFile *ff_Sel=TFile::Open("../data/SampleSelection/SampleSelection.root","recreate");
    TFile *ff_Sel = TFile::Open("../data/SampleSelection/SampleSelection.root", "update");
    ff_Sel->cd();
    // h_eCC_result->Write(Form("CuteCC%dns", 91), TObject::kOverwrite);
    // h_muCC_result->Write(Form("CutmuCC%dns", 118), TObject::kOverwrite);
    h_eCC_result->Write(Form("Pure_CuteCC%dns", 86), TObject::kOverwrite);
    h_muCC_result->Write(Form("Pure_CutmuCC%dns", 113), TObject::kOverwrite);
}

//may load data inte vector from txt file
void LoadFile(std::string filename,
              std::vector<std::vector<double>> &v,
              int Length,
              int SkipLines)
{
    std::ifstream infile;
    infile.open(filename, std::ios::in);
    if (!infile)
    {
        printf("Fail to open file:%s.", filename.c_str());
    }
    double t1;
    int CountSkip = 0;
    while (!infile.eof())
    {
        if (CountSkip < SkipLines)
        {
            if (infile.get() == '\n')
                CountSkip++;
        }
        else
        {
            std::vector<double> t(Length);
            for (int i = 0; i < Length; i++)
            {
                infile >> t1;
                t[i] = t1;
            }
            v.push_back(t);
        }
    }
    infile.close();
}

//prepraing flux for the unfolding
//which is honda flux times cross section
void Pre_Flux()
{
    TFile *ff_Xsec = TFile::Open("../data/Xsec/CCXsecFlux.root", "READ");
    TString XsecName[] = {
        "muCCXsec",
        "muCCXsecbar",
        "eCCXsec",
        "eCCXsecbar",
    };
    TH1 *h_Xsec[4];
    GetObjFromFile(ff_Xsec, h_Xsec, XsecName, 4);
    TString FluxName[] = {
        "muCCflux",
        "muCCbarfluc",
        "eCCflux",
        "eCCbarFlux",
    };
    TGraph *g_Flux[4];
    GetObjFromFile(ff_Xsec, g_Flux, FluxName, 4);
    // g_Flux[0]->Draw();
    TH1D *h_Honda_flux_e = new TH1D("Honda_flux_e", "HKKM14 Flux for #nu_{e} times cross section",
                                    Etrue_BINNUM_eCC,
                                    logEtrue_range_eCC[0], logEtrue_range_eCC[1]);
    TH1D *h_Honda_flux_mu = new TH1D("Honda_flux_mu", "HKKM14 Flux for #nu_{#mu} times cross section",
                                     Etrue_BINNUM_muCC,
                                     logEtrue_range_muCC[0], logEtrue_range_muCC[1]);
    double binw_eCC, binw_muCC;
    double binc_eCC, binc_muCC;
    for (int i = 0; i < Etrue_BINNUM_eCC; i++)
    {
        binw_eCC = h_Honda_flux_e->GetBinWidth(i + 1);
        binw_muCC = h_Honda_flux_mu->GetBinWidth(i + 1);
        binc_eCC = h_Honda_flux_e->GetBinCenter(i + 1);
        binc_muCC = h_Honda_flux_mu->GetBinCenter(i + 1);
        binc_eCC = exp10(binc_eCC);
        binc_muCC = exp10(binc_muCC);
        h_Honda_flux_e->SetBinContent(i + 1,
                                      h_Xsec[2]->Interpolate(binc_eCC) * g_Flux[2]->Eval(binc_eCC) + h_Xsec[3]->Interpolate(binc_eCC) * g_Flux[3]->Eval(binc_eCC));
        h_Honda_flux_mu->SetBinContent(i + 1, h_Xsec[0]->Interpolate(binc_eCC) * g_Flux[0]->Eval(binc_eCC) + h_Xsec[1]->Interpolate(binc_eCC) * g_Flux[1]->Eval(binc_eCC));
    }
    h_Honda_flux_e->Scale(1 / h_Honda_flux_e->Integral());
    h_Honda_flux_mu->Scale(1 / h_Honda_flux_mu->Integral());
    TCanvas *c_e = new TCanvas("c_e");
    h_Honda_flux_e->Draw();
    TCanvas *c_mu = new TCanvas("c_mu");
    h_Honda_flux_mu->Draw();
    TFile *ff_unfold = TFile::Open("../data/UnfoldData.root", "update");
    ff_unfold->cd();
    h_Honda_flux_e->Write(h_Honda_flux_e->GetName(), TObject::kOverwrite);
    h_Honda_flux_mu->Write(h_Honda_flux_mu->GetName(), TObject::kOverwrite);
}

//evaluate the uncertainties and show them
void ShowUncertainty_stat(int MCPtsNUM, int statDisBinNUM)
{
    TFile *ff_unfold_data = TFile::Open("../data/UnfoldData.root", "READ");
    TH2 *h_likeli_eCC = (TH2 *)ff_unfold_data->Get("eCC_Likely_hood");
    TH2 *h_likeli_muCC = (TH2 *)ff_unfold_data->Get("muCC_Likely_hood");
    TH1 *h_sel_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEeCCSel"));
    TH1 *h_sel_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEmuCCSel"));
    TH1 *h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
    TH1 *h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));
    // TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCTrue"));
    // TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCTrue"));
    TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCSel;1"));
    TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCSel;1"));
    TH1 *h_eCC_result = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_result"));
    TH1 *h_muCC_result = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_result"));
    const int Enu_BINNUM_eCC = h_Prior_eCC->GetNbinsX();
    const int Enu_BINNUM_muCC = h_Prior_muCC->GetNbinsX();
    double epsilon_i_eCC[Enu_BINNUM_eCC];
    double epsilon_i_muCC[Enu_BINNUM_muCC];
    for (int i = 0; i < Enu_BINNUM_eCC; i++)
    {
        epsilon_i_eCC[i] = 0;
    }
    for (int i = 0; i < Enu_BINNUM_muCC; i++)
    {
        epsilon_i_muCC[i] = 0;
    }
    // h_Prior_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_Prior"));
    // h_Prior_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_Prior"));
    // h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
    // h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());

    TH1 *h_err_eCC[Etrue_BINNUM_eCC];
    TH1 *h_err_muCC[Etrue_BINNUM_muCC];
    for (int i = 0; i < Etrue_BINNUM_eCC; i++)
    {
        h_err_eCC[i] = new TH1D(Form("err%deCC", i), Form("Event Number Distribution of %d-th bin for eCC", i), statDisBinNUM, h_MC_true_eCC->GetBinContent(i + 1) - 3 * sqrt(h_MC_true_eCC->GetBinContent(i + 1)), h_MC_true_eCC->GetBinContent(i + 1) + 3 * sqrt(h_MC_true_eCC->GetBinContent(i + 1)));
        h_err_muCC[i] = new TH1D(Form("err%dmuCC", i), Form("Event Number Distribution of %d-th bin for muCC", i), statDisBinNUM, h_MC_true_muCC->GetBinContent(i + 1) - 3 * sqrt(h_MC_true_muCC->GetBinContent(i + 1)), h_MC_true_muCC->GetBinContent(i + 1) + 3 * sqrt(h_MC_true_muCC->GetBinContent(i + 1)));
    }
    TRandom3 r3;
    // TF1 *f_gaus=new TF1("f_gaus","0.39894228/[1]*exp(-0.5*((x-[0])/[1])**2) ",0,200);
    for (int nstat = 0; nstat < MCPtsNUM; nstat++)
    {
        TH1 *h_eCC_input = dynamic_cast<TH1 *>(h_sel_eCC->Clone("eCC_input"));
        TH1 *h_muCC_input = dynamic_cast<TH1 *>(h_sel_muCC->Clone("muCC_input"));
        for (int i = 0; i < NPE_BINNUM_muCC; i++)
        {
            if (i < NPE_BINNUM_eCC)
            {
                double Content_i_eCC = h_sel_eCC->GetBinContent(i + 1);
                Content_i_eCC = r3.Poisson(Content_i_eCC);
                h_eCC_input->SetBinContent(i + 1, Content_i_eCC);
            }
            double Content_i_muCC = h_sel_muCC->GetBinContent(i + 1);
            Content_i_muCC = r3.Poisson(Content_i_muCC);
            h_muCC_input->SetBinContent(i + 1, Content_i_muCC);
        }

        //reset prior distribution
        h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
        h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));

        int Iter_NUM = 2;
        Iter_Bayes_Once(h_eCC_input, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
        Iter_Bayes_Once(h_muCC_input, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
        if (Iter_NUM > 1)
            for (int i = 1; i < Iter_NUM; i++)
            {
                h_Prior_eCC = dynamic_cast<TH1 *>(h_eCC_result->Clone("eCC_Prior"));
                h_Prior_muCC = dynamic_cast<TH1 *>(h_muCC_result->Clone("muCC_Prior"));
                h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
                h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());
                Iter_Bayes_Once(h_sel_eCC, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
                Iter_Bayes_Once(h_sel_muCC, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
            }
        for (int i = 0; i < Etrue_BINNUM_eCC; i++)
        {
            h_err_eCC[i]->Fill(h_eCC_result->GetBinContent(i + 1));
            h_err_muCC[i]->Fill(h_muCC_result->GetBinContent(i + 1));
        }
    }
    TCanvas *c_err_eCC[Etrue_BINNUM_eCC];
    TCanvas *c_err_muCC[Etrue_BINNUM_muCC];
    TH1 *h_eCC_err_stat = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_stat_err"));
    TH1 *h_muCC_err_stat = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_stat_err"));
    for (int i = 0; i < Etrue_BINNUM_eCC; i++)
    {
        gStyle->SetOptFit(1111);

        c_err_eCC[i] = new TCanvas(Form("c_err_eCC%d", i));
        // h_err_eCC[i]->Draw();
        TFitResultPtr r_eCC = h_err_eCC[i]->Fit("gaus", "S");
        h_eCC_err_stat->SetBinContent(i + 1, r_eCC->Parameter(2) / h_MC_true_eCC->GetBinContent(i + 1));
        c_err_muCC[i] = new TCanvas(Form("c_err_muCC%d", i));
        // h_err_muCC[i]->Draw();
        TFitResultPtr r_muCC = h_err_muCC[i]->Fit("gaus", "S");
        h_muCC_err_stat->SetBinContent(i + 1, r_muCC->Parameter(2) / h_MC_true_muCC->GetBinContent(i + 1));
    }
    TCanvas *c_eCC_err_stat = new TCanvas("c_eCC_err_stat");
    h_eCC_err_stat->Draw();
    TCanvas *c_muCC_err_stat = new TCanvas("c_muCC_err_stat");
    h_muCC_err_stat->Draw();
    // TFile *ff_err = TFile::Open("../data/Uncertainties/Uncertainties.root", "recreate");
    TFile *ff_err = TFile::Open("../data/Uncertainties/Uncertainties.root", "update");
    ff_err->cd();
    h_eCC_err_stat->SetYTitle("Relative Uncertainty");
    h_eCC_err_stat->Write(h_eCC_err_stat->GetName(), TObject::kOverwrite);
    h_muCC_err_stat->SetYTitle("Relative Uncertainty");
    h_muCC_err_stat->Write(h_muCC_err_stat->GetName(), TObject::kOverwrite);
}
void ShowUncertainty_Xsec(int MCPtsNUM, int statDisBinNUM)
{
    TFile *ff_unfold_data = TFile::Open("../data/UnfoldData.root", "READ");
    TH2 *h_likeli_eCC = (TH2 *)ff_unfold_data->Get("eCC_Likely_hood");
    TH2 *h_likeli_muCC = (TH2 *)ff_unfold_data->Get("muCC_Likely_hood");
    TH1 *h_sel_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEeCCSel"));
    TH1 *h_sel_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("NPEmuCCSel"));
    TH1 *h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
    TH1 *h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));
    // TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCTrue"));
    // TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCTrue"));
    TH1 *h_MC_true_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_eCCSel;1"));
    TH1 *h_MC_true_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Enu_muCCSel;1"));
    TH1 *h_eCC_result = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_result"));
    TH1 *h_muCC_result = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_result"));
    const int Enu_BINNUM_eCC = h_Prior_eCC->GetNbinsX();
    const int Enu_BINNUM_muCC = h_Prior_muCC->GetNbinsX();
    double epsilon_i_eCC[Enu_BINNUM_eCC];
    double epsilon_i_muCC[Enu_BINNUM_muCC];
    for (int i = 0; i < Enu_BINNUM_eCC; i++)
    {
        epsilon_i_eCC[i] = 0;
    }
    for (int i = 0; i < Enu_BINNUM_muCC; i++)
    {
        epsilon_i_muCC[i] = 0;
    }
    // h_Prior_eCC = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_Prior"));
    // h_Prior_muCC = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_Prior"));
    // h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
    // h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());

    TH1 *h_err_eCC[Etrue_BINNUM_eCC];
    TH1 *h_err_muCC[Etrue_BINNUM_muCC];
    for (int i = 0; i < Etrue_BINNUM_eCC; i++)
    {
        h_err_eCC[i] = new TH1D(Form("err%deCC", i), Form("Event Number Distribution of %d-th bin for eCC", i), statDisBinNUM, h_MC_true_eCC->GetBinContent(i + 1) - 7 * sqrt(h_MC_true_eCC->GetBinContent(i + 1)), h_MC_true_eCC->GetBinContent(i + 1) + 7 * sqrt(h_MC_true_eCC->GetBinContent(i + 1)));
        h_err_muCC[i] = new TH1D(Form("err%dmuCC", i), Form("Event Number Distribution of %d-th bin for muCC", i), statDisBinNUM, h_MC_true_muCC->GetBinContent(i + 1) - 7 * sqrt(h_MC_true_muCC->GetBinContent(i + 1)), h_MC_true_muCC->GetBinContent(i + 1) + 7 * sqrt(h_MC_true_muCC->GetBinContent(i + 1)));
    }
    { // std::vector<std::vector<double>> v_eCCXecErr;
        // std::vector<std::vector<double>> v_muCCXecErr;
        // LoadFile("../data/Xsec/eCCXsecErr.txt", v_eCCXecErr, 2, 1);
        // LoadFile("../data/Xsec/muCCXsecErr.txt", v_muCCXecErr, 2, 1);
        // TH1 *h_eCC_Err_input = new TH1D("h_eCC_Err_input", "Input Xsec Err eCC", Etrue_BINNUM_eCC, logEtrue_range_eCC[0], logEtrue_range_eCC[1]);
        // TH1 *h_muCC_Err_input = new TH1D("h_muCC_Err_input", "Input Xsec Err muCC", Etrue_BINNUM_muCC, logEtrue_range_muCC[0], logEtrue_range_muCC[1]);

        // for (int i = 0; i < Etrue_BINNUM_eCC; i++)
        // {
        //     h_eCC_Err_input->SetBinContent(i + 1, v_eCCXecErr[i][1]);
        //     h_muCC_Err_input->SetBinContent(i + 1, v_muCCXecErr[i][1]);
        // }
        // // h_eCC_Err_input->Draw();
        // // h_muCC_Err_input->Draw();
        // // printf("EV:%f\tErr:%f\n",v_eCCXecErr[0][0],v_eCCXecErr[0][1]);
    }

    TGraph *g_eCC_XsecErr_input = new TGraph("../data/Xsec/T2KeCCXsecErr.txt");
    TGraph *g_muCC_XsecErr_input = new TGraph("../data/Xsec/T2KmuCCXsecErr.txt");
    // g_eCC_XsecErr_input->Draw();

    TRandom3 r3;
    // TF1 *f_gaus=new TF1("f_gaus","0.39894228/[1]*exp(-0.5*((x-[0])/[1])**2) ",0,200);
    for (int nstat = 0; nstat < MCPtsNUM; nstat++)
    {
        TH1 *h_eCC_input = dynamic_cast<TH1 *>(h_sel_eCC->Clone("eCC_input"));
        TH1 *h_muCC_input = dynamic_cast<TH1 *>(h_sel_muCC->Clone("muCC_input"));
        double bin_center;
        for (int i = 0; i < NPE_BINNUM_muCC; i++)
        {
            if (i < NPE_BINNUM_eCC)
            {
                bin_center = h_sel_eCC->GetBinCenter(i + 1);
                double Content_i_eCC = h_sel_eCC->GetBinContent(i + 1);
                // Content_i_eCC = r3.Gaus(Content_i_eCC, Content_i_eCC * h_eCC_Err_input->Interpolate(bin_center));
                Content_i_eCC = r3.Gaus(Content_i_eCC, Content_i_eCC * g_eCC_XsecErr_input->Eval(bin_center));
                h_eCC_input->SetBinContent(i + 1, Content_i_eCC);
            }
            bin_center = h_sel_muCC->GetBinCenter(i + 1);
            double Content_i_muCC = h_sel_muCC->GetBinContent(i + 1);
            // Content_i_muCC = r3.Gaus(Content_i_muCC, Content_i_muCC * h_muCC_Err_input->Interpolate(bin_center));
            Content_i_muCC = r3.Gaus(Content_i_muCC, Content_i_muCC * g_muCC_XsecErr_input->Eval(bin_center));
            h_muCC_input->SetBinContent(i + 1, Content_i_muCC);
        }

        //reset prior distribution
        h_Prior_eCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_e"));
        h_Prior_muCC = dynamic_cast<TH1 *>(ff_unfold_data->Get("Honda_flux_mu"));

        int Iter_NUM = 2;
        Iter_Bayes_Once(h_eCC_input, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
        Iter_Bayes_Once(h_muCC_input, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
        if (Iter_NUM > 1)
            for (int i = 1; i < Iter_NUM; i++)
            {
                h_Prior_eCC = dynamic_cast<TH1 *>(h_eCC_result->Clone("eCC_Prior"));
                h_Prior_muCC = dynamic_cast<TH1 *>(h_muCC_result->Clone("muCC_Prior"));
                h_Prior_eCC->Scale(1 / h_Prior_eCC->Integral());
                h_Prior_muCC->Scale(1 / h_Prior_muCC->Integral());
                Iter_Bayes_Once(h_sel_eCC, h_Prior_eCC, h_likeli_eCC, h_eCC_result, epsilon_i_eCC);
                Iter_Bayes_Once(h_sel_muCC, h_Prior_muCC, h_likeli_muCC, h_muCC_result, epsilon_i_muCC);
            }
        for (int i = 0; i < Etrue_BINNUM_eCC; i++)
        {
            h_err_eCC[i]->Fill(h_eCC_result->GetBinContent(i + 1));
            h_err_muCC[i]->Fill(h_muCC_result->GetBinContent(i + 1));
        }
    }
    TCanvas *c_err_eCC[Etrue_BINNUM_eCC];
    TCanvas *c_err_muCC[Etrue_BINNUM_muCC];
    TH1 *h_eCC_err_stat = dynamic_cast<TH1 *>(h_MC_true_eCC->Clone("eCC_Xsec_err"));
    TH1 *h_muCC_err_stat = dynamic_cast<TH1 *>(h_MC_true_muCC->Clone("muCC_Xsec_err"));
    for (int i = 0; i < Etrue_BINNUM_eCC; i++)
    {
        gStyle->SetOptFit(1111);

        c_err_eCC[i] = new TCanvas(Form("c_err_eCC%d", i));
        // h_err_eCC[i]->Draw();
        TFitResultPtr r_eCC = h_err_eCC[i]->Fit("gaus", "S");
        h_eCC_err_stat->SetBinContent(i + 1, r_eCC->Parameter(2) / h_MC_true_eCC->GetBinContent(i + 1));
        c_err_muCC[i] = new TCanvas(Form("c_err_muCC%d", i));
        // h_err_muCC[i]->Draw();
        TFitResultPtr r_muCC = h_err_muCC[i]->Fit("gaus", "S");
        h_muCC_err_stat->SetBinContent(i + 1, r_muCC->Parameter(2) / h_MC_true_muCC->GetBinContent(i + 1));
    }
    TCanvas *c_eCC_err_stat = new TCanvas("c_eCC_err_stat");
    h_eCC_err_stat->SetTitle("");
    h_eCC_err_stat->SetYTitle("Relative Uncertainty");
    h_eCC_err_stat->Draw();
    TCanvas *c_muCC_err_stat = new TCanvas("c_muCC_err_stat");
    h_muCC_err_stat->SetTitle("");
    h_muCC_err_stat->SetYTitle("Relative Uncertainty");
    h_muCC_err_stat->Draw();
    // TFile *ff_err = TFile::Open("../data/Uncertainties/Uncertainties.root", "recreate");
    // TFile *ff_err = TFile::Open("../data/Uncertainties/Uncertainties.root", "update");
    // ff_err->cd();
    // h_eCC_err_stat->SetYTitle("Relative Uncertainty");
    // h_eCC_err_stat->Write(h_eCC_err_stat->GetName(), TObject::kOverwrite);
    // h_muCC_err_stat->SetYTitle("Relative Uncertainty");
    // h_muCC_err_stat->Write(h_muCC_err_stat->GetName(), TObject::kOverwrite);
}

void ShowUncertainty_SamSel(const int SampelNUM)
{
    TFile *ff_Sel = TFile::Open("../data/SampleSelection/SampleSelection.root", "READ");
    TH1 *h_eCCSel[SampelNUM];
    TH1 *h_muCCSel[SampelNUM];
    for (int i = 0; i < SampelNUM; i++)
    {
        h_eCCSel[i] = dynamic_cast<TH1 *>(ff_Sel->Get(Form("CuteCC%dns", i + 85)));
        h_muCCSel[i] = dynamic_cast<TH1 *>(ff_Sel->Get(Form("CutmuCC%dns", i + 112)));
    }
    TH1 *h_eCC_Sel_Err = dynamic_cast<TH1 *>(h_eCCSel[0]->Clone("h_eCC_Sel_Err"));
    TH1 *h_muCC_Sel_Err = dynamic_cast<TH1 *>(h_muCCSel[0]->Clone("h_muCC_Sel_Err"));
    for (int k_bin = 0; k_bin < h_eCCSel[0]->GetNbinsX(); k_bin++)
    {
        double Avg_eCC = 0, Avg_muCC = 0;
        double v_ith_eCC[SampelNUM];
        double v_ith_muCC[SampelNUM];
        for (int i = 0; i < SampelNUM; i++)
        {
            v_ith_eCC[i] = h_eCCSel[i]->GetBinContent(k_bin + 1);
            v_ith_muCC[i] = h_muCCSel[i]->GetBinContent(k_bin + 1);
            Avg_eCC += v_ith_eCC[i] / SampelNUM;
            Avg_muCC += v_ith_muCC[i] / SampelNUM;
        }
        double Variance_eCC = 0;
        double Variance_muCC = 0;
        for (int i = 0; i < SampelNUM; i++)
        {
            Variance_eCC += pow(v_ith_eCC[i] - Avg_eCC, 2) / (SampelNUM);
            Variance_muCC += pow(v_ith_muCC[i] - Avg_muCC, 2) / (SampelNUM);
        }
        h_eCC_Sel_Err->SetBinContent(k_bin + 1, sqrt(Variance_eCC) / Avg_eCC);
        h_muCC_Sel_Err->SetBinContent(k_bin + 1, sqrt(Variance_muCC) / Avg_muCC);
    }
    h_eCC_Sel_Err->SetYTitle("Relative Uncertainty");
    h_muCC_Sel_Err->SetYTitle("Relative Uncertainty");

    TCanvas *c_eCC_sel = new TCanvas("c_eCC_sel");
    h_eCC_Sel_Err->Draw("hist");
    TCanvas *c_muCC_sel = new TCanvas("c_muCC_sel");
    h_muCC_Sel_Err->Draw("hist");
}

void ShowUncertainty_Osci()
{
    TFile *ff_sam = TFile::Open("../data/SampleSelection/SampleSelection.root", "READ");
    TString ObjName[] = {
        "RooUnf_CuteCC86ns",
        "RooUnf_CutmuCC113ns",
        "Pure_CuteCC86ns",
        "Pure_CutmuCC113ns",
    };
    TH1 *h_sams[4];
    GetObjFromFile(ff_sam,h_sams,ObjName,4);
    h_sams[2]->SetLineColor(kRed);
    h_sams[3]->SetLineColor(kRed);
    TH1 *h_eCC_Osc_Err = dynamic_cast<TH1 *>(h_sams[0]->Clone("h_eCC_Osc_Err"));
    TH1 *h_muCC_Osc_Err = dynamic_cast<TH1 *>(h_sams[1]->Clone("h_muCC_Osc_Err"));
    h_eCC_Osc_Err->SetYTitle("Relative Difference [%]");
    h_muCC_Osc_Err->SetYTitle("Relative Difference [%]");
    h_eCC_Osc_Err->SetXTitle("log_{10}(E_{#nu}/GeV)");
    h_muCC_Osc_Err->SetXTitle("log_{10}(E_{#nu}/GeV)");

    h_eCC_Osc_Err->SetAxisRange(0.,1,"Y");
    h_muCC_Osc_Err->SetAxisRange(0.,5,"Y");
    for (int i = 0; i < 7; i++)
    {
        double All,Pure;
        All=h_sams[0]->GetBinContent(i+1);
        Pure=h_sams[2]->GetBinContent(i+1);
        h_eCC_Osc_Err->SetBinContent(i+1,(All-Pure)*2/(All+Pure)*100);
        All=h_sams[1]->GetBinContent(i+1);
        Pure=h_sams[3]->GetBinContent(i+1);
        h_muCC_Osc_Err->SetBinContent(i+1,(All-Pure)*2/(All+Pure)*100);
    }
    

    TCanvas *c_e=new TCanvas("ce_sam");
    h_eCC_Osc_Err->Draw("hist");
    // h_sams[0]->Divide(h_sams[2]);
    // h_sams[0]->Draw("hist");
    // h_sams[2]->Draw("SAME");
    TCanvas *c_mu=new TCanvas("cmu_sam");
    h_muCC_Osc_Err->Draw("hist");
    // h_sams[1]->Divide(h_sams[3]);
    // h_sams[1]->Draw("hist");
    // h_sams[3]->Draw("SAME");
}

void ShowUncertainties_all()
{
    gStyle->SetOptStat(0);
    TFile *ff_Sel = TFile::Open("../data/SampleSelection/SampleSelection.root", "READ");
    TH1 *h_CutmuCC109ns = dynamic_cast<TH1 *>(ff_Sel->Get("CutmuCC114ns"));
    TH1 *h_CuteCC82ns = dynamic_cast<TH1 *>(ff_Sel->Get("CuteCC88ns"));
    TH1 *h_CutmuCC113ns = dynamic_cast<TH1 *>(ff_Sel->Get("CutmuCC113ns"));
    TH1 *h_CuteCC86ns = dynamic_cast<TH1 *>(ff_Sel->Get("CuteCC87ns"));

    TH1 *h_eCC_Sel_Err = dynamic_cast<TH1 *>(h_CuteCC82ns->Clone("h_eCC_Sel_Err"));
    TH1 *h_muCC_Sel_Err = dynamic_cast<TH1 *>(h_CutmuCC113ns->Clone("h_muCC_Sel_Err"));
    // h_CuteCC82ns->Scale(h_CuteCC86ns->Integral() / h_CuteCC82ns->Integral());
    // h_CutmuCC109ns->Scale(h_CutmuCC113ns->Integral() / h_CutmuCC109ns->Integral());
    double binv_c, binv_up;
    for (int i = 0; i < h_eCC_Sel_Err->GetNbinsX(); i++)
    {
        binv_c = h_CuteCC86ns->GetBinContent(i + 1);
        binv_up = h_CuteCC82ns->GetBinContent(i + 1);
        h_eCC_Sel_Err->SetBinContent(i + 1, sqrt(abs(binv_up - binv_c) * 2) / (binv_c + binv_up));
        binv_c = h_CutmuCC113ns->GetBinContent(i + 1);
        binv_up = h_CutmuCC109ns->GetBinContent(i + 1);
        h_muCC_Sel_Err->SetBinContent(i + 1, sqrt(abs(binv_up - binv_c) * 2) / (binv_c + binv_up));
    }
    h_muCC_Sel_Err->SetBinContent(7, 0.24);
    TCanvas *c_muSel = new TCanvas("c_muSel");
    h_muCC_Sel_Err->SetYTitle("Relative Uncertainty");
    // h_muCC_Sel_Err->Scale(0.1 / h_eCC_Sel_Err->Integral());
    h_muCC_Sel_Err->Draw("hist");
    TCanvas *c_eSel = new TCanvas("c_eSel");
    // h_eCC_Sel_Err->Scale(0.2 / h_eCC_Sel_Err->Integral());
    h_eCC_Sel_Err->SetYTitle("Relative Uncertainty");
    h_eCC_Sel_Err->Draw("hist");

    TFile *ff_err = TFile::Open("../data/Uncertainties/Uncertainties.root", "READ");
    TH1 *h_eCC_stat_err = dynamic_cast<TH1 *>(ff_err->Get("eCC_stat_err"));
    TH1 *h_muCC_stat_err = dynamic_cast<TH1 *>(ff_err->Get("muCC_stat_err"));
    TH1 *h_eCC_Xsec_err = dynamic_cast<TH1 *>(ff_err->Get("eCC_Xsec_err"));
    TH1 *h_muCC_Xsec_err = dynamic_cast<TH1 *>(ff_err->Get("muCC_Xsec_err"));
    // h_muCC_stat_err->Draw("hist");

    TH1 *h_eCC_Err_all = dynamic_cast<TH1 *>(h_eCC_Xsec_err->Clone("h_eCC_Err_all"));
    TH1 *h_muCC_Err_all = dynamic_cast<TH1 *>(h_muCC_Xsec_err->Clone("h_muCC_Err_all"));

    double errXsec, errStat, errSel, errall;
    for (int i = 0; i < h_eCC_Err_all->GetNbinsX(); i++)
    {
        errXsec = h_eCC_Xsec_err->GetBinContent(i + 1);
        errStat = h_eCC_stat_err->GetBinContent(i + 1);
        errSel = h_eCC_Sel_Err->GetBinContent(i + 1);
        errall = sqrt(errXsec * errXsec + errStat * errStat + errSel * errSel);
        h_eCC_Err_all->SetBinContent(i + 1, errall);
        errXsec = h_muCC_Xsec_err->GetBinContent(i + 1);
        errStat = h_muCC_stat_err->GetBinContent(i + 1);
        errSel = h_muCC_Sel_Err->GetBinContent(i + 1);
        errall = sqrt(errXsec * errXsec + errStat * errStat + errSel * errSel);
        h_muCC_Err_all->SetBinContent(i + 1, errall);
    }
    h_eCC_stat_err->SetLineColor(kRed);
    h_eCC_Xsec_err->SetLineColor(kBlue);
    h_eCC_Sel_Err->SetLineColor(kGreen + 3);
    h_eCC_stat_err->SetLineStyle(9);
    h_eCC_Xsec_err->SetLineStyle(10);
    h_eCC_Sel_Err->SetLineStyle(2);
    h_muCC_stat_err->SetLineColor(kRed);
    h_muCC_Xsec_err->SetLineColor(kBlue);
    h_muCC_Sel_Err->SetLineColor(kGreen + 3);
    h_muCC_stat_err->SetLineStyle(9);
    h_muCC_Xsec_err->SetLineStyle(10);
    h_muCC_Sel_Err->SetLineStyle(2);
    TLegend *leg_eCC_err = new TLegend();
    TLegend *leg_muCC_err = new TLegend();
    leg_eCC_err->AddEntry(h_eCC_stat_err, "statistical");
    leg_eCC_err->AddEntry(h_eCC_Xsec_err, "cross section");
    leg_eCC_err->AddEntry(h_eCC_Sel_Err, "sample selection");
    leg_eCC_err->AddEntry(h_eCC_Err_all, "total");
    leg_muCC_err->AddEntry(h_muCC_stat_err, "statistical");
    leg_muCC_err->AddEntry(h_muCC_Xsec_err, "cross section");
    leg_muCC_err->AddEntry(h_muCC_Sel_Err, "sample selection");
    leg_muCC_err->AddEntry(h_muCC_Err_all, "total");

    TCanvas *c_eCC_all = new TCanvas("c_eCC_all");
    h_eCC_Err_all->SetAxisRange(0, 0.3, "Y");
    h_eCC_Err_all->Draw("hist");
    h_eCC_stat_err->Draw("SAME hist");
    h_eCC_Xsec_err->Draw("SAME hist");
    h_eCC_Sel_Err->Draw("SAME hist");
    leg_eCC_err->DrawClone("SAME");
    TCanvas *c_muCC_all = new TCanvas("c_muCC_all");
    h_muCC_Err_all->SetAxisRange(0, 0.4, "Y");
    h_muCC_Err_all->Draw("hist");
    h_muCC_stat_err->Draw("SAME hist");
    h_muCC_Xsec_err->Draw("SAME hist");
    h_muCC_Sel_Err->Draw("SAME hist");
    leg_muCC_err->DrawClone("SAME");
}