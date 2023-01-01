# LHE READER code to read MG5 LHE output


import xml.etree.ElementTree as ET
import ROOT
from ROOT import TFile, TTree, std
from array import array


def read_xml(filename="unweighted_events.lhe"):

    tree = ET.parse(filename)
    root = tree.getroot()
    return root


def read_xml_child(root):
    data_collect = []
    for child in root:
        if child.tag!='event': # for the moment storing only event data and no subchilds or other info
            continue
        data = child.text.split()
        data_collect.append(data)
    return data_collect


def build_TTree(data):

    m_file = TFile.Open("lhe.root","recreate")
    m_tree = TTree("lhedata","lhedata")

    m_Npart = array('i',[0])
    m_eventweight = array('f',[0.0])
    m_scale = array('f',[0.0])
    m_qed = array('f',[0.0])
    m_qcd = array('f',[0.0])
 
    #define particle-level variables
    m_pid = std.vector('int')()
    m_status = std.vector('int')()
    m_mother1 = std.vector('int')()
    m_mother2 = std.vector('int')()
    m_color1 = std.vector('int')()
    m_color2 = std.vector('int')()
    m_px = std.vector('float')()
    m_py = std.vector('float')()
    m_pz = std.vector('float')()
    m_e = std.vector('float')()
    m_mass = std.vector('float')()
    m_tau = std.vector('float')()
    m_spin = std.vector('float')()
    
    #define tree branches
    m_tree.Branch('numParticles',m_Npart,'numParticles/I')
    m_tree.Branch("eventweight",m_eventweight,"eventweight/F")
    m_tree.Branch("scale",m_scale,"scale/F")
    m_tree.Branch("alpha_qed",m_qed,"alpha_qed/F")
    m_tree.Branch("alpha_qcd",m_qcd,"alpha_qcd/F")


    m_tree.Branch("pid",m_pid)
    m_tree.Branch("status",m_status)
    m_tree.Branch("mother1",m_mother1)
    m_tree.Branch("mother2",m_mother2)
    m_tree.Branch("color1",m_color1)
    m_tree.Branch("color2",m_color2)
    m_tree.Branch("px",m_px)
    m_tree.Branch("py",m_py)
    m_tree.Branch("pz",m_pz)
    m_tree.Branch("energy",m_e)
    m_tree.Branch("mass",m_mass)
    m_tree.Branch("color1",m_color1)
    m_tree.Branch("tau",m_tau)
    m_tree.Branch("spin",m_spin)


    k = 0
    for i in range(len(data)):
        m_Npart[0] = int(data[i][0])
        m_eventweight[0] = float(data[i][2])
        m_scale[0] = float(data[i][3])
        m_qed[0] = float(data[i][4])
        m_qcd[0] = float(data[i][5])
        
        x , y = 6, 19
        
        for j in range(int(data[i][0])):
            m_pid.push_back(int(data[i][x]))
            m_status.push_back(int(data[i][x+1]))
            m_mother1.push_back(int(data[i][x+2]))
            m_mother2.push_back(int(data[i][x+3]))
            m_color1.push_back(int(data[i][x+4]))
            m_color2.push_back(int(data[i][x+5]))
            m_px.push_back(float(data[i][x+6]))
            m_py.push_back(float(data[i][x+7]))
            m_pz.push_back(float(data[i][x+8]))
            m_e.push_back(float(data[i][x+9]))
            m_mass.push_back(float(data[i][x+10]))
            m_tau.push_back(float(data[i][x+11]))
            m_spin.push_back(float(data[i][x+12]))
     
            
            
            k = k + 1
            x, y = y, y  + 13
        
        m_tree.Fill()
        m_pid.clear()
        m_pid.clear()
        m_status.clear()
        m_mother1.clear()
        m_mother2.clear()
        m_color1.clear()
        m_color2.clear()
        m_px.clear()
        m_py.clear()
        m_pz.clear()
        m_e.clear()
        m_mass.clear()
        m_tau.clear()
        m_spin.clear()


    m_file.Write("", TFile.kOverwrite)
    m_file.Close()
    print(k)
    return 0 


def main():
    root = read_xml(filename="unweighted_events.lhe")
    data_collect = read_xml_child(root)
    build_TTree(data_collect)
    return 0


main()
