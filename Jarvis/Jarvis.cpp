
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <sapi.h>
#include <cctype>

#pragma comment(lib, "sapi.lib")

using namespace std;

// =====================================
// JARVIS VOICE SYSTEM LOVE YOU 3000
// =====================================

ISpVoice* voice = nullptr;

void Speak(string message)
{
    cout << "\nJARVIS: " << message << endl;

    if (voice == nullptr)
        return;

    int size = MultiByteToWideChar(
        CP_UTF8, 0, message.c_str(), -1, nullptr, 0
    );

    if (size <= 0)
        return;

    wstring wideMessage(size, L'\0');

    MultiByteToWideChar(
        CP_UTF8, 0, message.c_str(), -1,
        &wideMessage[0], size
    );

    voice->Speak(wideMessage.c_str(), SPF_DEFAULT, nullptr);
}

// =====================================
// GAME DATA
// =====================================

struct Answer
{
    string text;
    int points;
};

struct Question
{
    string question;
    vector<Answer> answers;
};

vector<Question> questions =
{
    {
        "Name something you bring to the beach.",
        {
            {"Towel", 30},
            {"Sunscreen", 25},
            {"Umbrella", 20},
            {"Cooler", 15},
            {"Sunglasses", 10}
        }
    },

    {
        "Name something you find in a refrigerator.",
        {
            {"Milk", 30},
            {"Eggs", 25},
            {"Cheese", 20},
            {"Vegetables", 15},
            {"Juice", 10}
        }
    },

    {
        "Name an animal people keep as a pet.",
        {
            {"Dog", 40},
            {"Cat", 30},
            {"Fish", 15},
            {"Bird", 10},
            {"Hamster", 5}
        }
    },

    {
        "Name a fruit you peel before eating",
        {
            {"Banana", 30},
            {"Orange", 25},
            {"Mango", 20},
            {"Pineapple", 15},
            {"Kiwi", 10}
        }
    },

    {
        "Name something people do when they are bored.",
        {
            {"Watch TV", 30},
            {"Play video games", 25},
            {"Use their phone", 20},
            {"Sleep", 15},
            {"Read", 10}
        }
    }
};

// =====================================
// GAME VARIABLES
// =====================================

int team1Score = 0;
int team2Score = 0;

int strikes = 0;

string team1 = "Team 1";
string team2 = "Team 2";

// =====================================
// LOWERCASE FUNCTION
// =====================================

string Lowercase(string text)
{
    transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(tolower(c));
        }
    );

    return text;
}

// =====================================
// DISPLAY SCORES
// =====================================

void DisplayScores()
{
    cout << "\n========== SCOREBOARD ==========\n";

    cout << team1 << ": " << team1Score << endl;
    cout << team2 << ": " << team2Score << endl;

    cout << "================================\n";
}

// =====================================
// DISPLAY STRIKES
// =====================================

void DisplayStrikes()
{
    cout << "\nSTRIKES: ";

    for (int i = 0; i < strikes; i++)
        cout << "X ";

    cout << endl;
}

// =====================================
// DISPLAY QUESTION
// =====================================

void DisplayQuestion(Question q)
{
    cout << "\n====================================\n";

    cout << "       FAMILY FEUD WITH JARVIS       \n";

    cout << "====================================\n";

    cout << "\nQUESTION:\n";

    cout << q.question << endl;

    cout << "====================================\n";
}

// =====================================
// REVEAL ANSWERS
// =====================================

void RevealAnswers(Question q)
{
    cout << "\n========== ANSWER BOARD ==========\n";

    for (int i = 0; i < q.answers.size(); i++)
    {
        cout << i + 1 << ". "
            << q.answers[i].text
            << " - "
            << q.answers[i].points
            << " points\n";
    }

    cout << "==================================\n";
}

// =====================================
// PLAY ONE ROUND
// =====================================

void PlayRound(Question q, int round)
{
    strikes = 0;

    int roundPoints = 0;

    vector<bool> revealed(q.answers.size(), false);

    int activeTeam;

    cout << "\n====================================\n";
    cout << "             ROUND " << round << endl;
    cout << "====================================\n";

    DisplayQuestion(q);

    Speak("Round " + to_string(round) + ".");

    Speak(q.question);

    cout << "\nEnter the team number playing this round (1 or 2): ";

    cin >> activeTeam;

    while (activeTeam != 1 && activeTeam != 2)
    {
        cout << "Please enter 1 or 2: ";
        cin >> activeTeam;
    }

    string teamName = (activeTeam == 1) ? team1 : team2;

    Speak(teamName + ", you are playing this round.");

    int guessesLeft = 3;

    while (guessesLeft > 0)
    {
        string guess;

        cout << "\nEnter your answer: ";
        getline(cin >> ws, guess);

        guess = Lowercase(guess);

        bool found = false;

        for (int i = 0; i < q.answers.size(); i++)
        {
            if (guess == Lowercase(q.answers[i].text))
            {
                if (revealed[i])
                {
                    Speak("That answer has already been revealed.");
                    found = true;
                    break;
                }

                revealed[i] = true;

                roundPoints += q.answers[i].points;

                Speak("Good answer! " +
                    to_string(q.answers[i].points) +
                    " points.");

                cout << "\nCORRECT ANSWER: "
                    << q.answers[i].text
                    << " - "
                    << q.answers[i].points
                    << " points!\n";

                found = true;

                break;
            }
        }

        if (!found)
        {
            strikes++;

            guessesLeft--;

            Speak("Sorry! That is incorrect. Strike!");

            DisplayStrikes();
        }

        bool allRevealed = true;

        for (bool answer : revealed)
        {
            if (!answer)
                allRevealed = false;
        }

        if (allRevealed)
            break;
    }

    // Reveal remaining answers

    Speak("Let's reveal the remaining answers!");

    RevealAnswers(q);

    // Award round points

    if (activeTeam == 1)
        team1Score += roundPoints;
    else
        team2Score += roundPoints;

    Speak("The round is over!");

    Speak(teamName + " earned " +
        to_string(roundPoints) + " points.");

    DisplayScores();
}

// =====================================
// MAIN GAME
// =====================================

int main()
{
    HRESULT result = CoInitialize(nullptr);

    if (SUCCEEDED(result))
    {
        CoCreateInstance(
            CLSID_SpVoice,
            nullptr,
            CLSCTX_ALL,
            IID_ISpVoice,
            reinterpret_cast<void**>(&voice)
        );
    }

    cout << "\n";
    cout << "====================================\n";
    cout << "       JARVIS FAMILY FEUD GAME       \n";
    cout << "====================================\n";

    Speak("Welcome to Family Feud!");

    Speak("I will be your host today.");

    cout << "\nEnter Team 1 name: ";
    getline(cin, team1);

    cout << "Enter Team 2 name: ";
    getline(cin, team2);

    if (team1.empty())
        team1 = "Team 1";

    if (team2.empty())
        team2 = "Team 2";

    Speak("Welcome " + team1 + " and " + team2 + "!");

    Speak("Let's start the game!");

    for (int i = 0; i < questions.size(); i++)
    {
        PlayRound(questions[i], i + 1);
    }

    // =================================
    // FINAL RESULTS
    // =================================

    cout << "\n====================================\n";
    cout << "           FINAL RESULTS             \n";
    cout << "====================================\n";

    DisplayScores();

    if (team1Score > team2Score)
    {
        Speak("Congratulations " + team1 +
            "! You are the Family Feud champions!");
    }
    else if (team2Score > team1Score)
    {
        Speak("Congratulations " + team2 +
            "! You are the Family Feud champions!");
    }
    else
    {
        Speak("We have a tie! Both teams are champions!");
    }

    Speak("Thank you for playing Family Feud!");

    if (voice != nullptr)
    {
        voice->Release();
        voice = nullptr;
    }

    if (SUCCEEDED(result))
        CoUninitialize();

    return 0;
}
