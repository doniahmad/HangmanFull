#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>

using namespace std;
const string db_basepath = "./database/";
vector<string> categoryAndDifficulty{"city easy", "city normal", "city hard", "country easy", "country normal", "country hard", "province easy", "province normal", "province hard"};
string username, category, difficulty, secretWord, alertTxt;
vector<string> question, guessedWord;
vector<char> guessedCharacter;
int health, hint, level;
bool gameOver = false;

struct UserData
{
    string username;
    string completed;
    int hint;
    int health;
};

string ToLowerCase(string text)
{
    string newText = text;
    for (char &t : newText)
    {
        t = tolower(t);
    }
    return newText;
}
string ToUpperCase(string text)
{
    string newText = text;
    for (char &t : newText)
    {
        t = toupper(t);
    }
    return newText;
}

vector<UserData> ReadUserData(const string &filename)
{
    vector<UserData> userDataList;
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return userDataList;
    }

    string line;
    UserData currentUser;

    while (getline(file, line))
    {
        if (line == "-")
        {
            userDataList.push_back(currentUser);
            currentUser = UserData(); // Reset the current user for the next iteration
        }
        else
        {
            size_t delimiterPos = line.find(':');
            if (delimiterPos != string::npos)
            {
                string key = line.substr(0, delimiterPos);
                string value = line.substr(delimiterPos + 1);

                // Remove leading and trailing whitespaces
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);

                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (key == "username")
                {
                    currentUser.username = value;
                }
                else if (key == "completed")
                {
                    currentUser.completed = value;
                }
                else if (key == "hint")
                {
                    currentUser.hint = stoi(value);
                }
                else if (key == "health")
                {
                    currentUser.health = stoi(value);
                }
            }
        }
    }

    // Add the last user data
    if (!currentUser.username.empty())
    {
        userDataList.push_back(currentUser);
    }

    file.close();
    return userDataList;
}

bool CheckUserFound(string username, vector<UserData> &userDataList)
{
    for (const auto &userData : userDataList)
    {
        if (userData.username == username)
        {
            return true; // Pengguna ditemukan
        }
    }
    return false;
}

bool CheckCompletedCategory(const string &username, const vector<UserData> &userDataList, string categoryAndDifficulty)
{
    for (const auto &userData : userDataList)
    {
        if (userData.username == username)
        {
            string completedCategories = userData.completed;

            // Hapus whitespace di awal dan akhir string
            completedCategories.erase(0, completedCategories.find_first_not_of(" \t"));
            completedCategories.erase(completedCategories.find_last_not_of(" \t") + 1);

            size_t pos = 0;
            string delimiter = ",";

            // Pisahkan kategori yang sudah diselesaikan secara manual
            while ((pos = completedCategories.find(delimiter)) != string::npos)
            {
                string completedCategory = completedCategories.substr(0, pos);
                completedCategory.erase(0, completedCategory.find_first_not_of(" \t"));
                completedCategory.erase(completedCategory.find_last_not_of(" \t") + 1);

                // Implementasi pengecekan kategori dan difficulty yang sesuai
                if (completedCategory == categoryAndDifficulty)
                {
                    return true;
                }

                completedCategories.erase(0, pos + delimiter.length());
            }

            // Cek kategori terakhir setelah loop selesai
            completedCategories.erase(0, completedCategories.find_first_not_of(" \t"));
            completedCategories.erase(completedCategories.find_last_not_of(" \t") + 1);

            // Implementasi pengecekan kategori dan difficulty yang sesuai
            if (completedCategories == categoryAndDifficulty)
            {
                return true;
            }
        }
    }

    return false; // Jika username tidak ditemukan
}

void SaveUser()
{
    ofstream user_db;
    user_db.open(db_basepath + "users/users.txt", ios_base::app);
    if (!user_db.is_open())
    {
        cerr << "Gagal membuka file users.txt" << endl;
        return;
    }

    user_db << "username : " << username << endl;
    user_db << "completed : " << ToLowerCase(category) << " " << ToLowerCase(difficulty) << endl;
    user_db << "hint : " << hint << endl;
    user_db << "health : " << health << endl;
    user_db << "-" << endl;

    user_db.close();
}

void UpdateUser(string username, string filename, vector<UserData> &userDataList)
{
    ofstream file(filename);

    // Find and modify the user data for "Doni"
    for (auto &userData : userDataList)
    {
        if (userData.username == username)
        {
            // Modify the data as needed
            userData.completed = userData.completed + ',' + category;
            userData.hint = hint;
            userData.health = health;
            break;
        }
    }

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    for (const auto &userData : userDataList)
    {
        file << "username : " << userData.username << "\n";
        file << "completed : " << userData.completed << "\n";
        file << "hint : " << userData.hint << "\n";
        file << "health : " << userData.health << "\n-\n";
    }

    file.close();
}

bool fileExists(const string fileName)
{
    ifstream file(db_basepath + "contents/" + fileName);
    return file.good();
}

bool CheckCategory(string category)
{
    string theChategory = ToLowerCase(category);

    if (fileExists(theChategory + ".txt"))
    {
        return true;
    }
    return false;
}

bool CheckDifficulty(string difficulty)
{
    ifstream db_content;
    string theDifficulty = ToLowerCase(difficulty);

    db_content.open(db_basepath + "contents/" + category + ".txt");

    if (db_content.is_open())
    {
        string line;
        while (getline(db_content, line))
        {
            if (line.find(theDifficulty) != string::npos)
            {
                db_content.close();
                return true;
            }
        }
        db_content.close();
    }

    return false;
}

bool CheckWord(string word)
{
    for (string guessed : guessedWord)
    {
        if (word == guessed)
        {
            return true;
        }
    }
    return false;
}

string RandomWord(vector<string> words)
{
    int randomIndex;
    do
    {
        randomIndex = rand() % words.size();
    } while (CheckWord(words[randomIndex]));
    return words[randomIndex];
}

void LoadQuestion()
{
    ifstream db_content;

    db_content.open(db_basepath + "contents/" + category + ".txt");

    if (db_content.is_open())
    {
        string line;
        bool inDifficultySection = false;
        while (getline(db_content, line))
        {
            if (line == "-")
            {
                inDifficultySection = false;
            }
            else if (line.find(ToLowerCase(difficulty)) != string::npos)
            {
                difficulty = line;
                inDifficultySection = true;
            }
            else if (inDifficultySection)
            {
                question.push_back(line);
            }
        }
        db_content.close();
    }
}
bool CheckGuess(char character)
{
    for (char guessed : guessedCharacter)
    {
        if (character == guessed)
        {
            return true;
        }
    }
    return false;
}
char RandomChar()
{
    int randomIndex;
    do
    {
        randomIndex = rand() % secretWord.length();
    } while (CheckGuess(tolower(secretWord[randomIndex])));

    return secretWord[randomIndex];
}

void AddGuessedWord(string word)
{
    guessedWord.push_back(ToLowerCase(word));
}
void AddGuessedChar(char character)
{
    guessedCharacter.push_back(tolower(character));
}

void Init(string username, vector<UserData> &userDataList)
{
    level = 1;
    int freeHint;
    for (const auto userData : userDataList)
    {
        if (userData.username == username)
        {
            hint = userData.hint;
            health = userData.health;
        }
    }
    hint += 5;
    health += 5;
    freeHint = 1;

    for (int i = 1; i <= freeHint; i++)
    {
        AddGuessedChar(RandomChar());
    }
}

void useHint()
{
    AddGuessedChar(RandomChar());
    hint--;
}

bool CheckWin()
{
    for (char c : secretWord)
    {
        if (!CheckGuess(tolower(c)))
        {
            return false;
        }
    }
    return true;
}

void DisplayQuestion()
{
    cout << "-----------------------------------------" << endl;
    cout << "| Difficulty : " << ToUpperCase(difficulty) << "\tCategory : " << ToUpperCase(category) << "\t|" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "| Level : " << level << "\t\t\t\t"
         << "|" << endl;
    cout << "| Health : " << health << "\t\t\t\t"
         << "|" << endl;
    cout << "| Hint : " << hint << "\t\t\t\t"
         << "|" << endl;
    cout << "-----------------------------------------" << endl;
    cout << " ";
    for (char w : secretWord)
    {

        if (CheckGuess(tolower(w)))
        {
            // jika sudah ditebak
            cout << " " << static_cast<char>(toupper(w)) << " ";
        }
        else
        {
            // jika belum tertebak
            cout << " _ ";
        }
    }
    cout << " " << endl;
    cout << "-----------------------------------------" << endl;
}

void SelectCategory()
{

    bool categorySelected = false;
    while (categorySelected == false)
    {
        cout << "( City, Country, Province )\n";
        cout << "Silahkan memilih category : ";
        cin >> category;
        if (CheckCategory(category))
        {
            char changeCategory;

            cout << "Anda memilih category : " << category << endl;
            categorySelected = true;
        }
        else
        {
            cout << "Category yang anda masukkan tidak ditemukan" << endl;
            categorySelected = false;
        }
    }
}

void SelectDifficulty()
{
    bool difficultySelected = false;

    while (difficultySelected == false)
    {
        cout << "( Easy, Normal, Hard )\n";
        cout << "Silahkan memilih difficulty : ";
        cin >> difficulty;
        if (CheckDifficulty(difficulty))
        {
            cout << "Anda memilih difficulty : " << difficulty << endl;
            difficultySelected = true;
        }
        else
        {
            cout << "difficulty yang anda masukkan tidak ditemukan" << endl;
            difficultySelected = false;
        }
    }
}

void SuccesMessage()
{
    DisplayQuestion();
    cout << "Selamat Anda berhasil menjawab kata " << ToUpperCase(secretWord) << endl;
}

void FailedMessage()
{
    gameOver = true;
    DisplayQuestion();
    cout << "Sayang sekali anda gagal menjawab kata " << ToUpperCase(secretWord) << ". Coba lagi nanti! Semangat!";
}

void GameOverMessage()
{
    cout << "Sampai jumpa lagi." << endl;
}

void WinMessage()
{
    cout << "Selamat anda berhasil menjawab semua soal. Anda sangatlah pinta!" << endl;
}
void NextLevel()
{
    char input;
    AddGuessedWord(secretWord);
    if (guessedWord.size() != question.size())
    {
        cout << "\nLevel Selanjutnya : (y/n)";
        cin >> input;
        if (input == 'y')
        {
            system("cls");
            secretWord = RandomWord(question);
            guessedCharacter.clear();
            level++;
            hint++;
            health++;
            int freeHint = 1;

            for (int i = 1; i <= freeHint; i++)
            {
                AddGuessedChar(RandomChar());
            }
        }
        else
        {
            gameOver = true;
            GameOverMessage();
        }
    }
    else
    {
        gameOver = true;
        WinMessage();
    }
}

int main()
{
    srand(time(0));
    system("cls");
    char guess;
    bool categoryAndDifficultySelected = false;
    vector<UserData> userDataList = ReadUserData(db_basepath + "users/users.txt");

    cout << "-------------------------------------------" << endl;
    cout << "|------- Selamat Datang Di Hangman -------|" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "|- "
         << "Anda perlu memasukkan username anda !"
         << " -|" << endl;
    cout << "-------------------------------------------" << endl;
    cout << endl;

    cout << "Masukkan Username anda : ";
    cin >> username;
    cout << endl;

    if (CheckUserFound(username, userDataList))
    {
        char lanjut;
        cout << "User sudah ada! apakah anda ingin melanjutkan ? (y/n)";
        cin >> lanjut;

        if (lanjut != 'y')
        {
            return 0;
        }
        cout << endl;
    }

    while (categoryAndDifficultySelected == false)
    {
        SelectCategory();
        SelectDifficulty();
        cout << category + " " + difficulty;
        string categoryAndDifficultyWantToCheck = category + " " + difficulty;
        if (CheckCompletedCategory(username, userDataList, categoryAndDifficultyWantToCheck))
        {
            cout << "Category Dan Difficulty sudah diselesaikan!\n";
            cout << "Pilih yang lain!\n";
            categoryAndDifficultySelected = false;
        }
        else
        {
            categoryAndDifficultySelected = true;
        }
    }

    LoadQuestion();

    if (!question.empty())
    {
        secretWord = RandomWord(question);
    }

    Init(username, userDataList);
    while (gameOver == false)
    {
        system("cls");
        DisplayQuestion();
        cout << alertTxt << endl;
        cout << "(ketik 'q' untuk menggunakan 'hint')\n";
        cout << "Masukkan huruf yang ingin anda tebak : ";
        cin >> guess;

        if (guess == 'q')
        {
            useHint();
        }
        else if (!CheckGuess(tolower(guess)))
        {
            if (secretWord.find(static_cast<char>(tolower(guess))) != string::npos)
            {
                AddGuessedChar(tolower(guess));
            }
            else
            {
                alertTxt = "kata tidak ditemukan";
                health--;
            }
        }
        else
        {
            alertTxt = "Kata sudah tertebak";
        }
        if (CheckWin() && health > 0)
        {
            SuccesMessage();
            NextLevel();
        }
        if (health <= 0)
        {
            system("cls");
            FailedMessage();
        }
    }

    if (CheckUserFound(username, userDataList))
    {
        UpdateUser(username, db_basepath + "users/users.txt", userDataList);
    }
    else
    {
        SaveUser();
    }
}