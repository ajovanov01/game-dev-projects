#pragma once
#include <BWAPI.h>

class TerranAIModule : public BWAPI::AIModule{

public:
    //Necessary virtual method headers.
    virtual void onStart();
    virtual void onEnd(bool isWinner);
    virtual void onFrame();
    virtual void onSendText(std::string text);
    virtual void onReceiveText(BWAPI::Player player, std::string text);
    virtual void onPlayerLeft(BWAPI::Player player);
    virtual void onNukeDetect(BWAPI::Position target);
    virtual void onUnitDiscover(BWAPI::Unit unit);
    virtual void onUnitEvade(BWAPI::Unit unit);
    virtual void onUnitShow(BWAPI::Unit unit);
    virtual void onUnitHide(BWAPI::Unit unit);
    virtual void onUnitCreate(BWAPI::Unit unit);
    virtual void onUnitDestroy(BWAPI::Unit unit);
    virtual void onUnitMorph(BWAPI::Unit unit);
    virtual void onUnitRenegade(BWAPI::Unit unit);
    virtual void onSaveGame(std::string gameName);
    virtual void onUnitComplete(BWAPI::Unit unit);
    
private:
	//Methods I created personally.
	int gatherResources(BWAPI::Unit unit, int numRefineries, int numGasWorkers);
	int createBuilding(BWAPI::Unit unit, BWAPI::UnitType targetBuilding);
	int trainUnit(BWAPI::Unit unit, BWAPI::UnitType targetUnit);
	int createAddon(BWAPI::Unit unit, BWAPI::UnitType targetAddon);
	int makeUpgrade(BWAPI::Unit unit, BWAPI::UpgradeType targetUpgrade);
	int doResearch(BWAPI::Unit unit, BWAPI::TechType targetTech);
	int attackEnemy(BWAPI::Unit unit, BWAPI::Position enemyPosition, int radius);
	int rally(BWAPI::Unit unit, int rallyX, int rallyY);

};