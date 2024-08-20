#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <limits>
#include <chrono>
#include <iomanip>


#define NUM_LEGAL_ANSWERS 2315
#define NUM_LEGAL_GUESSES 12546 + 2315
#define RESET "\033[0m"
#define CORRECT "\033[42m"
#define MISPLACED "\033[43m"

using namespace std;

string legal_guesses[NUM_LEGAL_GUESSES];
string legal_answers[NUM_LEGAL_ANSWERS];

class Wordle {

public:
    Wordle() {


        get_words(legal_guesses,"wordle-allowed-guesses.txt");
        get_words(legal_answers,"wordle-answers-alphabetical.txt");
        
        cout << "\n\nTo play Wordle type \"play\"\nTo run a simulation type the number of games to simulate.\nTo assist your normal game type \"assist\"." << endl;
        string answer;
        cin >> answer;
        if (answer == "play") play_game();
        if (answer == "assist") assist();
        else {
            try {
                int num = std::stoi(answer);
                run_simul(num);
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid response";
                Wordle();
            } catch (const std::out_of_range& oor) {
                std::cerr << "Invalid response";
                Wordle();
            }
        }
    }  

    void assist(){
        string guesses[6];
        int guess_results[6][5] = {{0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0}};
        cout << "Enter your guess: \n";
        string guess;
        cin >> guess;
        cout << "Enter the results as a 5 digit numbe where: Green is 2, yellow is 1, and grey is 0:\n";
        string resultStr;
        cin >> resultStr;

        guesses[0] = guess;

        for (int i = 0; i < 5; i++){
            guess_results[0][i] = resultStr[i] - '0';
        }
        int guess_num = 1;
        while (guess_num < 5){
            cout << "Enter your guess or type help to get assistance: \n";
            string guess;
            cin >> guess;

            if (guess == "help"){
                string best = get_best_guess(guess_results,guesses);
                cout << "Your best guess is \"" << best << "\"\n\n";
                continue;
            }

            cout << "Enter the results as a 5 digit numbe where: Green is 2, yellow is 1, and grey is 0:\n";
            cin >> resultStr;
            
            guesses[guess_num] = guess;

            for (int i = 0; i < 5; i++){
                guess_results[guess_num][i] = resultStr[i] - '0';
            }

            guess_num++;
        }
    }

    void play_game(){
        string guesses[6];
        int guess_results[6][5] = {{0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0}};

        int guess_num;
        guess_num = 0;
        cout << RESET << "WORDLE\n\n";
        

        string secret = get_rand_word(legal_answers);
        for (int i = 0; i < NUM_LEGAL_ANSWERS; i++){
            legal_guesses[i + 12546] = legal_answers[i];
        }

        while (guess_num < 6){
            guess_num = accept_guess(guess_num,guess_results,guesses,secret);
            if (guesses[guess_num - 1] == secret){
                cout << "YOU WIN";
                return;
            }
            print_game(guess_num,guess_results,guesses);
        }

        cout << "YOU LOSE\nWord was " << secret;
    }

    void get_words(string* words,string file_name){
        ifstream file(file_name);

        if (!file.is_open()){
            std::cout << "Cannot open file" << std::endl;
        }

        string word;
        int index = 0;
        while (file >> word){
            words[index++] = word;
        }
        file.close();
    }

    string get_rand_word(string* words){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, NUM_LEGAL_ANSWERS - 1);
        int rand = dist(gen);
        string word = words[rand];
        return word;
    }

    bool verify_guess(string word){
        for (int i = 0; i < NUM_LEGAL_GUESSES; i++){
            if (legal_guesses[i] == word) return true;
        }
        return false;
    }

    void print_game(int guess_num, int results[][5], string gses[]){
        cout << "\n\n";
        cout << "\n\nGusses left: " << (6 - guess_num) << "\n";
        char colour;
        for (int j = 0; j < guess_num; j++){
            for (int i = 0; i < 5; i++){
                if (results[j][i] == 2){
                    cout << CORRECT << gses[j][i] << RESET;
                } else if (results[j][i] == 1){
                    cout << MISPLACED << gses[j][i] << RESET;
                } else cout << gses[j][i];
            }
            cout << endl;
        }
    }

    int accept_guess(int guess_num, int guess_results[][5], string gses[],string secret){
        string entry;
        cin >> entry;
        if (entry == "a"){
            cout << secret;
            return guess_num;
        }
        if (entry == "b"){
            string best_word = get_best_guess(guess_results,gses);
            cout << best_word << endl;
            return guess_num;
        }
        if (entry.length() != 5 || !verify_guess(entry)){
            cout << "\"" << entry << "\" is an invalid guess" << endl;
            print_game(guess_num,guess_results,gses);
            guess_num = accept_guess(guess_num,guess_results,gses,secret);
            return guess_num;
        } 

        gses[guess_num] = entry;

        for (int i = 0; i < 5; i++){
            if (gses[guess_num][i] == secret[i]){
                guess_results[guess_num][i] = 2;
            } else {
                for (int j = 0; j < 5; j++){
                    if (gses[guess_num][i] == secret[j]){
                        guess_results[guess_num][i] = 1;
                    }
                }
            }
        }
        return ++guess_num;
    }

    template <typename T, typename U>
    vector<string> get_possibilities(int results[][5], string gses[], const T& answers, U size){
        int guess_num = 0;

        while(!gses[guess_num].empty() && guess_num < 6) ++guess_num;

        vector<string> possibles;
        for (auto it = 0; it < size; ++it){
            const string& current_answer = answers[it];
            bool word_fits = true;
            for (int j = 0; j < guess_num; j++){
                string non_greys = "";
                for (int i = 0; i < 5; i++){
                    if (results[j][i] != 0) non_greys += gses[j][i];
                }
                for (int i = 0; i < 5; i++){
                    bool letter_in_word = (current_answer.find(gses[j][i]) != string::npos);
                    if (results[j][i] == 2 && gses[j][i] != current_answer[i]){
                        word_fits = false;
                        break;
                    }
                    if (results[j][i] == 1 && (!letter_in_word || gses[j][i] == current_answer[i])){ 
                        word_fits = false;
                        break;
                    }
                    if (results[j][i] == 0 && ((letter_in_word && non_greys.find(gses[j][i]) == string::npos) || current_answer[i] == gses[j][i])){
                        word_fits = false;
                        break;
                    }
                }
                if (!word_fits) break;   
            }
                
            if (word_fits){
                possibles.push_back(current_answer);
            }  
                    
        }
                
        return possibles;    
    }

    string get_best_guess(int results[][5], string gses[]){
        int min_possibles = std::numeric_limits<int>::max();
        int min_possibles_index = 0;
        vector<string> possibles = get_possibilities(results,gses,legal_answers,NUM_LEGAL_ANSWERS);
        int this_results[6][5];
        string this_gses[6];
        int row;
        for (row = 0; row < 6; row++){
            if (gses[row].empty()) break;
            for (int i = 0; i < 5; i++){
                this_results[row][i] = results[row][i];
            }
            this_gses[row] = gses[row];
        }

        for (int n = 0; n < possibles.size(); n++){
            int sub_max = 0;
            // cout << "\r" << n + 1 << " / " << possibles.size();
            
            for (int x = 0; x < possibles.size(); x++){

                calculate_guess_results(this_results,this_gses,possibles[n],possibles[x],row);
                
                vector<string> new_possibles = get_possibilities(this_results,this_gses,possibles,possibles.size());
                sub_max += new_possibles.size();
            }
            
            if (sub_max < min_possibles){
                min_possibles = sub_max;
                min_possibles_index = n;
            }
        }
        
        return possibles[min_possibles_index];
    }

    void calculate_guess_results(int results[][5], string gses[], string guess, string answer, int rowToFill){

        gses[rowToFill] = guess;

        for (int i = 0; i < 5; i++){
            results[rowToFill][i] = 0;
            if (gses[rowToFill][i] == answer[i]){
                results[rowToFill][i] = 2;
            } else if (answer.find(gses[rowToFill][i]) != string::npos){
                results[rowToFill][i] = 1;
            }
        }
    }

    void run_simul(int MAX_ITER){
        int results[7] = {0,0,0,0,0,0,0};

        for (int i = 0; i < MAX_ITER; i++){
            cout << i + 1 << " / " << MAX_ITER << endl;
            string secret = get_rand_word(legal_answers);
            string guesses[6];
            int guess_num = 0;
            int guess_results[6][5] =  {{0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0}};
            

            string guess = "crane";
            while(guess != secret && guess_num < 5){
                cout << guess << endl;
                calculate_guess_results(guess_results, guesses, guess, secret,guess_num);
                guess = get_best_guess(guess_results,guesses);
                guess_num++;
            }
            if (guess != secret){
                 cout << "WRONG" << endl;
                 results[6]++;
            } else results[guess_num]++;

            cout << secret << " in " << guess_num + 1 << "\n\n\n";
            
            
        }

        double avg = 0;
        for (int n = 0; n < 7; n++){
            cout << n + 1 <<  " | " << fixed << setw(2) << setprecision(0) << ((double) results[n]) / MAX_ITER * 100 << "% ";
            for (int i = 1; i <= ((25 * results[n])/results[3]); i++){
                cout << ".";
            }
            avg += (n + 1) * results[n];
            cout << endl;
        }

        avg /= MAX_ITER;
        cout << "Average: " << fixed << setprecision(2) << avg << endl;
        cout << "Wrongs: " << results[6] << endl;
        
    }
};

int main() {
    Wordle myWordle;
}