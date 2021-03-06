void ALL_systcut_Nue(){
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetPadBottomMargin(0.02);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadTopMargin(0.02);
  std::ifstream true_array("outputs_systcut_Nue/Unf_spectrum-75_7_7.txt");
  std::ifstream bins("../../matrix/likelihood/5YRS/output_Nue_osc/energybins_Nue_7.txt");
  std::ofstream rel_err("outputs_systcut_Nue/systcut_err_Nue_7_7.txt");
  TFile *f_out = new TFile("outputs_systcut_Nue/systcut_Nue_7_7.root", "RECREATE");
  char* input_file = new char[256];
  float var, binvalue, mod_var;
  std::vector <float> vec_unf_arr;
  std::vector <float> vec_unf_bins;
  while (bins >> binvalue) vec_unf_bins.push_back(binvalue);
  while (true_array >> var) vec_unf_arr.push_back(var);
  static const int NBINS = vec_unf_bins.size();
  TVectorF unf_bins;
  TVectorF unf_arr;
  TVectorF modcuts[11]; //number of spectra
  TVectorF modratio[11]; //cut wrt nominal cut
  TGraph *arr_graphs[11];
  TGraph *arr_ratios[11];
  for (int hh=0; hh<=10; hh++) {
    modcuts[hh].ResizeTo(vec_unf_bins.size());
    modratio[hh].ResizeTo(vec_unf_bins.size());
  }
  unf_bins.ResizeTo(vec_unf_bins.size());
  unf_arr.ResizeTo(vec_unf_arr.size());
  for(int ii=0; ii<(vec_unf_bins.size()); ii++) {
    unf_bins[ii] = (vec_unf_bins)[ii]-3.0;
    //unf_arr[ii] = log10((vec_unf_arr)[ii]);
    unf_arr[ii] = ((vec_unf_arr)[ii]);
  }
  Float_t scale_factor[7] = {0.7,0.25,0.07,0.025,0.003,3.5E-4,0.52E-4};
  TCanvas *histos_c[NBINS];
  TH1F *Flux_bin_r_h[NBINS];
  TH1F *Flux_bin_r_copy_h[NBINS];
  TLine *true_flux_l[NBINS];
  for (int qq=0;qq<NBINS;qq++) {
    char *histname= new char[128];
    char *canvname= new char[128];
    sprintf(histname, "Flux_bin_%i_h",qq);
    sprintf(canvname, "c%i",qq);
    //Flux_bin_r_h[qq]=new TH1F(histname,"",500,-3.0,4.0);
    histos_c[qq]=new TCanvas(canvname,"",800,600);
    Flux_bin_r_h[qq]=new TH1F(histname,"",100,0.2*scale_factor[qq],scale_factor[qq]);
    true_flux_l[qq] = new TLine(unf_arr[qq],0.0,unf_arr[qq],100.);
    true_flux_l[qq]->SetLineWidth(3);
    true_flux_l[qq]->SetLineColor(kRed);
  }
  TH2F *Flux_var_h = new TH2F("Flux_var_h","",7,-1.0,1.2,200,1.E-7,1.0);
  TGraph *flux_true = new TGraph(unf_bins,unf_arr);
  flux_true->SetMarkerStyle(20);
  flux_true->SetMarkerSize(2.0);
  flux_true->SetMarkerColor(kRed);
  flux_true->GetXaxis()->SetTitle("log_{10}(E_{#nu} / GeV)");
  flux_true->GetYaxis()->SetTitle("Flux [cm^{-2} s^{-1} sr^{-1} GeV^{-1}]");
  flux_true->GetYaxis()->CenterTitle(true);
  flux_true->GetYaxis()->SetTitleSize(0.07);
  flux_true->GetYaxis()->SetTitleOffset(0.74);
  flux_true->GetYaxis()->SetLabelSize(0.07);
  flux_true->GetXaxis()->SetTitleSize(0.07);
  flux_true->GetXaxis()->SetTitleOffset(1.1);
  flux_true->GetXaxis()->SetLabelSize(0.07);
  flux_true->GetYaxis()->SetRangeUser(0.5E-6,0.7);
  flux_true->SetTitle("");
  int index;
  for (int k=0; k<=10; k++) {
    index = 70+k;
    sprintf(input_file,"outputs_systcut_Nue/Unf_spectrum-%i_7_7.txt",index);
    //std::cout << "reading file " << input_file << "..." <<std::endl;
    std::ifstream modspec_in(input_file);
    for(int jj=0; jj<(vec_unf_bins.size()); jj++) {
      modspec_in >> mod_var;
      Flux_bin_r_h[jj]->Fill((mod_var));
      //std::cout<<(unf_bins)[jj]<<"\t"<<mod_var<<std::endl;
      Flux_var_h->Fill((unf_bins)[jj], (mod_var));
      modcuts[k][jj] = mod_var;
      modratio[k][jj] = mod_var/unf_arr[jj];
      //Flux_bin_r_h[jj]->Fill(log10(mod_var));
      //Flux_var_h->Fill((unf_bins)[jj], log10(mod_var));
    }
  }
  Flux_var_h->Write();
  flux_true->Write("original_flux");

  for(int ii=0; ii<NBINS; ii++) {
    Flux_bin_r_h[ii]->Write();
    char *histname_copy= new char[128];
    sprintf(histname_copy, "Flux_bin_%i_copy_h",ii); 
    Flux_bin_r_copy_h[ii] = (TH1F*)Flux_bin_r_h[ii]->Clone(histname_copy);
    //histos_c[ii]->cd();
    //Flux_bin_r_h[ii]->Draw();
    //true_flux_l[ii]->Draw();
    //histos_c[ii]->Write();
  }
  /*
  //FIT
  TVectorF fit_peak;
  TVectorF fit_sigma;
  TVectorF err0;
  fit_peak.ResizeTo(vec_unf_bins.size());
  fit_sigma.ResizeTo(vec_unf_bins.size());
  err0.ResizeTo(vec_unf_bins.size());
  for( int ra=0; ra<NBINS; ra++){
    //TF1 *gausfit_flux = new TF1("gausfit_flux", "gaus", -3.0,4.0);
    TF1 *gausfit_flux = new TF1("gausfit_flux", "gaus", 1.E-7,1.0);
    Flux_bin_r_h[ra]->Fit("gausfit_flux","R");
    fit_peak[ra] = gausfit_flux->GetParameter(1);
    fit_sigma[ra] = (gausfit_flux->GetParameter(2))/2.;
    err0[ra] = 0.;
    }*/
  //RMS
  TVectorF fit_peak;
  TVectorF fit_sigma;
  TVectorF err0;
  err0.ResizeTo(vec_unf_bins.size());
  fit_peak.ResizeTo(vec_unf_bins.size());
  fit_sigma.ResizeTo(vec_unf_bins.size());
  for( int ra=0;ra<NBINS;ra++){
    fit_peak[ra] = Flux_bin_r_h[ra]->GetMean(1);
    fit_sigma[ra] = Flux_bin_r_h[ra]->GetMean(11);
    //fit_sigma[ra] = sqrt(pow(Flux_bin_r_h[ra]->GetRMS(1),2.0) - pow(fit_peak[ra],2.0));
    std::cout<<fit_sigma[ra]/fit_peak[ra]<<std::endl;
    err0[ra] = 0.;
  }
  std::cout<<endl;
  TLine *fit_flux_l[NBINS];
  for (int pp=0;pp<NBINS;pp++) {
    fit_flux_l[pp] = new TLine(fit_peak[pp],0.0,fit_peak[pp],100.);
    fit_flux_l[pp]->SetLineWidth(3);
    fit_flux_l[pp]->SetLineColor(kBlue);
  }

  for(int ii=0; ii<NBINS; ii++) {
    //Flux_bin_r_h[ii]->Write();
    rel_err << (abs(fit_peak[ii]-unf_arr[ii])/unf_arr[ii]) << "\t";
    //rel_err << (abs(modcuts[0][ii]-modcuts[10][ii])/(2.73*unf_arr[ii])) << "\t";
    std::cout<<abs(fit_peak[ii]-unf_arr[ii])/unf_arr[ii]<<std::endl;
    histos_c[ii]->cd();
    Flux_bin_r_copy_h[ii]->Draw();
    true_flux_l[ii]->Draw();
    fit_flux_l[ii]->Draw();
    histos_c[ii]->Write();
  }
  rel_err << "\n";
  TGraphErrors *fitspread = new TGraphErrors(unf_bins,fit_peak,err0,fit_sigma);
  fitspread->SetMarkerStyle(47);
  fitspread->SetMarkerSize(1);
  fitspread->SetLineWidth(3);
  fitspread->Write("fit_flux");

  TCanvas *cfin = new TCanvas("cfin","",800,800);
  cfin->cd();
  TPad *p1 = new TPad("p1", "p1", 0.01, 0.5, 0.95, 0.95, 0, 0, 0);
  p1->SetTopMargin(0);
  p1->SetBottomMargin(0);
  //p1->SetLeftMargin(0);
  p1->SetRightMargin(0.02);
  //gPad->SetTickx(2);
  //gPad->SetTicky(2);
  p1->Draw();

  TPad *p2 = new TPad("p2", "p2", 0.01, 0.0, 0.95, 0.5, 0, 0, 0);
  p2->SetTopMargin(0);
  //p2->SetLeftMargin(0);
  p2->SetRightMargin(0.02);
  p2->SetBottomMargin(0.25);
  //gPad->SetTickx(2);
  //gPad->SetTicky(2);
  p2->Draw();

  TLegend *leg1 = new TLegend(0.15,0.07,0.3,0.85);
  leg1->SetBorderSize(0);
  p1->cd();
  arr_graphs[0] = new TGraph(unf_bins,modcuts[0]);
  arr_graphs[0]->SetMarkerStyle(20);
  arr_graphs[0]->SetMarkerColor(46);
  arr_graphs[0]->SetMarkerSize(1.8);
  arr_graphs[0]->GetYaxis()->SetRangeUser(0.5E-6,0.7);
  arr_graphs[0]->GetYaxis()->SetTitle("Flux [cm^{-2} s^{-1} sr^{-1} GeV^{-1}]");
  arr_graphs[0]->GetYaxis()->CenterTitle(true);
  arr_graphs[0]->GetXaxis()->SetTitle("log_{10}(E_{#nu} / GeV)");
  arr_graphs[0]->GetYaxis()->SetTitleSize(0.07);
  arr_graphs[0]->GetYaxis()->SetTitleOffset(0.74);
  arr_graphs[0]->GetYaxis()->SetLabelSize(0.06);
  arr_graphs[0]->GetXaxis()->SetTitleSize(0.07);
  arr_graphs[0]->GetXaxis()->SetTitleOffset(1.1);
  arr_graphs[0]->GetXaxis()->SetLabelSize(0.07);
  arr_graphs[0]->SetTitle("");
  arr_graphs[0]->Draw("AP");
  leg1->AddEntry(arr_graphs[0], "70 ns", "p");
  gPad->SetLogy();
  int legindex = 0;
  for (int rr=1; rr<=10; rr++) {
    legindex = 70 + rr;
    char *legname = new char[128];
    arr_graphs[rr] = new TGraph(unf_bins,modcuts[rr]);
    arr_graphs[rr]->SetMarkerColor(rr);
    arr_graphs[rr]->SetMarkerSize(2.0);
    arr_graphs[rr]->SetMarkerStyle(20+rr);
    if (rr==10) arr_graphs[rr]->SetMarkerStyle(34);
    if (rr>4) arr_graphs[rr]->SetMarkerColor(rr+1);
    if (rr==5) {
      arr_graphs[rr]->SetMarkerColor(kRed);
      arr_graphs[rr]->SetMarkerStyle(20);
    }
    if (rr>8) arr_graphs[rr]->SetMarkerColor(rr+2);
    arr_graphs[rr]->Draw("P");
    sprintf(legname, "%i ns", legindex);
    leg1->AddEntry(arr_graphs[rr], legname, "p");
  }
  leg1->Draw();

  p2->cd();
  TLine *zero = new TLine(-1.0,1.0,1.05,1.0);
  zero->SetLineWidth(3);
  zero->SetLineStyle(9);
  arr_ratios[0] = new TGraph(unf_bins,modratio[0]);
  arr_ratios[0]->SetMarkerStyle(20);
  arr_ratios[0]->SetMarkerColor(46);
  arr_ratios[0]->SetMarkerSize(1.8);
  arr_ratios[0]->GetYaxis()->SetRangeUser(0.73, 1.27);
  arr_ratios[0]->GetYaxis()->SetTitle("Ratio");
  arr_ratios[0]->GetXaxis()->SetTitle("log_{10}(E_{#nu} / GeV)");
  arr_ratios[0]->GetYaxis()->CenterTitle(true);
  arr_ratios[0]->GetYaxis()->SetTitleSize(0.07);
  arr_ratios[0]->GetYaxis()->SetTitleOffset(0.74);
  arr_ratios[0]->GetYaxis()->SetLabelSize(0.055);
  arr_ratios[0]->GetXaxis()->SetTitleSize(0.07);
  arr_ratios[0]->GetXaxis()->SetTitleOffset(1.1);
  arr_ratios[0]->GetXaxis()->SetLabelSize(0.07);
  arr_ratios[0]->SetTitle("");
  arr_ratios[0]->Draw("AP");
  zero->Draw();
  legindex = 0;
  for (int rr=1;rr<=10;rr++) {                                                                                                          
    legindex = 70 + rr;
    arr_ratios[rr] = new TGraph(unf_bins,modratio[rr]);
    arr_ratios[rr]->SetMarkerStyle(20+rr);
    arr_ratios[rr]->SetMarkerColor(rr);
    arr_ratios[rr]->SetMarkerSize(2.0);
    if (rr==10) arr_ratios[rr]->SetMarkerStyle(34);
    if (rr>4) arr_ratios[rr]->SetMarkerColor(rr+1);
    if (rr==5) {
      arr_ratios[rr]->SetMarkerColor(kRed);
      arr_ratios[rr]->SetMarkerStyle(20);
    }
    if (rr>8) arr_ratios[rr]->SetMarkerColor(rr+2);
    arr_ratios[rr]->Draw("P");
  }

  cfin->Write("comp_spectra");
  std::cout << "file outputs_systcut_Nue/systcut_Nue_7_7.root created" << std::endl;
  f_out->Close();
  cfin->SaveAs("plot_err/systcut_nue.pdf");
}
