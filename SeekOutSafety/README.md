#Seek Out Safety

This is a small homebrew game for the Nintendo DS. It serves as my introduction to embedded systems.

##Compilation/Execution

Run the Makefile to generate the .nds file (this is the ROM). To run the ROM, either open it in a DS emulator or copy to a flash cart and run it from an actual DS console.

##Gameplay

You will play as a red square inside of a maze. Move around with the directional pad and try to move over all of the green squares (these are items). There are also mobs which show up as grey squares. Bump into them to fight them. If you can hit them from behind, they won't hit back. If you can reduce a mob's hit points to zero, they will die.

The game ends when your hit points are reduced to zero (defeat) or when you collect all of the items (victory). The lower screen will display your hitpoints and score. The score does not serve any purpose presently, but was added for possible future developments.