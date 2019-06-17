# big-time-disc-tomorrow
## TODO:

### COLLISION:
- Fix GJK algorithm to actually be correct
- Generalize ActorEntity movement to allow sliding over collision surfaces

### AI:
- Fix math for distanceToPatrol
- Issue w/ targetPos on same axis as Pos
- Sometimes rotate wrong/awkward direction

### Geometry:
- Fix panel transition math
- Add 'smooth' transitioning
- Player/AI location is misaligned with model, leads to awkward interactions

### Gamestate:
- Add inventory/player-stattracker
- Add loading/saving

### Gameplay:
- Potentially remove 'thrust' dynamic from Ent_Missile
- Basic effects? (ie "Call EffectManager to display text/effect at given loc, for given time, etc)


### After Pow-wow Ideas:
- Flesh out dual character control (Having a guy and a crawler that you can toggle control betweem)
- Add functionality to panel system to allow the guy to use them as normal collision brushes
- Finalizing a level format (a list of panels, a list of entities, start and end zones etc...)
- update level export script for blender to allow above
- Create an inventory system - grid based tetris inventory
    -consumables
    -having equipment in the inventory gives different abilities (reflected on the model?)
- Enemy AI (stealth mechanics, GOAP, patrol/pursue/fight, etc...)
- Improved lighting (lights in level format, point/spot lights, light rays)
- interaction with objects/elements of the world
- Level events (getting caught, scripted events, cutscenes)
- Save/loading (track 'career' progress, what should be in the inter-mission hub zone)
- inter mission hubs































































- Online functionality and lootboxes...
