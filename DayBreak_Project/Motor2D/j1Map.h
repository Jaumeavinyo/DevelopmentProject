#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

//Flags to be able to rotate a tile --> Extracted from http://docs.mapeditor.org/en/stable/reference/tmx-map-format/#tile-flipping
const unsigned FLIPPED_HORIZONTALLY_FLAG =	0x60000000;
const unsigned FLIPPED_VERTICALLY_FLAG =	0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG =	0x20000000;


// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;
	p2List<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;


	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
			return data[(y*width) + x];

	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	const char*			name;
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;

	const char* GetName() const { return name; }
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();
	// Called each loop iteration
	void Draw(MapData &data);

	// Called before quitting
	bool CleanUp(MapData &data);

	// Load new map
	bool Load(const char* path, MapData &data);
	iPoint MapToWorld(int x, int y, MapData &data);
	iPoint WorldToMap(int x, int y, MapData &data);
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer, MapData &data);

	TileSet* GetTilesetFromTileId(int id, MapData &data);
	

private:

	bool LoadMap(MapData &data);
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	//MapData data;

public:
	
	p2List<MapData> Map_List;

	p2SString			folder;
	pugi::xml_document	map_file;

	int spawnMargin = 400;

	bool TriggerActive = false;

private:
	bool				map_loaded;
};

#endif // __j1MAP_H__
