#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

class Block
{
    public:
    Block(std::string params){
        // String is formatted as Hauteur Largeur Profondeur
        std::stringstream ss(params);

        ss >> hauteur;
        ss >> largeur;
        ss >> profondeur;
    }

    int hauteur;
    int largeur;
    int profondeur;

    bool CanFit(Block* previous){
        return (this->largeur <= previous->largeur) && (this->profondeur <= previous->profondeur);
    }
};

// This is part of the glouton we're supposed to implement
std::vector<Block*> vorace(std::vector<Block*> sortedBlocks){
    std::vector<Block*> stackedBlocks;

    auto it = sortedBlocks.begin();

    // First block has biggest surface => tower's base
    stackedBlocks.push_back(*it++);

    // Iterate over all the sorted blocks
    while(it != sortedBlocks.end()){
        // If the block fits over the previous one
        if((*it)->CanFit(*(stackedBlocks.end()-1))){
            // Stack it
            stackedBlocks.push_back(*it);
        }
        it++;
    }

    return stackedBlocks;
}

std::vector<Block*> progDyn(std::vector<Block*> sorted){
    std::vector<int> maxStackedHeight;
    std::vector<int> precBlockIndex;
    int i = 0;

    // Initialize vectors for storing the maximum height & the previous blocks for the block at index i
    // O(n)
    for(auto it = sorted.begin(); it != sorted.end(); it++){
        maxStackedHeight.push_back((*it)->hauteur);
        precBlockIndex.push_back(i++);
    }

    // O(n²)
    for(int i = 1; i < sorted.size(); i++){
        for(int j = 0; j < i; j++){
            if(sorted[i]->CanFit(sorted[j])){
                if(maxStackedHeight[i] < sorted[i]->hauteur + maxStackedHeight[j]){
                    maxStackedHeight[i] = sorted[i]->hauteur + maxStackedHeight[j];
                    precBlockIndex[i] = j;
                }
            }
        }
    }

    // Find maximum height
    int maxHeight = -1;
    int index;

    // O(n)
    for(int i = 0; i < maxStackedHeight.size(); i++){
        if(maxHeight < maxStackedHeight[i]){
            maxHeight = maxStackedHeight[i];
            index = i;
        }
    }

    std::vector<Block*> resultStack;

    // O(n)
    while(true){
        resultStack.push_back(sorted[index]);

        if(index == precBlockIndex[index])
        {
            break;
        }

        index = precBlockIndex[index];
    }

    // Reverse results to display from bottom to top
    std::reverse(resultStack.begin(), resultStack.end());

    return resultStack;
}


int main(int argc, char *argv[]) {
    struct {
        std::string algo;
        std::string file_path;
        bool print_res{false};
        bool print_time{false};
    } prog_args;

    // Read program arguments
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-a") {
            prog_args.algo = argv[i+1]; i++;
        } else if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        }
    }

    // Read numbers into vector
    std::vector<Block*> blocks;
    std::fstream ex_file(prog_args.file_path);
    std::string n;
    while (getline(ex_file, n))
    {
        blocks.push_back(new Block(n));
    }


    // Sort by surface (decreasing)
    std::sort(blocks.begin(), blocks.end(),[](Block* a, Block* b){return a->largeur*a->profondeur > b->largeur*b->profondeur;});

    std::vector<Block*> stackedBlocks;

    auto start = std::chrono::high_resolution_clock::now();

    // Pick algorithm
    if (prog_args.algo == "vorace")
        stackedBlocks = vorace(blocks);
    else if(prog_args.algo == "progdyn"){
        stackedBlocks = progDyn(blocks);
    }
	else if(prog_args.algo == "tabou")
		return 0;


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_s = end-start;

    // Print tower
    if(prog_args.print_res)
        for(auto it = stackedBlocks.begin(); it != stackedBlocks.end(); it++)
            std::cout << std::setw(10) << std::left << (*it)->hauteur << std::setw(10) << std::left << (*it)->largeur << std::setw(10) << std::left << (*it)->profondeur << std::endl;

    // Output time in ms
    if(prog_args.print_time)
        std::cout << elapsed_s.count()*1000 << std::endl;

    return 0;
}


/*

Block are object : H,L,P.


VORACE:

Sort list by surface (LxP) en ordre décroissant
while list isn't empty
	x <- choose(list)

	
choose(list):
	Parcourre la liste, si la taille est plus petite (strictement) que le bloc prédédant, try to add.
	Try to add => Probabilité p = H/(LxP)

	
	
	
DYNAMIQUE:

Sort list by surface (LxP) en ordre décroissant

Choisir la plus grande surface (first one).

Ajouter le prochain block qui a la plus grande surface ET qui entre sur le block précédant.




TABOU:
*/