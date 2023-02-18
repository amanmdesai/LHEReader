# LHEReader

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7642763.svg)](https://doi.org/10.5281/zenodo.7642763)


A python script to read Les Houches Event (LHE) files

Uses PyROOT to convert the output into a ROOT Tree

The structure of the LHE file is:

Event Level Info: Number of Particles, Status, event weight, scales, alpha_QED, alpha_QCD

Particle Level Info: PDG id, status, mother1, mother2, color1, color2, px, py, pz, energy, mass, lifetime, spin


## Installation

```code
git clone https://github.com/amanmdesai/LHEReader.git
```

## Usage
```code
usage: LHEReader [-h] [--input INPUT] [--output OUTPUT]

Converts LHE files to ROOT Trees

options:
  -h, --help       show this help message and exit
  --input INPUT    Input file path and Name
  --output OUTPUT  Output file path and Name
```

where input and output refer to the path of the input and output files respectively

References: 
1. http://arxiv.org/abs/hep-ph/0609017
