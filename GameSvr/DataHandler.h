#pragma once

#pragma pack(1)
typedef struct tagMir2MapHeader
{
	/** 地图宽度(横向长度) */
	WORD	wWidth;
	/** 地图高度(纵向长度) */
	WORD	wHeight;
	/** 标题 */
	CHAR	szDesc[17];
	/** 更新日期 */
	CHAR	szDate[8];
	/** 保留字符 */
	CHAR	szReserved[23];
}Mir2MapHeader, *LPMir2MapHeader;

typedef struct tagMir2MapTile {

	/** 背景图索引 */
	WORD	wBngImgIdx;//tile
	/** 补充背景图索引 */
	WORD	wMidImgIdx;//smstile
	/** 对象图索引 */
	WORD	wObjImgIdx;
	/** 门索引 */
	byte	bDoorIdx;
	/** 门偏移 */
	byte	bDoorOffset;
	/** 动画帧数 */
	byte	bAniFrame;
	/** 动画跳帧数 */
	byte	bAniTick;
	/** 资源文件索引 */
	byte	bObjFileIdx;
	/** 亮度 */
	byte	bLight;
}Mir2MapTile, *LPMir2MapTile;
typedef struct tagMAPFILEHEADER
{
	CHAR	szDesc[20];
	SHORT	shVer;
	SHORT	shWidth;
	SHORT	shHeight;
	CHAR	cEventFileIdx;
	CHAR	cFogColor;
}MAPFILEHEADER, *LPMAPFILEHEADER;

typedef struct tagTILEINFO
{
	CHAR	cFileIdx;
	SHORT	shTileIdx;
}TILEINFO, *LPTILEINFO;

typedef struct tagCELLINFO
{
	CHAR	cFlag;
	BYTE	bObj1Ani;
	BYTE	bObj2Ani;
	CHAR	cFileIdx;
	SHORT	shObj1;
	SHORT	shObj2;
	SHORT	shLigntNEvent;
}CELLINFO, *LPCELLINFO;
#pragma pack(8)

#define OS_MOVINGOBJECT		1
#define OS_ITEMOBJECT		2
#define OS_EVENTOBJECT		3
#define OS_GATEOBJECT		4
#define OS_SWITCHOBJECT		5
#define OS_MAPEVENT			6
#define OS_DOOR				7
#define OS_ROON				8

typedef struct tag_TOSOBJECT
{
	BYTE		btType;
	VOID		*pObject;
	DWORD		dwAddTime;
} _TOSOBJECT, *_LPTOSOBJECT;

class CMapCellInfo
{
public:
//	CRITICAL_SECTION			m_cs;

	CHAR						m_chFlag;
	SHORT						m_sLightNEvent;
	CWHList<_LPTOSOBJECT>*		m_xpObjectList;

	CMapCellInfo();
	~CMapCellInfo();
};
typedef struct tagMirMapEX
{
public:
	USHORT				m_usMiniMap;
	USHORT				m_usBigMap;
	USHORT				m_usMusic;
	/*
	*Normal = 0,
	*Dawn = 1,
	*Day = 2,
	*Evening = 3,
	*Night = 4
	*/
	BYTE				m_bLights;
	BYTE				m_bLighting;
	BYTE				m_bFire;

}TMIRMAPEX,*LPTMIRMAPEX;

class CMirMap
{
public:
	MAPFILEHEADER		m_stMapFH;

	CMapCellInfo*		m_pMapCellInfo;

	BYTE				m_btSeries;					// 瘤开 盔家 加己
	BYTE				m_btSeriesVal;				// 瘤开 盔家 加己蔼

	char				m_szMapName[16];
	char				m_szMapTextName[40];
	TMIRMAPEX			m_exMapDiscEx;

public:
	CMirMap();
	~CMirMap();

	BOOL			LoadMapData(char *pszName,const int mapType=_Mir2Map);
	VOID			FreeMapData();
	BOOL			CanMove(int nX, int nY, int nFlag = FALSE);
	BOOL			CanSafeWalk(int nX, int nY);

	BOOL			MoveToMovingObject(int nX, int nY, int nTargetX, int nTargetY, CCharObject* pObject);
	BOOL			RemoveObject(int nX, int nY, BYTE btType, VOID* pRemoveObject);
	BOOL			AddNewObject(int nX, int nY, BYTE btType, VOID* pAddObject);

	CEvent*			GetEvent(int nX, int nY);
	CMapItem*		GetItem(int nX, int nY);
	CCharObject*	GetObject(int nX, int nY);
	void			GetMapObject(int nX, int nY, int nArea, CWHList<CCharObject*>* pList);
	void			GetAllObject(int nX, int nY, CWHList<CCharObject*>* pList);
	int				GetDupCount(int nX, int nY);

	BOOL			GetDropPosition(int nOrgX, int nOrgY, int nRange, int &nX, int &nY);

	int				CheckDoorEvent(int nX, int nY, int &nEvent);
	int				CheckEvent(int nX, int nY);

	BOOL			IsValidObject(int nX, int nY, int nCheckRange, CCharObject* pCharObject);

	__inline CMapCellInfo* GetMapCellInfo(int nX, int nY) 
		{ 
			if ((nX >= 0 && nX < m_stMapFH.shWidth) && (nY >= 0 && nY < m_stMapFH.shHeight)) 
				return &m_pMapCellInfo[nX * m_stMapFH.shHeight + nY]; 
			return NULL; 
		}
};
