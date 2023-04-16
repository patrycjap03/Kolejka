
#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <string>

class LetterGenerator {
public:
    void generateLetters(std::queue<char>& queue) {
        std::srand(std::time(nullptr));
        while (true) {
            char c = 'A' + std::rand() % 4;
            queue.push(c);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));//wątek jest uśpiony
        }
    }
    void display_queue(std::queue<char>& queue) {

        // wyświetl całą kolejkę
        std::queue<char> tmp_queue = queue;// tworzę  tymczasową kopię kolejki
        std::cout << "Kolejka: ";
        while (!tmp_queue.empty()) {// każdy element kolejki jest wypisywany na standardowe wyjście
            std::cout << tmp_queue.front() << " ";// zwracam 1 element tmp_queue
            tmp_queue.pop();// usuwam go z kolejki
        }
        std::cout << std::endl;
    }
};

class TrackDisplay {

public:
    void displayTracks(std::queue<char> tracks[], int num_tracks, int track_length, const std::string track_names[]) {// - tracks: tablica kolejek reprezentujących tory
        std::cout << "Stan torow:" << std::endl;
        for (int i = 0; i < num_tracks; i++) { // dla kazdego toru tracks wyświetlam nazwę z tablicy track_names
            std::cout << track_names[i] << " ";
            std::queue<char> tmp_queue = tracks[i];// oraz zawartość kolejki
            while (!tmp_queue.empty()) {
                std::cout << tmp_queue.front();
                tmp_queue.pop();
            }
            for (int j = 0; j < track_length - tracks[i].size(); j++) {
                std::cout << "=";// dopełniam całą długość toru "="
            }
            std::cout << std::endl;
        }
    }


};

class UserInputHandler {
private:
    std::queue<char> queue_;// prywatna kolejka przzechowujaca wygenerowane litery
    const int TRACK_CAPACITY = 10;
public:
    int check_for_win(std::queue<char>& track, int& score, const char* win_pattern) {// wskaznik na charakter, reprezentujacy wygrywajacy wzor
        int pattern_length = std::strlen(win_pattern);// licze dlugosc lancucha win pattern i przechowuje w zmiennej pattern_lenght

        if (track.size() >= pattern_length) {// sprawdzm czy dlugosc wzoru jest wystarczajaca aby pomiescic wzor
            std::string track_str;// wyodrebniam elementy kolejki tworzac ciag znakow strack_str
            for (int i = 0; i < pattern_length; i++) {
                track_str += track.front();//Następnie dla każdego elementu wzorca win_pattern pobierany jest element z początku kolejki track, dopisywany do łańcucha track_str, a następnie usuwany z kolejki.
                track.pop();
            }
            if (track_str == win_pattern) {
                std::cout << "Gratulacje, zdobywasz 10 punktow!" << std::endl;
                score += 10;
            }
            else {
                for (auto c : track_str) {
                    track.push(c);// funkcja ponownie wstawia elementy toru z powrotem do kolejki (track.push(c)).
                }
            }
        }
        return score;
    }
    int check_for_loss(std::queue<char>& track, int& score) {
        if (track.size() >= TRACK_CAPACITY) {
            std::cout << "Tor jest pelny! Tracisz 10 punktow." << std::endl;
            score -= 10;
            while (!track.empty()) {
                track.pop();// oprozniam kolejke
            }
        }
        return score;
    }
    void handleUserInput(std::queue<char>& queue, std::queue<char> tracks[], int& current_track, int& score) {
        char input;
        std::cout << "Nacisnij klawisz 2-5, aby przestawic rozjazd, lub 'w', aby zwolnic wagon" << std::endl;
        std::cin >> input;
        if (input >= '1' && input <= '5') {
            current_track = input - '2' + 1;
            std::cout << "Znajdujesz sie na torze: " << current_track + 1 << std::endl;
        }
        else if (input == 'w') {
            if (!queue.empty()) {
                system("cls");
                tracks[current_track].push(queue.front());// przepycham 1 element z kolejki do aktualnego toru
                check_for_win(tracks[0], score, "AABB");
                check_for_win(tracks[1], score, "ABAB");
                check_for_win(tracks[2], score, "CDDC");
                check_for_win(tracks[3], score, "DCCA");
                check_for_win(tracks[4], score, "BCBA");
                check_for_loss(tracks[current_track], score);
                queue.pop();

            }


            else {
                std::cout << "Nieprawidlowe wejscie. Sprobuj ponownie." << std::endl;
            }

        }
    }


};

class TrainController {
public:
    TrainController(int num_tracks, int track_length, const std::string track_names[])// Konstruktor przyjmuje trzy parametry: liczbę torów, długość toru i nazwy torów
        : m_numTracks(num_tracks), m_trackLength(track_length), m_trackNames(track_names) {}//przypisuje je do odpowiednich pól prywatnych (m_numTracks, m_trackLength i m_trackNames) za pomocą listy inicjalizacyjnej. 
   
    void run() {
        // maksymalna liczba wagonów na torze
        std::queue<char> queue;
        std::queue<char> tracks[5];
        int current_track = 0;
        int score = 0;
        //obiekty klas
        LetterGenerator letterGenerator;
        TrackDisplay trackDisplay;
        UserInputHandler userInputHandler;

        std::thread letterThread(&LetterGenerator::generateLetters, &letterGenerator, std::ref(queue));

        while (true) {//3 metody z obiektow wyzej
            letterGenerator.display_queue(queue);
            trackDisplay.displayTracks(tracks, m_numTracks, m_trackLength, m_trackNames);
            userInputHandler.handleUserInput(queue, tracks, current_track, score);
            std::cout << "Twoj wynik: " << score << std::endl;

            if (score >= 50) {
                std::cout << "Gratulacje, wygrywasz gre!" << std::endl;
                break;
            }
            else if (score <= -10) {
                std::cout << "Przegrales. Koniec gry!" << std::endl;
                break;
            }
        }

        letterThread.join();
    }

private:
    int m_numTracks;
    int m_trackLength;
    const std::string* m_trackNames;
};

int main() {
    const int NUM_TRACKS = 5;
    const int TRACK_CAPACITY = 10;
    const std::string TRACK_NAMES[NUM_TRACKS] = { "[AABB] Tor 1", "[ABAB] Tor 2", "[CDDC] Tor 3", "[DCCA] Tor 4", "[BCBA] Tor 5" };

    TrainController trainController(NUM_TRACKS, 10, TRACK_NAMES);// obiekt TrainController z wykorzystaniem konstruktora klasy i przekazanych mu argumentów: liczby torów, długości toru oraz tablicy z nazwami torów
    trainController.run();

    return 0;
}
