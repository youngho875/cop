#include <vector>
#include <map>
#include <vector>
#include <string>

using namespace std;

#include "GPoint.h"

/* -------------------------------------------------------------------- */
/*      Shape types (nSHPType)                                          */
/* -------------------------------------------------------------------- */
#define SHPT_NULL	0
#define SHPT_POINT	1
#define SHPT_ARC	3
#define SHPT_POLYGON	5
#define SHPT_MULTIPOINT	8
#define SHPT_POINTZ	11
#define SHPT_ARCZ	13
#define SHPT_POLYGONZ	15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM	21
#define SHPT_ARCM	23
#define SHPT_POLYGONM	25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31

#define	MAX_STRING_LENGTH					256

#define MIN_LINE_WIDTH					0
#define MAX_LINE_WIDTH					5

#define MAP_GENERAL_COAST				0	
#define MAP_GENERAL_BOUNDS				1	
#define MAP_GENERAL_OFFMAPBOUNDS		2
#define MAP_GENERAL_OFFMAPCOAST			3	
#define MAP_GENERAL_AIRWAYS				4
#define MAP_GENERAL_CONTOUR_LINES		5
#define MAP_GENERAL_EXPRESS_WAYS		6
#define MAP_GENERAL_RIVER_LAKES			7
#define MAP_GENERAL_ROADS				8

#define MAP_GENERAL_FIRST				MAP_GENERAL_COAST
#define MAP_GENERAL_LAST				MAP_GENERAL_ROADS
	
#define MAP_TACTICAL_KADIZ				0
#define MAP_TACTICAL_KLIZ				1
#define	MAP_TACTICAL_NLL				2
#define MAP_TACTICAL_MDL				3
#define MAP_TACTICAL_KAAO				4
#define	MAP_TACTICAL_ROUTES				5
#define MAP_TACTICAL_CLOSED_SEA			6
#define MAP_TACTICAL_NFL_CORRIDOR		7
#define	MAP_TACTICAL_ADOA				8
#define MAP_TACTICAL_CADA				9
#define MAP_TACTICAL_ADABELT			10
#define	MAP_TACTICAL_MOA_RES			11
#define MAP_TACTICAL_FCLL				12
#define MAP_TACTICAL_RUNWAY_VECTOR		13
#define	MAP_TACTICAL_SAFETY_CORR		14
#define MAP_TACTICAL_SSS				15
#define MAP_TACTICAL_TAL				16
#define	MAP_TACTICAL_NBZ				17
#define	MAP_TACTICAL_KTO				18
#define	MAP_TACTICAL_PLS				19
#define MAP_TACTICAL_FLINES				20
#define	MAP_TACTICAL_WARMOAS			21				
#define MAP_TACTICAL_COMBATBOUNDARY		22
#define MAP_TACTICAL_HCOMBATBOUNDARY	23

#define	MAP_TACTICAL_KILLBOX			24


#define	MAP_TACTICAL_FIRST				MAP_TACTICAL_KADIZ
#define	MAP_TACTICAL_LAST				MAP_TACTICAL_HCOMBATBOUNDARY

#define POINT_GRID_GEOREF				0
#define	POINT_GRID_LATLONG				1
#define	POINT_GRID_STEREO				2
#define	POINT_GRID_MGRS					3

#define	POINT_GRID_FIRST				POINT_GRID_GEOREF
#define	POINT_GRID_LAST					POINT_GRID_MGRS

#define POINT_TACTICAL_CITIES			0
#define POINT_TACTICAL_MOUNTAIN			1
#define POINT_TACTICAL_RADAR			2
#define POINT_TACTICAL_AIRBASE			3
#define	POINT_TACTICAL_PARTICIPANT		4
#define POINT_TACTICAL_HELIPAD			5
#define POINT_TACTICAL_EMERRWY			6
#define POINT_TACTICAL_TST				7
#define POINT_TACTICAL_AADS				8


#define POINT_TACTICAL_FIRST			POINT_TACTICAL_CITIES
#define POINT_TACTICAL_LAST				POINT_TACTICAL_AADS






#define	POINT_BO_HOSTILE1				0
#define	POINT_BO_HOSTILE2				1
#define	POINT_BO_HOSTILE3				2
#define	POINT_BO_FRIEND1				3
#define	POINT_BO_FRIEND2				4
#define	POINT_BO_FRIEND3				5

#define	POINT_BO_FIRST					POINT_BO_HOSTILE1
#define	POINT_BO_LAST					POINT_BO_FRIEND3


#define POINT_NAVY_HOSTILE1				0
#define POINT_NAVY_HOSTILE2				1
#define POINT_NAVY_HOSTILE3				2
#define POINT_NAVY_FRIEND1				3
#define POINT_NAVY_FRIEND2				4
#define POINT_NAVY_FRIEND3				5

#define	POINT_NAVY_FIRST				POINT_NAVY_HOSTILE1
#define	POINT_NAVY_LAST					POINT_NAVY_FRIEND3