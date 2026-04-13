PewterCity_Mart_MapScripts::
	.byte 0

PewterCity_Mart_EventScript_Youngster::
	msgbox PewterCity_Mart_Text_BoughtWeirdFishFromShadyGuy, MSGBOX_NPC
	end

PewterCity_Mart_EventScript_Boy::
	msgbox PewterCity_Mart_Text_GoodThingsIfRaiseMonsDiligently, MSGBOX_NPC
	end

PewterCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	goto_if_set FLAG_BADGE06_GET, PewterCity_Mart_Tier3
	goto_if_set FLAG_BADGE03_GET, PewterCity_Mart_Tier2
	pokemart PewterCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

PewterCity_Mart_Tier3::
	pokemart PewterCity_Mart_Items_Tier3
	msgbox Text_PleaseComeAgain
	release
	end

PewterCity_Mart_Tier2::
	pokemart PewterCity_Mart_Items_Tier2
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
PewterCity_Mart_Items::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_HARD_STONE
	.2byte ITEM_CHESTO_BERRY
	.2byte ITEM_NONE

	.align 2
PewterCity_Mart_Items_Tier2::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_HARD_STONE
	.2byte ITEM_CHESTO_BERRY
	.2byte ITEM_NONE

	.align 2
PewterCity_Mart_Items_Tier3::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_HARD_STONE
	.2byte ITEM_CHESTO_BERRY
	.2byte ITEM_NONE
	end
