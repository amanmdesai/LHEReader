# LHE READER code to read MG5 LHE output
# Author Aman Desai
# https://github.com/amanmdesai/LHEReader

import xml.etree.ElementTree as ET
import ROOT
from ROOT import TFile, TTree, std
from array import array
import argparse
import pathlib
import gzip



def read_event(file_object):

    for _, elem in ET.iterparse(file_object, events=("end",)):
        if elem.tag != "event":
            continue
        text = elem.text or ""
        yield text.split()
        elem.clear()


def build_TTree(event_iter, outputname):

    m_file = TFile.Open(outputname, "recreate")
    m_tree = TTree("events", "events")

    m_Npart = array("i", [0])
    m_eventweight = array("f", [0.0])
    m_scale = array("f", [0.0])
    m_qed = array("f", [0.0])
    m_qcd = array("f", [0.0])

    # define particle-level variables
    m_pid = std.vector("int")()
    m_status = std.vector("int")()
    m_mother1 = std.vector("int")()
    m_mother2 = std.vector("int")()
    m_color1 = std.vector("int")()
    m_color2 = std.vector("int")()
    m_px = std.vector("float")()
    m_py = std.vector("float")()
    m_pz = std.vector("float")()
    m_e = std.vector("float")()
    m_mass = std.vector("float")()
    m_tau = std.vector("float")()
    m_spin = std.vector("float")()

    # define tree branches
    #event level
    m_tree.Branch("numParticles", m_Npart, "numParticles/I")
    m_tree.Branch("eventweight", m_eventweight, "eventweight/F")
    m_tree.Branch("scale", m_scale, "scale/F")
    m_tree.Branch("alpha_qed", m_qed, "alpha_qed/F")
    m_tree.Branch("alpha_qcd", m_qcd, "alpha_qcd/F")

    #particle level

    m_tree.Branch("pid", m_pid)
    m_tree.Branch("status", m_status)
    m_tree.Branch("mother1", m_mother1)
    m_tree.Branch("mother2", m_mother2)
    m_tree.Branch("color1", m_color1)
    m_tree.Branch("color2", m_color2)
    m_tree.Branch("px", m_px)
    m_tree.Branch("py", m_py)
    m_tree.Branch("pz", m_pz)
    m_tree.Branch("energy", m_e)
    m_tree.Branch("mass", m_mass)
    m_tree.Branch("tau", m_tau)
    m_tree.Branch("spin", m_spin)

    k = 0 #
    for data in event_iter:
        if len(data) < 6:
            continue

        m_Npart[0] = int(data[0]) # first entry is number of particles
        m_eventweight[0] = float(data[2]) # 3rd entry is the event weight
        m_scale[0] = float(data[3]) #4th quantity is the scale
        m_qed[0] = float(data[4]) # second last quantity is the alpha qed
        m_qcd[0] = float(data[5]) # last quantity is the alpha qcd

        x, y = 6, 19 # 6 as above five are already filled, 19 represents (13 quantities+6 indices)

        for j in range(int(data[0])):
            if (x + 12) >= len(data):
                break
            m_pid.push_back(int(data[x]))
            m_status.push_back(int(data[x + 1]))
            m_mother1.push_back(int(data[x + 2]))
            m_mother2.push_back(int(data[x + 3]))
            m_color1.push_back(int(data[x + 4]))
            m_color2.push_back(int(data[x + 5]))
            m_px.push_back(float(data[x + 6]))
            m_py.push_back(float(data[x + 7]))
            m_pz.push_back(float(data[x + 8]))
            m_e.push_back(float(data[x + 9]))
            m_mass.push_back(float(data[x + 10]))
            m_tau.push_back(float(data[x + 11]))
            m_spin.push_back(float(data[x + 12]))

            k = k + 1
            x, y = y, y + 13

        m_tree.Fill()

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
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog = 'LHEReader',description = 'Converts LHE files to ROOT Trees')
    parser.add_argument("--input", type=str,help='Input file path and Name')
    parser.add_argument("--output", type=str,help='Output file path and Name')
    args = parser.parse_args()

    input_path = pathlib.Path(args.input)
    output_path = pathlib.Path(args.output)

    # Accept .lhe OR .lhe.gz
    valid_input = (
        input_path.suffix == ".lhe" or
        (input_path.suffix == ".gz" and input_path.name.endswith(".lhe.gz"))
    )

    if not valid_input:
        print('ONLY .lhe OR .lhe.gz FILES ARE ACCEPTED AS INPUT')
        exit()

    if output_path.suffix != ".root":
        print('ONLY .root FILES ARE ACCEPTED AS OUTPUT')
        exit()

    # Handle compressed file
    if input_path.suffix == ".gz":
        with gzip.open(input_path, "rt") as f:
            build_TTree(read_event(f), outputname=args.output)
    else:
        with open(args.input, "rt") as f:
            build_TTree(read_event(f), outputname=args.output)
