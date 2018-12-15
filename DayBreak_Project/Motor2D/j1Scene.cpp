#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Collisions.h"
#include "j1PathFinding.h"
#include "j1Fade.h"
#include "j1EntityManager.h"
#include "j1Enemy.h"
#include "j1Player.h"
#include "j1Fonts.h"
#include "j1Objects.h"
#include "j1Gui.h"

#include "Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
	Main_Menu = true;
	Level1 = false;
	Level2 = false;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{

	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
  
	pugi::xml_parse_result load_scenedoc_res = SceneDocument.load_file("config.xml");
	music_node = SceneDocument.child("config").child("music");
	bloodTex =  App->tex->Load("gui/Blood.png");
	if (load_scenedoc_res == NULL)
		LOG("The xml file containing the music fails. Pugi error: %s", load_scenedoc_res.description());
	
	App->map->Load("Main_Menu.tmx", Intro_map);
	App->map->Load("Level1.tmx", Level1_map);
	App->map->Load("Level2.tmx", Level2_map);
	App->map->Load("Level1_WalkabilityMap.tmx", Level1_pathfinding_map);
	App->map->Load("Level2_WalkabilityMap.tmx", Level2_pathfinding_map);


	if (Main_Menu == true) {

		current_map = Intro_map;
		currentLevel = MAIN_MENU;

		App->audio->PlayMusic(music_node.attribute("intro2").as_string());

		pathfinding = false;
	}

	if (Level1 == true) {

		App->collisions->AssignMapColliders("Level1.tmx");
		LoadObjects("Level1.tmx");
	
		if (Player == nullptr)
			Player = Player->CreatePlayer(iPoint(200, 1080));

		current_map = Level1_map;
		currentLevel = LEVEL1;

		pathfinding = true;

		if(Player->active)
			Player->Start();

		if(!App->collisions->active)
			App->collisions->Start();

	}

	else if (Level2 == true) {

		App->collisions->AssignMapColliders("Level2.tmx");
		LoadObjects("Level2.tmx");

		if (Player == nullptr)
			Player = Player->CreatePlayer(iPoint(580, 1400));

		current_map = Level2_map;
		current_pathfinding_map = Level2_pathfinding_map;
		currentLevel = LEVEL2;

		pathfinding = false;

		if (Player->active)
			Player->Start();

		if (!App->collisions->active)
			App->collisions->Start();

	}

	App->audio->ControlMUSVolume(30);
	App->audio->ControlSFXVolume(30);
	App->render->ResetCamera();
	float scale = 0.8;

	//MAIN MENU
	gameName = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 50), SDL_Rect{ 515, 3, 506, 117 }, NONE_LOGIC);

	playButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 200), { 254, 178, 242, 76 }, PLAY, { 254, 88, 242, 76 }, { 254, 0, 242, 76 },scale);
	labelPlayButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, playButton, "PLAY");

	continueButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 300), { 254, 178, 242, 76 }, CONTINUE, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelContinueButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, continueButton, "CONTINE");

	settingsButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 400), { 254, 178, 242, 76 }, ACTIVEWIN, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelSettingsButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsButton, "SETTINGS");


	webPageButton = App->gui->Add_UIElement(BUTTON, iPoint(770,400), { 387,267,108,110 }, WEB, { 389,390,108, 110 }, { 387,267,108,110 }, scale);
	iconWebPageButton = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(30, 20), { 269, 404, 32, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, webPageButton);

	quitButton = App->gui->Add_UIElement(BUTTON, iPoint(15,430), { 0, 178, 242, 76 },QUIT, { 0, 88, 242, 76 }, { 0, 0, 242, 76 }, scale);
	labelQuitButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, quitButton, "QUIT");

	//IN GAME UI

	pauseButton= App->gui->Add_UIElement(BUTTON, iPoint(780,10), { 94,438,75,78 }, ACTIVEWIN, { 94,349,75,78 }, { 94,258,75,78 }, scale);
	iconPauseButton= App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(20, 20), { 201, 301, 31,31 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, pauseButton);
	pauseButton->isActive = false;

	lifeBarBackground= App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(10, 10), {6, 525, 345, 87 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	lifeBarBackground->isActive = false;
	lifeBar = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(67,18), { 513, 481, 254, 40 }, LIFE, NULL_RECT,NULL_RECT, scale,None,lifeBarBackground);

	pausePanel = App->gui->Add_UIElement(PANEL, iPoint(310,80), { 776, 479, 247, 382 }, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.98);
	pausePanel->isActive = false;
	
	settingsButton2 = App->gui->Add_UIElement(BUTTON, iPoint(55, 160), { 254, 178, 242, 76 }, ACTIVEWIN, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7,None,pausePanel);
	labelSettingsButton2 = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, settingsButton2, "SETTINGS");

	returnMainMenuButton = App->gui->Add_UIElement(BUTTON, iPoint(55, 280), { 0, 178, 242, 76 }, RETURN_MAIN_MENU, { 0, 88, 242, 76 }, { 0, 0, 242, 76 }, 0.7, None, pausePanel);
	labelreturnMainMenuButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, returnMainMenuButton, "QUIT");

	resumeButton = App->gui->Add_UIElement(BUTTON, iPoint(55, 30), { 254, 178, 242, 76 }, RESUME_GAME, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7, None, pausePanel);
	labelResumeButton= App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, resumeButton, "RESUME");

	loadButton= App->gui->Add_UIElement(BUTTON, iPoint(55,95), { 254, 178, 242, 76 }, LOAD, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7, None, pausePanel);
	labelLoadButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, loadButton, "LOAD");
	
	BloodAlert = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(0, 0), { 0, 0, 900, 500 }, LIFE_ALERT, NULL_RECT, NULL_RECT);
	BloodAlert->texture = bloodTex;
	BloodAlert->isActive = false;

	SDL_SetTextureBlendMode(bloodTex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(bloodTex, 200);

	//GENERAL UI
	settingsPanel = App->gui->Add_UIElement(PANEL, iPoint(180, 170), { 0, 657, 698, 365 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	settingsPanel->isActive = false;

	closeWinButon = App->gui->Add_UIElement(BUTTON, iPoint(670 * scale, 0), { 3, 438, 76, 76 }, CLOSEWIN, { 3, 349, 76, 76 }, { 3, 259, 76, 76 }, scale, None, settingsPanel);
	iconCloseWinButton = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(18, 18), { 197, 352, 34, 34 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, closeWinButon);

	slideMusic = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 120), { 8, 616, 388, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel);
	slideSFX = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 190), { 8, 616, 388, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel);
	labelMusic = App->gui->Add_UIElement(LABEL, iPoint(30, 100), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel, "MUSIC");
	labelSFX = App->gui->Add_UIElement(LABEL, iPoint(30, 170), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel, "SFX");

	thumbMusic = App->gui->Add_UIElement(PANEL, iPoint(0, -10), { 249, 260, 78, 64 }, DRAGVOLUME, NULL_RECT, NULL_RECT, scale, None, slideMusic);
	thumbSFX = App->gui->Add_UIElement(PANEL, iPoint(0, -10), { 249, 260, 78, 64 }, DRAGVOLUME, NULL_RECT, NULL_RECT, scale, None, slideSFX);

	UI_Elements_List.add(playButton);
	UI_Elements_List.add(quitButton);
	UI_Elements_List.add(continueButton);
	UI_Elements_List.add(settingsButton);
	UI_Elements_List.add(webPageButton);
	UI_Elements_List.add(settingsPanel);
	UI_Elements_List.add(settingsButton2);
	UI_Elements_List.add(closeWinButon);
	UI_Elements_List.add(resumeButton);
	UI_Elements_List.add(returnMainMenuButton);
	UI_Elements_List.add(loadButton);
	UI_Elements_List.add(pauseButton);
	UI_Elements_List.add(thumbMusic);
	UI_Elements_List.add(thumbSFX);
	UI_Elements_List.add(lifeBar);
	UI_Elements_List.add(BloodAlert);


	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene PreUpdate", Profiler::Color::GreenYellow);

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	
	lastMousePos = newMousePos;
	App->input->GetMousePosition(newMousePos.x, newMousePos.y);
	p2List_item<UI_Element*>*UI_Item = UI_Elements_List.end;
	for (; UI_Item != nullptr; UI_Item = UI_Item->prev) {
		if (UI_Item->data->isActive) {
			if (UI_Item->data->Logic == DRAG) {
				if (UI_Item->data->Clicked()) {
					UI_Item->data->Drag();
					break;
				}
			}
			if (UI_Item->data->Logic == LIFE_ALERT) {
				if (Player != nullptr && Player->life <= 20) {
					UI_Item->data->Active(BloodAlert);
					UI_Item->data->Alert(bloodTex, bloodalpha);
				}
				else
					UI_Item->data->Deactive(BloodAlert);
					
			}
			if (UI_Item->data->Logic == ACTIVEWIN) {
				if (UI_Item->data->Clicked()) {
					if (UI_Item->data == settingsButton || UI_Item->data == settingsButton2) {
						if(UI_Item->data == settingsButton)
							settingsPanel->Position.y = 170;
						else
							settingsPanel->Position.y = 60;

						UI_Item->data->Active(settingsPanel);
					}
					else if (UI_Item->data == pauseButton) {
				
						UI_Item->data->Active(pausePanel);
						pausedGame = true;
					}
				}
			}
			if (UI_Item->data->Logic == CLOSEWIN) {
				if (UI_Item->data->Clicked()&&!onAction) {
					if (UI_Item->data == closeWinButon) {
						UI_Item->data->Deactive(settingsPanel);
					}
				}
			}
			if (UI_Item->data->Logic == LIFE_ALERT) {

			}
			if (UI_Item->data->Logic == DRAGVOLUME) {

				if (UI_Item->data->Clicked()) {
					UI_Item->data->VolumeControl(newMousePos, lastMousePos);
					onAction = true;
				}
			}
			if (UI_Item->data->Logic == WEB) {
				if (UI_Item->data->Clicked())
					UI_Item->data->goWeb();
			}
			if (UI_Item->data->Logic == QUIT) {
				if (UI_Item->data->Clicked()) {
					SDL_Quit();
				}
			}
			if (UI_Item->data->Logic == PLAY) {
				if (UI_Item->data->Clicked()) {
					Change_Level = true;
					//Desactive al Main menu
					if (currentLevel == MAIN_MENU) {
						UI_Item->data->Deactive(playButton);
						UI_Item->data->Deactive(quitButton);
						UI_Item->data->Deactive(continueButton);
						UI_Item->data->Deactive(settingsButton);
						UI_Item->data->Deactive(webPageButton);
						UI_Item->data->Deactive(gameName);
						//active inGame UI
						UI_Item->data->Active(pauseButton);
						UI_Item->data->Active(lifeBarBackground);
						pausedGame = false;
					}
				}
			}
			if (UI_Item->data->Logic == RESUME_GAME) {
				if (UI_Item->data->Clicked() && !onAction) {
					UI_Item->data->Deactive(pausePanel);
					UI_Item->data->Deactive(settingsPanel);
					pausedGame = false;
				}
			}
			if (UI_Item->data->Logic == RETURN_MAIN_MENU) {
				if (UI_Item->data->Clicked()&&!onAction) {
					if (currentLevel == LEVEL1) {
						currentLevel = LEVEL2;
						Change_Level = true;
					}
					else
						Change_Level = true;
					UI_Item->data->Active(playButton);
					UI_Item->data->Active(quitButton);
					UI_Item->data->Active(continueButton);
					UI_Item->data->Active(settingsButton);
					UI_Item->data->Active(webPageButton);
					UI_Item->data->Active(gameName);
					UI_Item->data->Deactive(pauseButton);
					UI_Item->data->Deactive(lifeBarBackground);
					UI_Item->data->Deactive(pausePanel);
				}
			}
			if (UI_Item->data->Logic == LIFE) {
				if (Player != nullptr) {
					float initialw = 363;
					UI_Item->data->UI_Rect.w = initialw*((float)Player->life / 100);
				}
			}
		}
	}
	onAction = false;
	BROFILER_CATEGORY("Scene Update", Profiler::Color::PaleVioletRed);

	if (Change_Level) {

		App->fade->Fade(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {
			ChangeLevel(currentLevel);
			Change_Level = false;
		}
	}

	else if (changing_same_Level) {

		App->fade->Fade(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {

			ChangeLevel(NO_CHANGE);
			changing_same_Level = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
		App->LoadGame("save_game.xml");
	

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && !changing_same_Level) {
		//App->fade->Fade(2.0f);
		App->map->TriggerActive = false;
		changing_same_Level = true;
		//ChangeLevel(currentLevel);
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && !Change_Level) {

		Change_Level = true;
		App->map->TriggerActive = false;
	}

	if (currentLevel == MAIN_MENU && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && !Change_Level)
		Change_Level = true;
	
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {

		App->cap = !App->cap;

		if(App->cap)
			App->frame_cap = 30;
		else 
			App->frame_cap = 60;
		
		App->capped_ms = 1000 / App->frame_cap;
	}

	if (Player != nullptr) {

		if (Player->life <= 0) {
			if(Player->Dead.Finished())
			ChangeLevel(LEVEL2+1);
		}
	}

	App->map->Draw(current_map);
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{

	BROFILER_CATEGORY("Scene PostUpdate", Profiler::Color::YellowGreen);
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{

	LOG("Freeing scene");
	return true;
}

void j1Scene::ChangeLevel(int level_change) {
	
	App->entity_manager->DesrtroyEnemies();

	if(Player!=nullptr)
		Player->entity_collider->to_delete = true;

	App->entity_manager->DestroyEntity(Player);
	RELEASE(Player);
	LEVELS aux = currentLevel;


	if (level_change != NO_CHANGE) {
		App->collisions->CleanUp();
		IterateLevel(level_change);
	}

	if (pathfinding) {
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data, current_pathfinding_map))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	if (currentLevel == LEVEL1) {

		App->audio->PlayMusic(music_node.attribute("level").as_string());
		App->render->camera.x = -7;
		App->render->camera.y = -1242;
		Player = Player->CreatePlayer(iPoint(580, 1400));
		Player->LoadPlayer("Character_tileset.png");


		//LOAD SCENE ENEMIES
		//Load Enemy Flyers
		pugi::xml_parse_result Enemies_doc_res = EnemiesDocument.load_file("Enemy2_Settings.xml");
		if (Enemies_doc_res == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res.description());

		pugi::xml_parse_result enemies_pos_result = EnemiesPositions.load_file("Enemy_Positions.xml");
		en_pos = EnemiesPositions.child("config").child("Level1").child("Enemy2");

		App->entity_manager->LoadSceneEnemeies(en_pos, FLYER, "Enemy2_Tileset.png", EnemiesDocument);

		//Load enemy Walkers
		pugi::xml_parse_result Enemies_doc_res2 = EnemiesDocument.load_file("Enemy1_Settings.xml");
		if (Enemies_doc_res2 == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res2.description());

		en_pos = EnemiesPositions.child("config").child("Level1").child("Enemy1");

		App->entity_manager->LoadSceneEnemeies(en_pos, WALKER, "Enemy1_Tileset.png", EnemiesDocument);

	
	}
	if (currentLevel == LEVEL2) {

		App->audio->PlayMusic(music_node.attribute("level").as_string());
		App->render->camera.x = -7;
		App->render->camera.y = -903;
		Player = Player->CreatePlayer(iPoint(200, 1116));
		Player->LoadPlayer("Character_tileset.png");

		//LOAD SCENE ENEMIES
		//Load Enemy Flyers
		pugi::xml_parse_result Enemies_doc_res = EnemiesDocument.load_file("Enemy2_Settings.xml");
		if (Enemies_doc_res == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res.description());

		pugi::xml_parse_result enemies_pos_result = EnemiesPositions.load_file("Enemy_Positions.xml");
		en_pos = EnemiesPositions.child("config").child("Level2").child("Enemy2");

		App->entity_manager->LoadSceneEnemeies(en_pos, FLYER, "Enemy2_Tileset.png", EnemiesDocument);

		//Load enemy Walkers
		pugi::xml_parse_result Enemies_doc_res2 = EnemiesDocument.load_file("Enemy1_Settings.xml");
		if (Enemies_doc_res2 == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res2.description());

		en_pos = EnemiesPositions.child("config").child("Level2").child("Enemy1");

		App->entity_manager->LoadSceneEnemeies(en_pos, WALKER, "Enemy1_Tileset.png", EnemiesDocument);

	}
	if (currentLevel == MAIN_MENU) {
		App->collisions->CleanUp();
	}
	if (currentLevel!=aux) {
		LoadObjects(current_map.Filename.GetString());
		App->collisions->AssignMapColliders(current_map.Filename.GetString());
	}
}

void j1Scene::IterateLevel(int level_change) {

	level_change++;
	LevelIterator = level_change;
	

	if (LevelIterator > 2)
		LevelIterator = 0;
	

	if (LevelIterator == 0) {

		Main_Menu = true;
		Level2 = false;
		Level1 = false;
		currentLevel = MAIN_MENU;
		current_map = Intro_map;
		pathfinding = false;
		
	}

	else if (LevelIterator == 1) {

			Main_Menu = false;
			Level2 = false;
			Level1 = true;
			currentLevel = LEVEL1;
			current_map = Level1_map;
			current_pathfinding_map = Level1_pathfinding_map;
			pathfinding = true;
	}
	
	else if(LevelIterator == 2) {

			Main_Menu = false;
			Level1 = false;
			Level2 = true;
			currentLevel = LEVEL2;
			current_map = Level2_map;
			current_pathfinding_map = Level2_pathfinding_map;
			pathfinding = true;
	}
}


bool j1Scene::LoadObjects(const char*file_name) {

	p2SString tmp("%s%s", App->map->folder.GetString(), file_name);

	pugi::xml_parse_result result = App->map->map_file.load_file(tmp.GetString());

	if (result == NULL) {

		LOG("Could not load xml file %s. pugi error: %s", file_name, result.description());
		return true;
	}

	pugi::xml_node collider;
	pugi::xml_node type;
	const char* collidertype;

	for (type = App->map->map_file.child("map").child("objectgroup"); type && result; type = type.next_sibling("objectgroup")) {

		collidertype = type.attribute("name").as_string();

		for (collider = type.child("object"); collider&&result; collider = collider.next_sibling("object")) {

			if (strcmp(collidertype, "Push_Triggers") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::TRIGGER_PUSH);
				j1Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::PUSHON);


			}

			if (strcmp(collidertype, "PushOff_Triggers") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::TRIGGER_PUSHOFF);
				j1Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::PUSHOFF);

			}

			if (strcmp(collidertype, "Win_Trigger") == 0) {
				
				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::TRIGGER_WIN);
				j1Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::WIN);
			}

			if (strcmp(collidertype, "Bloc_Colliders") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::COLLIDER_BLINKING);
				j1Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::BLINK_BLOCK);

			}
		}
	}


	return true;
}