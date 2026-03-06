# LHEReader

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7642763.svg)](https://doi.org/10.5281/zenodo.7642763)
[![Website](https://img.shields.io/badge/Website-LHEReader-blue)](https://amanmdesai.github.io/LHEReader/)
[![arXiv](https://img.shields.io/badge/arXiv-2603.01489-b31b1b.svg)](https://arxiv.org/abs/2603.01489)

A python script to read Les Houches Event (LHE) files and use PyROOT to convert it into a ROOT Tree

The structure of the LHE file is:

Event Level Info: Number of Particles, Status, event weight, scales, alpha_QED, alpha_QCD

Particle Level Info: PDG id, status, mother1, mother2, color1, color2, px, py, pz, energy, mass, lifetime, spin


## Installation

```code
git clone https://github.com/amanmdesai/LHEReader.git
```

## Usage


```code
python LHEReader.py --input input.lhe --output output.root


usage: LHEReader [-h] [--input INPUT] [--output OUTPUT]

Converts LHE files to ROOT Trees

options:
  -h, --help       show this help message and exit
  --input INPUT    Input file path and Name (can be .lhe or .lhe.gz)
  --output OUTPUT  Output file path and Name (.root file)
```

where input and output refer to the path of the input and output files respectively


## Citation

@article{Desai:2026lst,
    author = "Desai, Aman",
    title = "{LHEReader: Simplified Conversion from Les Houches Event Files to ROOT Format}",
    eprint = "2603.01489",
    archivePrefix = "arXiv",
    primaryClass = "hep-ph",
    month = "3",
    year = "2026"
}

References: 
1. http://arxiv.org/abs/hep-ph/0609017
