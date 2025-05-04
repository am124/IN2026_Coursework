#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <algorithm>
// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	// Create Custom Start Menu
	CreateStartGUI();

	// Create a spaceship and add it to the world
	//mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	//CreateAsteroids(10);
	// commented out as not required here

	CreateAsteroids(10);
	//Create the GUI
	CreateGUI();

	HideScoreLives();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{ 
	if (mAskUser) {
		// I feel like this should be in cases, as, this is not robust given that both exist if and cases exist.
		// All characters within ASCII range
		if (key >= 32 && key <= 126) {
			// Append each character to string
			mStringInput += (char)key;  
		}
		// backspace to remove last character. 
		else if (key == 8 && !mStringInput.empty()) {
			// this call to a string takes off the last character
			mStringInput.pop_back();  
		}
	
		// Update the label text to reflect the player's current input
		mUserInputLabel->SetText(mStringInput);  

		// Draw the updated label immediately after changing the text
		mUserInputLabel->Draw();
	}

    switch (key)
	{
	case 13:

		if (mAskUser) {
			// enter key here finalises user input
			HideAskUserGUI();
			mAskUser = false;
			std::cout << "Player Name Entered: " << mAskUser << std::endl;
			ShowStartMenuComponents();
         }
		else if (mGameOver) {
			GameOverView();
			mGameOverScreen = true;
			mScoreKeeper.ResetScore();
			// reset game state method
			 mGameOver = false;
			 mGameOverLabel->SetVisible(false);
			 
		}
		else if (mGameOverScreen) {
			ShowStartMenuComponents();
			mGameOverLabel->SetVisible(false);
			HideScoreLives();
			HideScoreboard();
			mGameStarted = false;
			mViewingInstructions = false;
			mAskUser = false;
			mGameOverScreen = false;
			mGameOver = false;
		}

        else if (!mGameStarted) {
			
			if (mIndex == 0) {
				// for the start functionality, boolean for state tracking, initalised mGameStart to false in the header
		       // enter key value is 13
		       // lets now turn off the labels, with a helper method
				HideStartMenuComponents();
				CreateGUI();
				mGameWorld->AddObject(CreateSpaceship());
				// enter turns to true state
				GameStartedBoolean();

			}
			else if (mIndex == 1) {
				// difficulty processing

			}
			else if (mIndex == 2) {
				// Show instructions processing
				// Conditional statement required, as you can spam the state instuctions state and it will be softlocked into 
				// permanently displaying them
				if (!mViewingInstructions) {
					HideScoreLives();
					ShowInstructions();
					// BOOLEAN
					ViewingInstructionsBoolean();
					
					

				}
			}
			else if (mIndex == 3) {
				// Ask user for name processing
				// enter game tag
				AskUserGUI();
				// enters the ask user state screen
				mAskUser = true;
				// show current user name
				

			}	
		    // prevent further processing, as somehow shooting occurs if not early return.
			return;
		}
		break;



	case ' ':
		// space bar also a traveller 
		if (!mGameStarted) {
			mIndex = (mIndex + 1) % 4;
			HighlightLabels();
		}
		else {
			// it has to be here as spaceship only created when boolean turns true
			mSpaceship->Shoot();
		}
		if (mViewingInstructions) {
			// exit out of instructions
			HideInstructions();
			mViewingInstructions = false;
			ShowStartMenuComponents();
		
		}

		break;
	

	default:  
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {

	
}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	// as mentioned above this prevents crashing as should only apply when in game state
	if (mGameStarted) {

		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;


			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;


			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;


			// Default case - do nothing
		default: break;
		}

	}
	
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{	
	// as mentioned above this prevents crashing as should only apply when in game state
	if (mGameStarted) {

		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}

	}
	
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
		// can use everywhere
		mGameOver = true;
		
		
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}
void Asteroids::GameOverView() {
	Gamer newGamer;
	newGamer.name = mStringInput;
	newGamer.score = mScoreRetrieved;
	mGamerVector.push_back(newGamer);

	// sort the score in descending order
	//sort(mGamerVector.begin(), mGamerVector.end(), [](const Gamer& a, const Gamer& b) {
		//return a.score > b.score;
		//});
	// display the make shift table by looping through vector
	for (int i = 0; i < mGamerVector.size(); i++) {
		// crete label. use auto so it doesn't overwrite removed my prev declaration in header
		// vector gets populated when you are press enter in game over state.
		auto mEntryLabel = make_shared<GUILabel>("Enter Name");
		mEntryLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);

		// we need to put score and name into a single string. makes it all a string. For use in label to view
		std::ostringstream entry;
		entry << (i + 1) << ". " << mGamerVector[i].name << " " << mGamerVector[i].score;
		std::string entry_msg = entry.str();
		mEntryLabel->SetText(entry_msg);
	
		float offsetX = 0.05f;
		float y = 0.5f;

		// uses i, so that the y coordinate changes depending on how many you have.
		shared_ptr<GUIComponent> entry_label_component
			= static_pointer_cast<GUIComponent>(mEntryLabel);
		mGameDisplay->GetContainer()->AddComponent(entry_label_component, GLVector2f(0.37f, y + i * offsetX));	

		mScoreboardLabels.push_back(mEntryLabel);
	}
}
void Asteroids::HideScoreboard() {
	for (auto& label : mScoreboardLabels) {
		mGameDisplay->GetContainer()->RemoveComponent(label);
	}
	mScoreboardLabels.clear(); // Clean up list after removal
}



void Asteroids::AskUserGUI() {


	HideStartMenuComponents();
	mAskUserLabel = make_shared<GUILabel>("Enter Name");
	mAskUserLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> ask_user_component
		= static_pointer_cast<GUIComponent>(mAskUserLabel);
	mGameDisplay->GetContainer()->AddComponent(ask_user_component, GLVector2f(0.37f, 0.7));

	mAskUserLabel2 = make_shared<GUILabel>("Enter to Confirm");
	mAskUserLabel2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> ask_user2_component
		= static_pointer_cast<GUIComponent>(mAskUserLabel2);
	mGameDisplay->GetContainer()->AddComponent(ask_user2_component, GLVector2f(0.3f, 0.3));

	// user input label
	mUserInputLabel = make_shared<GUILabel>("");
	mUserInputLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> user_input_component
		= static_pointer_cast<GUIComponent>(mUserInputLabel);
	mGameDisplay->GetContainer()->AddComponent(user_input_component, GLVector2f(0.3f, 0.5));

}

void Asteroids::CreateStartGUI() {
    // Start Screen Label
	mStartScreenLabel = make_shared<GUILabel>("Start Screen");
	mStartScreenLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> menu_component
		= static_pointer_cast<GUIComponent>(mStartScreenLabel);
	mGameDisplay->GetContainer()->AddComponent(menu_component, GLVector2f(0.37f, 1.0f));
	// Information On Navigating
	mNavigationLabel = make_shared<GUILabel>("(Space Bar Scroll)");
	mNavigationLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> navigate_component
		= static_pointer_cast<GUIComponent>(mNavigationLabel);
	mGameDisplay->GetContainer()->AddComponent(navigate_component, GLVector2f(0.3, 0.95f));

	// Start Label
	mStartLabel = make_shared<GUILabel>("Start Game");
	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f));
	shared_ptr<GUIComponent> start_component
		= static_pointer_cast<GUIComponent>(mStartLabel);
	mGameDisplay->GetContainer()->AddComponent(start_component, GLVector2f(0.4f, 0.8f));
	// Difficulty Label. An option so binary
	mDifficultyLabel = make_shared<GUILabel>("Hard Mode");
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> difficulty_component
		= static_pointer_cast<GUIComponent>(mDifficultyLabel);
	mGameDisplay->GetContainer()->AddComponent(difficulty_component, GLVector2f(0.4f, 0.6f));
	// Gameplay Instructions.
	mInstructionsLabel = make_shared<GUILabel>("Instructions");
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> instructions_component
		= static_pointer_cast<GUIComponent>(mInstructionsLabel);
	mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.4f, 0.4f));
	// Gametag Label
	mGamerTagLabel = make_shared<GUILabel>("Enter Gamer Tag");
	mGamerTagLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> tag_component
		= static_pointer_cast<GUIComponent>(mGamerTagLabel);
	mGameDisplay->GetContainer()->AddComponent(tag_component, GLVector2f(0.4f, 0.2f));
	HideScoreLives();

}
void Asteroids::HighlightLabels() {
	// this will use the mIndex we declared in header.
	// we are using yellow colour (first set thats wrapped around GLVector3f) and white
	// with the second set, so if true its yellow, if not true its white, hence why we initialsied the start
	// label to be yellow as well.

	mStartLabel->SetColor(mIndex == 0 ? GLVector3f(1.0f, 1.0f, 0.0f): GLVector3f(1.0, 1.0f, 1.0f));
	mDifficultyLabel->SetColor(mIndex == 1 ? GLVector3f(1.0f, 1.0f, 0.0f) : GLVector3f(1.0, 1.0f, 1.0f));
	mInstructionsLabel->SetColor(mIndex == 2 ? GLVector3f(1.0f, 1.0f, 0.0f) : GLVector3f(1.0, 1.0f, 1.0f));
	mGamerTagLabel->SetColor(mIndex == 3 ? GLVector3f(1.0f, 1.0f, 0.0f) : GLVector3f(1.0, 1.0f, 1.0f));

}
void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}
// for viewing instructions
void Asteroids::ShowInstructions() {
	// lets also use a boolean for viewing instuctions, as we want to use to escape the instructions screen
	// declared in header file.
	HideStartMenuComponents();
	// we also need to create a new label that has a shared pointer assigned to it. shared pointer declared 
	// in header file.
	CreateInstructions();


}
// instuction text creation 
void Asteroids::CreateInstructions() {
	mInstructionsTextLabel1 = make_shared<GUILabel>("1) Arrow keys to navigate");
	mInstructionsTextLabel1->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component1 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel1);
	mGameDisplay->GetContainer()->AddComponent(instructions_component1, GLVector2f(0.23, 0.8f));

	mInstructionsTextLabel2 = make_shared<GUILabel>("2) Spacebar to shoot");
	mInstructionsTextLabel2->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component2 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel2);
	mGameDisplay->GetContainer()->AddComponent(instructions_component2, GLVector2f(0.23, 0.7f));

	mInstructionsTextLabel3 = make_shared<GUILabel>("3) Shoot Asteroids");
	mInstructionsTextLabel3->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component3 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel3);
	mGameDisplay->GetContainer()->AddComponent(instructions_component3, GLVector2f(0.23, 0.6f));

	mInstructionsTextLabel4 = make_shared<GUILabel>("4) Get hit lose a life");
	mInstructionsTextLabel4->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component4 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel4);
	mGameDisplay->GetContainer()->AddComponent(instructions_component4, GLVector2f(0.23, 0.5f));

	mInstructionsTextLabel5 = make_shared<GUILabel>("5) Hard mode available");
	mInstructionsTextLabel5->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component5 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel5);
	mGameDisplay->GetContainer()->AddComponent(instructions_component5, GLVector2f(0.23, 0.4f));

	mInstructionsTextLabel6 = make_shared<GUILabel>("6) Scoreboard available");
	mInstructionsTextLabel6->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component6 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel6);
	mGameDisplay->GetContainer()->AddComponent(instructions_component6, GLVector2f(0.23, 0.3f));

	mInstructionsTextLabel7 = make_shared<GUILabel>("Spacebar to go back");
	mInstructionsTextLabel7->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> instructions_component7 = static_pointer_cast<GUIComponent>(mInstructionsTextLabel7);
	mGameDisplay->GetContainer()->AddComponent(instructions_component7, GLVector2f(0.23, 0.2f));

}
// hide ask user method
void Asteroids::HideAskUserGUI() {
	if (mAskUserLabel) {
		mAskUserLabel->SetVisible(false);
	}
	if (mAskUserLabel2) {
		mAskUserLabel2->SetVisible(false);
	}
	if (mUserInputLabel) {
		mUserInputLabel->SetVisible(false);
	}
	//...

}
// hide instructions method
void Asteroids::HideInstructions() {
	if (mInstructionsTextLabel1) {
		mInstructionsTextLabel1->SetVisible(false);
	}
	if (mInstructionsTextLabel2) {
		mInstructionsTextLabel2->SetVisible(false);

	}if (mInstructionsTextLabel3) {
		mInstructionsTextLabel3->SetVisible(false);

	}if (mInstructionsTextLabel4) {
		mInstructionsTextLabel4->SetVisible(false);

	}if (mInstructionsTextLabel5) {
		mInstructionsTextLabel5->SetVisible(false);

	}if (mInstructionsTextLabel6) {
		mInstructionsTextLabel6->SetVisible(false);

	}if (mInstructionsTextLabel7) {
		mInstructionsTextLabel7->SetVisible(false);
	}
}
// helper method
void Asteroids::HideStartMenuComponents() {
	// check if null before accessing pointers to guard against null pointers
	// when starting the game essentially
	if (mStartScreenLabel) {
		mStartScreenLabel->SetVisible(false);
     }
	if (mStartLabel) {
		mStartLabel->SetVisible(false);
	}
	if (mDifficultyLabel) {
		mDifficultyLabel->SetVisible(false);
	}
	if (mInstructionsLabel) {
		mInstructionsLabel->SetVisible(false);
	}
	if (mGamerTagLabel) {
		mGamerTagLabel->SetVisible(false);
	}
	if (mNameEntryLabel) {
		mNameEntryLabel->SetVisible(false);
	}
	if (mNavigationLabel) {
		mNavigationLabel->SetVisible(false);
	}

	
}
// Hide score and lives called after CreateStartGUI.
void Asteroids::HideScoreLives() {
	if (mLivesLabel) {
		mLivesLabel->SetVisible(false);

	}
	if (mScoreLabel) {
		mScoreLabel->SetVisible(false);

	}
 }
// View score and lives 
void Asteroids::ViewScoreLives() {
	if (mLivesLabel) {
		mLivesLabel->SetVisible(true);

	}
	if (mScoreLabel) {
		mScoreLabel->SetVisible(true);

	}
}
// View HideStart method good to have
// helper method
void Asteroids::ShowStartMenuComponents() {
	// check if null before accessing pointers to guard against null pointers
	// when starting the game essentially
	if (mStartScreenLabel) {
		mStartScreenLabel->SetVisible(true);
	}
	if (mStartLabel) {
		mStartLabel->SetVisible(true);
	}
	if (mDifficultyLabel) {
		mDifficultyLabel->SetVisible(true);
	}
	if (mInstructionsLabel) {
		mInstructionsLabel->SetVisible(true);
	}
	if (mGamerTagLabel) {
		mGamerTagLabel->SetVisible(true);
	}
	if (mNameEntryLabel) {
		mNameEntryLabel->SetVisible(true);
	}
	if (mNavigationLabel) {
		mNavigationLabel->SetVisible(true);
	}
}
// BOOLEAN MANAGEMENT:
void Asteroids::GameStartedBoolean() {
	mGameStarted = true;
	mViewingInstructions = false;
	mAskUser = false;
	mGameOverScreen = false;
	// this is done by game.  
	//mGameOver = false;
}
void Asteroids::ViewingInstructionsBoolean() {
	mGameStarted = false;
	mViewingInstructions = true;
	mAskUser = false;
	mGameOverScreen = false;
	// this is done by game.  
	//mGameOver = false;
}
void Asteroids::AskUserBoolean() {
	mGameStarted = false;
	mViewingInstructions = false;
	mAskUser = true;
	mGameOverScreen = false;
	// this is done by game.  
	//mGameOver = false;
}
void Asteroids::GameOverScreenBoolean() {
	mGameStarted = false;
	mViewingInstructions = false;
	mAskUser = false;
	mGameOverScreen = true;
	// this is done by game.  
	//mGameOver = false;
}
void Asteroids::GameOverBoolean() {
	mGameStarted = false;
	mViewingInstructions = false;
	mAskUser = false;
	mGameOverScreen = false;
	//this is done by game.  
	//mGameOver = true;
}



void Asteroids::OnScoreChanged(int score)
{
	mScoreRetrieved = score;
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




