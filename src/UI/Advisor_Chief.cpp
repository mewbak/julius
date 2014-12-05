#include "Advisors_private.h"
#include "../Calc.h"
#include "../Event.h"
#include "../Data/Scenario.h"

void UI_Advisor_Chief_drawBackground(int *advisorHeight)
{
	int baseOffsetX = Data_Screen.offset640x480.x;
	int baseOffsetY = Data_Screen.offset640x480.y;
	int width;

	*advisorHeight = 23;
	Widget_Panel_drawOuterPanel(baseOffsetX, baseOffsetY, 40, *advisorHeight);
	Graphics_drawImage(GraphicId(ID_Graphic_AdvisorIcons) + 11, baseOffsetX + 10, baseOffsetY + 10);

	Widget_GameText_draw(61, 0, baseOffsetX + 60, baseOffsetY + 12, Font_LargeBlack);
	Widget_Panel_drawInnerPanel(baseOffsetX + 32, baseOffsetY + 60, 36, 16);

	// workers
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 67);
	Widget_GameText_draw(61, 1, baseOffsetX + 60, baseOffsetY + 66, Font_NormalWhite);
	if (Data_CityInfo.unemploymentPercentage > 0) {
		width = Widget_GameText_draw(61, 12, baseOffsetX + 240, baseOffsetY + 66, Font_NormalRed);
		Widget_Text_drawNumber(Data_CityInfo.unemploymentPercentage, '@', "%",
			baseOffsetX + 240 + width, baseOffsetY + 66, Font_NormalRed);
	} else if (Data_CityInfo.workersNeeded > 0) {
		width = Widget_GameText_draw(61, 13, baseOffsetX + 240, baseOffsetY + 66, Font_NormalRed);
		Widget_GameText_drawNumberWithDescription(8, 12, Data_CityInfo.workersNeeded,
			baseOffsetX + 240 + width, baseOffsetY + 66, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 14, baseOffsetX + 240, baseOffsetY + 66, Font_NormalGreen);
	}

	// finance
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 87);
	Widget_GameText_draw(61, 2, baseOffsetX + 60, baseOffsetY + 86, Font_NormalWhite);
	if (Data_CityInfo.treasury > Data_CityInfo.financeBalanceLastYear) {
		width = Widget_GameText_draw(61, 15, baseOffsetX + 240, baseOffsetY + 86, Font_NormalGreen);
		Widget_Text_drawNumber(Data_CityInfo.treasury - Data_CityInfo.financeBalanceLastYear, '@', "Dn",
			baseOffsetX + 240 + width, baseOffsetY + 86, Font_NormalGreen);
	} else if (Data_CityInfo.treasury < Data_CityInfo.financeBalanceLastYear) {
		width = Widget_GameText_draw(61, 16, baseOffsetX + 240, baseOffsetY + 86, Font_NormalRed);
		Widget_Text_drawNumber(Data_CityInfo.financeBalanceLastYear - Data_CityInfo.treasury, '@', "Dn",
			baseOffsetX + 240 + width, baseOffsetY + 86, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 17, baseOffsetX + 240, baseOffsetY + 86, Font_NormalGreen);
	}

	// migration
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 107);
	Widget_GameText_draw(61, 3, baseOffsetX + 60, baseOffsetY + 106, Font_NormalGreen);
	if (Data_CityInfo.numEnemiesInCity + Data_CityInfo.numImperialSoldiersInCity > 3) {
		Widget_GameText_draw(61, 79, baseOffsetX + 240, baseOffsetY + 106, Font_NormalGreen);
	} else if (Data_CityInfo.populationNewcomersThisMonth >= 5) {
		Widget_GameText_draw(61, 25, baseOffsetX + 240, baseOffsetY + 106, Font_NormalGreen);
	} else if (Data_CityInfo.populationRefusedImmigrantsNoRoom || Data_CityInfo.populationRoomInHouses <= 0) {
		Widget_GameText_draw(61, 18, baseOffsetX + 240, baseOffsetY + 106, Font_NormalRed);
	} else if (Data_CityInfo.populationMigrationPercentage >= 80) {
		Widget_GameText_draw(61, 25, baseOffsetX + 240, baseOffsetY + 106, Font_NormalGreen);
	} else {
		int textId;
		switch (Data_CityInfo.populationEmigrationCauseTextId) {
			case 0: textId = 19; break;
			case 1: textId = 20; break;
			case 2: textId = 21; break;
			case 3: textId = 22; break;
			case 4: textId = 70; break;
			case 5: textId = 71; break;
			default: textId = 0; break;
		}
		if (textId) {
			Widget_GameText_draw(61, textId, baseOffsetX + 240, baseOffsetY + 106, Font_NormalGreen);
		}
	}

	// food stocks
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 127);
	Widget_GameText_draw(61, 4, baseOffsetX + 60, baseOffsetY + 126, Font_NormalWhite);
	if (Data_Scenario.romeSuppliesWheat) {
		Widget_GameText_draw(61, 26, baseOffsetX + 240, baseOffsetY + 126, Font_NormalGreen);
	} else if (Data_CityInfo.foodInfoFoodSupplyMonths > 0) {
		width = Widget_GameText_draw(61, 28, baseOffsetX + 240, baseOffsetY + 126, Font_NormalGreen);
		Widget_GameText_drawNumberWithDescription(8, 4, Data_CityInfo.foodInfoFoodSupplyMonths,
			baseOffsetX + 240 + width, baseOffsetY + 126, Font_NormalGreen);
	} else {
		Widget_GameText_draw(61, 27, baseOffsetX + 240, baseOffsetY + 126, Font_NormalRed);
	}

	// food consumption
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 147);
	Widget_GameText_draw(61, 62, baseOffsetX + 60, baseOffsetY + 146, Font_NormalWhite);
	if (Data_Scenario.romeSuppliesWheat) {
		Widget_GameText_draw(61, 26, baseOffsetX + 240, baseOffsetY + 146, Font_NormalGreen);
	} else {
		int pct = Calc_getPercentage(Data_CityInfo.foodInfoFoodStoredLastMonth, Data_CityInfo.foodInfoFoodConsumedLastMonth);
		if (pct > 150) {
			Widget_GameText_draw(61, 63, baseOffsetX + 240, baseOffsetY + 146, Font_NormalGreen);
		} else if (pct > 105) {
			Widget_GameText_draw(61, 64, baseOffsetX + 240, baseOffsetY + 146, Font_NormalGreen);
		} else if (pct > 95) {
			Widget_GameText_draw(61, 65, baseOffsetX + 240, baseOffsetY + 146, Font_NormalGreen);
		} else if (pct > 75) {
			Widget_GameText_draw(61, 66, baseOffsetX + 240, baseOffsetY + 146, Font_NormalRed);
		} else if (pct > 30) {
			Widget_GameText_draw(61, 67, baseOffsetX + 240, baseOffsetY + 146, Font_NormalRed);
		} else if (pct > 0) {
			Widget_GameText_draw(61, 68, baseOffsetX + 240, baseOffsetY + 146, Font_NormalRed);
		} else {
			Widget_GameText_draw(61, 69, baseOffsetX + 240, baseOffsetY + 146, Font_NormalRed);
		}
	}

	// military
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 167);
	Widget_GameText_draw(61, 5, baseOffsetX + 60, baseOffsetY + 166, Font_NormalWhite);
	if (Data_CityInfo.numImperialSoldiersInCity) {
		Widget_GameText_draw(61, 76, baseOffsetX + 240, baseOffsetY + 166, Font_NormalRed);
	} else if (Data_CityInfo.numEnemiesInCity) {
		Widget_GameText_draw(61, 75, baseOffsetX + 240, baseOffsetY + 166, Font_NormalRed);
	} else if (Event_existsUpcomingInvasion()) {
		Widget_GameText_draw(61, 74, baseOffsetX + 240, baseOffsetY + 166, Font_NormalRed);
	} else if (Data_CityInfo.distantBattleRomanMonthsToReturn > 0 ||
			Data_CityInfo.distantBattleRomanMonthsToTravel > 0) {
		Widget_GameText_draw(61, 78, baseOffsetX + 240, baseOffsetY + 166, Font_NormalGreen);
	} else if (Data_CityInfo.distantBattleMonthsToBattle > 0) {
		Widget_GameText_draw(61, 77, baseOffsetX + 240, baseOffsetY + 166, Font_NormalRed);
	} else if (Data_CityInfo.numSoldiersInCity >= 0) { // BUGFIX? should be >0? >=0 is always true)
		Widget_GameText_draw(61, 73, baseOffsetX + 240, baseOffsetY + 166, Font_NormalGreen);
	} else {
		Widget_GameText_draw(61, 72, baseOffsetX + 240, baseOffsetY + 166, Font_NormalGreen);
	}

	// crime
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 187);
	Widget_GameText_draw(61, 6, baseOffsetX + 60, baseOffsetY + 186, Font_NormalWhite);
	if (Data_CityInfo.numRiotersInCity) {
		Widget_GameText_draw(61, 33, baseOffsetX + 240, baseOffsetY + 186, Font_NormalRed);
	} else if (Data_CityInfo.numCriminalsThisMonth > 10) {
		Widget_GameText_draw(61, 32, baseOffsetX + 240, baseOffsetY + 186, Font_NormalRed);
	} else if (Data_CityInfo.numCriminalsThisMonth) {
		Widget_GameText_draw(61, 31, baseOffsetX + 240, baseOffsetY + 186, Font_NormalRed);
	} else if (Data_CityInfo.numProtestersThisMonth) {
		Widget_GameText_draw(61, 30, baseOffsetX + 240, baseOffsetY + 186, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 29, baseOffsetX + 240, baseOffsetY + 186, Font_NormalGreen);
	}

	// health
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 207);
	Widget_GameText_draw(61, 7, baseOffsetX + 60, baseOffsetY + 206, Font_NormalWhite);
	if (Data_CityInfo.healthRate >= 40) {
		Widget_GameText_draw(56, Data_CityInfo.healthRate / 10 + 27,
			baseOffsetX + 240, baseOffsetY + 206, Font_NormalGreen);
	} else {
		Widget_GameText_draw(56, Data_CityInfo.healthRate / 10 + 27,
			baseOffsetX + 240, baseOffsetY + 206, Font_NormalRed);
	}

	// education
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 227);
	Widget_GameText_draw(61, 8, baseOffsetX + 60, baseOffsetY + 226, Font_NormalWhite);
	if (Data_CityInfo.educationDemand == 1) {
		Widget_GameText_draw(61, 39, baseOffsetX + 240, baseOffsetY + 226, Font_NormalRed);
	} else if (Data_CityInfo.educationDemand == 2) {
		Widget_GameText_draw(61, 40, baseOffsetX + 240, baseOffsetY + 226, Font_NormalRed);
	} else if (Data_CityInfo.educationDemand == 3) {
		Widget_GameText_draw(61, 41, baseOffsetX + 240, baseOffsetY + 226, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 42, baseOffsetX + 240, baseOffsetY + 226, Font_NormalGreen);
	}

	// religion
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 247);
	Widget_GameText_draw(61, 9, baseOffsetX + 60, baseOffsetY + 246, Font_NormalWhite);
	if (Data_CityInfo.religionDemand == 1) {
		Widget_GameText_draw(61, 46, baseOffsetX + 240, baseOffsetY + 246, Font_NormalRed);
	} else if (Data_CityInfo.religionDemand == 2) {
		Widget_GameText_draw(61, 47, baseOffsetX + 240, baseOffsetY + 246, Font_NormalRed);
	} else if (Data_CityInfo.religionDemand == 3) {
		Widget_GameText_draw(61, 48, baseOffsetX + 240, baseOffsetY + 246, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 49, baseOffsetX + 240, baseOffsetY + 246, Font_NormalGreen);
	}

	// entertainment
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 267);
	Widget_GameText_draw(61, 10, baseOffsetX + 60, baseOffsetY + 266, Font_NormalWhite);
	if (Data_CityInfo.entertainmentDemand == 1) {
		Widget_GameText_draw(61, 43, baseOffsetX + 240, baseOffsetY + 266, Font_NormalRed);
	} else if (Data_CityInfo.entertainmentDemand == 2) {
		Widget_GameText_draw(61, 44, baseOffsetX + 240, baseOffsetY + 266, Font_NormalRed);
	} else {
		Widget_GameText_draw(61, 45, baseOffsetX + 240, baseOffsetY + 266, Font_NormalGreen);
	}

	// sentiment
	Graphics_drawImage(GraphicId(ID_Graphic_Bullet), baseOffsetX + 40, baseOffsetY + 287);
	Widget_GameText_draw(61, 11, baseOffsetX + 60, baseOffsetY + 286, Font_NormalWhite);
	if (Data_CityInfo.citySentiment <= 0) {
		Widget_GameText_draw(61, 50, baseOffsetX + 240, baseOffsetY + 286, Font_NormalRed);
	} else if (Data_CityInfo.citySentiment >= 100) {
		Widget_GameText_draw(61, 61, baseOffsetX + 240, baseOffsetY + 286, Font_NormalGreen);
	} else {
		Widget_GameText_draw(61, Data_CityInfo.citySentiment / 10 + 51,
			baseOffsetX + 240, baseOffsetY + 286, Font_NormalGreen);
	}
}
