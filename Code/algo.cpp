#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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

    bool CanFit(int largeur, int profondeur){
        return (this->largeur < largeur) && (this->profondeur < profondeur);
    }
};


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

    // Validate that blocks are properly sorted
    // for(int i = 0; i < blocks.size(); i++)
    //     std::cout << blocks[i]->hauteur << " " << blocks[i]->largeur << " " << blocks[i]->profondeur << " " << blocks[i]->largeur * blocks[i]->profondeur << std::endl;


    // Pick algorithm
    if (prog_args.algo == "vorace")
        return 0;
    else if(prog_args.algo == "progdyn")
        return 0;
	else if(prog_args.algo == "tabou")
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