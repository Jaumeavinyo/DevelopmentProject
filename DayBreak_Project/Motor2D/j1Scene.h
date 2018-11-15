#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Map.h"

struct SDL_Texture;
struct MapData;

enum LEVELS {

	MAIN_MENU = 0,
	LEVEL1,
	LEVEL2,
	NO_LEVEL

};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Used to iterate between Levels
	void IterateLevel(int level_change);

	//Used to change the actual level
	void ChangeLevel(int level_change);

public:

	bool Level1 = false;
	bool Level2 = true;
	bool Main_Menu = true;
	
	int LevelIterator = 0;

	LEVELS currentLevel = NO_LEVEL;

	MapData current_map;
	MapData current_pathfinding_map;

private:

	MapData Intro_map;
	MapData Level1_map;
	MapData Level2_map;

	MapData Level1_pathfinding_map;
	//MapData Level2_pathfinding_map;

	bool pathfinding = false;

	pugi::xml_document	SceneDocument;
	pugi::xml_node		music_node;

};

#endif // __j1SCENE_H__