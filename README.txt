Timing experiments:
	Run the "timing-experiments.sh" file.
	The results are stored in file "res/times".
Accuracy experiments:
	Run the "quality-experiments.sh" file [calls the "run.sh" file].
	The results are stored in folder "res".
	Example file: BOTTOM_K-STRUCTURED-1000000-24500-2000-1_4.txt 
	This means that the bottom-k algorithm is used on structured data of:
		cardinality 1000000, k 24500, repetitions 2000
		using tabulation-1permutation with 4 characters
	For more explanation on the filenames, look at the "run.sh" file. 
Source code:
	In the "src" folder.
Datasets:
	The atmospheric noise dataset can be found in atmospheric.txt.
	The geometric data is in dataset_processed.txt.
	The demographic data is in newDataset.txt.
Randomness:
	In the "seed" folder. 
Other files:
	Folder "scripts" contains the scripts we used to produce the figures.
	Folder "figures" contains all the figures of the paper.