//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct 28 11:17:28 2019 by ROOT version 6.12/06
// from TTree Data/Tree with custom variables
// found on file: /storage/DATA-02/juno/Users/sgiulio/Atmo/analysis/unfolding/treemaker/trees/MC_500K_1m_TOT_NEW.root
//////////////////////////////////////////////////////////

#ifndef Data_h
#define Data_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class Data {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           evtID;
   Int_t           ID_gen;
   Int_t           init_nuID;
   Int_t           init_lepID;
   Float_t         nuEnergy;
   Float_t         lepEnergy;
   Float_t         visEnergy;
   Float_t         L_osc_baseline;
   Float_t         P_surv;
   Float_t         P_trans;
   Int_t           target;
   Int_t           Int_flag;
   Bool_t          CCint;
   Bool_t          NCint;
   Bool_t          FC;
   Bool_t          PC;
   Int_t           nHadr;
   Int_t           trueNPE;
   Int_t           trueNPE_LPMT_CD;
   Int_t           trueNPE_LPMT_WP;
   Int_t           trueNPE_SPMT;
   Float_t         trueVX;
   Float_t         trueVY;
   Float_t         trueVZ;
   Float_t         newVX;
   Float_t         newVY;
   Float_t         newVZ;
   Float_t         recoVX;
   Float_t         recoVY;
   Float_t         recoVZ;
   Float_t         trueVradius;
   Float_t         newVradius;
   Float_t         recoVradius;
   Float_t         vertex_shift_TS;
   Float_t         vertex_shift_TR;
   Float_t         trueZENITH;
   Float_t         trueAZIMUTH;
   Float_t         Tres_Mean;
   Float_t         Tres_RMS;
   vector<int>     *pdgHadr;
   vector<float>   *EHadr;
   vector<float>   *pHadr;
   vector<float>   *theta_hitSPMT;
   vector<float>   *phi_hitSPMT;
   vector<float>   *SPMT_Time;
   vector<float>   *SPMT_Tres;
   vector<float>   *LTrack;

   // List of branches
   TBranch        *b_evtID;   //!
   TBranch        *b_ID_gen;   //!
   TBranch        *b_init_nuID;   //!
   TBranch        *b_init_lepID;   //!
   TBranch        *b_nuEnergy;   //!
   TBranch        *b_lepEnergy;   //!
   TBranch        *b_visEnergy;   //!
   TBranch        *b_L_osc_baseline;   //!
   TBranch        *b_P_surv;   //!
   TBranch        *b_P_trans;   //!
   TBranch        *b_target;   //!
   TBranch        *b_Int_flag;   //!
   TBranch        *b_CCint;   //!
   TBranch        *b_NCint;   //!
   TBranch        *b_FC;   //!
   TBranch        *b_PC;   //!
   TBranch        *b_nHadr;   //!
   TBranch        *b_trueNPE;   //!
   TBranch        *b_trueNPE_LPMT_CD;   //!
   TBranch        *b_trueNPE_LPMT_WP;   //!
   TBranch        *b_trueNPE_SPMT;   //!
   TBranch        *b_trueVX;   //!
   TBranch        *b_trueVY;   //!
   TBranch        *b_trueVZ;   //!
   TBranch        *b_newVX;   //!
   TBranch        *b_newVY;   //!
   TBranch        *b_newVZ;   //!
   TBranch        *b_recoVX;   //!
   TBranch        *b_recoVY;   //!
   TBranch        *b_recoVZ;   //!
   TBranch        *b_trueVradius;   //!
   TBranch        *b_newVradius;   //!
   TBranch        *b_recoVradius;   //!
   TBranch        *b_vertex_shift_TS;   //!
   TBranch        *b_vertex_shift_TR;   //!
   TBranch        *b_trueZENITH;   //!
   TBranch        *b_trueAZIMUTH;   //!
   TBranch        *b_Tres_Mean;   //!
   TBranch        *b_Tres_RMS;   //!
   TBranch        *b_pdgHadr;   //!
   TBranch        *b_EHadr;   //!
   TBranch        *b_pHadr;   //!
   TBranch        *b_theta_hitSPMT;   //!
   TBranch        *b_phi_hitSPMT;   //!
   TBranch        *b_SPMT_Time;   //!
   TBranch        *b_SPMT_Tres;   //!
   TBranch        *b_LTrack;   //!

   Data(TTree *tree=0);
   virtual ~Data();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Data_cxx
Data::Data(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/storage/DATA-02/juno/Users/sgiulio/Atmo/analysis/unfolding/treemaker/trees/MC_500K_1m_TOT_NEW.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/storage/DATA-02/juno/Users/sgiulio/Atmo/analysis/unfolding/treemaker/trees/MC_500K_1m_TOT_NEW.root");
      }
      f->GetObject("Data",tree);

   }
   Init(tree);
}

Data::~Data()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Data::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Data::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Data::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   pdgHadr = 0;
   EHadr = 0;
   pHadr = 0;
   theta_hitSPMT = 0;
   phi_hitSPMT = 0;
   SPMT_Time = 0;
   SPMT_Tres = 0;
   LTrack = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evtID", &evtID, &b_evtID);
   fChain->SetBranchAddress("ID_gen", &ID_gen, &b_ID_gen);
   fChain->SetBranchAddress("init_nuID", &init_nuID, &b_init_nuID);
   fChain->SetBranchAddress("init_lepID", &init_lepID, &b_init_lepID);
   fChain->SetBranchAddress("nuEnergy", &nuEnergy, &b_nuEnergy);
   fChain->SetBranchAddress("lepEnergy", &lepEnergy, &b_lepEnergy);
   fChain->SetBranchAddress("visEnergy", &visEnergy, &b_visEnergy);
   fChain->SetBranchAddress("L_osc_baseline", &L_osc_baseline, &b_L_osc_baseline);
   fChain->SetBranchAddress("P_surv", &P_surv, &b_P_surv);
   fChain->SetBranchAddress("P_trans", &P_trans, &b_P_trans);
   fChain->SetBranchAddress("target", &target, &b_target);
   fChain->SetBranchAddress("Int_flag", &Int_flag, &b_Int_flag);
   fChain->SetBranchAddress("CCint", &CCint, &b_CCint);
   fChain->SetBranchAddress("NCint", &NCint, &b_NCint);
   fChain->SetBranchAddress("FC", &FC, &b_FC);
   fChain->SetBranchAddress("PC", &PC, &b_PC);
   fChain->SetBranchAddress("nHadr", &nHadr, &b_nHadr);
   fChain->SetBranchAddress("trueNPE", &trueNPE, &b_trueNPE);
   fChain->SetBranchAddress("trueNPE_LPMT_CD", &trueNPE_LPMT_CD, &b_trueNPE_LPMT_CD);
   fChain->SetBranchAddress("trueNPE_LPMT_WP", &trueNPE_LPMT_WP, &b_trueNPE_LPMT_WP);
   fChain->SetBranchAddress("trueNPE_SPMT", &trueNPE_SPMT, &b_trueNPE_SPMT);
   fChain->SetBranchAddress("trueVX", &trueVX, &b_trueVX);
   fChain->SetBranchAddress("trueVY", &trueVY, &b_trueVY);
   fChain->SetBranchAddress("trueVZ", &trueVZ, &b_trueVZ);
   fChain->SetBranchAddress("newVX", &newVX, &b_newVX);
   fChain->SetBranchAddress("newVY", &newVY, &b_newVY);
   fChain->SetBranchAddress("newVZ", &newVZ, &b_newVZ);
   fChain->SetBranchAddress("recoVX", &recoVX, &b_recoVX);
   fChain->SetBranchAddress("recoVY", &recoVY, &b_recoVY);
   fChain->SetBranchAddress("recoVZ", &recoVZ, &b_recoVZ);
   fChain->SetBranchAddress("trueVradius", &trueVradius, &b_trueVradius);
   fChain->SetBranchAddress("newVradius", &newVradius, &b_newVradius);
   fChain->SetBranchAddress("recoVradius", &recoVradius, &b_recoVradius);
   fChain->SetBranchAddress("vertex_shift_TS", &vertex_shift_TS, &b_vertex_shift_TS);
   fChain->SetBranchAddress("vertex_shift_TR", &vertex_shift_TR, &b_vertex_shift_TR);
   fChain->SetBranchAddress("trueZENITH", &trueZENITH, &b_trueZENITH);
   fChain->SetBranchAddress("trueAZIMUTH", &trueAZIMUTH, &b_trueAZIMUTH);
   fChain->SetBranchAddress("Tres_Mean", &Tres_Mean, &b_Tres_Mean);
   fChain->SetBranchAddress("Tres_RMS", &Tres_RMS, &b_Tres_RMS);
   fChain->SetBranchAddress("pdgHadr", &pdgHadr, &b_pdgHadr);
   fChain->SetBranchAddress("EHadr", &EHadr, &b_EHadr);
   fChain->SetBranchAddress("pHadr", &pHadr, &b_pHadr);
   fChain->SetBranchAddress("theta_hitSPMT", &theta_hitSPMT, &b_theta_hitSPMT);
   fChain->SetBranchAddress("phi_hitSPMT", &phi_hitSPMT, &b_phi_hitSPMT);
   fChain->SetBranchAddress("SPMT_Time", &SPMT_Time, &b_SPMT_Time);
   fChain->SetBranchAddress("SPMT_Tres", &SPMT_Tres, &b_SPMT_Tres);
   fChain->SetBranchAddress("LTrack", &LTrack, &b_LTrack);
   Notify();
}

Bool_t Data::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Data::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Data::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Data_cxx
