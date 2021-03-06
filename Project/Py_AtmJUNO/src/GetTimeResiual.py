#!/usr/bin/env python
# -*- coding: utf-8 -*-
from array import array
from lib.PreImport import *
import sys
import os
sys.path.append("..")


def ViewHitTime(NFiles, WhichEntry=0, SaveFileName="hitTime"):
    evt = ROOT.TChain("evt")
    AddUserFile2TChain(evt, NFiles)
    c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    evt.Draw("hitTime>>+h_hit", "pmtID>300000", "")  # ,1,WhichEntry)
    ROOT.gStyle.SetOptStat("ne")
    c.SaveAs("./pics/" + SaveFileName + ".png")


def ViewnCaptureT(NFiles, WhichEntry=0, SaveFileName="nCaptureT"):
    nCapture = ROOT.TChain("nCapture")
    AddUserFile2TChain(nCapture, NFiles=NFiles)
    c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    nCapture.Draw("NeutronCaptureT>>h_ncapT", "NeutronCaptureT<3000")
    ROOT.gStyle.SetOptStat("ne")
    c.SaveAs("./pics/" + SaveFileName + ".png")


def ViewGlobalPos(NFiles, WhichEntry=0, SaveFileName="GlobalPos"):
    ROOT.ROOT.EnableImplicitMT()
    evt = ROOT.TChain("evt")
    AddUserFile2TChain(evt, NFiles=NFiles)
    c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    evt.Draw(
        "sqrt(GlobalPosX*GlobalPosX+GlobalPosY*GlobalPosY+GlobalPosZ*GlobalPosZ)/1000>>+h_GPos",
        "pmtID>300000", "", 1, WhichEntry)
    h_GPos = ROOT.gDirectory.Get("h_GPos")
    h_GPos.SetXTitle("m")
    ROOT.gStyle.SetOptStat("ne")
    c.SaveAs("./pics/" + SaveFileName + ".png")


# Same particles for geninfo and prmtrkdep


def ViewPDGID(NFiles, WhichEntry=0, SaveFileName="PDGID"):
    ROOT.ROOT.EnableImplicitMT()
    geninfo = ROOT.TChain("geninfo")
    AddUserFile2TChain(geninfo, NFiles)
    geninfo.SetBranchStatus("*", 0)
    geninfo.SetBranchStatus("InitPDGID", 1)
    # geninfo.GetEntry(WhichEntry)
    # InitPDGID=np.asarray(geninfo.InitPDGID)
    # print("geninfo:\t",InitPDGID)
    prmtrkdep = ROOT.TChain("prmtrkdep")
    AddUserFile2TChain(prmtrkdep, NFiles=NFiles)
    prmtrkdep.SetBranchStatus("*", 0)
    prmtrkdep.SetBranchStatus("PDGID", 1)
    for entry in range(prmtrkdep.GetEntries()):
        geninfo.GetEntry(entry)
        prmtrkdep.GetEntry(entry)
        print("gen:", np.asarray(geninfo.InitPDGID))
        print("prm:", np.asarray(prmtrkdep.PDGID))
    # prmtrkdep.GetEntry(WhichEntry)
    # PDGID=np.asarray(prmtrkdep.PDGID)
    # print("prmtrkdep:\t",PDGID)
    # c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    # prmtrkdep.Draw("PDGID>>+h_PDGID")
    # # h_PDGID=(ROOT.TH1F)ROOT.gDirectory.Get("h_PDGID")
    # ROOT.gStyle.SetOptStat("ne")
    # c.SaveAs("./pics/"+SaveFileName + ".png")


def ViewWaterPoolPEs(NFiles, WhichEntry=0, SaveFileName="WPnpe"):
    SimEvent = ROOT.TChain("/Event/Sim/SimEvent")
    AddDetsimFile2TChain(SimEvent, NFiles=NFiles)
    c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    SimEvent.Draw("SimEvent.m_wp_hits>>+h_wp_npe")
    ROOT.gStyle.SetOptStat("ne")
    c.SaveAs("./pics/" + SaveFileName + ".png")
    # SimEvent.GetEntry(WhichEntry)
    # npe=np.asarray(SimEvent.SimEvent.m_wp_hits.npe)
    # print(npe)


def ViewPMTID(NFiles, WhichEntry=0, SaveFileName="PMTid"):
    ROOT.ROOT.EnableImplicitMT()
    evt = ROOT.TChain("evt")
    AddUserFile2TChain(evt, NFiles=NFiles)
    evt.SetBranchStatus("*", 0)
    evt.SetBranchStatus("pmtID", 1)
    c = ROOT.TCanvas("myCanvasName", "The Canvas Title", 800, 600)
    evt.Draw("pmtID>>+h_pmtID", "pmtID>30000 && pmtID<50000")
    ROOT.gStyle.SetOptStat("ne")
    c.SaveAs("./pics/" + SaveFileName + ".png")


# basicly relective index


def ViewOptPar(NFiles):
    opticalparam = ROOT.TChain("opticalparam")
    print(opticalparam.AsMatirx(columns=["LS_RI_idx"]))


# get smeared value for vertex position, default: sima_v=1m


# a single point
def GetSmearedVertex(InitialX, InitialY, InitialZ, SmearSigma=1):
    SmearR = np.random.normal(0, SmearSigma)
    SmearCosTheta = np.random.rand(1) * 2. - 1
    SmearSinTheta = np.sqrt(1. - SmearCosTheta**2)
    SmearPhi = np.random.rand(1) * np.pi * 2.
    dx, dy, dz = SmearR * SmearSinTheta * np.cos(
        SmearPhi), SmearR * SmearSinTheta * np.sin(
            SmearPhi), SmearR * SmearCosTheta
    X_new, Y_new, Z_new = InitialX + dx, InitialY + dy, InitialZ + dz
    return X_new, Y_new, Z_new


# get smeared value both hitTime,default: sigma_hittime=4ns


def GetSmearedHittime(InitialhitTime, SmearSigma=4):
    hitTime_new = np.random.normal(InitialhitTime, SmearSigma)
    return hitTime_new


# Get Distance from vertex to pmt


def GetDistanceR_Vi(V_x, V_y, V_z, Hit_x, Hit_y, Hit_z):
    R_Vi = np.sqrt((V_x - Hit_x)**2 + (V_y - Hit_y)**2 + (V_z - Hit_z)**2)
    return R_Vi


def SmearVertexAndGetDistance(InitialX,
                              InitialY,
                              InitialZ,
                              Hit_x,
                              Hit_y,
                              Hit_z,
                              SmearSigma=1):
    V_x, V_y, V_z = GetSmearedVertex(InitialX, InitialY, InitialZ, SmearSigma)
    R_Vi = np.sqrt((V_x - Hit_x)**2 + (V_y - Hit_y)**2 + (V_z - Hit_z)**2)
    return R_Vi


def ViewTimeProfile(NFiles, StartFile=1, SaveFileName="TimeProfile"):
    ROOT.ROOT.EnableImplicitMT()
    h_muCC = ROOT.TH1D(
        "muCC0", "muon Charge Current: %.2e<NPE<%.2e" %
        (LPMT_NPE_steps[0], LPMT_NPE_steps[1]), NumofBins, TimeP_low, TimeP_up)
    h_eCC = ROOT.TH1D(
        "eCC0", "electron Charge Current: %.2e<NPE<%.2e" %
        (LPMT_NPE_steps[0], LPMT_NPE_steps[1]), NumofBins, TimeP_low, TimeP_up)
    h_NC = ROOT.TH1D(
        "NC0", "Neutral Current: %.2e<NPE<%.2e" %
        (LPMT_NPE_steps[0], LPMT_NPE_steps[1]), NumofBins, TimeP_low, TimeP_up)
    h_muCC_list = [h_muCC]
    h_eCC_list = [h_eCC]
    h_NC_list = [h_NC]
    for i in range(len(LPMT_NPE_steps) - 2):
        h_muCC_t = h_muCC.Clone("muCC" + str(i + 1))
        h_muCC_t.SetTitle("muon Charge Current: %.2e<NPE<%.2e" %
                          (LPMT_NPE_steps[i], LPMT_NPE_steps[i + 1]))
        h_muCC_list.append(h_muCC_t)
        h_eCC_t = h_eCC.Clone("eCC" + str(i + 1))
        h_eCC_t.SetTitle("electron Charge Current: %.2e<NPE<%.2e" %
                         (LPMT_NPE_steps[i], LPMT_NPE_steps[i + 1]))
        h_eCC_list.append(h_eCC_t)
        h_NC_t = h_NC.Clone("NC" + str(i + 1))
        h_NC_t.SetTitle("Neutral Current: %.2e<NPE<%.2e" %
                        (LPMT_NPE_steps[i], LPMT_NPE_steps[i + 1]))
        h_NC_list.append(h_NC_t)
    i = len(LPMT_NPE_steps) - 2
    h_muCC_t = h_muCC.Clone("muCC" + str(i + 1))
    h_muCC_t.SetTitle("muon Charge Current: NPE>%.2e" % (LPMT_NPE_steps[i]))
    h_muCC_list.append(h_muCC_t)
    h_eCC_t = h_eCC.Clone("eCC" + str(i + 1))
    h_eCC_t.SetTitle("electron Charge Current: NPE>%.2e" % (LPMT_NPE_steps[i]))
    h_eCC_list.append(h_eCC_t)
    h_NC_t = h_NC.Clone("NC" + str(i + 1))
    h_NC_t.SetTitle("Neutral Current: NPE>%.2e" % (LPMT_NPE_steps[i]))
    h_NC_list.append(h_NC_t)

    evt = ROOT.TChain("evt")
    geninfo = ROOT.TChain("geninfo")
    AddUserFile2TChain(evt, NFiles=NFiles, StartFile=StartFile)
    AddUserFile2TChain(geninfo, NFiles=NFiles, StartFile=StartFile)
    evt.SetBranchStatus("*", 0)
    geninfo.SetBranchStatus("*", 0)
    geninfo.SetBranchStatus("InitX", 1)
    geninfo.SetBranchStatus("InitY", 1)
    geninfo.SetBranchStatus("InitZ", 1)
    geninfo.SetBranchStatus("InitPDGID", 1)
    evt.SetBranchStatus("hitTime", 1)
    evt.SetBranchStatus("pmtID", 1)
    evt.SetBranchStatus("GlobalPosX", 1)
    evt.SetBranchStatus("GlobalPosY", 1)
    evt.SetBranchStatus("GlobalPosZ", 1)
    for entry in range(evt.GetEntries()):
        geninfo.GetEntry(entry)
        # one vertex, use first one
        InitX, InitY, InitZ = np.asarray(geninfo.InitX)[0] / 1e3, np.asarray(
            geninfo.InitY)[0] / 1e3, np.asarray(geninfo.InitZ)[0] / 1e3
        Smear_X, Smear_Y, Smear_Z = GetSmearedVertex(InitX, InitY, InitZ,
                                                     sigma_vertex)
        # print(np.sqrt(Smear_Z**2+Smear_Y**2+Smear_X**2))
        if (np.sqrt(Smear_X**2 + Smear_Y**2 + Smear_Z**2) < R_vertex_cut):
            evt.GetEntry(entry)
            pmtID = np.asarray(evt.pmtID)
            # index for different kind of pmts
            SPMTs = np.where((pmtID >= sPMTID_low) & (pmtID <= sPMTID_up))[0]
            WPPMTs = np.where((pmtID >= WPPMTID_low)
                              & (pmtID <= WPPMTID_up))[0]
            LPMTs = np.where((pmtID >= LPMTID_low) & (pmtID <= LPMTID_up))[0]
            if (WPPMTs.shape[0] < WP_NPE_cut) & (
                    LPMTs.shape[0] > LPMT_NPE_cut) & (LPMTs.shape[0] <
                                                      LPMT_NPE_cut_up):
                # hit position only for sPMT
                Hit_x, Hit_y, Hit_z = np.asarray(
                    evt.GlobalPosX)[SPMTs] / 1e3, np.asarray(
                        evt.GlobalPosY)[SPMTs] / 1e3, np.asarray(
                            evt.GlobalPosZ)[SPMTs] / 1e3
                R_Vi = np.sqrt((Smear_X - Hit_x)**2 + (Smear_Y - Hit_y)**2 +
                               (Smear_Z - Hit_z)**2)

                hitTime = np.asarray(evt.hitTime)[SPMTs]
                # smear hitTime
                Smear_t = np.random.normal(hitTime, sigma_hitTime)
                # prompt time cut less than 3 times of meadian value
                # this cut is not valid, even though better
                # hit_pr_idx=np.where(Smear_t<np.median(hitTime)*10)[0]
                hit_pr_idx = np.where(
                    Smear_t < HitTimeCut_up)[0]  # <np.median(hitTime)*10)[0]
                t_res_i = Smear_t[hit_pr_idx] - \
                    (R_Vi[hit_pr_idx]*LS_RI_idx/LightSpeed_c)
                # t_res_i=Smear_t-(R_Vi*LS_RI_idx/LightSpeed_c)
                # print(t_res_i)
                # print(R_Vi)

                # takes RMS not standar devation?
                RMS_t_res = np.sqrt(np.mean(t_res_i**2))

                # lepton at first place
                InitPDGID = np.asarray(geninfo.InitPDGID)[0]

                # for value, find 0 means under the LPMT cut,
                # which will not happen, find 1 means first stage, thus need -1
                # to the histgram list
                At_Which_NPE_LPMT = np.searchsorted(LPMT_NPE_steps,
                                                    LPMTs.shape[0]) - 1
                # e-CC
                if (InitPDGID == 11) | (InitPDGID == -11):
                    h_eCC_list[At_Which_NPE_LPMT].Fill(RMS_t_res)
                # mu-CC
                elif (InitPDGID == 13) | (InitPDGID == -13):
                    h_muCC_list[At_Which_NPE_LPMT].Fill(RMS_t_res)
                else:
                    h_NC_list[At_Which_NPE_LPMT].Fill(RMS_t_res)
    ff_TimeP = ROOT.TFile(
        "./results/" + SaveFileName + str(StartFile) + ".root", "RECREATE")
    ff_TimeP.cd()
    for i in range(len(LPMT_NPE_steps)):
        h_muCC_list[i].SetXTitle("#sigma(t_{res}) [ns]")
        h_muCC_list[i].SetYTitle("entries")
        h_eCC_list[i].SetXTitle("#sigma(t_{res}) [ns]")
        h_eCC_list[i].SetYTitle("entries")
        h_NC_list[i].SetXTitle("#sigma(t_{res}) [ns]")
        h_NC_list[i].SetYTitle("entries")
        h_muCC_list[i].Write()
        h_eCC_list[i].Write()
        h_NC_list[i].Write()

    ff_TimeP.Close()


def GetNPE_Tres_Energy_Profile(NFiles,
                               StartFile=1,
                               SaveFileName="NPETresEnergyProfile"):
    ROOT.TH1.AddDirectory(False)
    ROOT.ROOT.EnableImplicitMT()
    ff_out = ROOT.TFile(
        "./results/" + SaveFileName + str(StartFile) + "_" + str(NFiles) +
        ".root", 'recreate')
    ff_out.cd()
    muCC_TresNPEE3D_tree = ROOT.TTree(
        "muCC_NPETresE",
        "muon Charge Current: NPE_LPMT, Time Residual and True Energy")
    eCC_TresNPEE3D_tree = ROOT.TTree(
        "eCC_NPETresE",
        "e Charge Current: NPE_LPMT, Time Residual and True Energy")
    NC_TresNPEE3D_tree = ROOT.TTree(
        "NC_NPETresE",
        "Neutral Current: NPE_LPMT, Time Residual and True Energy")

    sigma_tres = array('f', [0])  # sigma(t_res)
    NPE_LPMT = array('f', [0])  # NPE_LPMT
    E_nu_true = array('f', [0])  # true neutrin energy

    muCC_TresNPEE3D_tree.Branch("sigma_tres", sigma_tres, 'sigma_tres/F')
    muCC_TresNPEE3D_tree.Branch("NPE_LPMT", NPE_LPMT, 'NPE_LPMT/F')
    muCC_TresNPEE3D_tree.Branch("E_nu_true", E_nu_true, 'E_nu_true/F')
    eCC_TresNPEE3D_tree.Branch("sigma_tres", sigma_tres, 'sigma_tres/F')
    eCC_TresNPEE3D_tree.Branch("NPE_LPMT", NPE_LPMT, 'NPE_LPMT/F')
    eCC_TresNPEE3D_tree.Branch("E_nu_true", E_nu_true, 'E_nu_true/F')
    NC_TresNPEE3D_tree.Branch("sigma_tres", sigma_tres, 'sigma_tres/F')
    NC_TresNPEE3D_tree.Branch("NPE_LPMT", NPE_LPMT, 'NPE_LPMT/F')
    NC_TresNPEE3D_tree.Branch("E_nu_true", E_nu_true, 'E_nu_true/F')

    evt = ROOT.TChain("evt")
    geninfo = ROOT.TChain("geninfo")
    pgst = ROOT.TChain("pgst")
    AddUserFile2TChain(evt, NFiles=NFiles, StartFile=StartFile)
    AddUserFile2TChain(geninfo, NFiles=NFiles, StartFile=StartFile)
    AddUserFile2TChain(pgst, NFiles=NFiles, StartFile=StartFile)

    evt.SetBranchStatus("*", 0)
    geninfo.SetBranchStatus("*", 0)
    pgst.SetBranchStatus("*", 0)
    pgst.SetBranchStatus("Ev", 1)  #initial neutrino energy
    geninfo.SetBranchStatus("InitX", 1)
    geninfo.SetBranchStatus("InitY", 1)
    geninfo.SetBranchStatus("InitZ", 1)
    geninfo.SetBranchStatus("InitPDGID", 1)
    evt.SetBranchStatus("hitTime", 1)
    evt.SetBranchStatus("nPE", 1)
    evt.SetBranchStatus("pmtID", 1)
    evt.SetBranchStatus("GlobalPosX", 1)
    evt.SetBranchStatus("GlobalPosY", 1)
    evt.SetBranchStatus("GlobalPosZ", 1)

    ff_out.cd()
    for entry in range(evt.GetEntries()):
        geninfo.GetEntry(entry)
        # one vertex, use first one, mm to meter
        InitX, InitY, InitZ = np.asarray(geninfo.InitX)[0] / 1e3, np.asarray(
            geninfo.InitY)[0] / 1e3, np.asarray(geninfo.InitZ)[0] / 1e3
        Smear_X, Smear_Y, Smear_Z = GetSmearedVertex(InitX, InitY, InitZ,
                                                     sigma_vertex)
        # print(np.sqrt(Smear_Z**2+Smear_Y**2+Smear_X**2))
        if (np.sqrt(Smear_X**2 + Smear_Y**2 + Smear_Z**2) < R_vertex_cut):
            evt.GetEntry(entry)
            pmtID = np.asarray(evt.pmtID)
            nPE = np.asarray(evt.nPE)
            # index for different kind of pmts
            SPMTs = np.where((pmtID >= sPMTID_low) & (pmtID <= sPMTID_up))[0]
            WPPMTs = np.where((pmtID >= WPPMTID_low)
                              & (pmtID <= WPPMTID_up))[0]
            NPE_WPPMTs = np.sum(nPE[WPPMTs])
            LPMTs = np.where((pmtID >= LPMTID_low) & (pmtID <= LPMTID_up))[0]
            LPMTs_badTime = np.where(
                np.asarray(evt.hitTime)[LPMTs] > HitTimeCut_up)[0]
            NPE_LPMT[0] = np.sum(nPE[LPMTs]) - np.sum(nPE[LPMTs_badTime])
            if (NPE_WPPMTs < WP_NPE_cut):  #only WP cut
                # & (NPE_LPMT > LPMT_NPE_cut) & (NPE_LPMT <LPMT_NPE_cut_up):
                # hit position only for sPMT
                Hit_x, Hit_y, Hit_z = np.asarray(
                    evt.GlobalPosX)[SPMTs] / 1e3, np.asarray(
                        evt.GlobalPosY)[SPMTs] / 1e3, np.asarray(
                            evt.GlobalPosZ)[SPMTs] / 1e3
                R_Vi = np.sqrt((Smear_X - Hit_x)**2 + (Smear_Y - Hit_y)**2 +
                               (Smear_Z - Hit_z)**2)
                hitTime = np.asarray(evt.hitTime)[SPMTs]
                # smear hitTime
                Smear_t = np.random.normal(hitTime, sigma_hitTime)
                # prompt time cut given by Giulio: 1.2 mus
                if np.size(Smear_t) > 1:
                    hit_pr_idx = np.where(Smear_t < HitTimeCut_up)[0]
                    t_res_i = Smear_t[hit_pr_idx] - \
                        (R_Vi[hit_pr_idx]*LS_RI_idx/LightSpeed_c)
                else:
                    if Smear_t < HitTimeCut_up:
                        t_res_i = Smear_t - \
                            (R_Vi*LS_RI_idx/LightSpeed_c)
                    else:
                        continue

                # takes RMS not standar devation
                sigma_tres[0] = np.sqrt(np.mean(t_res_i**2))
                pgst.GetEntry(entry)
                E_nu_true[0] = np.asarray(pgst.Ev)  #in GeV

                # lepton at first place
                InitPDGID = np.asarray(geninfo.InitPDGID)[0]
                if (InitPDGID == 11) | (InitPDGID == -11):
                    eCC_TresNPEE3D_tree.Fill()
                elif (InitPDGID == 13) | (InitPDGID == -13):
                    muCC_TresNPEE3D_tree.Fill()
                else:
                    NC_TresNPEE3D_tree.Fill()

    ff_out.Write()
    ff_out.Close()


if __name__ == "__main__":
    pass
