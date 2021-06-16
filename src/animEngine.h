#pragma once

class AnimEngine
{
public:
	AnimEngine();
	~AnimEngine();
private:
	static void Engine(AnimEngine& parent);
	bool EngineRunningState;
};