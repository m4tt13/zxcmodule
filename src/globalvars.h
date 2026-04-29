
#pragma once

class CGlobalVars {
public:
	float	realtime;
	int		framecount;
	float	absoluteframetime;
	double	curtime;
	float	frametime;
	int		maxClients;
	int		tickcount;
	double	interval_per_tick;
	float	interpolation_amount;
	int		simTicksThisFrame;
	int		network_protocol;
	void*	pSaveData;

private:
	bool	m_bClient;
	int		nTimestampNetworkingBase;
	int		nTimestampRandomizeWindow;
};
