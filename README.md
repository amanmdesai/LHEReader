# LHE-Parser

A python script to read Les Houches Event (LHE) files

Uses PyROOT to convert the output into a ROOT Tree

The structure of the LHE file is: 

Event Level Info: Number of Particles, Status (not included in this script), event weight, scales, alpha_QED, alpha_QCD

Particle Level Info: PDG id, status, mother1, mother2, color1, color2, px, py, pz, energy, mass, lifetime, spin 



