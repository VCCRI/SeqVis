# SeqVis

Visualization of compositional heterogeneity in large alignments of nucleotides

## Summary

SeqVis is a web application developed to facilitate analysis and 3D visualization of compositional heterogeneity in species-rich alignments of nucleotide sequences. Each sequence is represented by a dot in a tetrahedron plot (i.e., an extension of the de Finetti; Fig. 1), where the position of the dot depends uniquely on the nucleotide content of that sequence. SeqVis also allows users to analyse their data set using, for example, the useful matched-pairs test of symmetry (Ababneh et al. 2006).

## Background

### Compositional heterogeneity

Most model-based phylogenetic methods assume that the sequences have evolved under the same time-reversible conditions (i.e., the evolutionary process is assumed to have been stationary, reversible, and globally homogeneous). Compositional heterogeneity in sequence data occurs when the nucleotide composition of homologous sequences varies across the taxa, and it implies that the sequences cannot have evolved under stationary, reversible, and homogeneous conditions. Therefore, if a phylogenetic analysis is to be conducted with a time-reversible Markov model (such as the HKY or GTR models), then it would be wise to survey the phylogenetic data first for evidence of violation of these assumptions.

### Existing methods for assessing compositional heterogeneity

There are many ways to survey alignments of nucleotides for evidence of compositional heterogeneity but most of the available methods are either likely to give an unreliable result (due to the methods' poor statistical design) or impractical because they were not designed to analyse the increasingly large, species-rich alignments that are now available (for details, see Jermiin et al. 2004, 2009).

## Getting Started

https://vccri.github.io/SeqVis/

### Features

* SeqVis reads and writes alignments in the sequential PHYLIP(.phy) format, the NEXUS(.nex) format, and the FASTA(.fasta) format.

* The tetrahedron can be rotated in all directions, animated, and manipulated interactively; all items (i.e., wireframe, axes, labels, level, point, and background) on display can be changed (e.g., colour, size, and visibility).

* The nucleotide composition at the codon sites can be surveyed independently and visualized on a single canvas. This can help assessing the heterogeneity of nucleotide composition across the three codon sites.

* Sequence information can be obtained by mouse-clicking on points of interest or by using inbuilt tools that query the data based on the sequences’ names.

* By projection, the tetrahedron can be illustrated as a de Finetti plot or a linear plot.

* Matched-pairs test of symmetry analytical tool is provided.

* On-screen images may be saved in the PNG and SVG formats.

## Contact

Dr. Joshua W. K. Ho<br />
Associate Professor<br />
School of Biomedical Sciences, Li Ka Shing Faculty of Medicine<br />
Rm 4-44, Laboratory Block, 21 Sassoon Road<br />
Hong Kong SAR, China<br />
Email: jwkho [ AT ] hku [ DOT ] hk 

## References
Ababneh F, Jermiin LS, Ma C, Robinson J (2006). Matched-pairs tests of homogeneity with applications to homologous nucleotide sequences. Bioinformatics 22, 1225-1231.<br /> 

Jermiin LS et al. (2004). The biasing effect of compositional heterogeneity on phylogenetic estimates may be underestimated. Syst. Biol. 53, 638-644.<br /> 

Jermiin LS, Ho JWK, Lau KW, Jayaswal V (2009). SeqVis: A tool for detecting compositional heterogeneity among aligned nucleotide sequences. In Bioinformatics for DNA sequence analysis (Ed. Posada D), Humana Press, Totowa (NJ). Pp. 65-91<br /> 

Rokas A, Krüger D, Carroll SB (2005). Animal evolution and the molecular signature of radiations compressed in time. Science 310, 1933-1938.<br />

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
