#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	// labels can act as buttons as well with some flourishes involving the keyboard listener and visual cues.
	shared_ptr<GUILabel> mStartScreenLabel;
	shared_ptr<GUILabel> mStartLabel;
	shared_ptr<GUILabel> mDifficultyLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<GUILabel> mGamerTagLabel;
	shared_ptr<GUILabel> mNameEntryLabel;
	shared_ptr<GUILabel> mNavigationLabel;
	// they are for the game start
	// need to keep aware of game state for later requirements
	bool mGameStarted = false;
	// keep track of labels to apply highligting in the start menu
	int mIndex = 0; 
	// corresponsing method for the highlighting
	void HighlightLabels();
	// method for displaying instructions, essentially like a helper method as will do all the state changes
	void ShowInstructions();;
	// a corresponding boolean for this as well
	bool mViewingInstructions = false;
	// a pointer variable of type label for the information to be displayed. multiple required can figure out 
	// new line character
	shared_ptr<GUILabel> mInstructionsTextLabel1;
	shared_ptr<GUILabel> mInstructionsTextLabel2;
	shared_ptr<GUILabel> mInstructionsTextLabel3;
	shared_ptr<GUILabel> mInstructionsTextLabel4;
	shared_ptr<GUILabel> mInstructionsTextLabel5;
	shared_ptr<GUILabel> mInstructionsTextLabel6;
	shared_ptr<GUILabel> mInstructionsTextLabel7;
	// a method to initliase them
	void CreateInstructions();
	void HideInstructions();




	
	uint mLevel;
	uint mAsteroidCount;

	

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
    shared_ptr<GameObject> CreateExplosion();
	// declare start GUI
	void CreateStartGUI();
	// ask user GUI for name and boolean
	void AskUserGUI();
	void HideAskUserGUI();
	// boolean for ask user
	bool mAskUser = false;
	shared_ptr<GUILabel> mAskUserLabel;
	shared_ptr<GUILabel> mAskUserLabel2;
	// this will be the container for user input and will converted to a label
	string mStringInput;
	// converts to this
	shared_ptr<GUILabel> mUserInputLabel;



	// table view
	void CreateTableView();
	// hide start menu method when gaming 
	void HideStartMenuComponents();
	// also show the componentes 
	void ShowStartMenuComponents();
	// hide score and lives
	void HideScoreLives();
	// view score and lives
	void ViewScoreLives();
	// game over state
	bool mGameOver = false;
	// player struct
	struct Gamer {
		string name;
		int score;
	 };
	// will fill up vector with player from the struct
	vector<Gamer> mGamerVector;

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	// this player will be used more for part 2, but for part 1, going to use a struct for
	// storing value and lives
	Player mPlayer;
};

#endif