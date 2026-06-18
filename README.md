# LHEReader

[![Website](https://img.shields.io/badge/Website-LHEReader-blue)](https://amanmdesai.github.io/LHEReader/)
[![Journal DOI](https://img.shields.io/badge/Journal_DOI-10.1088%2F1402--4896%2Fae7772-green.svg)](https://doi.org/10.1088/1402-4896/ae7772)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7642763.svg)](https://doi.org/10.5281/zenodo.7642763)
[![arXiv](https://img.shields.io/badge/arXiv-2603.01489-b31b1b.svg)](https://arxiv.org/abs/2603.01489)

**Citation:** A. Desai, *LHEReader: simplified conversion from Les Houches event files to ROOT format*, Phys. Scripta **101** (2026) 231501, DOI: 10.1088/1402-4896/ae7772, arXiv:2603.01489.

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


## EventLoop (Example)

The `EventLoop/` directory contains an example ROOT C++ macro (`EventLoop.cpp`) demonstrating how to read the ROOT TTree produced by `LHEReader.py` and apply event selection to produce histograms and plots.

```code
root -l -q EventLoop.cpp
```

## Citation

@article{Desai:2026lst,
    author = "Desai, Aman",
    title = "{LHEREADER: simplified conversion from Les Houches event files to ROOT format}",
    eprint = "2603.01489",
    archivePrefix = "arXiv",
    primaryClass = "hep-ph",
    doi = "10.1088/1402-4896/ae7772",
    journal = "Phys. Scripta",
    volume = "101",
    number = "23",
    pages = "231501",
    year = "2026"
}
References: 
1. http://arxiv.org/abs/hep-ph/0609017
