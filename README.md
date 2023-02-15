# LHEReader

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7642763.svg)](https://doi.org/10.5281/zenodo.7642763)


A python script to read Les Houches Event (LHE) files

Uses PyROOT to convert the output into a ROOT Tree

The structure of the LHE file is:

Event Level Info: Number of Particles, Status, event weight, scales, alpha_QED, alpha_QCD

Particle Level Info: PDG id, status, mother1, mother2, color1, color2, px, py, pz, energy, mass, lifetime, spin


References: 
1. http://arxiv.org/abs/hep-ph/0609017
