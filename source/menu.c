#include "menu.h"

static BUTTON playOnlineButton;
static BUTTON playBotsButton;
static BUTTON playFriendsButton;
static BUTTON playPuzzlesButton;
static BUTTON learnSkillsButton;

void InitializeMenu(){
    InitializeButton(&playOnlineButton, "assets/play_online_button.png", (Vector2){1340, 200});
    InitializeButton(&playBotsButton, "assets/play_bots_button.png", (Vector2){1340, 320});
    InitializeButton(&playFriendsButton, "assets/play_friend_button.png", (Vector2){1340, 440});
    InitializeButton(&playPuzzlesButton, "assets/play_puzzle_button.png", (Vector2){1340, 560});
    InitializeButton(&learnSkillsButton, "assets/learn_skills_button.png", (Vector2){1340, 680}); 
}

void UpdateMenu(){
    UpdateButton(&playOnlineButton);
    UpdateButton(&playBotsButton);
    UpdateButton(&playFriendsButton);
    UpdateButton(&playPuzzlesButton);
    UpdateButton(&learnSkillsButton);
    if(IsButtonPressed(&playFriendsButton)){
        ChangeScreen(GAME);
    }
}
void RenderMenu(){
    RenderButton(&playOnlineButton);
    RenderButton(&playBotsButton);
    RenderButton(&playFriendsButton);
    RenderButton(&playPuzzlesButton);
    RenderButton(&learnSkillsButton);
}

void UnloadMenu(){
    UnloadButton(&playOnlineButton);
    UnloadButton(&playBotsButton);
    UnloadButton(&playFriendsButton);
    UnloadButton(&playPuzzlesButton);
    UnloadButton(&learnSkillsButton);
}