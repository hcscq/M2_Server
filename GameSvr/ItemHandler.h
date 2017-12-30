
#define _ITEM_WEAPON		0
#define _ITEM_ARMOR			1
#define _ITEM_ACCESSORY		2
#define _ITEM_ETC			3
#define _ITEM_GOLD			10

class CMapItem
{
public:
	LONG	pItem;
	char	szName[41];
	WORD	wLooks;
	BYTE	btAniCount;
	int		nCount;
};

class CItem
{
public:
	BYTE		btType;
	BYTE		btAniCount;
	WORD		Index;
	char		szName[20];

	WORD		wStdMode;
	WORD		wShape;
	WORD		wWeight;
	WORD		wDuraMax;		// Val1
	DWORD		dwLooks;
	DWORD		wRSource;		// Val2

	DWORD		dwPrice;

public:
	int			GetUpgrade(int nCount, int nRandom);

	virtual		void GetStandardItem(_LPTCLIENTITEMRCD lpClientItemRcd) = 0;
//	virtual		void UpgradeRandomItem(_LPTMAKEITEMRCD lpMakeItemRcd) = 0;
	virtual		void GetUpgradeStdItem(_LPTCLIENTITEMRCD lpClientItemRcd, _LPTUSERITEMRCD lpUserItemRcd) = 0;
};

class CStdItem : public CItem
{
public:
	CStdItem() {};

	virtual		void GetStandardItem(_LPTCLIENTITEMRCD lpClientItemRcd);
//	virtual		void UpgradeRandomItem(_LPTMAKEITEMRCD lpMakeItemRcd) {};
	virtual		void GetUpgradeStdItem(_LPTCLIENTITEMRCD lpClientItemRcd, _LPTUSERITEMRCD lpUserItemRcd) {};
};

class CStdItemSpecial : public CItem
{
public:
	//BYTE		btType;

	//WORD		wAniCount;
	//WORD		wSource;

	WORD		HP;
	WORD		MP;
	BYTE		AttackSpeed;
	BYTE		Luck;

	BYTE		wAC;				// Defence
	BYTE		wAC2;				// Defence Max
	BYTE		wMAC;				// Magic Defence
	BYTE		wMAC2;				// Magic Defence Max
	BYTE		wDC;				// Attack
	BYTE		wDC2;				// Attack Max
	BYTE		wMC;				// Magic
	BYTE		wMC2;				// Magic Max
	BYTE		wSC;				//
	BYTE		wSC2;				// Max

	BYTE		m_btWater;
	BYTE		m_btWater2;
	BYTE		m_btFire;
	BYTE		m_btFire2;
	BYTE		m_btWind;
	BYTE		m_btWind2;
	BYTE		m_btLight;
	BYTE		m_btLight2;
	BYTE		m_btEarth;
	BYTE		m_btEarth2;

	WORD		wNeed;
	WORD		wNeedLevel;

	DWORD		dwStock;
	DWORD		dwFeature;
	//char		szPrefixName[20];
public:
	CStdItemSpecial();

	void		ApplyItemParameters(CObjectAddAbility *m_pAddAbility);

	void		UpgradeRandomItem(BYTE* btValue, WORD &nDura, WORD &nDuraMax);//_LPTMAKEITEMRCD lpMakeItemRcd)

	virtual		void GetStandardItem(_LPTCLIENTITEMRCD lpClientItemRcd);
//	virtual		void UpgradeRandomItem(_LPTMAKEITEMRCD lpMakeItemRcd);
	virtual		void GetUpgradeStdItem(_LPTCLIENTITEMRCD lpClientItemRcd, _LPTUSERITEMRCD lpUserItemRcd);
};
