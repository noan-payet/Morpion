#include <iostream>
#include <fstream>
#include <array>
#include <conio.h>

using Board = std::array<char, 9>;
int parties = 0;

unsigned int memory = 0; // memory = 0b0000 0000 0000 0000 0000 0000 0000 0000 -> 32 bits

unsigned short GetPlayerMemory()
{
    return memory & 1;
}

void SetPlayerMemory()
{
    memory ^= 1;
}

unsigned short GetPlayer1WinMemory()
{
    return (memory >> 3) & 7;
}

unsigned short GetPlayer2WinMemory()
{
    return (memory >> 6) & 7;
}

unsigned short GetNullMatch()
{
    return (memory >> 9) & 31;
}

void SetWinnerMemory(uint8_t winner)
{
    if (winner == 0)
        return;

    memory += (1 << 3 * winner);

    if ((memory >> 3 * winner) == 0b111)
        memory &= ~(0b111 << 3 * winner); // reset player 1 wins if it reaches 7
}

uint8_t GetStates()
{
    return (memory >> 1) & 3;
}

void SetStates(uint8_t states)
{
    if (states == 0)
        memory = memory & ~6;

    memory = memory | (states << 1);
}

unsigned short GetBoard(uint8_t square)
{
    return (memory >> (14 + square * 2)) & 0b11;
}

void SetBoard(uint8_t block, uint8_t player)
{
	unsigned short boardMask = (14 + block * 2);

    switch (player)
    {
    case 0:
        memory = memory | (1 << boardMask);
        break;

    case 1:
        
        memory = memory | (2 << boardMask);
        break;
    }
}

void CleanBoard()
{
    memory &= (0b11111111111111);
}

void afficherPlateau(const Board& b)
{
	system("cls");

    std::cout << "Morpion (Tic-Tac-Toe) en C++\n";
    std::cout << "Disposition des cases :\n";
    std::cout << " 7 | 8 | 9\n";
    std::cout << "---+---+---\n";
    std::cout << " 4 | 5 | 6\n";
    std::cout << "---+---+---\n";
    std::cout << " 1 | 2 | 3\n\n";

    std::cout << "\n";
    std::cout << "Partie : " << (parties+1) << "\n";
    std::cout << "\n";
    std::cout << " " << b[6] << " | " << b[7] << " | " << b[8] << "\n";
    std::cout << "---+---+---\n";
    std::cout << " " << b[3] << " | " << b[4] << " | " << b[5] << "\n";
    std::cout << "---+---+---\n";
    std::cout << " " << b[0] << " | " << b[1] << " | " << b[2] << "\n";
    std::cout << "\n";
}

bool caseLibre(const Board& b, int index)
{
    return b[index] == ' ';
}

bool coupGagnant(const Board& b, char joueur)
{
    const int lignesGagnantes[8][3] = {
        {0, 1, 2}, // lignes
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6}, // colonnes
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8}, // diagonales
        {2, 4, 6}
    };

    for (const auto& ligne : lignesGagnantes)
	{
        if (b[ligne[0]] == joueur && b[ligne[1]] == joueur && b[ligne[2]] == joueur)
            return true;
    }
    return false;
}

bool plateauPlein(const Board& b)
{
    for (char c : b)
	{
        if (c == ' ')
			return false;
    }
    return true;
}

int demanderCoup(const Board& b, char joueur)
{
    int choix = -1;
    while (true)
	{
        std::cout << "Joueur " << joueur << ", choisissez une case (1-9) : ";
        if (!(std::cin >> choix))
		{
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Saisie invalide, recommencez.\n";
            continue;
        }

        if (choix < 1 || choix > 9)
		{
            std::cout << "Veuillez entrer un nombre entre 1 et 9.\n";
            continue;
        }

        int index = choix - 1;
        if (caseLibre(b, index)==false)
		{
            std::cout << "Case déjà prise, choisissez-en une autre.\n";
            continue;
        }

        return index;
    }
}

char SwitchPlayer()
{
    if (GetPlayerMemory() == 0)
        return 'X';
    else
		return 'O';
}

void SetBoardState(uint8_t state, Board& b, uint8_t index)
{
    switch (state)
    {
    case 0:
        b[index] = ' ';
		break;
    case 1:
        b[index] = 'X';
        break;
    case 2:
        b[index] = 'O';
        break;
    }

	afficherPlateau(b);
}

bool OpenFile(bool open)
{
    if (!open)
    {
        std::cout << "Erreur lors de l'ouverture du fichier.\n";
        return false;
    }

    return true;
}

int main()
{
    Board plateau;
    plateau.fill(' ');

    std::ifstream importFile;
    std::ofstream exportFile;

	std::string path = "..\\x64\\Debug\\Save\\main.txt";

	importFile.open(path);
    if (!OpenFile(importFile.is_open()))
    {
		path = "Save\\main.txt";
		importFile.close();
        importFile.open(path);
    }

	importFile >> memory;
    importFile.close();

    char joueurCourant = SwitchPlayer();
    bool fini = false;

	parties = GetPlayer1WinMemory() + GetPlayer2WinMemory() + GetNullMatch();

	while(true)
	{

		for (uint8_t i = 0; i < 9; i++) // Initialiser le plateau à partir de la mémoire
        {
            uint8_t state = GetBoard(i);
            SetBoardState(state, plateau, i);
        }

		while (fini==false)
        {
			afficherPlateau(plateau);

			exportFile.open(path);
			exportFile << memory << " \n-> unsigned int memory\n";
			exportFile.close();

            std::cout << memory << '\n';

            std::cout << GetPlayerMemory() << " -> Joueur\n";

            std::cout << GetPlayer1WinMemory() << " -> Victoire j1\n";
            std::cout << GetPlayer2WinMemory() << " -> Victoire j2\n";
            std::cout << GetNullMatch() << " -> Null\n";

			for (int c = 0; c < 9; ++c)
            {
                std::cout << GetBoard(c) << ' ';
			}

			std::cout << "-> Board states\n";

			int index = demanderCoup(plateau, joueurCourant);
            SetBoard(index, GetPlayerMemory());
			plateau[index] = joueurCourant;

			if (coupGagnant(plateau, joueurCourant))
			{
				afficherPlateau(plateau);
				std::cout << "Le joueur " << joueurCourant << " a gagné !\n";

                if (joueurCourant == 'X')
                    SetStates(1);
                else if (joueurCourant == 'O')
                    SetStates(2);

				fini = true;
			}
			else if (plateauPlein(plateau))
			{
				afficherPlateau(plateau);
				std::cout << "Match nul !\n";
                SetStates(3);
				fini = true;
			}
			else
			{
                SetPlayerMemory();
                joueurCourant = SwitchPlayer();
                SetStates(0);
			}

            SetWinnerMemory(GetStates());
		}
        parties++;
		fini = false;
        CleanBoard();
        _getch();
	}

    return 0;
}
