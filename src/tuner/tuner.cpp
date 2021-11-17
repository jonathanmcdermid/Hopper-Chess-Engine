#include <thread>
#include <math.h>
#include <cstring>
#include "tuner.h"

namespace Hopper
{
	std::vector<position> myPositions;
	std::vector<bool> myTurns;
	std::vector<double> myResults;
	Evaluate myEval = Evaluate();
	static int numCores = 4;
	long double answers = 0;

	void tune() {
		std::string fileName = "tuner/E12.33-1M-D12-Resolved.book";
		std::ifstream logFile;
		logFile.open(fileName.c_str(), std::ifstream::in);

		std::string line;

		while (true) {
			if (logFile.eof())
				break;
			getline(logFile, line);
			if (line != "") {
				myResults.push_back(std::stod(line.substr(line.length() - 4, 3)));
				myPositions.push_back(position(line.substr(0, line.size() - 6).c_str()));
				myTurns.push_back(line.at(line.find(' ') + 1) == 'w' ? WHITE : BLACK);
			}
		}
		logFile.close();

		std::cout << "positions done loading " << myPositions.size() << " "<< myResults.size() << " " << myTurns.size() << "\n";

		const unsigned numWeights =
			// 12*64 for endgame and middlegame PSQTs
			12 * SPACES +
			// 6*2 for endgame and middlegame piece values
			6 * 2 +
			// 8 for passed pawn rank bonus
			8 +
			// 5 for pawn file population penalty
			5 +
			// 1 for connected pawns
			1 +
			// 1 for backward pawns
			1 +
			// 1 for isolated pawns
			1 +
			// 2 for knight and bishop outpost
			2 +
			// 1 for rook open file
			1 +
			// 1 for bishop pair
			1;

		int* weights[numWeights];
		bool skips[numWeights];
		memset(skips, 0, sizeof(skips));

		mapWeightsToParams(weights, &skips[0]);

		std::string fen;
		long double MSE;
	//long double K = 1.09374;//findK();
		long double K = findK();
		long double bestMSE = meanSquaredError(K);
		int bestWeights[numWeights];
		memset(bestWeights, 0, sizeof(bestWeights));
		bool improved = true;
		int adjustment = 1;
		for (unsigned iterations = 0; improved; ++iterations) {
			improved = false;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
				if (1){//skips[weightIndex] == false) {
					*weights[weightIndex] += adjustment;
					MSE = meanSquaredError(K);
					if (MSE < bestMSE) {
						bestMSE = MSE;
						improved = true;
						for (int i = 0; i < numWeights; ++i)
							bestWeights[i] = *weights[i];
					}
					else {
						//if (weightIndex >= 768 && *weights[weightIndex] <= 0) {
						//	weights[weightIndex] += 1;
						//	continue;
						//}
						*weights[weightIndex] -= 2 * adjustment;
						MSE = meanSquaredError(K);
						if (MSE < bestMSE) {
							bestMSE = MSE;
							improved = true;
							for (int i = 0; i < numWeights; ++i)
								bestWeights[i] = *weights[i];
						}
						else
							* weights[weightIndex] += adjustment;
					}
				}
				std::cout << "weight " << weightIndex << " mse: " << bestMSE << " adjustment: "<< adjustment << "\n";
			}
			if (improved == false && adjustment > 1) {
				improved = true;
				--adjustment;
			}
			for (int i = 0; i < numWeights; ++i)
				*weights[i] = bestWeights[i];
			std::cout << "iteration " << iterations << " complete\n";
			printParameters(weights);
		}
		std::cout << "\ndone!\n";
	}

	long double meanSquaredError(long double K) 
	{
		myEval.initEvalTables();
		answers = 0;

		int batchSize = myPositions.size() / numCores;

		// most disgusting multithreading application youve ever seen
		int start = 0;
		int end = batchSize;
		std::thread th1(processor, start, end, K);

		start = end;
		end = start + batchSize;
		std::thread th2(processor, start, end, K);

		start = end;
		end = start + batchSize;
		std::thread th3(processor, start, end, K);

		start = end;
		end = myPositions.size();
		processor(start, end, K);

		th1.join();
		th2.join();
		th3.join();

		return answers / myPositions.size();
	}

	void processor(int start, int end, long double K) 
	{
		long double errorSum = 0;
		long double sigmoid = 0;
		int score;
		for (int i = start; i < end; ++i) {
			score = myEval.eval(&myPositions[i]) + myEval.pawnEval(&myPositions[i]);
			sigmoid = 1 / (1 + pow(10, -K * score / 400));
			errorSum += pow(myResults[i] - sigmoid, 2);
		}
		answers += errorSum; //threading shit
	}

	long double findK()
	{
		int KPrecision = 10;
		long double start = 0;
		long double end = 10; 
		long double step = 1;
		long double err = 0;

		long double curr = start;
		long double best = meanSquaredError(start);

		for (int i = 0; i < KPrecision; ++i) {
			curr = start - step;
			while (curr < end) {
				curr = curr + step;
				err = meanSquaredError(curr);
				if (err <= best) {
					best = err;
					start = curr;
				}
			}

			std::cout << "Best K of " << start << " on iteration " << i << "\n";

			end = start + step;
			start = start - step;
			step = step / 10.0;
		}

		return start;
	}

	void mapWeightsToParams(int* weights[], bool* skips) 
	{
		unsigned i = 0;
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_pawn_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_pawn_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_knight_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_knight_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_bishop_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_bishop_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_rook_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_rook_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_queen_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_queen_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &mg_king_table[temp];
		for (int temp = 0; temp < SPACES; ++temp)
			weights[i++] = &eg_king_table[temp];
		for (int temp = 0; temp < 6; ++temp)
			weights[i++] = &mg_value[temp];
		for (int temp = 0; temp < 6; ++temp)
			weights[i++] = &eg_value[temp];
		for (int temp = 0; temp < 8; ++temp)
			weights[i++] = &passed_rank_bonus[temp];
		for (int temp = 0; temp < 5; ++temp)
			weights[i++] = &pawn_file_population[temp];
		weights[i++] = &pawn_connected;
		weights[i++] = &pawn_backward;
		weights[i++] = &pawn_isolated;
		weights[i++] = &knight_outpost;
		weights[i++] = &bishop_outpost;
		weights[i++] = &rook_open_file;
		weights[i++] = &bishop_pair;

		i = 0;
		// pawn mg psqt
		for (int temp = 0; temp < 8; ++temp)
			skips[i++] = true;
		i += 48;
		for (int temp = 0; temp < 8; ++temp)
			skips[i++] = true;
		// pawn eg psqt
		for (int temp = 0; temp < 8; ++temp)
			skips[i++] = true;
		i += 48;
		for (int temp = 0; temp < 8; ++temp)
			skips[i++] = true;
		// all other psqt
		i += 64 * 10;
		// piece values mg
		i += 5;
		skips[i++] = true;
		// piece values eg
		i += 5;
		skips[i++] = true;
		// passed rank bonus
		skips[i++] = true;
		i += 6;
		skips[i++] = true;
		// file population penalty
		skips[i++] = true;
		skips[i++] = true;
		i += 3;

		i += 7;
	}

	void printParameters(int* weights[])
	{
		unsigned i = 0;

		std::cout << "\nmg_pawn_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_pawn_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_knight_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_knight_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_bishop_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_bishop_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_rook_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_rook_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_queen_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_queen_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_king_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}
		std::cout << "\neg_king_table\n";
		for (int temp = 0; temp < SPACES; ++temp) {
			std::cout << *weights[i++] << ",";
			(temp % 8 == 7) ? std::cout << "\n" : std::cout << " ";
		}

		std::cout << "\nmg_value_table\n";
		for (int temp = 0; temp < 6; ++temp)
			std::cout << *weights[i++] << ", ";
		std::cout << "\neg_value_table\n";
		for (int temp = 0; temp < 6; ++temp)
			std::cout << *weights[i++] << ", ";

		std::cout << "\npassed_rank_table\n";
		for (int temp = 0; temp < 8; ++temp)
			std::cout << *weights[i++] << ", ";

		std::cout << "\npawn_population_table\n";
		for (int temp = 0; temp < 5; ++temp)
			std::cout << *weights[i++] << ", ";

		std::cout << "\npawn_connected " << *weights[i++] << "\n";
		std::cout << "\npawn_backward " << *weights[i++] << "\n";
		std::cout << "\npawn_isolated " << *weights[i++] << "\n";
		std::cout << "\nknight_outpost " << *weights[i++] << "\n";
		std::cout << "\nbishop_outpost " << *weights[i++] << "\n";
		std::cout << "\nrook_open " << *weights[i++] <<"\n";
		std::cout << "\nbishop_pair " << *weights[i++] << "\n";
	}
}