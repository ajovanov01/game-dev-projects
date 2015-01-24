/*Andrija Jovanov
  2014 12 13
  This program will run a simple AI for Starcraft: Broodwar using BWAPI.
  This AI will work for the Terran Race and it uses a strategy designed
  for use against Terran players 1v1.
  Note: This AI is a personal project that has only been used against the
  default Starcraft AI and not other players online.
*/

#include "TerranAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

//Executes when the game starts.
void TerranAIModule::onStart(){

    //Enable user input.
    Broodwar->enableFlag(Flag::UserInput);

    //Disable Fog of War.
    Broodwar->enableFlag(Flag::CompleteMapInformation);

    /*Set the command optimization level to group certain commands so that
	  the AI uses fewer actions (ie. not issuing the same command one at a
	  time for each unit).*/
    Broodwar->setCommandOptimizationLevel(2);

}

//Executes when the game ends.
void TerranAIModule::onEnd(bool isWinner){

    if(isWinner){

        Broodwar << "TerranAI has won!" << std::endl;
  
	}

}

//Send worker to gather resources.
int TerranAIModule::gatherResources(BWAPI::Unit unit, int numRefineries, int numGasWorkers){

	//Make sure there are three workers per refinery.
	if(numRefineries > 0 && numGasWorkers < numRefineries * 3){

		if(!unit->gather(unit->getClosestUnit(IsRefinery))){

			Broodwar << Broodwar->getLastError() << std::endl;
			return 0;

	    }
		return 1;

	}else{

		if(!unit->gather(unit->getClosestUnit(IsMineralField))){
			Broodwar << Broodwar->getLastError() << std::endl;
		}

	}

	return 0;

}

//Send worker to create a building.
int TerranAIModule::createBuilding(BWAPI::Unit unit, BWAPI::UnitType targetBuilding){

	TilePosition location;

	//This uses Starcraft's AI to determine the exact building location.
	location = Broodwar->getBuildLocation(targetBuilding, unit->getTilePosition());
	unit->build(targetBuilding, location);

	return 0;

}

//Have building train a unit.
int TerranAIModule::trainUnit(BWAPI::Unit unit, BWAPI::UnitType targetUnit){

	unit->train(targetUnit);

	return 0;

}

//Have a building create an addon to itself.
int TerranAIModule::createAddon(BWAPI::Unit unit, BWAPI::UnitType targetAddon){

	unit->buildAddon(targetAddon);

	return 0;

}

//Have a building perform an upgrade.
int TerranAIModule::makeUpgrade(BWAPI::Unit unit, BWAPI::UpgradeType targetUpgrade){

	unit->upgrade(targetUpgrade);

	return 0;

}

//Have a building research new tech.
int TerranAIModule::doResearch(BWAPI::Unit unit, BWAPI::TechType targetTech){

	unit->research(targetTech);

	return 0;

}

/*Have units nearby another unit attack-move to an enemy (move toward the enemy unit's
  position, attacking nearby enemy units along the way).*/
int TerranAIModule::attackEnemy(BWAPI::Unit unit, BWAPI::Position enemyPosition, int radius){

	Unitset attackUnits; //The set of units being ordered to attack.

	attackUnits = unit->getUnitsInRadius(radius, CanAttack && !IsWorker);
	attackUnits.attack(enemyPosition); 

	return 0;

}

//Set a building's rally point. The rally point is where units will move when they are created.
int TerranAIModule::rally(BWAPI::Unit unit, int rallyX, int rallyY){

	int x = unit->getPosition().x + rallyX;
	int y = unit->getPosition().y + rallyY;
	Position rallyPoint(x, y);

	unit->setRallyPoint(rallyPoint);

	return 0;

}

//Executes once per frame.
void TerranAIModule::onFrame(){

	int numWorkers = 0; //Number of worker units owned by this AI.
	static int numGasWorkers = 0; //Number of owned worker units gathering gas.
	int numRefineries = 0; //Number of owned refineries.

	static int centerFrames = 0; //Number of frames when last resource center was created.
	static int supplyFrames = 0; //Number of frames when last supply depot was created.
	static int refineryFrames = 0; //Number of frames when last refinery was created.
	static int barracksFrames = 0; //Number of frames when last barracks was created.
	static int academyFrames = 0; //Number of frames when last academy was created.
	static int factoryFrames = 0; //Number of frames when last factory was created.
	static int starportFrames = 0; //Number of frames when last starport was created.
	static int raidFrames = 0; //Number of frames when last order was made to raid enemy base.
	static int helpFrames = 0; //Number of frames when last order was made to assist ally unit.

	int maxUpgrade = 0; //Maximum level of an upgrade.

	int frameBuffer = 400; //Number of frames to wait before re-attempting to create a building besides resource center and supply depot.
	int centerBuffer = 5000; //Number of frames to wait before re-attempting to create a resource center.
	int supplyFrameBuffer = 200; //Number of frames to wait before re-attempting to create a supply depot.
	int raidBuffer = 1000; //Number of frames to wait before re-attempting to send units to raid enemy base.
	int helpBuffer = 50; //Number of frames to wait before re-attempting to send units to assist ally unit.

	static Unit enemyUnit = NULL; //One enemy unit.
	static Unit enemyBase = NULL; //The enemy's main building.
	static Unit myBase = NULL; //My main building.

	UnitType unitType; //The type of the current unit being checked.
	Race unitRace; //The race of the current unit being checked.

	UnitType targetBuilding; //The type of building being considered, usually for construction.
	UnitType targetUnit; //The type of unit being considered, usually for training.
	UnitType targetAddon; //The type of addon being considered, usually for construction.
	UpgradeType targetUpgrade; //The type of upgrade being considered, usually for research.
	TechType targetTech; //The type of tech being considered, usually for research.

	Unitset myUnits; //The set of units owned by this AI.
	Unitset enemyUnits; //The set of units owned by the opponent.

	TilePosition location; //The location where a building will be placed.

	int rallyX = 200; //Used to calculate the X-coordinate of a rally point.
	int rallyY = 200; //Used to calculate the Y-coordinate of a rally point.

	static bool firstWave = true; //True uf the first attack wave has not been sent.

    //Do not run this method if the game is a replay or paused.
    if(Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
        return;

    //Makes sure that commands have time to process before this method is rerun.
    if(Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
        return;

    //Loop through enemy units to find their main base.
	enemyUnits = Broodwar->enemy()->getUnits();
	if(enemyBase == NULL || !enemyBase->exists()){

	    for(Unitset::iterator unit = enemyUnits.begin(); unit != enemyUnits.end(); unit++){

		    //Skip non-existent units.
            if(!unit->exists())
                continue;

		    unitType = unit->getType();
		    if(unitType.isResourceDepot())
			    enemyBase = *unit;

	    }

	}

	//Give orders to every unit owned by the AI.
	myUnits = Broodwar->self()->getUnits();
    for(Unitset::iterator unit = myUnits.begin(); unit != myUnits.end(); unit++){

		//Skip non-existent units.
        if(!unit->exists())
            continue;

        //Skip units that are immobilized due to status ailments.
        if(unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised())
            continue;

        //Skip units that are immobilized for other reasons.
        if(unit->isLoaded() || !unit->isPowered() || unit->isStuck())
            continue;

        //Skip units that are constructing or being constructed.
        if(!unit->isCompleted() || unit->isConstructing())
            continue;

        //Check the unit's type and race.
		unitType = unit->getType();
		unitRace = unitType.getRace();

        //If the unit is a worker.
        if(unitType.isWorker()){
			
            //If the worker is idle.
            if(unit->isIdle()){

                //Make the worker gather resources or deposit held resources.
                if(unit->isCarryingGas() || unit->isCarryingMinerals()){

                    unit->returnCargo();

				//Gather resources if the worker is not in a state where they are unable to.
				}else if(!unit->getPowerUp() && !unit->isConstructing()){

					//Gather resources, making sure that there are three workers at each refinery.
					targetBuilding = unitRace.getRefinery();
					numRefineries = Broodwar->self()->completedUnitCount(targetBuilding);
					
					numGasWorkers = numGasWorkers + gatherResources(*unit, numRefineries, numGasWorkers);

                }

            }

			//Build a supply provider.
			targetBuilding = unitRace.getSupplyProvider();
			if(Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() <= 10 &&
			   Broodwar->getFrameCount() > supplyFrames + supplyFrameBuffer &&
			   Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			   Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50){

				supplyFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				if(unit->isGatheringGas()){
					numGasWorkers++;
				}

			}

			//Build a refinery.
			targetBuilding = unitRace.getRefinery();
			if(Broodwar->self()->supplyUsed() >= 22 &&
			   Broodwar->self()->completedUnitCount(targetBuilding) == 0 &&
			   Broodwar->getFrameCount() > refineryFrames + frameBuffer &&
			   Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			   Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50){

				refineryFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				numGasWorkers++;

			}

			//Build a barracks.
			targetBuilding = UnitTypes::Terran_Barracks;
			if(Broodwar->self()->supplyUsed() >= 22 &&
			   Broodwar->self()->completedUnitCount(targetBuilding) < 1 &&
			   Broodwar->getFrameCount() > barracksFrames + frameBuffer &&
			   Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			   Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50 &&
			   Broodwar->self()->gas() >= targetBuilding.gasPrice()){

				barracksFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				if(unit->isGatheringGas()){
					numGasWorkers++;
				}

			}

			//Build an academy.
			targetBuilding = UnitTypes::Terran_Academy;
			if(Broodwar->self()->supplyUsed() >= 22 &&
			   Broodwar->self()->completedUnitCount(targetBuilding) == 0 &&
			   Broodwar->getFrameCount() > academyFrames + frameBuffer &&
			   Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			   Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50 &&
			   Broodwar->self()->gas() >= targetBuilding.gasPrice()){

				academyFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				if(unit->isGatheringGas()){
					numGasWorkers++;
				}

			}
			
			//Build a factory.
			targetBuilding = UnitTypes::Terran_Factory;
			if(((Broodwar->self()->supplyUsed() >= 32 && Broodwar->self()->completedUnitCount(targetBuilding) == 0) ||
				(Broodwar->self()->supplyUsed() >= 36 && Broodwar->self()->completedUnitCount(targetBuilding) <= 2)) &&
			     Broodwar->getFrameCount() > factoryFrames + frameBuffer &&
			     Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			     Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50 &&
			     Broodwar->self()->gas() >= targetBuilding.gasPrice()){
					 
				factoryFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				if(unit->isGatheringGas()){
					numGasWorkers++;
				}

			}

			//Build a starport.
			targetBuilding = UnitTypes::Terran_Starport;
			if(Broodwar->self()->supplyUsed() >= 84 &&
			   Broodwar->self()->completedUnitCount(targetBuilding) == 0 &&
			   Broodwar->getFrameCount() > starportFrames + frameBuffer &&
			   Broodwar->self()->incompleteUnitCount(targetBuilding) == 0 &&
			   Broodwar->self()->minerals() >= targetBuilding.mineralPrice() + 50 &&
			   Broodwar->self()->gas() >= targetBuilding.gasPrice()){
					 
				starportFrames = Broodwar->getFrameCount();
				if(unit->isGatheringGas()){
					numGasWorkers--;
				}
				createBuilding(*unit, targetBuilding);
				if(unit->isGatheringGas()){
					numGasWorkers++;
				}

			}

		//If the unit is a resource depot.
        }else if(unitType.isResourceDepot()){

			myBase = *unit;
			if(unit->isIdle()){

				//Train new workers gradually until there are 15.
				targetUnit = unitRace.getWorker();
				numWorkers = Broodwar->self()->allUnitCount(targetUnit);
				if(numWorkers < 15 &&
				   Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired()) &&
				   Broodwar->self()->minerals() >= targetUnit.mineralPrice()){

					trainUnit(*unit, targetUnit);

				}

			}

			if(enemyBase != NULL && enemyBase->exists()){

				/*This will make sure that all unit creating buildings have their rally points
				  set in the direction of the enemy base. This should result in new units
				  moving toward an area where enemy units will appear when they attack.*/
				if(enemyBase->getPosition().x < myBase->getPosition().x)
					rallyX = rallyX * -1;
				if(enemyBase->getPosition().y < myBase->getPosition().y)
					rallyY = rallyY * -1;

				//If an attack force is ready, attack the enemy base.
			    if(Broodwar->self()->completedUnitCount(UnitTypes::Terran_Vulture) >= 9 &&
			       Broodwar->self()->completedUnitCount(UnitTypes::Terran_Siege_Tank_Tank_Mode) >= 3 &&
			       Broodwar->getFrameCount() > raidFrames + raidBuffer){

				       raidFrames = Broodwar->getFrameCount();
				       attackEnemy(*unit, enemyBase->getPosition(), 10000);

			    }

			/*If the enemy's resource depot is destroyed but the game is not over, have all
			  combat units attack the nearest enemy unit.*/
			}else if(Broodwar->getFrameCount() > raidFrames + raidBuffer){

				raidFrames = Broodwar->getFrameCount();
				attackEnemy(*unit, unit->getClosestUnit(IsEnemy)->getPosition(), 999999);

			}

		//If the unit is a barracks.
        }else if(unitType == UnitTypes::Terran_Barracks){

			//Set rally point.
			rally(*unit, rallyX, rallyY);
			if(unit->isIdle()){

				//Train a marine.
			    targetUnit = UnitTypes::Terran_Marine;
			    if(Broodwar->self()->completedUnitCount(targetUnit) < 10 &&
				  (Broodwar->self()->completedUnitCount(targetUnit) <= (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Firebat) * 2) ||
				   Broodwar->self()->completedUnitCount(UnitTypes::Terran_Academy) == 0) &&
			       Broodwar->self()->minerals() >= targetUnit.mineralPrice() &&
			       Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired())){

				    trainUnit(*unit, targetUnit);

			    }

				//Train a firebat
			    targetUnit = UnitTypes::Terran_Firebat;
			    if(Broodwar->self()->completedUnitCount(targetUnit) < 5 &&
				   Broodwar->self()->completedUnitCount(UnitTypes::Terran_Marine) > (Broodwar->self()->completedUnitCount(targetUnit) * 2) &&
			       Broodwar->self()->minerals() >= targetUnit.mineralPrice() &&
				   Broodwar->self()->gas() >= targetBuilding.gasPrice() &&
			       Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired())){

				    trainUnit(*unit, targetUnit);

				}

			}

		//If the unit is a factory.
		}else if(unitType == UnitTypes::Terran_Factory){

			targetBuilding = unitType;
			targetAddon = UnitTypes::Terran_Machine_Shop;
			targetUnit = UnitTypes::Terran_Vulture;
			targetUpgrade = UpgradeTypes::Ion_Thrusters;
			maxUpgrade = Broodwar->self()->getMaxUpgradeLevel(targetUpgrade);

			rally(*unit, rallyX, rallyY);
			if(unit->isIdle()){

				//Create an addon.
			    if(unit->getAddon() == nullptr &&
				   Broodwar->self()->completedUnitCount(targetUnit) > 0 &&
			       Broodwar->self()->minerals() >= targetAddon.mineralPrice() &&
			       Broodwar->self()->gas() >= targetAddon.gasPrice()){

				    createAddon(*unit, targetAddon);

				}

				//Train a vulture.
				if(Broodwar->self()->completedUnitCount(targetUnit) < 12 &&
			       Broodwar->self()->minerals() >= targetUnit.mineralPrice() &&
			       Broodwar->self()->gas() >= targetUnit.gasPrice() &&
			       Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired())){

				    trainUnit(*unit, targetUnit);

				}

				//Train a siege tank.
				targetUnit = UnitTypes::Terran_Siege_Tank_Tank_Mode;
				if(Broodwar->self()->completedUnitCount(targetUnit) < 6 &&
				   Broodwar->self()->completedUnitCount(UnitTypes::Terran_Vulture) >= 12 &&
			       Broodwar->self()->minerals() >= targetUnit.mineralPrice() &&
			       Broodwar->self()->gas() >= targetUnit.gasPrice() &&
			       Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired())){

				    trainUnit(*unit, targetUnit);

				}

			}

		//If the unit is a machine shop.
		}else if(unitType == UnitTypes::Terran_Machine_Shop){

			targetBuilding = UnitTypes::Terran_Factory;
			targetUpgrade = UpgradeTypes::Ion_Thrusters;
			maxUpgrade = Broodwar->self()->getMaxUpgradeLevel(targetUpgrade);

			rally(*unit, rallyX, rallyY);
			if(unit->isIdle()){

				//Research an upgrade.
				if(Broodwar->self()->getUpgradeLevel(targetUpgrade) != maxUpgrade &&
				  !Broodwar->self()->isUpgrading(targetUpgrade) &&
				   Broodwar->self()->completedUnitCount(targetBuilding) == 2 &&
			       Broodwar->self()->minerals() >= targetUpgrade.mineralPrice() &&
			       Broodwar->self()->gas() >= targetUpgrade.gasPrice()){

				    makeUpgrade(*unit, targetUpgrade);

				}

			}

		//If the unit is a starport.
		}else if(unitType == UnitTypes::Terran_Starport){

			targetUnit = UnitTypes::Terran_Wraith;

			rally(*unit, rallyX, rallyY);
			if(unit->isIdle()){

				//Train a wraith.
				if(Broodwar->self()->completedUnitCount(targetUnit) < 3 &&
			       Broodwar->self()->minerals() >= targetUnit.mineralPrice() &&
			       Broodwar->self()->gas() >= targetUnit.gasPrice() &&
			       Broodwar->self()->supplyUsed() <= (Broodwar->self()->supplyTotal() - targetUnit.supplyRequired())){

				    trainUnit(*unit, targetUnit);

				}

			}

		//If the unit is a vulture.
		}else if(unitType == UnitTypes::Terran_Vulture){

			targetBuilding = UnitTypes::Terran_Starport;
			//Launch the first wave of attacks.
			if(firstWave &&
			   Broodwar->self()->completedUnitCount(unitType) >= 9 &&
			   Broodwar->self()->completedUnitCount(targetBuilding) == 0 &&
			   enemyBase->exists()){

				attackEnemy(*unit, enemyBase->getPosition(), 1000);
				firstWave = false;

			}

			//If the vulture is idle and away from the main base, return to the base.
			if(unit->isIdle() && unit->getClosestUnit(IsResourceDepot, 1000) != myBase)
				unit->move(myBase->getPosition());

		//If the unit is a siege tank.
		}else if(unitType == UnitTypes::Terran_Siege_Tank_Tank_Mode){

			if(unit->isIdle() && unit->getClosestUnit(IsResourceDepot, 1000) != myBase)
				unit->move(myBase->getPosition());

		//If the unit is a wraith.
		}else if(unitType == UnitTypes::Terran_Wraith){

			if(unit->isIdle() && unit->getClosestUnit(IsResourceDepot, 1000) != myBase)
				unit->move(myBase->getPosition());

		//If the unit is a marine.
		}else if(unitType == UnitTypes::Terran_Marine){

			if(unit->isIdle() && unit->getClosestUnit(IsResourceDepot, 1000) != myBase)
				unit->move(myBase->getPosition());

		//If the unit is a firebat.
		}else if(unitType == UnitTypes::Terran_Firebat){

			if(unit->isIdle() && unit->getClosestUnit(IsResourceDepot, 1000) != myBase)
				unit->move(myBase->getPosition());

		}

		//If the unit is under attack, get nearby allied combat units to come help.
		if(unit->isUnderAttack() && Broodwar->getFrameCount() > helpFrames + helpBuffer){

			helpFrames = Broodwar->getFrameCount();
			enemyUnit = unit->getClosestUnit(IsEnemy, 500);
			if(enemyUnit != NULL && enemyUnit->exists()){
				attackEnemy(*unit, enemyUnit->getPosition(), 1000);
			}

		}

    }

}

/*The contents of the following methods are largely taken from the example AI module
  in the BWAPI package. They do not affect the AI's gameplay in any way and have been
  left largely untouched.*/

//Executes when text is sent.
void TerranAIModule::onSendText(std::string text){

	//Send the text to the game if it is not being processed.
    Broodwar->sendText("%s", text.c_str());

}

//Executes when text is recieved.
void TerranAIModule::onReceiveText(BWAPI::Player player, std::string text){

	//Parse the received text.
    Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;

}

//Executes when a player leaves the game.
void TerranAIModule::onPlayerLeft(BWAPI::Player player){

	//Inform other players when a player leaves the game.
    Broodwar->sendText("%s has left the game.", player->getName().c_str());

}

//Executes when a nuclear launch is detected.
void TerranAIModule::onNukeDetect(BWAPI::Position target){

    //If the target of the nuclear launch is valid, print its location.
    if(target){
        Broodwar << "Nuclear Launch Detected at " << target << std::endl;
    }

}

void TerranAIModule::onUnitDiscover(BWAPI::Unit unit){
}

void TerranAIModule::onUnitEvade(BWAPI::Unit unit){
}

void TerranAIModule::onUnitShow(BWAPI::Unit unit){
}

void TerranAIModule::onUnitHide(BWAPI::Unit unit){
}

void TerranAIModule::onUnitCreate(BWAPI::Unit unit){
}

void TerranAIModule::onUnitDestroy(BWAPI::Unit unit){
}

void TerranAIModule::onUnitMorph(BWAPI::Unit unit){
}

void TerranAIModule::onUnitRenegade(BWAPI::Unit unit){
}

//Executes when the game is saved.
void TerranAIModule::onSaveGame(std::string gameName){

	//Prints the name the game is saved under.
    Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;

}

void TerranAIModule::onUnitComplete(BWAPI::Unit unit){
}
