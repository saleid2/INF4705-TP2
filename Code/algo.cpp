#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <random>

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
    int tabouIterationsLeft = 0;

    bool CanFit(Block* previous){
        return (this->largeur < previous->largeur) && (this->profondeur < previous->profondeur);
    }
};

// This is part of the glouton we're supposed to implement
std::vector<Block*> vorace(std::vector<Block*> sortedBlocks){
    std::vector<Block*> stackedBlocks;

    std::default_random_engine gen;
    std::uniform_real_distribution<double> distro(0.0,1.0);

    auto it = sortedBlocks.begin();

    // First block has biggest surface => tower's base
    stackedBlocks.push_back(*it++);

    // Iterate over all the sorted blocks
    // O(n)
    while(it != sortedBlocks.end()){
        // If the block fits over the previous one
        if((*it)->CanFit(*(stackedBlocks.end()-1))){
            double p = (*it)->hauteur/((*it)->largeur * (*it)->profondeur);
            if(p < distro(gen)){
                stackedBlocks.push_back(*it);
            }
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

std::vector<Block*> tabou(std::vector<Block*> sorted){
    const int MaxIterations = 100;
    int currentIterationWithSolution = 0;

    std::vector<Block*> stackedBlocks;
    int stackedBlocksHeight = 0;

    // Initial solution with greedy algorithm
    auto it = sorted.begin();

    stackedBlocks.push_back(*it++);

    // O(n)
    while(it != sorted.end()){
        if((*it)->CanFit(stackedBlocks.back())){
            stackedBlocks.push_back(*it);
            stackedBlocksHeight += (*it)->hauteur;
        }
        it++;
    }

    // Tabu algorithm
    // O(nlogn)
    std::sort(sorted.begin(), sorted.end(), [](Block* a, Block* b){return a->hauteur > b->hauteur;});

    // Inverted so that we can iterate from top to bottom
    std::reverse(stackedBlocks.begin(), stackedBlocks.end());

    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(7, 10);

    while(currentIterationWithSolution < 100){
        // We're gonna start seeing what can be replaced from top to bottom
        std::vector<Block*> tempSolution(stackedBlocks);
        int tabuHeight = stackedBlocksHeight;

        // Iterates over the blocks that can be inserted
        for(auto tit = sorted.begin(); tit != sorted.end(); tit++){

            // Iterate over the currently stacked blocks from top to bottom
            for(int j = tempSolution.size() - 1; j >= 0; j--){

                // If the block that we want to insert is not in the tabu list
                if((*tit)->tabouIterationsLeft-- < 1){
                    // If the block can fit on the previous block
                    if((*tit)->CanFit(tempSolution[j])){
                        // If the insertion position is not the top of the stack
                        if(j != (tempSolution.size() - 1)){
                            // If the items higher than the insertion position can fit on block we want to insert
                            if(tempSolution[j+1]->CanFit(*tit)){
                                auto tempTit = tempSolution.begin() + j + 1;
                                tempSolution.insert(tempTit, *tit);
                                tabuHeight += (*tit)->hauteur;
                            }
                            else{
                                while(tempSolution.size() > j){
                                    auto tabuBlock = tempSolution.back();
                                    tempSolution.pop_back();
                                    tabuHeight -= tabuBlock->hauteur;
                                    tabuBlock->tabouIterationsLeft = dis(gen);
                                }
                                tempSolution.push_back(*tit);
                                tabuHeight += (*tit)->hauteur;
                            }
                        }
                        else{
                            tempSolution.push_back(*tit);
                            tabuHeight += (*tit)->hauteur;
                        }
                        break;
                    }
                }
            }
        }

        if(tabuHeight > stackedBlocksHeight){
            stackedBlocks = std::vector<Block*>(tempSolution);
            currentIterationWithSolution = 0;
        }
        else{
            currentIterationWithSolution++;
        }
    }
    
    return stackedBlocks;
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
    if (prog_args.algo == "vorace"){
        stackedBlocks = vorace(blocks);
    }
    else if(prog_args.algo == "progdyn"){
        stackedBlocks = progDyn(blocks);
    }
	else if(prog_args.algo == "tabou"){
        stackedBlocks = tabou(blocks);
    }


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